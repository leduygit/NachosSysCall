// exception.cc
//	Entry point into the Nachos kernel from user programs.
//	There are two kinds of things that can cause control to
//	transfer back to here from user code:
//
//	syscall -- The user code explicitly requests to call a procedure
//	in the Nachos kernel.  Right now, the only function we support is
//	"Halt".
//
//	exceptions -- The user code does something that the CPU can't handle.
//	For instance, accessing memory that doesn't exist, arithmetic errors,
//	etc.
//
//	Interrupts (which can also cause control to transfer from user
//	code into the Nachos kernel) are handled elsewhere.
//
// For now, this only handles the Halt() system call.
// Everything else core dumps.
//
// Copyright (c) 1992-1993 The Regents of the University of California.
// All rights reserved.  See copyright.h for copyright notice and limitation
// of liability and disclaimer of warranty provisions.

#include "copyright.h"
#include "system.h"
#include "syscall.h"

//----------------------------------------------------------------------
// ExceptionHandler
// 	Entry point into the Nachos kernel.  Called when a user program
//	is executing, and either does a syscall, or generates an addressing
//	or arithmetic exception.
//
// 	For system calls, the following is the calling convention:
//
// 	system call code -- r2
//		arg1 -- r4
//		arg2 -- r5
//		arg3 -- r6
//		arg4 -- r7
//
//	The result of the system call, if any, must be put back into r2.
//
// And don't forget to increment the pc before returning. (Or else you'll
// loop making the same system call forever!
//
//	"which" is the kind of exception.  The list of possible exceptions
//	are in machine.h.
//----------------------------------------------------------------------

void IncreasePC()
{
    int pcAfter = machine->ReadRegister(NextPCReg) + 4;
    machine->WriteRegister(PrevPCReg, machine->ReadRegister(PCReg));
    machine->WriteRegister(PCReg, machine->ReadRegister(NextPCReg));
    machine->WriteRegister(NextPCReg, pcAfter);
}

void ExceptionHandlerHalt()
{
    DEBUG('a', "\n Shutdown, initiated by user program.");
    printf("\n Shutdown, initiated by user program.");
    interrupt->Halt();
}

/*
Input: - User space address (int)
 - Limit of buffer (int)
Output:- Buffer (char*)
Purpose: Copy buffer from User memory space to System memory space
*/
char *User2System(int virtAddr, int limit)
{
    int i; // index
    int oneChar;
    char *kernelBuf = NULL;

    kernelBuf = new char[limit + 1]; // need for terminal string
    if (kernelBuf == NULL)
        return kernelBuf;
    memset(kernelBuf, 0, limit + 1);

    // printf("\n Filename u2s:");
    for (i = 0; i < limit; i++)
    {
        machine->ReadMem(virtAddr + i, 1, &oneChar);
        kernelBuf[i] = (char)oneChar;
        // printf("%c",kernelBuf[i]);
        if (oneChar == 0)
            break;
    }
    return kernelBuf;
}

/*
Input: - User space address (int)
 - Limit of buffer (int)
 - Buffer (char[])
Output:- Number of bytes copied (int)
Purpose: Copy buffer from System memory space to User memory space
*/
int System2User(int virtAddr, int len, char *buffer)
{
    if (len < 0)
        return -1;
    if (len == 0)
        return len;
    int i = 0;
    int oneChar = 0;
    do
    {
        oneChar = (int)buffer[i];
        machine->WriteMem(virtAddr + i, 1, oneChar);
        i++;
    } while (i < len && oneChar != 0);
    return i;
}


