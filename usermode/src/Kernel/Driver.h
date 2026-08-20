#pragma once

#include <Windows.h>

namespace Driver {
    namespace Codes {
        constexpr ULONG Attach = CTL_CODE(FILE_DEVICE_UNKNOWN, 0x800, METHOD_BUFFERED, FILE_SPECIAL_ACCESS);

        constexpr ULONG Read = CTL_CODE(FILE_DEVICE_UNKNOWN, 0x801, METHOD_BUFFERED, FILE_SPECIAL_ACCESS);

        constexpr ULONG Write = CTL_CODE(FILE_DEVICE_UNKNOWN, 0x802, METHOD_BUFFERED, FILE_SPECIAL_ACCESS);
    }

    struct Request {
        HANDLE processId;

        PVOID target;
        PVOID buffer;

        SIZE_T size;
        SIZE_T returnSize;
    };

    bool AttachToProcess(HANDLE driverHandle, const DWORD pID) {
        Request request;
        request.processId = reinterpret_cast<HANDLE>(pID);

        return DeviceIoControl(driverHandle, Codes::Attach, &request, sizeof(request), &request, sizeof(request), nullptr, nullptr);
    }

    template<typename T>
    T Read(HANDLE driverHandle, const uintptr_t address) {
        T temp = {};

        Request request;
        request.target = reinterpret_cast<PVOID>(address);
        request.buffer = &temp;
        request.size = sizeof(T);

        DeviceIoControl(driverHandle, Codes::Read, &request, sizeof(request), &request, sizeof(request), nullptr, nullptr);

        return temp;
    }

    template<typename T>
    T Write(HANDLE driverHandle, const uintptr_t address, const T& value) {
        Request request;
        request.target = reinterpret_cast<PVOID>(address);
        request.buffer = (PVOID)&value;
        request.size = sizeof(T);

        DeviceIoControl(driverHandle, Codes::Write, &request, sizeof(request), &request, sizeof(request), nullptr, nullptr);
    }
}