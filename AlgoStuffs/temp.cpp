#include "temp.h"
#include <stdio.h>
#include <Windows.h>
#include <TlHelp32.h>

typedef enum _LAST_ERROR : BYTE {
    SUCCESS,
    BAD_PARAMETERS,
    CANNOT_FIND_ANY_PROCESSES,
    CANNOT_FIND_ANY_MODULES,
    CANNOT_FIND_PROCESS,
    CANNOT_FIND_MODULE,
    WILDCARDS_DONT_MATCH_BYTES,
    OUT_OF_MEMORY,
    CANNOT_OPEN_HANDLE_TO_PROCESS
} LAST_ERROR, * PLAST_ERROR;

typedef struct _MODULE_RANGE {
    UINT_PTR    FirstAddress;
    UINT_PTR    LastAddress;
} MODULE_RANGE, * PMODULE_RANGE;

LAST_ERROR
FindModuleRangeInProcess(
    DWORD           ProcessId,
    LPCWSTR         ModuleName,
    PMODULE_RANGE   Range
) {
    if ((NULL == ModuleName) || (L'\0' == *ModuleName) || (NULL == ProcessId)) {
        return BAD_PARAMETERS;
    }

    HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, ProcessId);

    if (NULL == hSnap) {
        return CANNOT_FIND_ANY_MODULES;
    }

    BOOLEAN             bFoundModule = FALSE;
    BOOLEAN             bFoundNextModule = FALSE;

    MODULEENTRY32W      ME32;
    ZeroMemory(&ME32, sizeof(ME32));
    ME32.dwSize = sizeof(ME32);

    bFoundNextModule = Module32FirstW(hSnap, &ME32);
    while (TRUE == bFoundNextModule) {

        if (NULL == _wcsicmp(ModuleName, ME32.szModule)) {
            Range->FirstAddress = (UINT_PTR)ME32.modBaseAddr;
            Range->LastAddress = (UINT_PTR)ME32.modBaseAddr + ME32.modBaseSize;
            bFoundModule = TRUE;
            break;
        }

        bFoundNextModule = Module32NextW(hSnap, &ME32);
    }

    CloseHandle(hSnap);

    if (FALSE == bFoundModule) {
        return CANNOT_FIND_MODULE;
    }

    return SUCCESS;
}

LAST_ERROR
FindProcessIdByName(
    LPCWSTR     ProcessName,
    PDWORD      ProcessId
) {
    if ((NULL == ProcessName) || (L'\0' == *ProcessName) || (NULL == ProcessId)) {
        return BAD_PARAMETERS;
    }

    HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);

    if (NULL == hSnap) {
        return CANNOT_FIND_ANY_PROCESSES;
    }

    BOOLEAN             bFoundProcess = FALSE;
    BOOLEAN             bFoundNextProcess = FALSE;

    PROCESSENTRY32W     PE32;
    ZeroMemory(&PE32, sizeof(PE32));
    PE32.dwSize = sizeof(PE32);

    bFoundNextProcess = Process32FirstW(hSnap, &PE32);
    while (TRUE == bFoundNextProcess) {

        if (NULL == _wcsicmp(ProcessName, PE32.szExeFile)) {
            *ProcessId = PE32.th32ProcessID;
            bFoundProcess = TRUE;
            break;
        }

        bFoundNextProcess = Process32NextW(hSnap, &PE32);
    }

    CloseHandle(hSnap);

    if (FALSE == bFoundProcess) {
        return CANNOT_FIND_PROCESS;
    }

    return SUCCESS;
}

typedef struct _FOUND_ADDRESSES {
    UINT_PTR* Addresses;
    UINT_PTR    NumberOfAddresses;
} FOUND_ADDRESSES, * PFOUND_ADDRESSES;

