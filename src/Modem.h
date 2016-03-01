#pragma once

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

class Modem
{
public:
	HANDLE file;
	COMMTIMEOUTS timeouts;
	DCB port;
	char port_name[128];		// "\\\\.\\COM4";
	DWORD read, written;
	bool initialized;

	static void system_error(char *name);
public:
	Modem();
	virtual ~Modem();

	void init();
};

