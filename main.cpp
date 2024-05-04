#include <Windows.h>
#include <winscard.h>

#include <iostream>
#include <vector>

#pragma comment(lib, "winscard.lib")

int main() {
    LONG rv;
    SCARDCONTEXT scardContext;
    LPWSTR mszReaders = nullptr;
    auto cch = SCARD_AUTOALLOCATE;

    rv = SCardEstablishContext(SCARD_SCOPE_USER, nullptr, nullptr, &scardContext);
    if (rv != SCARD_S_SUCCESS) {
        std::cerr << "Failed to establish context, error: " << rv << std::endl;
        return 1;
    }

    rv = SCardListReaders(scardContext, nullptr, (LPWSTR)&mszReaders, &cch);
    if (rv != SCARD_S_SUCCESS) {
        std::cerr << "Failed to list readers, error: " << rv << std::endl;
        return 1;
    }

    int numReaders = 0;
    auto p = mszReaders;
    while (p != nullptr && *p) {
        p += wcslen(p) + 1;
        numReaders++;
    }

    std::vector<SCARD_READERSTATE> rgReaderStates(numReaders + 1);
    p = mszReaders;
    for (int i = 0; i < numReaders; i++) {
        rgReaderStates[i].szReader = p;
        rgReaderStates[i].dwCurrentState = SCARD_STATE_UNAWARE;
        p += wcslen(p) + 1;
    }

    rgReaderStates[numReaders].szReader = LR"(\\?PnP?\Notification)";
    rgReaderStates[numReaders].dwCurrentState = SCARD_STATE_UNAWARE;

    rv = SCardGetStatusChange(scardContext, 0, rgReaderStates.data(), numReaders + 1);
    if (rv != SCARD_S_SUCCESS) {
        std::cerr << "Failed to get status change, error: " << rv << std::endl;
        return 1;
    }

    for (int i = 0; i <= numReaders; i++) {
        rgReaderStates[i].dwCurrentState = rgReaderStates[i].dwEventState;
    }

    rv = SCardGetStatusChange(scardContext, INFINITE, rgReaderStates.data(), numReaders + 1);
    if (rv != SCARD_S_SUCCESS) {
        std::cerr << "Failed to get status change, error: " << rv << std::endl;
        return 1;
    }

    for (int i = 0; i <= numReaders; i++) {
        if (rgReaderStates[i].dwEventState & SCARD_STATE_CHANGED) {
            std::cout << "Reader state changed.\n";
            std::cout << "Reader: " << rgReaderStates[i].szReader << std::endl;
            if (rgReaderStates[i].dwEventState & SCARD_STATE_UNKNOWN) {
                std::cout << "New reader detected.\n";
            }
        }

        rgReaderStates[i].dwCurrentState = rgReaderStates[i].dwEventState;
    }

    if (mszReaders == nullptr) {
        std::cerr << "Failed to list readers, error: " << rv << std::endl;
        return 1;
    }

    rv = SCardFreeMemory(scardContext, mszReaders);
    if (rv != SCARD_S_SUCCESS) {
        std::cerr << "Failed to free memory, error: " << rv << std::endl;
    }

    rv = SCardReleaseContext(scardContext);
    if (rv != SCARD_S_SUCCESS) {
        std::cerr << "Failed to release context, error: " << rv << std::endl;
    }

    return 0;
}