LAST_ERROR
FindAddressOfByteArrayWithDelimiters(
    PFOUND_ADDRESSES    FoundAddresses,
    HANDLE              ProcessHandle,
    BYTE* Data,
    INT                 DataSize,
    LPCWSTR             Wildcards,
    BOOLEAN             SkipCompleteMatches,
    UINT_PTR            FirstAddress,
    UINT_PTR            LastAddress
)
{
    if ((NULL == ProcessHandle) || (NULL == Data) || (NULL == DataSize)) {
        return BAD_PARAMETERS;
    }

    if (wcslen(Wildcards) != DataSize) {
        return WILDCARDS_DONT_MATCH_BYTES;
    }

    DWORD dwReadableMask = (PAGE_READONLY | PAGE_READWRITE | PAGE_WRITECOPY
        | PAGE_EXECUTE_READ | PAGE_EXECUTE_READWRITE
        | PAGE_EXECUTE_WRITECOPY);
    DWORD dwProtectedMask = (PAGE_GUARD | PAGE_NOACCESS);

    INT                         iFoundSize = 10;
    UINT_PTR                    ulCurrAddr = NULL;
    BYTE* lpBuff = NULL;
    SIZE_T                      ulBytesRead = NULL;
    BOOLEAN                     bFound = FALSE;

    MEMORY_BASIC_INFORMATION    Mbi;
    ZeroMemory(&Mbi, sizeof(Mbi));

    FoundAddresses->Addresses = (UINT_PTR*)(malloc(iFoundSize * sizeof(UINT_PTR)));

    ulCurrAddr = (UINT_PTR)(FirstAddress);

    while ((sizeof(Mbi) == VirtualQueryEx(ProcessHandle, (LPVOID)(ulCurrAddr), &Mbi, sizeof(Mbi))),
        (ulCurrAddr <= LastAddress)) {
        if ((dwReadableMask & Mbi.Protect) && (FALSE == (dwProtectedMask & Mbi.Protect))) {
            lpBuff = (BYTE*)(malloc(Mbi.RegionSize));

            if (TRUE == ReadProcessMemory(ProcessHandle, (LPVOID)(ulCurrAddr), lpBuff, Mbi.RegionSize,
                &ulBytesRead)) {

                if (ulBytesRead == Mbi.RegionSize) {

                    for (UINT i = NULL; i < Mbi.RegionSize; ++i) {

                        bFound = TRUE;

                        for (UINT j = NULL; j < DataSize; ++j) {
                            if (L'x' == Wildcards[j]) {
                                if (lpBuff[i + j] != Data[j]) {
                                    bFound = FALSE;
                                    break;
                                }
                            }
                        }

                        if (TRUE == bFound) {
                            if (iFoundSize == (FoundAddresses->NumberOfAddresses + 1)) {
                                LPVOID lpTemp = realloc(FoundAddresses->Addresses, (iFoundSize += 50)
                                    * sizeof(UINT_PTR));

                                if (NULL == lpTemp) {
                                    free(FoundAddresses->Addresses);
                                    free(lpBuff);

                                    return OUT_OF_MEMORY;
                                }

                                FoundAddresses->Addresses = (UINT_PTR*)(lpTemp);
                            }

                            FoundAddresses->Addresses[FoundAddresses->NumberOfAddresses]
                                = (ulCurrAddr + i);

                            ++FoundAddresses->NumberOfAddresses;

                            if (TRUE == SkipCompleteMatches) {
                                i += DataSize;
                            }
                        }
                    }
                }
            }

            free(lpBuff);
        }

        ulCurrAddr = (UINT_PTR)(Mbi.BaseAddress) + Mbi.RegionSize;
    }

    return SUCCESS;
}

LAST_ERROR
FindAddressOfByteArrayInEntireProcess(
    PFOUND_ADDRESSES    FoundAddresses,
    LPCWSTR             ProcessName,
    BYTE* Data,
    INT                 DataSize,
    LPCWSTR             Wildcards,
    BOOLEAN             SkipCompleteMatches
) {
    LAST_ERROR  leErr = SUCCESS;
    DWORD       dwProcessId = NULL;

    leErr = FindProcessIdByName(ProcessName, &dwProcessId);

    if (SUCCESS != leErr) {
        return leErr;
    }

    HANDLE      hProc = OpenProcess(PROCESS_ALL_ACCESS, FALSE, dwProcessId);

    if (NULL == hProc) {
        return CANNOT_OPEN_HANDLE_TO_PROCESS;
    }

    SYSTEM_INFO sysInfo;
    ZeroMemory(&sysInfo, sizeof(sysInfo));

    GetSystemInfo(&sysInfo);

    leErr = FindAddressOfByteArrayWithDelimiters(FoundAddresses, hProc, Data, DataSize, Wildcards, SkipCompleteMatches,
        (UINT_PTR)sysInfo.lpMinimumApplicationAddress, (UINT_PTR)sysInfo.lpMaximumApplicationAddress);

    CloseHandle(hProc);

    return leErr;
}

