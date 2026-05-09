//
// Created by voltas on 5/9/2026.
//

#ifndef RBXSDK_CORE_H
#pragma once
#include <Windows.h>
#include <TlHelp32.h>
#include <cstdint>
#include <string>
#include <vector>

namespace rbx {

class process {
    HANDLE handle = nullptr;
    DWORD  pid = 0;
    uintptr_t image_base = 0;
    uintptr_t image_size = 0;
public:
    bool attach(const wchar_t* name = L"RobloxPlayerBeta.exe") {
        HANDLE snap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
        PROCESSENTRY32W pe = { sizeof(pe) };
        if (Process32FirstW(snap, &pe)) {
            do { if (!_wcsicmp(pe.szExeFile, name)) { pid = pe.th32ProcessID; break; }
            } while (Process32NextW(snap, &pe));
        }
        CloseHandle(snap);
        if (!pid) return false;

        handle = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pid);
        if (!handle) return false;

        snap = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE | TH32CS_SNAPMODULE32, pid);
        MODULEENTRY32W me = { sizeof(me) };
        if (Module32FirstW(snap, &me)) {
            do { if (!_wcsicmp(me.szModule, L"RobloxPlayerBeta.exe")) {
                image_base = (uintptr_t)me.modBaseAddr;
                image_size = me.modBaseSize;
                break;
            } } while (Module32NextW(snap, &me));
        }
        CloseHandle(snap);
        return true;
    }

    void detach() { if (handle) CloseHandle(handle); handle = nullptr; }

    template<typename T>
    T read(uintptr_t addr) const {
        T val{};
        SIZE_T r;
        ReadProcessMemory(handle, (void*)addr, &val, sizeof(T), &r);
        return val;
    }

    uintptr_t read_ptr(uintptr_t addr) const { return read<uintptr_t>(addr); }

    template<typename T>
    bool write(uintptr_t addr, const T& val) const {
        SIZE_T w;
        return WriteProcessMemory(handle, (void*)addr, &val, sizeof(T), &w);
    }

    std::string read_str(uintptr_t addr, size_t max = 264) const {
        char buf[2256] = {0};
        SIZE_T r;
        ReadProcessMemory(handle, (void*)addr, buf, max, &r);
        return std::string(buf);
    }

    uintptr_t base() const { return image_base; }
    uintptr_t size() const { return image_size; }
    HANDLE native() const { return handle; }
    DWORD process_id() const { return pid; }

    bool valid_ptr(uintptr_t addr) const {
        return addr > 0x10000 && addr < 0x7FFFFFFF0000;
    }
};

inline process proc;

} // rbx
#define RBXSDK_CORE_H

#endif //RBXSDK_CORE_H