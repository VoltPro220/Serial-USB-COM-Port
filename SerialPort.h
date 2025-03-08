#pragma once

#ifndef _SERIALPORT_H_
#define _SERIALPORT_H_

#define WAIT_TIME 3000 // Ожидание после подключения, чтобы дескриптор
// успел за заданное время стабилизироваться

#define INPUT_DATA_BYTES 10 // Кол-во принимаемых байтов (Можно менять)

// ****** Настройки подлкюченного порта **********
#define CBR_BAUD CBR_9600
#define BYTESIZE 8
#define STOPBITS ONESTOPBIT
#define PARITY NOPARITY
#define FDTRCONTROL DTR_CONTROL_ENABLE
// **** Настройки стоят по умолчанию, их менять не обязательно ****

// Коды ошибок при установки настроек подключенного дескриптора
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
	// Дескриптор об устройстве
	HANDLE serial;
	// true - подключение успешное
	// false - не успешное подключение
	BOOL isConnected;
	// Содержит много данных о состоянии канала связи
	COMSTAT comStatus;
	// Номер ошибки, в случаи возникновения ошибки
	DWORD err;
	
	std::wstring portName;

public:
	SerialPort(std::wstring portName);
	SerialPort();
	~SerialPort();
	BOOL connect(std::wstring portName);

	std::wstring readPort(const size_t size);
	BOOL writePort(const std::wstring& data);
	BOOL isConneted() const;
	VOID disconnect();
	std::vector<std::wstring> findAvailableComPorts();
	std::wstring getPortName();
};


#endif // !_SERIALPORT_H