LAST_ERROR
FindAddressOfByteArrayInProcessModule(
    PFOUND_ADDRESSES    FoundAddresses,
    LPCWSTR             ProcessName,
    BYTE* Data,
    INT                 DataSize,
    LPCWSTR             Wildcards,
    BOOLEAN             SkipCompleteMatches,
    LPCWSTR             ModuleName
) {
    LAST_ERROR  leErr = SUCCESS;
    DWORD       dwProcessId = NULL;

    leErr = FindProcessIdByName(ProcessName, &dwProcessId);

    if (SUCCESS != leErr) {
        return leErr;
    }

    HANDLE      hProc = OpenProcess(PROCESS_ALL_ACCESS, FALSE, dwProcessId);

    if (NULL == hProc) {
        return CANNOT_OPEN_HANDLE_TO_PROCESS;
    }

    MODULE_RANGE modRange;
    ZeroMemory(&modRange, sizeof(modRange));

    leErr = FindModuleRangeInProcess(dwProcessId, ModuleName, &modRange);

    if (SUCCESS != leErr) {
        CloseHandle(hProc);

        return leErr;
    }

    leErr = FindAddressOfByteArrayWithDelimiters(FoundAddresses, hProc, Data, DataSize, Wildcards,
        SkipCompleteMatches, modRange.FirstAddress, modRange.LastAddress);

    CloseHandle(hProc);

    return leErr;
}

VOID
CreateFoundAddresses(
    PFOUND_ADDRESSES FoundAddresses
) {
    if (NULL != FoundAddresses) {
        ZeroMemory(FoundAddresses, sizeof(*FoundAddresses));
    }
}

VOID
DestroyFoundAddresses(
    PFOUND_ADDRESSES FoundAddresses
) {
    if ((NULL != FoundAddresses) && (NULL != FoundAddresses->Addresses)) {
        free(FoundAddresses->Addresses);
        ZeroMemory(FoundAddresses, sizeof(*FoundAddresses));
    }
}

VOID
PressKeyToExit(
    VOID
) {
    printf("Press any key to exit...");
    getchar();
    ExitProcess(0);
}
/*
int main()
{
    // Store last error
    LAST_ERROR leErr = SUCCESS;

    // Create storage for found addresses
    FOUND_ADDRESSES faFound;
    CreateFoundAddresses(&faFound);

    BYTE lpData[] = { 0xff, 0xff };

    // Search for bytes FF ("x) and whatever comes afterwards (?") in process notepad.exe (First) in module notepad.exe (Second)
    leErr = FindAddressOfByteArrayInProcessModule(&faFound, L"notepad.exe", lpData, sizeof(lpData), L"?x", FALSE, L"notepad.exe");

    // If the function failed
    if (SUCCESS != leErr) {
        printf("Could not search for the byte array! Last error: %d\n", leErr);
        PressKeyToExit();
    }

    printf("Found %d addresses! Press any key to print all of them...\n", faFound.NumberOfAddresses);
    getchar();

    // This part is for testing so I can make sure the results are good, you don't need this
    DWORD dwProcessId = NULL;
    FindProcessIdByName(L"notepad.exe", &dwProcessId);

    HANDLE hProc = OpenProcess(PROCESS_ALL_ACCESS, FALSE, dwProcessId);

    BYTE* lpInData = (BYTE*)(malloc(sizeof(lpData)));

    for (UINT i = NULL; i < faFound.NumberOfAddresses; ++i) {
        if (TRUE == ReadProcessMemory(hProc, (LPVOID)(faFound.Addresses[i]), lpInData, sizeof(lpData), NULL)) {
            printf("0x%p [", faFound.Addresses[i]);
            for (UINT j = NULL; j < sizeof(lpData); ++j) {
                if (j != (sizeof(lpData) - 1)) printf("0x%X ", lpInData[j]);
                else printf("0x%X]\n", lpInData[j]);
            }
        }
    }

    free(lpInData);

    CloseHandle(hProc);

    // Destroy all found addresses
    DestroyFoundAddresses(&faFound);

    PressKeyToExit();

    return 0;
}*/