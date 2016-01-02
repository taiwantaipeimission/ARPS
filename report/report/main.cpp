#include <string.h>
#include <iostream>

#define STRICT
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <conio.h>
#include <stdio.h>

#include "Modem.h"

int main(int argc, char **argv) {

	
	char buffer[1];


	DWORD mode;
	DWORD read, written;


	Modem modem;

	HANDLE screen = GetStdHandle(STD_OUTPUT_HANDLE);

	char commands[] =	"AT+CPIN=\"0120\"\nAT + CMGS = \"+886910358944\"\nTHIS IS A MESSAGE.\u001A";

	// basic terminal loop:

	char ch;
	int command_index = 0;
	do
	{
		ReadFile(modem.file, buffer, sizeof(buffer), &read, NULL);
		if (read)
			WriteFile(screen, buffer, read, &written, NULL);

		ch = commands[command_index];
		if (ch != '\0')
		{
			if (ch == '\n')
				WriteFile(modem.file, "\r\n", 2, &written, NULL);
			else
				WriteFile(modem.file, &ch, 1, &written, NULL);
			//WriteFile(screen, &ch, 1, &written, NULL);
			command_index++;
		}

		/*if (_kbhit()) {
			ch = _getch();
			WriteFile(modem.file, &ch, 1, &written, NULL);
		}*/
		
	} while ( 1 );


	/*]
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
		// until user hits escape.
	} while (ch != 27);*/

	// close up and go home.
	return 0;
}