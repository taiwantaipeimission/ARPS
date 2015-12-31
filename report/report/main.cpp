#include <stdio.h>
#include <conio.h>
#include <string.h>

#define STRICT
#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include "Modem.h"


int main(int argc, char **argv) {

	int ch;
	char buffer[1];
	
	HANDLE keyboard = GetStdHandle(STD_INPUT_HANDLE);
	HANDLE screen = GetStdHandle(STD_OUTPUT_HANDLE);
	DWORD mode;
	DWORD read, written;

	Modem modem;

	if (!GetConsoleMode(keyboard, &mode))
		Modem::system_error("getting keyboard mode");
	mode &= ~ENABLE_PROCESSED_INPUT;
	if (!SetConsoleMode(keyboard, mode))
		Modem::system_error("setting keyboard mode");

	// basic terminal loop:
	do {
		// check for data on port and display it on screen.
		ReadFile(modem.file, buffer, sizeof(buffer), &read, NULL);
		if (read)
			WriteFile(screen, buffer, read, &written, NULL);

		// check for keypress, and write any out the port.
		if (_kbhit()) {
			ch = _getch();
			WriteFile(modem.file, &ch, 1, &written, NULL);
			WriteFile(screen, &ch, 1, &written, NULL);
		}
		// until user hits ctrl-backspace.
	} while (ch != 127);

	// close up and go home.
	CloseHandle(keyboard);
	return 0;
}