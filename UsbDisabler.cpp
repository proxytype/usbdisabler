#include "stdafx.h"
#include <windows.h>
#include <setupapi.h>
#include <devguid.h>
#include <devpkey.h>
#include <iostream>
#include <codecvt>

#pragma comment(lib,"Setupapi.lib")
using namespace std;

const DWORD MAX_DESCRIPTION = 2046;

HANDLE hConsole = NULL;
WORD attributes = 0;


void printError(const char * error, int exitcode) {

    SetConsoleTextAttribute(hConsole,
        FOREGROUND_RED | FOREGROUND_RED | FOREGROUND_INTENSITY);

    printf("%s\n", error);

    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), attributes);

    exit(exitcode);
}

std::wstring stringToWstring(const char* utf8Bytes)
{
    using convert_type = std::codecvt_utf8<typename std::wstring::value_type>;
    std::wstring_convert<convert_type, typename std::wstring::value_type> converter;
    return converter.from_bytes(utf8Bytes);
}

std::wstring getData(HDEVINFO hDevInfo, SP_DEVINFO_DATA deviceInfoData, DWORD SPDRP) {

    DWORD dataT;
    char value[MAX_DESCRIPTION] = { 0 };
    DWORD buffersize = MAX_DESCRIPTION;
    DWORD req_bufsize = 0;

    if (!SetupDiGetDeviceRegistryPropertyA(hDevInfo, &deviceInfoData, SPDRP, &dataT, (PBYTE)value, buffersize, &req_bufsize))
    {
        return NULL;
    }

    return stringToWstring(value);
}


bool changeDeviceStatus(HDEVINFO hDevInfo, SP_DEVINFO_DATA DeviceInfoData, bool enable) {

    SP_PROPCHANGE_PARAMS PropChangeParams = { sizeof(SP_CLASSINSTALL_HEADER) };
    PropChangeParams.ClassInstallHeader.InstallFunction = DIF_PROPERTYCHANGE;
    DWORD state = 1;
    if (!enable) {
        state = 2;
    }

    PropChangeParams.Scope = DICS_FLAG_GLOBAL;
    PropChangeParams.StateChange = state;


    if (!SetupDiSetClassInstallParams(hDevInfo, &DeviceInfoData, (SP_CLASSINSTALL_HEADER *)&PropChangeParams, sizeof(PropChangeParams)))
    {
        return false;
    } 

    if (!SetupDiCallClassInstaller(
        DIF_PROPERTYCHANGE,
        hDevInfo,
        &DeviceInfoData))
    {
        return true;
    }

    return true;
}


void showOptions() {

    printf(" - usbdisabler.exe <option> <arg> \n");
    printf("   Options:\n");
    printf("    -l               :: Display device list\n");
    printf("    -d <device guid> :: Disable specific device port\n");
}


void showHeader() {
    SetConsoleTextAttribute(hConsole,
        FOREGROUND_BLUE | FOREGROUND_BLUE | FOREGROUND_INTENSITY);

    printf("   __  _______ ____  ____  _            __    __         \n");
    printf("  / / / / ___// __ )/ __ \\(_)________ _/ /_  / /__  _____\n");
    printf(" / / / /\\__ \\/ __  / / / / / ___/ __ `/ __ \\/ / _ \\/ ___/\n");
    printf("/ /_/ /___/ / /_/ / /_/ / (__  ) /_/ / /_/ / /  __/ /    \n");
    printf("\\____//____/_____/_____/_/____/\\__,_/_.___/_/\\___/_/     \n");
    printf("----------------------------------------------------------\n");
    printf("[USBDisabler] by: RudeNetworks.com | version: 0.1 beta\n");
    printf(" - administrator privileges required.\n\n");
}

void displayDevices() {


    wprintf(L"USB Devices ID:\n");

    DEVPROPTYPE devPropType;
    SP_DEVINFO_DATA DeviceInfoData = { sizeof(DeviceInfoData) };

    HDEVINFO hDevInfo = SetupDiGetClassDevs(
        &GUID_DEVCLASS_USB,
        NULL,
        NULL,
        DIGCF_PRESENT);

    if (hDevInfo == INVALID_HANDLE_VALUE)
    {
        printError("Invalid handle value", -2);
    }

    for (int i = 0; SetupDiEnumDeviceInfo(hDevInfo, i, &DeviceInfoData); i++)
    {

        wstring description = getData(hDevInfo, DeviceInfoData, SPDRP_DEVICEDESC);
        wstring hardwareid = getData(hDevInfo, DeviceInfoData, SPDRP_HARDWAREID);
        wprintf(L"-> %s :: %s\n", hardwareid.c_str(), description.c_str());
    }
}

bool disableDevice(char * hardwareID) {

    DEVPROPTYPE devPropType;
    SP_DEVINFO_DATA DeviceInfoData = { sizeof(DeviceInfoData) };
    wstring argHardwareID = stringToWstring(hardwareID);

    wprintf(L"Looking for: %s\n", argHardwareID.c_str());

    HDEVINFO hDevInfo = SetupDiGetClassDevs(
        &GUID_DEVCLASS_USB,
        NULL,
        NULL,
        DIGCF_PRESENT);

    if (hDevInfo == INVALID_HANDLE_VALUE)
    {
        printError("Invalid handle value", -2);
    }

    for (int i = 0; SetupDiEnumDeviceInfo(hDevInfo, i, &DeviceInfoData); i++)
    {
        wstring hardwareID = getData(hDevInfo, DeviceInfoData, SPDRP_HARDWAREID);
        if(wcsstr(hardwareID.c_str(), argHardwareID.c_str()) != NULL) {
            wstring description = getData(hDevInfo, DeviceInfoData, SPDRP_DEVICEDESC);
            bool s = changeDeviceStatus(hDevInfo, DeviceInfoData, false);
            if (s) {
                wprintf(L"-> %s :: %s -> Disabled\n", hardwareID.c_str(), description.c_str());
            }
            else {
                wprintf(L"-> %s :: %s -> Change Status Failed\n", hardwareID.c_str(), description.c_str());
            }
            return s;
        }

    }

    return false;
}

int main(int argc, char* argv[])
{
    hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_SCREEN_BUFFER_INFO Info;
    GetConsoleScreenBufferInfo(hConsole, &Info);
    attributes = Info.wAttributes;

    showHeader();

    if (argc < 2) {
        showOptions();
        printError("", -1);
    }

    SetConsoleTextAttribute(hConsole, FOREGROUND_GREEN);

    if (argc < 3) {      
        if (strcmp(argv[1], "-l") == 0) {
            displayDevices();
            SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), attributes);
            exit(0);
        } 
    }

    if (argc < 4) {
        if (strcmp(argv[1], "-d") == 0) {
            char * hardwareID = argv[2]; 
            disableDevice(hardwareID);
            SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), attributes);
            exit(0);
        }
    }
  
    showOptions();

    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), attributes);
    return 0;
}

