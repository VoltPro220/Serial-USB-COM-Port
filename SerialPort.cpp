#include "SerialPort.h"

SerialPort::SerialPort(LPCTSTR portName)
{
}

SerialPort::~SerialPort()
{
}

INT32 SerialPort::ReadPort(const char* buffer, const size_t size)
{
	return INT32();
}

BOOL SerialPort::WritePort(const char* buffer, const size_t size)
{
	return 0;
}

BOOL SerialPort::isConneted()
{
	return 0;
}
