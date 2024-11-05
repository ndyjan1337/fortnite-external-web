#pragma once
#include <Windows.h>
#include <iostream>
#include <string>
#include <TlHelp32.h>

uintptr_t baseaddress;
uintptr_t cr3;

#define READ_CODE      CTL_CODE(FILE_DEVICE_UNKNOWN, 0x731, METHOD_BUFFERED, FILE_SPECIAL_ACCESS)
#define BASE_CODE      CTL_CODE(FILE_DEVICE_UNKNOWN, 0x732, METHOD_BUFFERED, FILE_SPECIAL_ACCESS)
#define DIR_CODE CTL_CODE(FILE_DEVICE_UNKNOWN, 0x733, METHOD_BUFFERED, FILE_SPECIAL_ACCESS)


typedef struct _Read_Struc {
    INT32       security;        
    INT32       process_id;    
    ULONGLONG   address;       
    ULONGLONG   buffer;        
    ULONGLONG   size;             
    BOOLEAN     write;           
} rs, * prs;

typedef struct _BASE_STRUC {
    INT32 security;
    INT32 process_id;
    ULONGLONG* address;
} base, * process_base;

typedef struct _PML4_STRUC {
    INT32 process_id;
    bool* operation;
} dir, * process_dir;


namespace mem
{
    HANDLE driver_handle;
    INT32 process_id;

    bool GetDrv()
    {
        driver_handle = CreateFileW((L"\\\\.\\\\Mikz1"), GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 0, NULL);

        if (!driver_handle || (driver_handle == INVALID_HANDLE_VALUE))
            return false;

        return true;
    }

    void read_physical(PVOID address, PVOID buffer, DWORD size)
    {
        _Read_Struc arguments = { NULL };

        arguments.address = (ULONGLONG)address;
        arguments.buffer = (ULONGLONG)buffer;
        arguments.size = size;
        arguments.process_id = process_id;
        arguments.write = FALSE;

        DeviceIoControl(driver_handle, READ_CODE, &arguments, sizeof(arguments), nullptr, NULL, NULL, NULL);
    }



    uintptr_t GetBase()
    {
        uintptr_t image_address = { NULL };
        _BASE_STRUC arguments = { NULL };

        arguments.process_id = process_id;
        arguments.address = (ULONGLONG*)&image_address;

        DeviceIoControl(driver_handle, BASE_CODE, &arguments, sizeof(arguments), nullptr, NULL, NULL, NULL);

        return image_address;
    }

    bool GetDir()
    {
        bool recievce = false;
        _PML4_STRUC arguments = { NULL };
        arguments.process_id = process_id;
        arguments.operation = (bool*)&recievce;
        DeviceIoControl(driver_handle, DIR_CODE, &arguments, sizeof(arguments), nullptr, NULL, NULL, NULL);

        return recievce;
    }

    INT32 find_process(LPCTSTR process_name)
    {
        PROCESSENTRY32 pt;
        HANDLE hsnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
        pt.dwSize = sizeof(PROCESSENTRY32);
        if (Process32First(hsnap, &pt)) {
            do {
                if (!lstrcmpi(pt.szExeFile, process_name)) {
                    CloseHandle(hsnap);
                    process_id = pt.th32ProcessID;
                    return pt.th32ProcessID;
                }
            } while (Process32Next(hsnap, &pt));
        }
        CloseHandle(hsnap);

        return { NULL };
    }

}

template <typename T>
T read(uint64_t address)
{
    T buffer{ };
    mem::read_physical((PVOID)address, &buffer, sizeof(T));

    return buffer;
}


bool is_valid(const uint64_t adress)
{
    if (adress <= 0x400000 || adress == 0xCCCCCCCCCCCCCCCC || reinterpret_cast<void*>(adress) == nullptr || adress > 0x7FFFFFFFFFFFFFFF)
    {
        return false;
    }
    return true;
}