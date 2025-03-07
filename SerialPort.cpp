#include "SerialPort.h"

SerialPort::SerialPort(LPCTSTR portName)
{
	this->err = 0;
	this->comStatus = { 0 };
	this->isConnected = FALSE;

	// ������ � ��������� ���������� �����/������.
	this->serial = CreateFile(portName, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, 0, NULL);

	// ***************** HANDLE errors *******************
	// ����������� ������� Win32 API ��� ��������� ��������� ������. 
	// ��� ���������� ������������� ��� ������. ���
	// �������� ����� 16 000 ����� ������, ��� ���������� ������ ��� 
	// (���������� �� ����������? � ������������ �� ��� ��� ������ ����������� ? ).

	DWORD errMsg = GetLastError();

	// win32 ��� ������ ��� ERROR_FILE_NOT_FOUND ��� DWORD 2L
	// ������� � ���, ��� ���������� �� ���������� ��� �� �������
	if (errMsg == ERROR_FILE_NOT_FOUND)
	{
		std::cout << "ERROR #2L: \"\\." << portName << "\" I/O device NOT found\n";
	}

	// win32 ��� ������ ��� ERROR_ACCESS_DENIED ��� DWORD 5L
	// ��� ������ ��, ��� ������ ��������, ��� ��� ��� ������������
	// � ������ ���������.
	else if (errMsg == ERROR_ACCESS_DENIED)
	{
		std::cout << "ERROR #5L: ������ ��������: \"\\." << portName << "\"";
	}

	// ****************** ������������ COM ����� ****************************
	// ���� �� ������, �� ������������� ���������������� COM ����
	else if (errMsg == 0)
	{
		// DCB ��� "Divece Control Block" - ��������, ������� ������ ���������
		// ������������. ������������� ��������� COM �����. (�������� �� �����)
		// � ������������ ����� SerialPort.h � ������������
		DCB dcbSerialParameters = { 0 };
		if (!GetCommState(serial, &dcbSerialParameters))
		{
			printf("ERROR #4L: ������ ��� ��������� ���������� ������������");
		}
		else
		{
			dcbSerialParameters.BaudRate = CBR_BAUD;
			dcbSerialParameters.ByteSize = BYTESIZE;
			dcbSerialParameters.StopBits = STOPBITS;
			dcbSerialParameters.Parity = PARITY;
			//dcbSerialParameters.fDtrControl = FDTRCONTROL;

			// ������������� ���������, ���� ��� �� �������, ���������� ������
			if (!GetCommState(serial, &dcbSerialParameters))
			{
				printf("ERROR #3L: ��������� �� ���� ����������� ����������������� COM �����");
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

	// ����������, ������� ���� ����� ��������� � ����������� ������ ReadFile().
	//
	// cdInQue - ��� ���������� ����, ���������� � ���������������� �����, ��
	// ��� �� ��������� ��������� ReadFile.
	// ������������� � ������ ����� ��������� ����, �� �� ���������� 
	// ����������� ���������� ����
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

	// ��������� ����������� ("��� ������") ����� � "�����" 
	// � ���������� ���������� ���� ���������� �����������
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
