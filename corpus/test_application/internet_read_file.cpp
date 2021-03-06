// include these in here
// https://social.msdn.microsoft.com/Forums/windowsdesktop/en-US/8f468d9f-3f15-452c-803d-fc63ab3f684e/cannot-use-both-winineth-and-winhttph?forum=windowssdk#acb8bca7-f2b7-46d2-ac22-9d203b6f4126

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

#include <Wininet.h>
#pragma comment(lib, "Wininet.lib")

#include "internet_read_file.h"

// InternetReadFile
int test_InternetReadFile(bool fuzzing)
{
    HINTERNET hInternet = InternetOpen(L"User Agent Here", INTERNET_OPEN_TYPE_PRECONFIG, NULL, NULL, 0);
    if (!hInternet || hInternet == INVALID_HANDLE_VALUE) {
       printf("failed to open the internet\n");
       return 1;
    }

    HINTERNET hWebpage = InternetOpenUrl(hInternet, L"http://neverssl.com", NULL, 0, INTERNET_FLAG_KEEP_CONNECTION, 0);
    if (!hWebpage || hWebpage == INVALID_HANDLE_VALUE)
    {
        printf("failed to open the web page\n");
        InternetCloseHandle(hInternet);
        return 1;
    }

    // this method reads webpages into the buffer
    // but it does it line by line for some dumbshit reason
    // so the buffer has to be at least the size of the longest line
    DWORD bufSize = 4096;
    char *buf = new char[bufSize];
    DWORD bytesRead = 0;

    if (InternetReadFile(hWebpage, buf, 8, &bytesRead)) { 
        if (bytesRead == 0) {
            printf("no bytes read");
            return 1;
        }

        buf[bytesRead] = 0;
        printf("%s", buf);
        int *crashPtr = *(int **)buf;
        printf("CRASH PTR: %p\n", crashPtr);
        if (fuzzing) {
            if((UINT64)crashPtr > 0x250A3E6C6D74683C) {
                printf("*CRASH PTR: %x\n", *crashPtr);
            }
        } else {
            printf("*CRASH PTR: %x\n", *crashPtr);
        }
    }
    else {
        printf("Error reading: %d\n", GetLastError());
    }

    printf("\n");

    InternetCloseHandle(hWebpage);
    InternetCloseHandle(hInternet);
    return 0;
}
