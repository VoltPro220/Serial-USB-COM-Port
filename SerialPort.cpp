#include "SerialPort.h"

SerialPort::SerialPort(std::wstring portName)
{
	this->err = 0;
	this->comStatus = { 0 };
	this->isConnected = FALSE;

	// Создаём и открываем дескриптор ввода/вывода.
	this->serial = CreateFile(portName.c_str(), GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, 0, NULL);

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
		std::wcout << "ERROR #2L: \"\\." << portName.c_str() << "\" I/O device NOT found\n";
	}

	// win32 код ошибки для ERROR_ACCESS_DENIED это DWORD 5L
	// Это значит то, что доступ запрещён, так как оно используется
	// в другой программе.
	else if (errMsg == ERROR_ACCESS_DENIED)
	{
		std::wcout << "ERROR #5L: доступ запрещён: \"\\." << portName.c_str() << "\"";
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
				this->portName = portName;
				PurgeComm(serial, PURGE_RXCLEAR | PURGE_TXCLEAR);
				Sleep(WAIT_TIME);
			}
		}

	}


}

SerialPort::SerialPort()
{
	this->err = 0;
	this->comStatus = { 0 };
	this->isConnected = FALSE;
}

SerialPort::~SerialPort()
{
	if (this->isConnected)
	{
		this->isConnected = FALSE;
		CloseHandle(serial);
	}
}

BOOL SerialPort::connect(std::wstring portName)
{
	if(this->isConnected == TRUE)
		return FALSE;

	this->serial = CreateFileW(portName.c_str(), GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, 0, NULL);

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
		std::wcout << "ERROR #2L: \"\\." << portName.c_str() << "\" I/O device NOT found\n";
	}

	// win32 код ошибки для ERROR_ACCESS_DENIED это DWORD 5L
	// Это значит то, что доступ запрещён, так как оно используется
	// в другой программе.
	else if (errMsg == ERROR_ACCESS_DENIED)
	{
		std::wcout << "ERROR #5L: can`t open: \"\\." << portName.c_str() << "\"";
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
			std::wcout << "ERROR #4L: \"\\." << portName.c_str() << "\" Distribution data could not be obtained\n";
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
				std::wcout << "ERROR #4L: \"\\." << portName.c_str() << "\" Couldn't set parameters\n";
				CloseHandle(serial);
			}
			else
			{
				this->isConnected = TRUE;
				this->portName = portName;
				PurgeComm(serial, PURGE_RXCLEAR | PURGE_TXCLEAR);
				Sleep(WAIT_TIME);
				return TRUE;
			}
		}

	}
	return FALSE;
}

std::wstring SerialPort::readPort(const size_t size)
{
	DWORD bytes;
	size_t toRead = 0;
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
	std::vector<char> buffer(toRead + 1);
	DWORD bytesRead;
	BOOL result = ReadFile(serial, buffer.data(), toRead, &bytesRead, NULL);

	// Конвертация из char в wchar_t
	std::wstring data = L"";
	if (result)
	{
		buffer[bytesRead] = '\0'; // Завершаем строку
		int wideCharCount = MultiByteToWideChar(CP_UTF8, 0, buffer.data(), bytesRead, NULL, 0);
		if (wideCharCount > 0)
		{
			data.resize(wideCharCount);
			MultiByteToWideChar(CP_UTF8, 0, buffer.data(), bytesRead, &data[0], wideCharCount);
		}
	}

	return data;
}

BOOL SerialPort::writePort(const std::wstring& data)
{
	DWORD bytesWritten;
	if (!WriteFile(serial, data.c_str(), data.size(), &bytesWritten, NULL))
	{
		std::wcout << "ERROR #6L: \"\\." << portName.c_str() << "\" Couldn't transmit data\n";
		return FALSE;
	}
	else
	{
		return TRUE;
	}
}

BOOL SerialPort::isConneted() const
{
	return this->isConnected;
}

void SerialPort::disconnect()
{
	if (this->isConnected)
	{
		this->isConnected = FALSE;
		this->portName = L"";
		CloseHandle(serial);
	}
}

std::vector<std::wstring> SerialPort::findAvailableComPorts()
{
	std::vector<std::wstring> comPorts;
	for (int i = 1; i <= 256; i++)
	{
		std::wstring portName = L"COM" + std::to_wstring(i);

		HANDLE hPort = CreateFileW(portName.c_str(), GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, 0, NULL);

		if (hPort != INVALID_HANDLE_VALUE)
		{
			comPorts.push_back(portName);
			CloseHandle(hPort);
		}
	}
	return comPorts;
}

std::wstring SerialPort::getPortName()
{
	return this->portName;
}
