#include "SerialPort.h"

void SerialPort::connectToComPort(std::wstring portName)
{
	this->err = 0;
	this->comStatus = { 0 };
	this->isConnected = FALSE;

	// Creating and opening an input/output descriptor.
	this->serial = CreateFileW(portName.c_str(), GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, 0, NULL);

	// ***************** HANDLE errors *******************
	// Use the Win32 API function to get the latest error.
	// It returns an integer error code. There
	// it contains about 16,000 error codes, we are only interested in two
	// (is the device connected? and is it already being used by another application? ).

	DWORD errMsg = GetLastError();

	// the win32 error code for ERROR_FILE_NOT_FOUND is DWORD 2L
	// Indicates that the device is not connected or has not been found.
	if (errMsg == ERROR_FILE_NOT_FOUND)
	{
		std::wcout << "ERROR #2L: \"\\." << portName.c_str() << "\" I/O device NOT found\n";
	}

	// the win32 error code for ERROR_ACCESS_DENIED is DWORD 5L
	// This means that access is denied because it is being used.
	// in another program.
	else if (errMsg == ERROR_ACCESS_DENIED)
	{
		std::wcout << "ERROR #5L: can`t open: \"\\." << portName.c_str() << "\"";
	}

	// ****************** COM Port Configuration ****************************
	// If everything is fine, it configures the serial COM port.
	else if (errMsg == 0)
	{
		// DCB is a "Device Control Block", a structure that stores settings
		// distribution kit. Setting the COM port parameters. (You can change them)
		// In the SerialPort.h header file in the definitions
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
#ifdef SETFDTRCONTROL
			dcbSerialParameters.fDtrControl = FDTRCONTROL;
#endif

			// Set the parameters, if this fails, an error appears.
			if (!GetCommState(serial, &dcbSerialParameters))
			{
				std::wcout << "ERROR #3L: \"\\." << portName.c_str() << "\" Couldn't set parameters\n";
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

SerialPort::SerialPort(std::wstring portName)
{
	this->connectToComPort(portName);
}

SerialPort::SerialPort()
{
	this->err = 0;
	this->comStatus = { 0 };
	this->isConnected = FALSE;
}

SerialPort::~SerialPort()
{
	this->disconnect();
}

void SerialPort::connect(std::wstring portName)
{
	if(this->isConnected == TRUE)
		return;
	this->connectToComPort(portName);
}

std::wstring SerialPort::readPort(const size_t size)
{
	size_t toRead = 0;
	ClearCommError(serial, &err, &comStatus);

	// Determine how many bytes to read in the subsequent ReadFile() method.
	// cbinque is the number of bytes received in the serial port, but
	// not yet read by the ReadFile operation.
	// Prepare to read any available bytes, but do not exceed
	// the requested number of bytes
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

	// Reads the requested data ("for reading") bytes to the "buffer"
	// and returns the number of bytes actually read
	std::vector<char> buffer(toRead + 1);
	DWORD bytesRead;
	BOOL result = ReadFile(serial, buffer.data(), toRead, &bytesRead, NULL);

	// Conversion from char to wchar_t
	std::wstring data = L"";
	if (result)
	{
		buffer[bytesRead] = '\0';
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
