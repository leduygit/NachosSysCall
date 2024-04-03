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

void ReadIntHandler() {
    long long llNumber;
    int nDigit, i, MAX_BUFFER, INT_MIN, INT_MAX, intNumber;
    bool isNegative;
    char* bufer;

    llNumber = 0;
    //intNumber = 0;
    nDigit = 0;
    MAX_BUFFER = 255;
    INT_MIN = -2147483648;
    INT_MAX = 2147483647;
    bufer = new char[MAX_BUFFER+1];

    syncCons->Write("Write an integer number: ", strlen("Write an integer number: ") + 1);
    nDigit = syncCons->Read(bufer, MAX_BUFFER);

    isNegative = (bufer[0] == '-' ? 1 : 0);
    
    // Handle weird character
    for (i = isNegative; i < nDigit; ++i) {
        if (bufer[i] < '0' || bufer[i] > '9') {
            DEBUG('a', "\n The integer number is not valid");
  	    syncCons->Write("Expected integer but ", strlen("Expected integer but ") + 1); 
	    syncCons->Write(bufer, strlen(bufer) + 1);
	    syncCons->Write(" found\n", strlen(" found\n") + 1);
            machine->WriteRegister(2, 0);
            delete[] bufer;
            return;
        }
        llNumber = llNumber * 10 + (bufer[i] - '0');
    }
    llNumber = (isNegative ? llNumber * -1 : llNumber);

    // Handle negation separately to prevent overflow
    if (llNumber < INT_MIN || llNumber > INT_MAX || nDigit - isNegative > 10) {
        DEBUG('a', "\n The integer number is not valid");
	syncCons->Write("Overflow occured!\n", strlen("Overflow occured!\n") + 1);
        machine->WriteRegister(2, 0);
        delete[] bufer;
        return;
    }
    intNumber = llNumber;
    machine->WriteRegister(2, intNumber);
    delete[] bufer;
    IncreasePC();
}


void ReadFloatHandler() {
	float ReadFloatResult;
	char ReadFloatBuffer[255 + 2];
	memset(ReadFloatBuffer, 0, sizeof(ReadFloatBuffer));		
	int ReadFloatLength, i3, integerNumber, dotCount;
	bool isFloat, isNegative;

	syncCons->Write("Write a float number: ", strlen("Write a float number: ") + 1);		
	ReadFloatLength = syncCons->Read(ReadFloatBuffer, 255 + 1);
	dotCount = 0;

	if (ReadFloatLength != 0)
	{
		isFloat = true;
		isNegative = (ReadFloatBuffer[0] == '-');
		for (i3 = isNegative; i3 < ReadFloatLength; ++i3)
		{
			if ((ReadFloatBuffer[i3] < '0' || ReadFloatBuffer[i3] > '9'))
			{
			   if (ReadFloatBuffer[i3] == '.' && dotCount == 0) {
				++dotCount; 
				continue;
			   }
			   syncCons->Write("Expected float but ", strlen("Expected float but ") + 1); 
			   syncCons->Write(ReadFloatBuffer, strlen(ReadFloatBuffer) + 1);
			   syncCons->Write(" found\n", strlen(" found\n") + 1);
			   isFloat = false;
			   break;
			}
		}	
		ReadFloatResult = 0.0f;
		if (isFloat)
		{
			ReadFloatResult = atof(ReadFloatBuffer);
		}

	}
	memcpy(&integerNumber, &ReadFloatResult, sizeof(float));
	machine->WriteRegister(2, integerNumber);
    IncreasePC();
}

void PrintIntHandler() {
    int PrintedNumber;
    char PrintedBuffer[255]; 
    int PrintedIndex = 0;

    PrintedNumber = machine->ReadRegister(4);

    PrintedIndex = sprintf(PrintedBuffer, "%d", PrintedNumber);

    // Ensure null termination of the string
    PrintedBuffer[PrintedIndex] = '\0';

    // Write the string to the console
    syncCons->Write("The integer number you entered is: ", strlen("The integer number you entered is: ") + 1);
    syncCons->Write(PrintedBuffer, PrintedIndex + 1); // +1 to include the null terminator
    IncreasePC();
}

void PrintFloatHandler() {
    float PrintFloatNumber;
    int PrintFloatInteger;
    char PrintFloatBuffer[255];

    PrintFloatInteger = (machine->ReadRegister(4));
    memcpy(&PrintFloatNumber, &PrintFloatInteger, sizeof(float));
    sprintf(PrintFloatBuffer, "%f", PrintFloatNumber);    

    syncCons->Write("The float number you entered is: ", strlen("The float number you entered is: ") + 1);
    syncCons->Write(PrintFloatBuffer, strlen(PrintFloatBuffer) + 1);
    IncreasePC();
}

//----------------------------------------------------------------------
// Function to handle system call

void Alert(char* msg)
{
    printf("\n %s", msg);
    DEBUG('a', "\n %s", msg);
}

void createFile()
{
    int virtAddr;
    char* filename;
    virtAddr = machine->ReadRegister(4);
    filename = User2System(virtAddr, MaxFileLength + 1);
    if (filename == NULL)
    {
        Alert("Not enough memory in system");
        machine->WriteRegister(2, -1);
        delete[] filename;
        return;
    }
    if (!fileSystem->Create(filename, 0))
    {
        // printf("\n Error create file '%s'", filename);
        Alert("Error create file");
        machine->WriteRegister(2, -1);
        delete[] filename;
        return;
    }
    delete[] filename;
    machine->WriteRegister(2, 0);
}

