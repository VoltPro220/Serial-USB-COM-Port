#include <windows.h>
#include <consoleapi2.h>
#include <processenv.h>
#include <conio.h>
#include <thread>
#include "SerialPort.h"

using namespace std;

LPCTSTR namePort = L"COM3";

char datas[INPUT_DATA_BYTES];

void CursorVisibility(BOOL status)
{
	HANDLE consoleHandle = GetStdHandle(STD_OUTPUT_HANDLE);
	CONSOLE_CURSOR_INFO info{ 100 , status };
	SetConsoleCursorInfo(consoleHandle, &info);
}

SerialPort port(namePort);
void task2()
{
	while (port.isConneted())
	{
		if (_kbhit())
		{
			switch (_getch())
			{
				case 97:
					cout << "KEY PRESSED" << endl;
					port.WritePort("a");
					break;
			}
		}
	}
}

int main()
{
	CursorVisibility(FALSE);
	setlocale(LC_ALL, "RU");

	//thread th(task2);

	if (port.isConneted())
	{
		cout << "Подключение к ";
		wcout << namePort << '\n';
		while (port.isConneted())
		{
			cout << port.ReadPort(datas, INPUT_DATA_BYTES);
			if (_kbhit())
			{
				switch (_getch())
				{
					case 97:
						cout << "KEY PRESSED" << endl;
						port.WritePort("a");
						break;
				}
			}
		}
	}
	//th.join();
	return 0;
}