void ReadCharHandler()
{
    int maxBytes, readBytes;
    char *buffer;
    maxBytes = 255;
    buffer = new char[maxBytes];
    readBytes = syncCons->Read(buffer, maxBytes);
    if (readBytes > 1)
    {
        printf("Error: Read more than one character\n");
        DEBUG('a', "Error: Read more than one character\n");
        interrupt->Halt();
    }
    else if (readBytes == 0)
    {
        printf("Error: Read zero characters\n");
        DEBUG('a', "Error: Read zero characters\n");
        interrupt->Halt();
    }
    else
    {
        machine->WriteRegister(2, buffer[0]);
    }
    delete[] buffer;
    // printf("Error: Read string failed\n");
    DEBUG('a', "SyscallException: SC_ReadChar\n");
    IncreasePC();
}

void ReadStringHandler()
{
    int virtAddr, length;
    virtAddr = machine->ReadRegister(4);
    length = machine->ReadRegister(5);
    char *str;
    str = new char[length];
    syncCons->Read(str, length);
    if (length < 0)
    {
        // printf("Error: Read string failed\n");
        // DEBUG('a', "Error: Read string failed\n");
        interrupt->Halt();
    }
    else
    {
        System2User(virtAddr, length, str);
    }
    delete[] str;
    IncreasePC();
}

void PrintCharHandler()
{
    char ch;
    ch = (char)machine->ReadRegister(4);
    syncCons->Write(&ch, 1);
    DEBUG('a', "SyscallException: SC_PrintChar\n");
    IncreasePC();
}

void PrintStringHandler()
{
    int virtAddrStr, maxBytes;
    maxBytes = 255;
    virtAddrStr = machine->ReadRegister(4);
    char *strPrint;
    strPrint = User2System(virtAddrStr, maxBytes);
    if (strPrint == NULL)
    {
        // printf("Error: Print string failed\n");
        // DEBUG('a', "Error: Print string failed\n");
        interrupt->Halt();
    }
    else
    {
        syncCons->Write(strPrint, strlen(strPrint));
    }
    delete[] strPrint;
    IncreasePC();
}




void ExceptionHandler(ExceptionType which)
{
    int type = machine->ReadRegister(2);
    switch (which)
    {
    case NoException:
        return;
    case SyscallException:
        switch (type)
        {
        case SC_Halt:
            ExceptionHandlerHalt();
            break;
        case SC_ReadChar:
            ReadCharHandler();
            break;
        case SC_PrintChar:
            PrintCharHandler();
            break;
        case SC_ReadString:
            ReadStringHandler();
            break;
        case SC_PrintString:
            PrintStringHandler();
            break;
        default:
            IncreasePC();
            break;
        }
        break;
    case PageFaultException:
        DEBUG('a', "Unexpected user mode exception PageFaultException\n");
        printf("Unexpected user mode exception PageFaultException\n");
        interrupt->Halt();
        break;
    case ReadOnlyException:
        DEBUG('a', "Unexpected user mode exception ReadOnlyException\n");
        printf("Unexpected user mode exception ReadOnlyException\n");
        interrupt->Halt();
        break;
    case BusErrorException:
        DEBUG('a', "Unexpected user mode exception BusErrorException\n");
        printf("Unexpected user mode exception BusErrorException\n");
        interrupt->Halt();
        break;
    case AddressErrorException:
        printf("Unexpected user mode exception AddressErrorException\n");
        interrupt->Halt();
        break;
    case OverflowException:
        DEBUG('a', "Unexpected user mode exception OverflowException\n");
        printf("Unexpected user mode exception OverflowException\n");
        interrupt->Halt();
        break;
    case IllegalInstrException:
        DEBUG('a', "Unexpected user mode exception IllegalInstrException\n");
        printf("Unexpected user mode exception IllegalInstrException\n");
        interrupt->Halt();
        break;
    case NumExceptionTypes:
        DEBUG('a', "Unexpected user mode exception NumExceptionTypes\n");
        printf("Unexpected user mode exception NumExceptionTypes\n");
        interrupt->Halt();
        break;
    default:
        DEBUG('a', "Unexpected user mode exception (%d %d)\n", which, type);
        printf("\n Unexpected user mode exception (%d %d)", which, type);
        interrupt->Halt();
    }
}
