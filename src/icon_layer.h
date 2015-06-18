#ifndef INCLUDE_GUARD_BF8DD5EA_39D7_4A24_AF47_CD8EB7E5B369
#define INCLUDE_GUARD_BF8DD5EA_39D7_4A24_AF47_CD8EB7E5B369

#include "dithering.h"


typedef struct {
	DitheringPattern mColor;
} icon_layer_data;

typedef Layer IconLayer;

IconLayer *icon_layer_create(GRect frame);
void icon_layer_destroy(IconLayer *layer);

#endif
