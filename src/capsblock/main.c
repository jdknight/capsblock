// Copyright jdknight

#include <stdbool.h>
#include <stdio.h>
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN 1
#endif
#include <windows.h>

// state to suppress hook events when generating new caps-lock events
volatile bool gSkip = false;

// invoked when a low-level keyboard event occurs
LRESULT CALLBACK LowLevelKeyboardProc(int code, WPARAM msg, LPARAM ctx)
{
    // listen for key-up events of the caps-lock key
    if (!gSkip && !code >= 0 && msg == WM_KEYUP) {
        KBDLLHOOKSTRUCT* kbd = (KBDLLHOOKSTRUCT*)ctx;
        if (kbd->vkCode == VK_CAPITAL) {
            // check if we have caps-lock enabled in our current state
            bool hasCapsLock = ((GetKeyState(VK_CAPITAL) & 0x0001) != 0);
            if (hasCapsLock) {
                // before we attempt to toggle off the caps-lock state,
                // flag our hook as disabled, to ensure we do not attempt
                // to process the state of any caps-lock keys issued from
                // our hook
                gSkip = true;

                // generate two additional key events:
                ///
                //  1) a key-up event to "complete" the toggled on state
                //  2) a key-down event to trigger a new caps-lock event
                //
                // do not consume this active event, to complete the final
                // key-up event
                INPUT input[2] = {0};
                input[0].type = input[1].type = INPUT_KEYBOARD;
                input[0].ki.wVk  = input[1].ki.wVk = VK_CAPITAL;
                input[0].ki.dwFlags = KEYEVENTF_KEYUP;
                SendInput(2, input, sizeof(INPUT));

                // after issuing our key changes, re-enable this hook
                gSkip = false;
            }
        }
    }

    return CallNextHookEx(NULL, code, msg, ctx);
}

#pragma warning(disable: 4100)
int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
        PWSTR pCmdLine, int nCmdShow)
{
    // create a hook to listen for keyboard events
    HHOOK hook = SetWindowsHookEx(
        WH_KEYBOARD_LL, LowLevelKeyboardProc, GetModuleHandle(NULL), 0);
	if (!hook) {
        fprintf(stderr, "unable to create keyboard hook\n");
		return 1;
	}

    // consume all messages
    MSG msg;
    while(GetMessage(&msg, NULL, 0, 0) != 0);

    return 0;
}
