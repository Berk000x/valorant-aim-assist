#include <windows.h>
#include <stdio.h>
#include <stdbool.h>

#include "helper.h"

void print_logo() {
    system("cls");
    disable_quickedit();
    printf("==================================================\n");
    printf("                  AIM ASSIST                     \n");
    printf("==================================================\n");
    printf(" _   _ _       _                        \n");
    printf("| | | (_) __ _| |____      ____ _ _   _ \n");
    printf("| |_| | |/ _` | '_ \\ \\ /\\ / / _` | | | |\n");
    printf("|  _  | | (_| | | | \\ V  V / (_| | |_| |\n");
    printf("|_| |_|_|\\__, |_| |_|\\_/\\_/ \\__,_|\\__, |\n");
    printf("         |___/                    |___/ \n");
    printf("\n");
    printf("==================================================\n");
    printf("Version 1.0                     \n");
    printf("Join us on Discord: https://discord.gg/ZWTfrqXMW4 \n");
    printf("Valorant Aim Assist is running successfully!    \n");
    printf("==================================================\n");
    printf("\n");
}

void disable_quickedit() {
    HANDLE hInput = GetStdHandle(STD_INPUT_HANDLE);
    DWORD prevMode;

    GetConsoleMode(hInput, &prevMode);
    SetConsoleMode(hInput, prevMode & ~ENABLE_QUICK_EDIT_MODE);
}

bool get_valorant_colors(const char* pColor, int* pRed, int* pGreen, int* pBlue) {
    if (strcmp(pColor, "purple") == 0) {
        *pRed = 250;
        *pGreen = 100;
        *pBlue = 250;

        return true;
    }
    else if (strcmp(pColor, "yellow") == 0) {
        *pRed = 254;
        *pGreen = 254;
        *pBlue = 64;

        return true;
    }
    else {
        return false;
    }
}

char* get_str(char* key_name) {
    FILE* file;
    char line[256];

    char* key;
    char* value;

    file = fopen("config.txt", "r");

    if (!file) {
        printf("The config.txt file was not found. Using default config.\n");

        file = fopen("config.txt", "w");
        if (file) {
            fprintf(file, "target_color=purple\n");
            fprintf(file, "color_sens=65\n");
            fprintf(file, "lock_power=15\n");
            fprintf(file, "scan_area_x=8\n");
            fprintf(file, "scan_area_y=8\n");
            fclose(file);
        }
        else {
            printf("Error: Could not create the .txt file.\n");
        }
        file = fopen("config.txt", "r");
    }


    // Read the file line by line
    while (fgets(line, sizeof(line), file)) {
        key = strtok(line, "=");
        value = strtok(0, "=");

        if (value != 0) {
            // Remove newline character from value if present
            size_t len = strlen(value);
            if (len > 0 && value[len - 1] == '\n') {
                value[len - 1] = '\0';
            }
        }

        if (strcmp(key_name, key) == 0) {
            fclose(file);
            char* ret = (char*)malloc(strlen(value) + 1);
            strcpy(ret, value);
            return ret;
        }
    }

    fclose(file);
    printf("Error: The variable name could not be found in the txt file.\n");
    return 0;
}

int get_int(char* key_name) {
    char* str = get_str(key_name);
    if (str == 0) {
        return -1;
    }
    int ret = atoi(str);
    free(str);
    return ret;
}

bool get_config(CONFIG* pCfg) {
    pCfg->target_color = get_str("target_color");
    pCfg->scan_area_x = get_int("scan_area_x");
    pCfg->scan_area_y = get_int("scan_area_y");
    pCfg->color_sens = get_int("color_sens");
    pCfg->lock_power = get_int("lock_power");

    if (pCfg->target_color == 0 ||
        pCfg->scan_area_x == -1 ||
        pCfg->scan_area_y == -1 ||
        pCfg->color_sens == -1  ||
        pCfg->lock_power == -1)
    {
        return false;
    }

    return true;
}

void free_config(CONFIG* pCfg) {
    free(pCfg->target_color);
}

