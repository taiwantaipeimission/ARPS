#include <iostream>

#define STRICT
#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include "Modem.h"

void Modem::system_error(char *name) {
	// Retrieve, format, and print out a message from the last error.  The 
	// `name' that's passed should be in the form of a present tense noun 
	// (phrase) such as "opening file".
	//
	char *ptr = NULL;
	FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER |
		FORMAT_MESSAGE_FROM_SYSTEM,
		0,
		GetLastError(),
		0,
		(LPWSTR)&ptr,
		1024,
		NULL);

	std::cout<<"\nError "<< name << ptr <<std::endl;
	LocalFree(ptr);
}

Modem::Modem()
 : initialized(false)
{
}


Modem::~Modem()
{
	if (initialized)
		CloseHandle(file);
}

void Modem::init()
{
	initialized = true;
	// open the comm port.
	file = CreateFile(
		L"\\\\.\\COM1",     // address of name of the communications device
		GENERIC_READ | GENERIC_WRITE,         // access (read-write) mode
		0,                  // share mode
		NULL,               // address of security descriptor
		OPEN_EXISTING,      // how to create
		0,                  // file attributes
		NULL                // handle of file with attributes to copy
		);

	if (INVALID_HANDLE_VALUE == file) {
		system_error("opening file");
		return;
	}

	// get the current DCB, and adjust a few bits to our liking.
	memset(&port, 0, sizeof(port));
	port.DCBlength = sizeof(port);
	port.fDtrControl = DTR_CONTROL_HANDSHAKE;
	if (!GetCommState(file, &port))
		system_error("getting comm state");
	if (!BuildCommDCB(L"baud=9600 parity=n data=8 stop=1", &port))
		system_error("building comm DCB");
	if (!SetCommState(file, &port))
		system_error("adjusting port settings");

	// set short timeouts on the comm port.
	timeouts.ReadIntervalTimeout = 1;
	timeouts.ReadTotalTimeoutMultiplier = 1;
	timeouts.ReadTotalTimeoutConstant = 1;
	timeouts.WriteTotalTimeoutMultiplier = 1;
	timeouts.WriteTotalTimeoutConstant = 1;
	if (!SetCommTimeouts(file, &timeouts))
		system_error("setting port time-outs.");

	// set keyboard to raw reading.


	if (!EscapeCommFunction(file, CLRDTR))
		system_error("clearing DTR");
	Sleep(200);
	if (!EscapeCommFunction(file, SETDTR))
		system_error("setting DTR");
}