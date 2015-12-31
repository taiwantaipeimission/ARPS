#include <stdio.h>
#include <conio.h>
#include <string.h>

#define STRICT
#define WIN32_LEAN_AND_MEAN
#include <windows.h>

void system_error(char *name) {
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
		(char *)&ptr,
		1024,
		NULL);

	fprintf(stderr, "\nError %s: %s\n", name, ptr);
	LocalFree(ptr);
}

int main(int argc, char **argv) {

	int ch;
	char buffer[1];
	HANDLE file;
	COMMTIMEOUTS timeouts;
	DWORD read, written;
	DCB port;
	HANDLE keyboard = GetStdHandle(STD_INPUT_HANDLE);
	HANDLE screen = GetStdHandle(STD_OUTPUT_HANDLE);
	DWORD mode;
	char port_name[128] = "COM4";// "\\\\.\\COM4";
	char init[] = "AT"; // e.g., "ATZ" to completely reset a modem.

	// open the comm port.

	file = CreateFile(
		"\\\\.\\COM4",     // address of name of the communications device
		GENERIC_READ | GENERIC_WRITE,         // access (read-write) mode
		0,                  // share mode
		NULL,               // address of security descriptor
		OPEN_EXISTING,      // how to create
		0,                  // file attributes
		NULL                // handle of file with attributes to copy
		);

	if (INVALID_HANDLE_VALUE == file) {
		system_error("opening file");
		return 1;
	}

	// get the current DCB, and adjust a few bits to our liking.
	memset(&port, 0, sizeof(port));
	port.DCBlength = sizeof(port);
	if (!GetCommState(file, &port))
		system_error("getting comm state");
	if (!BuildCommDCB("baud=9600 parity=n data=8 stop=1", &port))
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
	if (!GetConsoleMode(keyboard, &mode))
		system_error("getting keyboard mode");
	mode &= ~ENABLE_PROCESSED_INPUT;
	if (!SetConsoleMode(keyboard, mode))
		system_error("setting keyboard mode");

	if (!EscapeCommFunction(file, CLRDTR))
		system_error("clearing DTR");
	Sleep(200);
	if (!EscapeCommFunction(file, SETDTR))
		system_error("setting DTR");

	if (!WriteFile(file, init, sizeof(init), &written, NULL))
		system_error("writing data to port");

	if (written != sizeof(init))
		system_error("not all data written to port");

	// basic terminal loop:
	do {
		// check for data on port and display it on screen.
		ReadFile(file, buffer, sizeof(buffer), &read, NULL);
		if (read)
			WriteFile(screen, buffer, read, &written, NULL);

		// check for keypress, and write any out the port.
		if (_kbhit()) {
			ch = _getch();
			WriteFile(file, &ch, 1, &written, NULL);
			WriteFile(screen, &ch, 1, &written, NULL);
		}
		// until user hits ctrl-backspace.
	} while (ch != 127);

	// close up and go home.
	CloseHandle(keyboard);
	CloseHandle(file);
	return 0;
}