
// https://stackoverflow.com/questions/15439740/segmentation-fault-on-enummonitors-winapi

#include "pch.h"

#include <exception>        // for std::exception
#include <iostream>         // for std::wcout, std::wcerr, std::endl
#include <sstream>          // for std::ostringstream
#include <stdexcept>        // for std::runtime_error
#include <vector>           // for std::vector
#include <Windows.h>        // Win32 SDK main header
#include <Winspool.h>       // for EnumMonitors()
#include <string>

using namespace std;

void ThrowOnApiFailure(const char* apiName, DWORD errorCode)
{
	ostringstream errorMessage;
	errorMessage << apiName << "() failed with error code " << errorCode;
	throw runtime_error(errorMessage.str());
}

void PrintMT()
{
	static const int kMonitorInfoLevel = 2; // for MONITOR_INFO_2

	// Ask output buffer size
	DWORD bufferSize = 0;
	DWORD infoCount = 0;
	EnumMonitors(NULL, kMonitorInfoLevel, NULL, 0, &bufferSize, &infoCount);

	DWORD error = GetLastError();
	if (error != ERROR_INSUFFICIENT_BUFFER)
	{
		ThrowOnApiFailure("EnumMonitors", error);
	}

	// Size output buffer
	vector<BYTE> buffer(bufferSize);

	// Fill buffer with monitor info
	if (!EnumMonitors(NULL, kMonitorInfoLevel, buffer.data(), buffer.size(), &bufferSize, &infoCount))
	{
		error = GetLastError();
		ThrowOnApiFailure("EnumMonitors", error);
	}

	cout << "\n[Monitors list]" << endl;

	// Print monitor info
	const MONITOR_INFO_2 * monitorInfo = reinterpret_cast<const MONITOR_INFO_2 *>(buffer.data());
	for (DWORD i = 0; i < infoCount; i++)
	{
		wcout << monitorInfo[i].pName << endl;
	}
	cout << "\n총 " << infoCount << "개" << endl;
}

void AddMT() {
	DWORD result;
	wstring info_name = L"";
	wstring info_env = L"Windows x64";
	wstring info_dll = L"";
	DWORD error = GetLastError();

	cout << "\nMonitor name : ";
	wcin >> info_name;
	cout << "DLL name : ";
	wcin >> info_dll;

	MONITOR_INFO_2 info;
	info.pName = &info_name[0];
	info.pEnvironment = NULL;
	info.pDLLName = &info_dll[0];

	result = AddMonitor(NULL, 2, (LPBYTE)&info);

	if (result) {
		cout << "Monitor added successfully." << endl;
	}
	else {
		error = GetLastError();
		cout << "Failed to add monitor. --> " << error << endl;
	}
}

void DeleteMT()
{
	static const int kMonitorInfoLevel = 2;

	// Ask output buffer size
	DWORD bufferSize = 0;
	DWORD infoCount = 0;
	EnumMonitors(NULL, kMonitorInfoLevel, NULL, 0, &bufferSize, &infoCount);

	DWORD error = GetLastError();
	if (error != ERROR_INSUFFICIENT_BUFFER)
	{
		ThrowOnApiFailure("EnumMonitors", error);
	}

	vector<BYTE> buffer(bufferSize);

	// Fill buffer with monitor info
	if (!EnumMonitors(NULL, kMonitorInfoLevel, buffer.data(), buffer.size(), &bufferSize, &infoCount))
	{
		error = GetLastError();
		ThrowOnApiFailure("EnumMonitors", error);
	}

	cout << "\n[Installed monitors.]" << endl;
	const MONITOR_INFO_2 * monitorInfo = reinterpret_cast<const MONITOR_INFO_2 *>(buffer.data());
	for (DWORD i = 0; i < infoCount; i++)
	{
		wcout << i << ". " << monitorInfo[i].pName << endl;
	}

	DWORD sel;
	cout << "\nEnter the monitor number to be deleted." << endl << ">> ";
	cin >> sel;

	if (sel == -1) {
		return;
	}

	//	Delete Monitor
	wstring info_env = L"Windows x64";
	if (DeleteMonitor(NULL, &info_env[0], monitorInfo[sel].pName)) {
		cout << "Successfully removed monitor." << endl;
	}
	else {
		cout << "Failed to remove monitor." << endl;
	}
}

int main()
{
	DWORD command = 0;

	while (true) {
		cout << "[Command]" << endl;
		cout << "0. Exit Program" << endl;
		cout << "1. Print Monitors" << endl;
		cout << "2. Add Monitor" << endl;
		cout << "3. Delete Monitor" << endl;
		cout << ">> ";
		cin >> command;

		if (command == 0) {
			return 0;
		}

		else if (command == 1) {
			try
			{
				PrintMT();
			}
			catch (const exception& e)
			{
				wcerr << "\n*** ERROR: " << e.what() << endl;
			}
		}

		else if (command == 2) {
			AddMT();
		}

		else if (command == 3) {
			DeleteMT();
		}

		else {
			cout << "You entered the command incorrectly." << endl;
		}

		cout << endl;
		system("pause");
		system("cls");
	}

	
}