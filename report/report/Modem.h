#pragma once

#define STRICT
#define WIN32_LEAN_AND_MEAN
#include <windows.h>

class Modem
{
public:
	HANDLE file;
	COMMTIMEOUTS timeouts;
	DCB port;
	char port_name[128];		// "\\\\.\\COM4";
	char init[128];					// e.g., "ATZ" to completely reset a modem.
	DWORD read, written;

	static void system_error(char *name);
public:
	Modem();
	virtual ~Modem();
};

