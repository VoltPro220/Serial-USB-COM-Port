#include "SerialPort.h"

SerialPort::SerialPort(LPCTSTR portName)
{
	this->err = 0;
	this->comStatus = { 0 };
	this->isConnected = FALSE;

	// Создаём и открываем дескриптор ввода/вывода.
	this->serial = CreateFile(portName, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, 0, NULL);

	// ***************** HANDLE errors *******************
	// Используйте функцию Win32 API для получения последней ошибки. 
	// Она возвращает целочисленный код ошибки. Там
	// содержит около 16 000 кодов ошибок, нас интересуют только два 
	// (подключено ли устройство? и используется ли оно уже другим приложением ? ).

	DWORD errMsg = GetLastError();

	// win32 код ошибки для ERROR_FILE_NOT_FOUND это DWORD 2L
	// Говорит о том, что устройство не подключено или не найдено
	if (errMsg == ERROR_FILE_NOT_FOUND)
	{
		std::cout << "ERROR #2L: \"\\." << portName << "\" I/O device NOT found\n";
	}

	// win32 код ошибки для ERROR_ACCESS_DENIED это DWORD 5L
	// Это значит то, что доступ запрещён, так как оно используется
	// в другой программе.
	else if (errMsg == ERROR_ACCESS_DENIED)
	{
		std::cout << "ERROR #5L: доступ запрещён: \"\\." << portName << "\"";
	}

	// ****************** Конфигурация COM порта ****************************
	// Если всё хорошо, то конфигурирует последовательный COM порт
	else if (errMsg == 0)
	{
		// DCB это "Divece Control Block" - стуктура, которая хранит настройки
		// дестрибутива. Устанавливаем параметры COM порта. (Поменять их можно)
		// В заголовочном файле SerialPort.h в определениях
		DCB dcbSerialParameters = { 0 };
		if (!GetCommState(serial, &dcbSerialParameters))
		{
			printf("ERROR #4L: ошибка при получении параметров дестрибутива");
		}
		else
		{
			dcbSerialParameters.BaudRate = CBR_BAUD;
			dcbSerialParameters.ByteSize = BYTESIZE;
			dcbSerialParameters.StopBits = STOPBITS;
			dcbSerialParameters.Parity = PARITY;
			//dcbSerialParameters.fDtrControl = FDTRCONTROL;

			// Устанавливаем параметры, если это не удалось, появляется ошибка
			if (!GetCommState(serial, &dcbSerialParameters))
			{
				printf("ERROR #3L: Параметры не были установлены последовательному COM порту");
				CloseHandle(serial);
			}
			else
			{
				this->isConnected = TRUE;
				PurgeComm(serial, PURGE_RXCLEAR | PURGE_TXCLEAR);
				Sleep(WAIT_TIME);
			}
		}

	}


}

SerialPort::~SerialPort()
{
	if (this->isConnected)
	{
		this->isConnected = FALSE;
		CloseHandle(serial);
	}
}

char SerialPort::ReadPort(char* buffer, const size_t size)
{
	DWORD bytes;
	size_t toRead = 0;
	char d = 0;
	ClearCommError(serial, &err, &comStatus);

	// Определите, сколько байт нужно прочитать в последующем методе ReadFile().
	//
	// cdInQue - это количество байт, полученных в последовательном порту, но
	// еще не считанных операцией ReadFile.
	// Подготовьтесь к чтению любых доступных байт, но не превышайте 
	// запрошенное количество байт
	if (comStatus.cbInQue > 0)
	{
		if (comStatus.cbInQue > size)
		{
			toRead = size;
		}
		else
		{
			toRead = comStatus.cbInQue;
		}
	}

	// Считывает запрошенные ("для чтения") байты в "буфер" 
	// и возвращает количество байт фактически прочитанных
	if (ReadFile(serial, &d, 1, &bytes, NULL))
	{
		return d;
	}
	return 0;
}

BOOL SerialPort::WritePort(const std::string& data)
{
	DWORD bytesWritten;
	if (!WriteFile(serial, data.c_str(), data.size(), &bytesWritten, NULL))
	{
		std::cerr << "Error writing to port" << std::endl;
		return FALSE;
	}
	else
	{
		std::cout << "Sent " << bytesWritten << " bytes." << std::endl;
		return TRUE;
	}
}

BOOL SerialPort::isConneted() const
{
	return this->isConnected;
}