unsigned int* get_screenshot(const char* save_name, int crop_width, int crop_height) {
    unsigned int* pixels = 0;
    HDC mem_dc = 0;
    FILE* file = 0;

    int screen_width = GetSystemMetrics(SM_CXSCREEN);
    int screen_height = GetSystemMetrics(SM_CYSCREEN);

    HDC screen_dc = GetDC(0);
    HBITMAP bitmap = 0;
    BITMAP bmp = { 0 };
    BITMAPFILEHEADER bmp_file_header = { 0 };

    int crop_x = 0;
    int crop_y = 0;

    if (screen_dc == 0) {
        printf("ERROR: GetDC() failed!");
        return 0;
    }

    /* Calculate coordinates for center crop */
    crop_x = (screen_width - crop_width) / 2;
    crop_y = (screen_height - crop_height) / 2;

    /* Create memory DC and compatible bitmap */
    mem_dc = CreateCompatibleDC(screen_dc);

    if (mem_dc == 0) {
        printf("ERROR: CreateCompatibleDC() failed!");
        ReleaseDC(0, screen_dc);
        return 0;
    }

    bitmap = CreateCompatibleBitmap(screen_dc, crop_width, crop_height);

    if (bitmap == 0) {
        printf("ERROR: CreateCompatibleBitmap() failed.");
        ReleaseDC(0, screen_dc);
        DeleteDC(mem_dc);
        return 0;
    }

    SelectObject(mem_dc, bitmap);

    /* Copy cropped screen contents to memory DC */
    if (BitBlt(mem_dc, 0, 0, crop_width, crop_height, screen_dc, crop_x, crop_y, SRCCOPY) == 0) {
        printf("ERROR: BitBlt() failed!");
        ReleaseDC(0, screen_dc);
        DeleteDC(mem_dc);
        DeleteObject(bitmap);
        return 0;
    }

    /* Prepare bitmap header */
    GetObject(bitmap, sizeof(BITMAP), &bmp);

    /* Create bitmap file header */
    bmp_file_header.bfType = 0x4D42; /* BM */
    bmp_file_header.bfSize = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) +
        bmp.bmWidthBytes * crop_height;
    bmp_file_header.bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);

    /* Create bitmap info header */
    BITMAPINFOHEADER bmp_info_header;
    memset(&bmp_info_header, 0, sizeof(BITMAPINFOHEADER));
    bmp_info_header.biSize = sizeof(BITMAPINFOHEADER);
    bmp_info_header.biWidth = bmp.bmWidth;
    bmp_info_header.biHeight = bmp.bmHeight;
    bmp_info_header.biPlanes = 1;
    bmp_info_header.biBitCount = 32; /* Use 32-bit color */
    bmp_info_header.biCompression = BI_RGB;
    bmp_info_header.biSizeImage = bmp.bmWidthBytes * bmp.bmHeight;

    if (save_name) {
        /* Save bitmap to file */
        file = fopen(save_name, "wb");

        if (!file) {
            printf("ERROR: fopen() failed!");
            ReleaseDC(0, screen_dc);
            DeleteDC(mem_dc);
            DeleteObject(bitmap);
            return 0;
        }

        fwrite(&bmp_file_header, sizeof(BITMAPFILEHEADER), 1, file);
        fwrite(&bmp_info_header, sizeof(BITMAPINFOHEADER), 1, file);
    }

    /* Allocate buffer for bitmap data */
    pixels = (unsigned int*)malloc(bmp.bmWidthBytes * crop_height);

    if (!pixels) {
        printf("ERROR: malloc() failed!");
        ReleaseDC(0, screen_dc);
        DeleteDC(mem_dc);
        DeleteObject(bitmap);
        return 0;
    }

    /* Get bitmap data */
    if (GetDIBits(mem_dc, bitmap, 0, crop_height, pixels, (BITMAPINFO*)&bmp_info_header, DIB_RGB_COLORS) == 0) {
        printf("ERROR: GetDIBits() failed!");
        ReleaseDC(0, screen_dc);
        DeleteDC(mem_dc);
        DeleteObject(bitmap);
        free(pixels);
        return 0;
    }

    if (save_name) {
        fwrite(pixels, bmp.bmWidthBytes * crop_height, 1, file);
        fclose(file);
    }

    ReleaseDC(0, screen_dc);
    DeleteDC(mem_dc);
    DeleteObject(bitmap);

    return pixels;
}

bool is_color_found(DWORD* pixels, int pixel_count, int red, int green, int blue, int color_sens) {
    for (int i = 0; i < pixel_count; i++) {
        DWORD pixelColor = pixels[i];

        int r = GetRed(pixelColor);
        int g = GetGreen(pixelColor);
        int b = GetBlue(pixelColor);

        if (r + color_sens >= red && r - color_sens <= red) {
            if (g + color_sens >= green && g - color_sens <= green) {
                if (b + color_sens >= blue && b - color_sens <= blue) {
                    return true;
                }
            }
        }
    }

    return false;
}