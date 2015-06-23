#include <pebble.h>

#include "icon.h"


static icon_data *icon_data_get(Icon *aIcon) {
	return (icon_data *)layer_get_data(aIcon);
}

static void anim_stopped(Animation *aAnimation, bool aFinished, void *aCtx) {
  property_animation_destroy((PropertyAnimation *)aAnimation);
}

static void zoom_out(Icon *aIcon, int32_t aDelay) {
  GRect toRect = icon_get_to_frame(aIcon);
  PropertyAnimation *animation = property_animation_create_layer_frame(aIcon, NULL, &toRect);
  animation_set_duration((Animation *)animation, 500);
  animation_set_delay((Animation *)animation, aDelay);
  animation_set_curve((Animation *)animation, AnimationCurveEaseInOut);
  animation_set_handlers((Animation *)animation, (AnimationHandlers) {
    .stopped = anim_stopped
  }, NULL);
  animation_schedule((Animation *)animation);
}

static void zoom_stopped(Animation *aAnimation, bool aFinished, void *aCtx) {
  property_animation_destroy((PropertyAnimation *)aAnimation);
  
  Icon *layer = (Icon *)aCtx;
  zoom_out(layer, 2500);
}

void icon_zoom_in(Icon *aIcon) {
  GRect fromFrame = icon_get_from_frame(aIcon);
  PropertyAnimation *animation = property_animation_create_layer_frame(aIcon, NULL, &fromFrame);
  animation_set_duration((Animation *)animation, 500);
  animation_set_delay((Animation *)animation, 300);
  animation_set_curve((Animation *)animation, AnimationCurveEaseInOut);
  animation_set_handlers((Animation *)animation, (AnimationHandlers) {
    .stopped = zoom_stopped
  }, aIcon);
  animation_schedule((Animation *)animation);
}

static void update_layer(Icon *aIcon, GContext *aCtx) {
  GRect r = layer_get_frame(aIcon);
  if (r.origin.x + r.size.w < 0 || 144 < r.origin.x ||
      r.origin.y + r.size.h < 0 || 168 < r.origin.y) {
    // 不可視
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
    // アニメーション中は枠線だけ描画する
    if (!zoomedIn) {
      graphics_context_set_stroke_color(aCtx, GColorWhite);
      graphics_draw_circle(aCtx, center, radius);
    }
  } else {
    fill_dithered_circle(aCtx, center, radius,
      icon_get_color(aIcon));
  }
}

Icon *icon_create(GRect aFromFrame, GRect aToFrame) {
  return icon_create_with_data(aFromFrame, aToFrame, sizeof(icon_data));
}

Icon *icon_create_with_data(GRect aFromFrame, GRect aToFrame, size_t aDataSize) {
	Icon *layer = layer_create_with_data(aFromFrame, aDataSize);
  layer_set_update_proc(layer, update_layer);

	icon_data *data = icon_data_get(layer);
	data->mColor = rand() % 4;
  data->mFromFrame = aFromFrame;
	data->mToFrame   = aToFrame;
  
  zoom_out(layer, 300);
  
	return layer;
}

void icon_destroy(Icon *aIcon) {
	layer_destroy(aIcon);
}

DitheringPattern icon_get_color(Icon *aIcon) {
	return icon_data_get(aIcon)->mColor;
}

GRect icon_get_from_frame(Icon *aIcon) {
	return icon_data_get(aIcon)->mFromFrame;
}
GRect icon_get_to_frame(Icon *aIcon) {
	return icon_data_get(aIcon)->mToFrame;
}
