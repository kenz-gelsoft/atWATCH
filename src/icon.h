#ifndef INCLUDE_GUARD_BF8DD5EA_39D7_4A24_AF47_CD8EB7E5B369
#define INCLUDE_GUARD_BF8DD5EA_39D7_4A24_AF47_CD8EB7E5B369

#include "dithering.h"


#ifdef PBL_COLOR
typedef GColor IconColor;
#else
typedef DitheringPattern IconColor;
#endif


typedef struct {
    IconColor mColor;
    GRect mFromFrame;
    GRect mToFrame;
} icon_data;

typedef Layer Icon;

Icon *icon_create_with_data(GRect aFromFrame, GRect aToFrame, size_t aDataSize);
Icon *icon_create(GRect aFromFrame, GRect aToFrame);
void icon_destroy(Icon *aIcon);

IconColor icon_get_color(Icon *aIcon);
GRect icon_get_from_frame(Icon *aIcon);
GRect icon_get_to_frame(Icon *aIcon);

void icon_zoom_in(Icon *aIcon);

#endif
