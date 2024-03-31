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

char *User2System(int virtAddr, int limit)
{
    int i; // chi so index
    int oneChar;
    char *kernelBuf = NULL;
    kernelBuf = new char[limit + 1]; // can cho chuoi terminal
    if (kernelBuf == NULL)
        return kernelBuf;

    memset(kernelBuf, 0, limit + 1);

    for (i = 0; i < limit; i++)
    {
        machine->ReadMem(virtAddr + i, 1, &oneChar);
        kernelBuf[i] = (char)oneChar;
        if (oneChar == 0)
            break;
    }
    return kernelBuf;
}

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

void ExceptionHandlerHalt()
{
    DEBUG('a', "\n Shutdown, initiated by user program.");
    printf("\n Shutdown, initiated by user program.");
    interrupt->Halt();
}

void IncreasePC()
{
    int pcAfter = machine->ReadRegister(NextPCReg) + 4;
    machine->WriteRegister(PrevPCReg, machine->ReadRegister(PCReg));
    machine->WriteRegister(PCReg, machine->ReadRegister(NextPCReg));
    machine->WriteRegister(NextPCReg, pcAfter);
}

void ReadIntHandler()
{
    DEBUG('a', "Read integer number from console.\n");
    int number = 0;
    int nDigit = 0;
    int i;
    char* bufer = new char[MAX_INT_LENGTH];
    nDigit = SynchConsole->Read(bufer, MAX_INT_LENGTH);
    i = (bufer[0] == '-' ? 1 : 0);
    for (; i < nDigit; ++i)
    {
        if (bufer[i] < '0' || bufer[i] > '9')
        {
            DEBUG('a', "\n The integer number is not valid");
            machine->WriteRegister(2, 0);
            delete[] buffer;
            IncreasePC();
            return;
        }
        number = number*10 + (int) (bufer[i] & MASK_GET_NUM);
    }
    number = (bufer[0] == '-' ? -1 * number : number);
    machine->WriteRegister(2, number);
    delete[] bufer;
    IncreasePC();
}

void ReadFloatHandler()
{
    Debug('a', "Read float number from console.\n");
    // const int MAX_FLOAT_LENGTH = 32;
    // char* buffer = new char[MAX_FLOAT_LENGTH];
    // int nDigit = SynchConsole->Read(buffer, MAX_FLOAT_LENGTH);
    // int i = (buffer[0] == '-' ? 1 : 0);
    // float number = 0.0f;
    // float decimalMultiplier = 0.1f; // Used to calculate the fractional part
    // bool isNegative = (buffer[0] == '-' ? true : false);

    // for (; i < nDigit; ++i)
    // {
    //     if (buffer[i] == '.') // Check for decimal point
    //     {
    //         decimalMultiplier = 0.01f; // Initialize to 0.01 to handle the fractional part
    //         continue;
    //     }
    //     if (buffer[i] < '0' || buffer[i] > '9') // Check for valid digits
    //     {
    //         DEBUG('a', "\n The float number is not valid");
    //         machine->WriteRegister(2, 0);
    //         delete[] buffer;
    //         IncreasePC();
    //         return;
    //     }
    //     number = number * 10 + (buffer[i] - '0');
    //     if (decimalMultiplier != 0.1f) // If decimal point encountered, update multiplier
    //     {
    //         number += (buffer[i] - '0') * decimalMultiplier;
    //         decimalMultiplier *= 0.1f; // Move the multiplier one decimal place to the right
    //     }
    // }

    // if (isNegative)
    // {
    //     number *= -1;
    // }

    // int* numberPtr = reinterpret_cast<int*>(&number);
    // machine->WriteRegister(2, *numberPtr);
    // delete[] buffer;
    IncreasePC();
}


void PrintIntHandler()
{
    Debug('a', "Print integer number to console.\n");
    const int MAX_INT_LENGTH = 12; // Maximum digits for a 32-bit integer, including the sign
    char buffer[MAX_INT_LENGTH];
    int number = machine->ReadRegister(4);
    int index = 0;

    // Convert integer to string
    do {
        buffer[index++] = '0' + (number % 10);
    } while ((number /= 10) != 0);

    // If the number is negative, append '-' at the beginning
    if (number < 0)
    {
        buffer[index++] = '-';
    }

    // Reverse the string
    for (int i = 0, j = index - 1; i < j; i++, j--)
    {
        char temp = buffer[i];
        buffer[i] = buffer[j];
        buffer[j] = temp;
    }

    // Add newline character
    buffer[index++] = '\n';

    // Write the string to console
    SynchConsole->Write(buffer, index);
    IncreasePC();
}

void PrintFloatHandler()
{
    Debug('a', "Print float number to console.\n");
    IncreasePC();
}


void
ExceptionHandler(ExceptionType which)
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
                case SC_Open: //...
                    break;
                case SC_ReadInt:
                    ReadIntHandler();
                    IncreasePC();
                    break;
                case SC_ReadFloat:
                    ReadFloatHandler();
                    IncreasePC();
                    break;
                case SC_PrintInt:
                    PrintIntHandler();
                    IncreasePC();
                    break;
                case SC_PrintFloat:
                    PrintFloatHandler();
                    IncreasePC();
                    break;
                default:
                    IncreasePC();
            }
            break;
	case PageFaultException:
	    printf("Unexpected user mode exception PageFaultException\n");
	    interrupt->Halt();
            break;
    	case ReadOnlyException:
            printf("Unexpected user mode exception ReadOnlyException\n");
            interrupt->Halt();
            break;
    	case BusErrorException:
            printf("Unexpected user mode exception BusErrorException\n");
            interrupt->Halt();
            break;
    	case AddressErrorException:
            printf("Unexpected user mode exception AddressErrorException\n");
            interrupt->Halt();
            break;
    	case OverflowException:
            printf("Unexpected user mode exception OverflowException\n");
            interrupt->Halt();
            break;
   	case IllegalInstrException:
            printf("Unexpected user mode exception IllegalInstrException\n");
            interrupt->Halt();
            break;
    	case NumExceptionTypes:
            printf("Unexpected user mode exception NumExceptionTypes\n");
            interrupt->Halt();
            break;
    	default:
            printf("\n Unexpected user mode exception (%d %d)", which, type);
            interrupt->Halt();
    }
    

    if ((which == SyscallException) && (type == SC_Halt)) {
	DEBUG('a', "Shutdown, initiated by user program.\n");
   	interrupt->Halt();
    } else {
	printf("Unexpected user mode exception %d %d\n", which, type);
	ASSERT(FALSE);
    }
}