void openFile() {
    int virtAddr = machine->ReadRegister(4);
    int type = machine->ReadRegister(5);
    char* filename;
    filename = User2System(virtAddr, MaxFileLength);
    if (type == 2) {
        machine->WriteRegister(2, 0);
        delete[] filename;	
        return;    
    }
    if (type == 3) {
        machine->WriteRegister(2, 1);
        delete[] filename;	
        return;   
    }
    int freeSlot = fileSystem->FindFreeSlot();
    if (freeSlot != -1) {
        if ((fileSystem->openf[freeSlot] = fileSystem->Open(filename, type)) != NULL) {
          machine->WriteRegister(2, freeSlot);
 	    }
        delete[] filename;
        return;
    }
    
    delete[] filename;
    machine->WriteRegister(2, -1);
}

void closeFile() {
    int fid = machine->ReadRegister(4);
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
    Alert("Error: Can't close file. File not found");
    machine->WriteRegister(2, -1);
}

void readFile() {
    int virtAddr = machine->ReadRegister(4);
    int charcount = machine->ReadRegister(5);
    int id = machine->ReadRegister(6);
    int OldPos;
    int NewPos;
    char *buf;
    if (id < 0 || id > 10)
    {
        Alert("Error: Can't read file. File not found");
        machine->WriteRegister(2, -1);
        return;
    }
    if (fileSystem->openf[id] == NULL)
    {
        Alert("Error: Can't read file. File not found");
        machine->WriteRegister(2, -1);
        return;
    }
    if (fileSystem->openf[id]->type == 3)
    {
        Alert("Error: Can't read file stdout");
        machine->WriteRegister(2, -1);
        return;
    }
    OldPos = fileSystem->openf[id]->GetCurrentPos();
    buf = User2System(virtAddr, charcount);
    if (fileSystem->openf[id]->type == 2) {
        int size = syncCons->Read(buf, charcount); 
        System2User(virtAddr, size, buf);
        machine->WriteRegister(2, size);
        delete[] buf;
        return;
    }
    if ((fileSystem->openf[id]->Read(buf, charcount)) > 0) {
        NewPos = fileSystem->openf[id]->GetCurrentPos();
        System2User(virtAddr, NewPos - OldPos, buf); 
        machine->WriteRegister(2, NewPos - OldPos);
    }
    else
    {
        Alert("Error: Reached end of file");
        machine->WriteRegister(2, -2);
    }
    delete[] buf;
    return;
}

void write2File() {
	int virtAddr = machine->ReadRegister(4);
	int charcount = machine->ReadRegister(5);
	int id = machine->ReadRegister(6);
	int OldPos;
	int NewPos;
	char *buf;
	
	if (id < 0 || id > 10)
	{
        Alert("Error: Can't write file. File not found");
		machine->WriteRegister(2, -1);
		return;
	}
	// Kiem tra file co ton tai khong
	if (fileSystem->openf[id] == NULL)
	{
		Alert("Error: Can't write file. File not found");
		machine->WriteRegister(2, -1);
		return;
	}
	// Xet truong hop ghi file only read (type quy uoc la 1) hoac file stdin (type quy uoc la 2) thi tra ve -1
	if (fileSystem->openf[id]->type == 1 || fileSystem->openf[id]->type == 2)
	{
        Alert("Error: Can't write file stdin or file only read");
		machine->WriteRegister(2, -1);
		return;
	}
	OldPos = fileSystem->openf[id]->GetCurrentPos();
	buf = User2System(virtAddr, charcount);
	if (fileSystem->openf[id]->type == 0)
	{
		if ((fileSystem->openf[id]->Write(buf, charcount)) > 0)
		{
			// So byte thuc su = NewPos - OldPos
			NewPos = fileSystem->openf[id]->GetCurrentPos();
			machine->WriteRegister(2, NewPos - OldPos);
			delete[] buf;
			return;
		}
	}
    if (fileSystem->openf[id]->type == 3) {
        int i = 0;
        while (buf[i] != 0 && buf[i] != '\n')
        {
            syncCons->Write(buf + i, 1);
            i++;
        }
        buf[i] = '\n';
        syncCons->Write(buf + i, 1);
        machine->WriteRegister(2, i - 1);
        delete[] buf;
        return;
	}
	machine->WriteRegister(2, -1);
	
}

//----------------------------------------------------------------------


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
    //IncreasePC();
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
    //IncreasePC();
}

void PrintCharHandler()
{
    char ch;
    ch = (char)machine->ReadRegister(4);
    syncCons->Write(&ch, 1);
    DEBUG('a', "SyscallException: SC_PrintChar\n");
    //IncreasePC();
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
    //IncreasePC();
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
        case SC_ReadInt:
            ReadIntHandler();
            break;
        case SC_ReadFloat:
            ReadFloatHandler();
            break;
        case SC_PrintInt:
            PrintIntHandler();
            break;
        case SC_PrintFloat:
            PrintFloatHandler();
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
	case SC_Read:
	    readFile();
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
    IncreasePC();
}
