#ifndef INCLUDE_GUARD_BF8DD5EA_39D7_4A24_AF47_CD8EB7E5B369
#define INCLUDE_GUARD_BF8DD5EA_39D7_4A24_AF47_CD8EB7E5B369

#include "dithering.h"
#include "icon_colors.h"


typedef Layer Icon;

typedef void (*IconPainter)(Icon *aIcon, GContext *aCtx, GRect aFrame, GPoint aCenter, int32_t aRadius, bool aZoomedIn);
typedef void (*IconAnimationDoneHandler)(Icon *aIcon);

typedef struct {
    IconColor mColor;
    GRect mFromFrame;
    GRect mToFrame;
    IconPainter mPainter;
    IconPainter mAnimatingPainter;
    bool mAnimating;
    IconAnimationDoneHandler mAnimationDoneHandler;
} icon_data;

Icon *icon_create_with_data(GRect aFromFrame, GRect aToFrame, size_t aDataSize);
Icon *icon_create(GRect aFromFrame, GRect aToFrame);
void icon_destroy(Icon *aIcon);

IconColor icon_get_color(Icon *aIcon);
void icon_set_color(Icon *aIcon, IconColor aIconColor);

GRect icon_get_from_frame(Icon *aIcon);
GRect icon_get_to_frame(Icon *aIcon);

void icon_set_painter(Icon *aIcon, IconPainter aIconPainter);
IconPainter icon_get_painter(Icon *aIcon);
void icon_set_animating_painter(Icon *aIcon, IconPainter aIconPainter);
IconPainter icon_get_animating_painter(Icon *aIcon);

bool icon_is_animating(Icon *aIcon);
void icon_set_animating(Icon *aIcon, bool aAnimating);

void icon_zoom_in(Icon *aIcon, IconAnimationDoneHandler aDoneHandler);

#endif
