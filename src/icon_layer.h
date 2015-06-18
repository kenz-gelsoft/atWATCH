#ifndef INCLUDE_GUARD_BF8DD5EA_39D7_4A24_AF47_CD8EB7E5B369
#define INCLUDE_GUARD_BF8DD5EA_39D7_4A24_AF47_CD8EB7E5B369

#include "dithering.h"


typedef struct {
	DitheringPattern mColor;
	GRect mToFrame;
} icon_layer_data;

typedef Layer IconLayer;

IconLayer *icon_layer_create(GRect aFromFrame, GRect aToFrame);
void icon_layer_destroy(IconLayer *aLayer);

DitheringPattern icon_layer_get_color(IconLayer *aLayer);
GRect icon_layer_get_to_frame(IconLayer *aLayer);

#endif
