#ifndef INCLUDE_GUARD_5FCCE401_15A3_4A58_8136_CE29F57C3700
#define INCLUDE_GUARD_5FCCE401_15A3_4A58_8136_CE29F57C3700

#include <pebble.h>
#include "dithering.h"


#ifdef PBL_COLOR
typedef GColor IconColor;
#else
typedef DitheringPattern IconColor;
#endif


IconColor *icon_colors_create(uint8_t *aOutCount);
void icon_colors_destroy(IconColor *aIconColors);

IconColor icon_color_default();


#endif