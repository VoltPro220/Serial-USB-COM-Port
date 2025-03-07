#include <windows.h>
#include <iostream>
#include <fileapi.h>
#include <errhandlingapi.h>
#include <consoleapi2.h>
#include <processenv.h>
using namespace std;

HANDLE serial;
LPCTSTR name = L"COM3";
DCB sParams;

// data,time,status;

void CursorVisibility(BOOL status)
{
	HANDLE consoleHandle = GetStdHandle(STD_OUTPUT_HANDLE);
	CONSOLE_CURSOR_INFO info{ 100 , status };
	SetConsoleCursorInfo(consoleHandle, &info);
}

void SerialRead()
{
	DWORD inputDataSize;
	char receivedChar;
	while (true)
	{
		BOOL a = ReadFile(serial, &receivedChar, 1, &inputDataSize, 0);
		if (inputDataSize > 0) {
			cout << receivedChar;
		}
	}
}

int main()
{
	CursorVisibility(FALSE);
	cout << ">> begin\n";

	serial = CreateFile(name, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, 0, NULL);
	if (serial == INVALID_HANDLE_VALUE)
	{
		if (GetLastError() == ERROR_FILE_NOT_FOUND)
		{
			cout << ">> serial port does not exist\n";
			CloseHandle(serial);
			return 0;
		}
		cout << ">> some other error occurred\n";
		CloseHandle(serial);
		return 0;
	}

	if (!GetCommState(serial, &sParams))
	{
		cout << ">> getting state error\n";
		CloseHandle(serial);
		return 0;
	}
	sParams.BaudRate = CBR_9600;
	sParams.ByteSize = 8;
	sParams.StopBits = ONESTOPBIT;
	sParams.Parity = NOPARITY;
	if (!SetCommState(serial, &sParams))
	{
		cout << ">> error setting serial port state\n";
		CloseHandle(serial);
		return 0;
	}

	cout << ">> reading\n";
	SerialRead();

	return 0;
}