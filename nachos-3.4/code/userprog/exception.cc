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

const int MaxFileLength = 32;

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

void ExceptionHandlerHalt()
{
    DEBUG('a', "\n Shutdown, initiated by user program");
    printf("\n Shutdown, initiated by user program .");
    interrupt->Halt();
}

void IncreasePC()
{
    int pcAfter = machine->ReadRegister(NextPCReg) + 4;
    machine->WriteRegister(PrevPCReg, machine->ReadRegister(PCReg));
    machine->WriteRegister(PCReg, machine->ReadRegister(NextPCReg));
    machine->WriteRegister(NextPCReg, pcAfter);
}

char* User2System(int virtAddr, int limit) 
{ 
    int i;
    int oneChar; 
    char* kernelBuf = NULL; 
    kernelBuf = new char[limit + 1];
    if (kernelBuf == NULL) return kernelBuf;
    memset(kernelBuf, 0, limit + 1); 
    for (i = 0; i < limit; i++) { 
        machine->ReadMem(virtAddr + i, 1, &oneChar); 
        kernelBuf[i] = (char)oneChar; 
        if (oneChar == 0) 
        break; 
    } 
    return kernelBuf; 
} 

int System2User(int virtAddr, int len, char* buffer)  { 
    if (len < 0) return -1; 
    if (len == 0) return len; 
    int i = 0; 
    int oneChar = 0 ; 

    do { 
    oneChar = (int) buffer[i]; 
    machine->WriteMem(virtAddr + i, 1, oneChar); 
    i++; 
    } while(i < len && oneChar != 0); 
    
    return i; 
}

//----------------------------------------------------------------------
// Function to handle system call
void createFile()
{
    printf("Create File \n");
    int virtAddr;
    char* filename;
    virtAddr = machine->ReadRegister(4);
    filename = User2System(virtAddr, MaxFileLength + 1);
    if (filename == NULL)
    {
        printf("\n Not enough memory in system");
        machine->WriteRegister(2, -1);
        delete filename;
        return;
    }
    if (!fileSystem->Create(filename, 0))
    {
        printf("\n Error create file '%s'", filename);
        machine->WriteRegister(2, -1);
        delete filename;
        return;
    }
    machine->WriteRegister(2, 0);
    delete filename;
}

void openFile() {
    printf("Open File\n");
    int virtAddr = machine->ReadRegister(4); // Lay dia chi cua tham so name tu thanh ghi so 4
    int type = machine->ReadRegister(5); // Lay tham so type tu thanh ghi so 5
    char* filename;
    filename = User2System(virtAddr, MaxFileLength); // Copy chuoi tu vung nho User Space sang System Space voi bo dem name dai MaxFileLength
    //Kiem tra xem OS con mo dc file khong
    // update 4/1/2018
    int freeSlot = fileSystem->FindFreeSlot();
    if (freeSlot != -1)
    {
        if (type == 0 || type == 1)
        {
            if ((fileSystem->openf[freeSlot] = fileSystem->Open(filename, type)) != NULL) 
                machine->WriteRegister(2, freeSlot);
        }
        else if (type == 2) machine->WriteRegister(2, 0); 
        else machine->WriteRegister(2, 1); 
        delete[] filename;
        return;
    }
    machine->WriteRegister(2, -1);
    
    delete[] filename;
}

void closeFile() {
    int fid = machine->ReadRegister(4); // Lay tham so type tu thanh ghi so 4
    if (fid >= 0 && fid <= 10)
    {
        if (fileSystem->openf[fid] != NULL)
        {
            delete fileSystem->openf[fid];
            fileSystem->openf[fid] = NULL;
            machine->WriteRegister(2, 0);
            return;
        }
    }
    machine->WriteRegister(2, -1);
}


void write2File() {
    int virtAddr = machine->ReadRegister(4); // Lay dia chi cua tham so buffer tu thanh ghi so 4
    int size = machine->ReadRegister(5); // Lay tham so size tu thanh ghi so 5
    int fid = machine->ReadRegister(6); // Lay tham so fid tu thanh ghi so 6
    char* buffer;
    buffer = User2System(virtAddr, size); // Copy chuoi tu vung nho User Space sang System Space voi bo dem buffer dai size
    if (fid >= 0 && fid <= 10)
    {
        if (fileSystem->openf[fid] != NULL)
        {
            if (fileSystem->openf[fid]->type == 1)
            {
                if (fileSystem->openf[fid]->Write(buffer, size) > 0)
                {
                    machine->WriteRegister(2, size);
                    delete[] buffer;
                    return;
                }
            }
        }
    }
    machine->WriteRegister(2, -1);
    delete[] buffer;

}

//----------------------------------------------------------------------


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
                case SC_Create:
                    createFile();
                    break;
		case SC_Open:
		    openFile();
		    break;
		case SC_Close:
		    closeFile();
		    break;
		case SC_Write:
		    write2File();
		    break;
		IncreasePC();
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
