#include <windows.h>
#include <consoleapi2.h>
#include <processenv.h>
#include <string>
#include "SerialPort.h"

using namespace std;

void CursorVisibility(BOOL status)
{
	HANDLE consoleHandle = GetStdHandle(STD_OUTPUT_HANDLE);
	CONSOLE_CURSOR_INFO info{ 100 , status };
	SetConsoleCursorInfo(consoleHandle, &info);
}

SerialPort port(L"COM3");

int main()
{
	CursorVisibility(FALSE);
	setlocale(LC_ALL, "RU");


	if (port.isConneted())
	{
		cout << "Подключение к ";
		wcout << port.getPortName().c_str() << '\n';
		bool st = 0;
		while (port.isConneted())
		{
			wcout << port.readPort(INPUT_DATA_BYTES).c_str();
		}
	}

	return 0;
}