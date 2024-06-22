// Copyright jdknight

#include <stdbool.h>
#include <stdio.h>
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN 1
#endif
#include <windows.h>

// semaphore used to signal the toggle-key thread
HANDLE gCapsEventSem;

// shutdown flag for the thread
volatile bool gShutdown = false;

// invoked when a low-level keyboard event occurs
LRESULT CALLBACK LowLevelKeyboardProc(int code, WPARAM msg, LPARAM ctx)
{
    // listen for key-up events of the caps-lock key
    if (!code >= 0 && msg == WM_KEYUP) {
        KBDLLHOOKSTRUCT* kbd = (KBDLLHOOKSTRUCT*)ctx;
        if (kbd->vkCode == VK_CAPITAL) {
            // check if we have caps-lock enabled in our current state
            bool hasCapsLock = ((GetKeyState(VK_CAPITAL) & 0x0001) != 0);
            if (hasCapsLock) {
                // signal to the toggle-key thread to check/enforce
                // the key state
                ReleaseSemaphore(gCapsEventSem, 1, NULL);
            }
        }
    }

    return CallNextHookEx(NULL, code, msg, ctx);
}

// thread used to wait for events to check/enforce the key state
// of the caps lock key
#pragma warning(disable: 4100)
DWORD WINAPI ToggleKeyThread(void* data)
{
    while (!gShutdown) {
        DWORD signal = WaitForSingleObject(gCapsEventSem, INFINITE);
        switch (signal) {
        case WAIT_OBJECT_0:
            // check (again) if we have caps-lock enabled in our current state
            bool hasCapsLock = ((GetKeyState(VK_CAPITAL) & 0x0001) != 0);
            if (hasCapsLock) {
                // generate two additional key events:
                ///
                //  1) a key-down event to trigger a new caps-lock event
                //  2) a key-up event to "complete" the event
                INPUT input[2] = { 0 };
                input[0].type = input[1].type = INPUT_KEYBOARD;
                input[0].ki.wVk = input[1].ki.wVk = VK_CAPITAL;
                input[1].ki.dwFlags = KEYEVENTF_KEYUP;
                SendInput(2, input, sizeof(INPUT));
            }
            break;

        case WAIT_TIMEOUT:
            // spurious wakeup
            break;

        default:
            // broken state
            gShutdown = true;
            break;
        }
    }

    return 0;
}

#pragma warning(disable: 4100)
int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
        PWSTR pCmdLine, int nCmdShow)
{
    // create a signal between that can be issued by the keyboard hook to
    // out toggle-key thread
    gCapsEventSem = CreateSemaphore(NULL, 1, 1, NULL);
    if (!gCapsEventSem) {
        fprintf(stderr, "unable to create keyboard hook\n");
        return 1;
    }

    // create a hook to listen for keyboard events
    HHOOK hook = SetWindowsHookEx(
        WH_KEYBOARD_LL, LowLevelKeyboardProc, GetModuleHandle(NULL), 0);
    if (!hook) {
        fprintf(stderr, "unable to create keyboard hook\n");
        return 2;
    }

    // build a thread reposible for toggling the cap-lock key off
    HANDLE tkt = CreateThread(NULL, 0, ToggleKeyThread, NULL, 0, NULL);
    if (!tkt) {
        fprintf(stderr, "unable to create toggle-key thread\n");
        return 3;
    }

    // consume all messages
    MSG msg;
    while(GetMessage(&msg, NULL, 0, 0) != 0);

    // cleanup
    gShutdown = true;
    ReleaseSemaphore(gCapsEventSem, 1, NULL);
    WaitForSingleObject(tkt, INFINITE);
    CloseHandle(tkt);
    CloseHandle(gCapsEventSem);

    return 0;
}
