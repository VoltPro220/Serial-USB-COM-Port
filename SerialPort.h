#pragma once

#ifndef _SERIALPORT_H_
#define _SERIALPORT_H_

#define WAIT_TIME 3000 // �������� ����� �����������, ����� ����������
// ����� �� �������� ����� �����������������

#define INPUT_DATA_BYTES 10 // ���-�� ����������� ������ (����� ������)

// ****** ��������� ������������� ����� **********
#define CBR_BAUD CBR_9600
#define BYTESIZE 8
#define STOPBITS ONESTOPBIT
#define PARITY NOPARITY
#define FDTRCONTROL DTR_CONTROL_ENABLE
// **** ��������� ����� �� ���������, �� ������ �� ����������� ****

// ���� ������ ��� ��������� �������� ������������� �����������
#define PARAMS_COULDNT_SET 3L
#define PARAMS_COULDNT_FOUND 4L

#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <basetsd.h>
#include <cstdio>
#include <iostream>

class SerialPort
{
private:
	// ���������� �� ����������
	HANDLE serial;
	// true - ����������� ��������
	// false - �� �������� �����������
	BOOL isConnected;
	// �������� ����� ������ � ��������� ������ �����
	COMSTAT comStatus;
	// ����� ������, � ������ ������������� ������
	DWORD err;

	DWORD outBytesWritten;
public:
	SerialPort(LPCTSTR portName);
	~SerialPort();
	 
	char ReadPort(char* buffer, const size_t size);
	BOOL WritePort(const std::string& data);
	BOOL isConneted() const;
	void disconnect();
};


#endif // !_SERIALPORT_H