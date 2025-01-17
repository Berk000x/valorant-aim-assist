#include <windows.h>
#include <stdio.h>
#include <stdbool.h>

#include "mouse_hook.h"
#include "helper.h"

CONFIG cfg;
extern bool player_found;
HHOOK hook;

LRESULT CALLBACK hook_callback(int nCode, WPARAM wParam, LPARAM lParam) {

    if (nCode >= 0 && wParam == WM_MOUSEMOVE && player_found) {

        /**
        * This function is hooked by Vanguard, so the return statement doesn't affect it,
        * but calling the Sleep function does when Valorant is running.
        **/

        Sleep(cfg.lock_power); // Block the mouse movement input.
        // return 1;
    }

    return CallNextHookEx(hook, nCode, wParam, lParam);
}

void start_mouse_hook() {

    hook = SetWindowsHookEx(WH_MOUSE_LL, hook_callback, 0, 0);

    if (!hook) {
        printf("Failed to set hook!\n");
        return;
    }
}
