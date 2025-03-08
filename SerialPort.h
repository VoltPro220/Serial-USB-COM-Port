#pragma once

#ifndef _SERIALPORT_H_
#define _SERIALPORT_H_

// Waiting after connection so that the handle
// has time to stabilize in a given time
#define WAIT_TIME 3000 

// Number of bytes received (Can be changed)
#define INPUT_DATA_BYTES 10

// ************ Connected Port Settings **********
#define CBR_BAUD CBR_9600
#define BYTESIZE 8
#define STOPBITS ONESTOPBIT
#define PARITY NOPARITY

// **** The settings are set by default, you don't have to change them. ****

// fields of the DCB structure, which sets the exchange control mode for the DTR signal. 
// Optional parameter, but if you need to change it, then uncomment the definition and 
// set your own parameter (Read the documentation)

//#define SETFDTRCONTROL
#define FDTRCONTROL DTR_CONTROL_ENABLE

// Error codes when setting the settings of the connected descriptor
#define PARAMS_COULDNT_SET 3L
#define PARAMS_COULDNT_FOUND 4L
#define COULD_NOT_WRITE 6L

#include <windows.h>
#include <iostream>
#include <string>
#include <vector>

class SerialPort
{
private:

	// Device Descriptor
	HANDLE serial;
	// true - connection is successful
	// false - connection is not successful
	BOOL isConnected;
	// Contains a lot of data about the status of the communication channel
	// It is useful for setting parameters.
	COMSTAT comStatus;
	// Error number, in case of an error
	DWORD err;
	// The name of port, need for return when calling method getPortName();
	std::wstring portName;

	void connectToComPort(std::wstring portName);

public:
	
	SerialPort(std::wstring portName);
	SerialPort();
	~SerialPort();
	void connect(std::wstring portName);

	std::wstring readPort(const size_t size);
	BOOL writePort(const std::wstring& data);
	BOOL isConneted() const;
	void disconnect();
	std::vector<std::wstring> findAvailableComPorts();
	std::wstring getPortName();
};


#endif // !_SERIALPORT_H