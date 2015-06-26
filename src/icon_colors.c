#include "icon_colors.h"


#ifdef PBL_COLOR
#   define COLOR_INVALID (GColorClear)
#   define COLOR_AT(i) sColors[i]
#   define COLOR_NOT_SET(c) gcolor_equal((c),COLOR_INVALID)
#else
#   define COLOR_INVALID ((uint8_t)-1)
#   define COLOR_AT(i) (i)
#   define COLOR_NOT_SET(c) ((c)==COLOR_INVALID)
#endif


IconColor *icon_colors_create(uint8_t *aOutCount) {
#ifdef PBL_COLOR
    GColor sColors[] = {
        GColorChromeYellow,
        GColorPictonBlue,
        GColorSpringBud,
        GColorFolly,
        GColorScreaminGreen,
        GColorLightGray,
        GColorDarkGray,
    };
    uint8_t count = sizeof(sColors)/sizeof(sColors[0]);
#else
    uint8_t count = 4; 
#endif
    *aOutCount = count;
    
    IconColor *colors = (IconColor *)malloc(sizeof(IconColor) * count);
    for (int i = 0; i < count; ++i) {
        colors[i] = COLOR_INVALID;
    }
    
    uint8_t currentIndex = 0;
    uint8_t notDecided = count;
    while (notDecided) {
        int8_t index = rand() % notDecided;
        for (int i = 0; i < count; ++i) {
            if (COLOR_NOT_SET(colors[i])) {
                if (!index) {
                    colors[i] = COLOR_AT(currentIndex);
                    ++currentIndex;
                    break;
                }
                --index;
            }
        }
        --notDecided;
    }
    return colors;
}

void icon_colors_destroy(IconColor *aIconColors) {
	free(aIconColors);
}

IconColor icon_color_default() {
#ifdef PBL_COLOR
    return GColorWhite;
#else
    return 0;
#endif
}