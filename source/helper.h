#ifndef HELPER_H
#define HELPER_H

#define GetBlue(rgb)     (LOBYTE(rgb))
#define GetGreen(rgb)    (LOBYTE(((WORD)(rgb)) >> 8))
#define GetRed(rgb)      (LOBYTE((rgb)>>16))

typedef struct {
    char* target_color;
    int scan_area_x;
    int scan_area_y;
    int color_sens;
    int lock_power;
} CONFIG;

void disable_quickedit();
void free_config(CONFIG* pCfg);
bool get_valorant_colors(const char* pColor, int* pRed, int* pGreen, int* pBlue);
bool get_config(CONFIG* pCfg);
unsigned int* get_screenshot(const char* save_name, int crop_width, int crop_height);
bool is_color_found(DWORD* pPixels, int pixel_count, int red, int green, int blue, int color_sens);

#endif