#ifndef INCLUDE_GUARD_BF8DD5EA_39D7_4A24_AF47_CD8EB7E5B369
#define INCLUDE_GUARD_BF8DD5EA_39D7_4A24_AF47_CD8EB7E5B369

#include "dithering.h"


#define CLOCK_LAYER 9
void update_time2();
  // TODO 本来であればこの中にあるべきではない

typedef struct {
	int32_t mIndex;
	DitheringPattern mColor;
	GRect mToFrame;
} icon_layer_data;

typedef Layer IconLayer;

IconLayer *icon_layer_create(int32_t aIndex, GRect aFromFrame, GRect aToFrame);
void icon_layer_destroy(IconLayer *aLayer);

int32_t icon_layer_get_index(IconLayer *aLayer);
DitheringPattern icon_layer_get_color(IconLayer *aLayer);
GRect icon_layer_get_to_frame(IconLayer *aLayer);

#endif
