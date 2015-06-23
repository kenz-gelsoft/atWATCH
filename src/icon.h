#ifndef INCLUDE_GUARD_BF8DD5EA_39D7_4A24_AF47_CD8EB7E5B369
#define INCLUDE_GUARD_BF8DD5EA_39D7_4A24_AF47_CD8EB7E5B369

#include "dithering.h"


typedef struct {
	DitheringPattern mColor;
	GRect mFromFrame;
	GRect mToFrame;
} icon_data;

typedef Layer Icon;

Icon *icon_create_with_data(GRect aFromFrame, GRect aToFrame, size_t aDataSize);
Icon *icon_create(GRect aFromFrame, GRect aToFrame);
void icon_destroy(Icon *aLayer);

DitheringPattern icon_get_color(Icon *aLayer);
GRect icon_get_from_frame(Icon *aLayer);
GRect icon_get_to_frame(Icon *aLayer);

void icon_zoom_in(Icon *aLayer);

#endif
