/* a graceful terminating gui application for windows.
*
* Copyright (c) 2017, Rui Lopes (ruilopes.com)
* All rights reserved.
*
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions are met:
*
*   * Redistributions of source code must retain the above copyright notice,
*     this list of conditions and the following disclaimer.
*   * Redistributions in binary form must reproduce the above copyright
*     notice, this list of conditions and the following disclaimer in the
*     documentation and/or other materials provided with the distribution.
*   * Neither the name of Redis nor the names of its contributors may be used
*     to endorse or promote products derived from this software without
*     specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
* AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
* IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
* ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
* LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
* CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
* SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
* INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
* CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
* ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
* POSSIBILITY OF SUCH DAMAGE.
*/

#define _WIN32_WINNT 0x0501
#define WINVER _WIN32_WINNT
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <wchar.h>
#include <stdio.h>
#include <stdarg.h>
#include <time.h>

void LOG(const char *format, ...) {
    time_t t;
    time(&t);
    char buffer[20];
    strftime(buffer, 20, "%Y-%m-%d %H:%M:%S", localtime(&t));
    FILE *log = fopen("graceful-terminating-gui-application-windows.log", "a+");
    fprintf(log, "%s ", buffer);
    va_list args;
    va_start(args, format);
    vfprintf(log, format, args);
    va_end(args);
    fclose(log);
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {
        case WM_CLOSE:
            LOG("WndProc WM_CLOSE\n");
            PostQuitMessage(0);
            break;
        case WM_QUERYENDSESSION:
            LOG("WndProc WM_QUERYENDSESSION\n");
            if (!ShutdownBlockReasonCreate(hWnd, L"Starting Graceful Termination...")) {
                LOG("ERROR ShutdownBlockReasonCreate failed with 0x%x\n", GetLastError());
            }
            PostQuitMessage(0);
            return FALSE;
        default:
            return DefWindowProc(hWnd, uMsg, wParam, lParam);
    }
    return 0;
}

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow) {
    LOG("Running (pid=%d)...\n", GetCurrentProcessId());

    int n = __argc == 2 ? _wtoi(__wargv[1]) : 10;

    WNDCLASS wc = {
        .lpfnWndProc = WndProc,
        .hInstance = hInstance,
        .hCursor = LoadCursor(NULL, IDC_ARROW),
        .hbrBackground = (HBRUSH)COLOR_WINDOW,
        .lpszClassName = L"graceful-terminating-gui-application",
    };
    if (!RegisterClass(&wc)) {
        LOG("ERROR: Failed to RegisterClass.\n");
        return 1;
    }

    HWND hWnd = CreateWindow(
        wc.lpszClassName,
        L"Graceful Terminating Gui Application",
        WS_OVERLAPPEDWINDOW | WS_VISIBLE,
        0,
        0,
        640,
        480,
        NULL,
        NULL,
        hInstance,
        NULL);
    if (!hWnd) {
        LOG("ERROR: Failed to CreateWindow.\n");
        return 2;
    }

    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0) > 0) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    for (; n; --n) {
        LOG("Gracefully terminating the application in T-%d...\n", n);
        wchar_t buffer[50];
        swprintf_s(buffer, 50, L"Gracefully Terminating in T-%d...", n);
        if (!ShutdownBlockReasonCreate(hWnd, buffer)) {
            LOG("ERROR ShutdownBlockReasonCreate failed with 0x%x\n", GetLastError());
        }
        Sleep(1000);
    }

    if (!ShutdownBlockReasonDestroy(hWnd)) {
        LOG("ERROR ShutdownBlockReasonDestroy failed with 0x%x\n", GetLastError());
    }

    LOG("Exiting with code %d\n", msg.wParam);   
    return msg.wParam;
}
