#include <windows.h>
#include <stdio.h>
#include <stdbool.h>

#include "helper.h"
#include "mouse_hook.h"

CONFIG cfg;
int red, blue, green;
bool player_found = false;

DWORD WINAPI thread(DWORD param) {
    int pixel_count = cfg.scan_area_x * cfg.scan_area_y;
    while (true) {
        unsigned int* pixels = get_screenshot(0, cfg.scan_area_x, cfg.scan_area_x);
        if (is_color_found(pixels, pixel_count, red, green, blue, cfg.color_sens))
            player_found = true;
        else
            player_found = false;

        free(pixels);
        Sleep(1);
    }
    return 0;
}

// Dummy WndProc
LRESULT WndProc(HWND hWnd, unsigned int message, unsigned long long wParam, long long lParam) {
	return DefWindowProc(hWnd, message, wParam, lParam);
}

int main() {
    if (!get_config(&cfg)) {
        printf("Error: The variable name could not be found in the txt file.");
        printf("\nPress enter to exit: ");
        getchar();
        return 1;
    }

    if (!get_valorant_colors(cfg.target_color, &red, &green, &blue)) {
        printf("Error: Unknown color.");
        printf("\nPress enter to exit: ");
        getchar();
        return 1;
    }

    free_config(&cfg);

    print_logo();

    start_mouse_hook();

    CreateThread(NULL, 0, thread, NULL, 0, NULL);
	
	MSG msg;
	while (GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);

		Sleep(1);
	}
}
