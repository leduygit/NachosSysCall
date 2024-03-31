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
                case SC_ReadChar:
                    int maxBytes, readBytes;
                    char* buffer;
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
                    DEBUG('a', "SyscallException: SC_ReadChar\n");
                    IncreasePC();
                    break;
                case SC_PrintChar:
                    char ch;
                    ch = (char)machine->ReadRegister(4);
                    syncCons->Write(&ch, 1);
                    DEBUG('a', "SyscallException: SC_PrintChar\n");
                    IncreasePC();
                    break;
                default:
                    IncreasePC();
                    break;
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