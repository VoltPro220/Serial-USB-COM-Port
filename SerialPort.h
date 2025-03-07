#pragma once

#ifndef _SERIALPORT_H_
#define _SERIALPORT_H_

#define WAIT_TIME 2000
#define INPUT_DATA_BYTES 10

#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <basetsd.h>

class SerialPort
{
private:
	HANDLE serial;
	BOOL isConnected;
	COMSTAT comStatus;
	DWORD err;
public:
	SerialPort(LPCTSTR portName);
	~SerialPort();

	INT32 ReadPort(const char* buffer, const size_t size);
	BOOL WritePort(const char* buffer, const size_t size);
	BOOL isConneted();
};


#endif // !_SERIALPORT_H