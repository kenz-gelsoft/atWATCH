#include <pebble.h>

#include "common.h"
#include "icon.h"


#define ANIMATION_DURATION  500
#define ZOOM_DELAY          300
#define ZOOM_STOP_DELAY     2500


static icon_data *icon_data_get(Icon *aIcon) {
    return (icon_data *)layer_get_data(aIcon);
}

static IconAnimationDoneHandler animation_done_handler(Icon *aIcon) {
    return icon_data_get(aIcon)->mAnimationDoneHandler;
}
static void anim_stopped(Animation *aAnimation, bool aFinished, void *aCtx) {
    Icon *icon = (Icon *)aCtx;
    icon_set_animating(icon, false);
    
    IconAnimationDoneHandler done_handler = animation_done_handler(icon);
    if (done_handler) {
        done_handler(icon);
    }
    icon_data_get(icon)->mAnimationDoneHandler = NULL;
    
    property_animation_destroy((PropertyAnimation *)aAnimation);
}

static void zoom_out(Icon *aIcon, int32_t aDelay) {
    icon_set_animating(aIcon, true);
    
    GRect toRect = icon_get_to_frame(aIcon);
    PropertyAnimation *animation = property_animation_create_layer_frame(aIcon, NULL, &toRect);
    animation_set_duration((Animation *)animation, ANIMATION_DURATION);
    animation_set_delay((Animation *)animation, aDelay);
    animation_set_curve((Animation *)animation, AnimationCurveEaseInOut);
    animation_set_handlers((Animation *)animation, (AnimationHandlers) {
        .stopped = anim_stopped
    }, aIcon);
    animation_schedule((Animation *)animation);
}

int32_t icon_zoom_in_timeout() {
    if (!persist_exists(zoomInTimeout)) {
        return ZOOM_STOP_DELAY;
    }
    return persist_read_int(zoomInTimeout);
}

static void zoom_stopped(Animation *aAnimation, bool aFinished, void *aCtx) {
    property_animation_destroy((PropertyAnimation *)aAnimation);
    
    Icon *icon = (Icon *)aCtx;
    zoom_out(icon, icon_zoom_in_timeout());
}

void icon_zoom_in(Icon *aIcon, IconAnimationDoneHandler aDoneHandler) {
    icon_set_animating(aIcon, true);
    
    icon_data_get(aIcon)->mAnimationDoneHandler = aDoneHandler;
    
    GRect fromFrame = icon_get_from_frame(aIcon);
    PropertyAnimation *animation = property_animation_create_layer_frame(aIcon, NULL, &fromFrame);
    animation_set_duration((Animation *)animation, ANIMATION_DURATION);
    animation_set_delay((Animation *)animation, ZOOM_DELAY);
    animation_set_curve((Animation *)animation, AnimationCurveEaseInOut);
    animation_set_handlers((Animation *)animation, (AnimationHandlers) {
        .stopped = zoom_stopped
    }, aIcon);
    animation_schedule((Animation *)animation);
}

static void update_layer(Icon *aIcon, GContext *aCtx) {
    GRect r = layer_get_frame(aIcon);
    if (r.origin.x + r.size.w < 0 || SCREEN_WIDTH  < r.origin.x ||
        r.origin.y + r.size.h < 0 || SCREEN_HEIGHT < r.origin.y) {
        // invisible
        return;
    }
    GRect finalRect = icon_get_to_frame(aIcon);
    bool animating = !grect_equal(&r, &finalRect);
    GRect fromFrame = icon_get_from_frame(aIcon);
    bool zoomedIn  =  grect_equal(&r, &fromFrame);
  
    GPoint center = GPoint(r.size.w / 2,
                           r.size.h / 2-1);
    uint16_t radius = r.size.w / 2 - 1;
    if (animating) {
        icon_get_animating_painter(aIcon)(aIcon, aCtx, r, center, radius, zoomedIn);
    } else {
        icon_get_painter(aIcon)(aIcon, aCtx, r, center, radius, zoomedIn);
    }
}

static void paint_animating_icon(Icon *aIcon, GContext *aCtx, GRect aFrame, GPoint aCenter, int32_t aRadius, bool aZoomedIn) {
    // draws only borders on animation
    if (!aZoomedIn) {
        graphics_context_set_stroke_color(aCtx, GColorWhite);
        graphics_draw_circle(aCtx, aCenter, aRadius);
    }
}
static void paint_icon(Icon *aIcon, GContext *aCtx, GRect aFrame, GPoint aCenter, int32_t aRadius, bool aZoomedIn) {
#ifdef PBL_COLOR
    graphics_context_set_fill_color(aCtx, icon_get_color(aIcon));
    graphics_fill_circle(aCtx, aCenter, aRadius);
#else
    fill_dithered_circle(aCtx, aCenter, aRadius,
        icon_get_color(aIcon));
#endif
}

Icon *icon_create(GRect aFromFrame, GRect aToFrame) {
    return icon_create_with_data(aFromFrame, aToFrame, sizeof(icon_data));
}

Icon *icon_create_with_data(GRect aFromFrame, GRect aToFrame, size_t aDataSize) {
  	Icon *icon = layer_create_with_data(aFromFrame, aDataSize);
    layer_set_update_proc(icon, update_layer);
    icon_set_painter(icon, paint_icon);
    icon_set_animating_painter(icon, paint_animating_icon);

  	icon_data *data = icon_data_get(icon);
    data->mColor = icon_color_default();
    data->mFromFrame = aFromFrame;
  	data->mToFrame   = aToFrame;
    
    zoom_out(icon, ZOOM_DELAY);
    
  	return icon;
}

void icon_destroy(Icon *aIcon) {
  	layer_destroy(aIcon);
}

IconColor icon_get_color(Icon *aIcon) {
  	return icon_data_get(aIcon)->mColor;
}
void icon_set_color(Icon *aIcon, IconColor aColor) {
    icon_data_get(aIcon)->mColor = aColor;
}

GRect icon_get_from_frame(Icon *aIcon) {
  	return icon_data_get(aIcon)->mFromFrame;
}
GRect icon_get_to_frame(Icon *aIcon) {
  	return icon_data_get(aIcon)->mToFrame;
}

void icon_set_animating(Icon *aIcon, bool aAnimating) {
    icon_data_get(aIcon)->mAnimating = aAnimating;
}
bool icon_is_animating(Icon *aIcon) {
    return icon_data_get(aIcon)->mAnimating;
}

void icon_set_painter(Icon *aIcon, IconPainter aIconPainter) {
    icon_data_get(aIcon)->mPainter = aIconPainter;
}
IconPainter icon_get_painter(Icon *aIcon) {
    return icon_data_get(aIcon)->mPainter;
}

void icon_set_animating_painter(Icon *aIcon, IconPainter aIconPainter) {
    icon_data_get(aIcon)->mAnimatingPainter = aIconPainter;
}
IconPainter icon_get_animating_painter(Icon *aIcon) {
    return icon_data_get(aIcon)->mAnimatingPainter;
}
