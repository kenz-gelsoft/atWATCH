#include <pebble.h>

#include "icon_layer.h"


static int h=0, m=0, s=0;


void update_time2() {
  time_t temp = time(NULL);
  struct tm *tick_time = localtime(&temp);
  h = tick_time->tm_hour % 12;
  m = tick_time->tm_min;
  s = tick_time->tm_sec;
  //APP_LOG(APP_LOG_LEVEL_DEBUG, "%d:%d", h, m);
}


static icon_layer_data *icon_layer_data_get(IconLayer *aLayer) {
	return (icon_layer_data *)layer_get_data(aLayer);
}

static void draw_bold_line(GContext *aCtx, GPoint aPt1, GPoint aPt2) {
  int32_t x1 = aPt1.x;
  int32_t y1 = aPt1.y;
  int32_t x2 = aPt2.x;
  int32_t y2 = aPt2.y;
  graphics_draw_line(aCtx, aPt1, aPt2);
  graphics_draw_line(aCtx, GPoint(x1 + 1, y1),     GPoint(x2 + 1, y2));
  graphics_draw_line(aCtx, GPoint(x1,     y1 + 1), GPoint(x2    , y2 + 1));
  graphics_draw_line(aCtx, GPoint(x1 + 1, y1 + 1), GPoint(x2 + 1, y2 + 1));
}

static void update_layer(IconLayer *aLayer, GContext *aCtx) {
  GRect r = layer_get_frame(aLayer);
  if (r.origin.x + r.size.w < 0 || 144 < r.origin.x ||
      r.origin.y + r.size.h < 0 || 168 < r.origin.y) {
    // 不可視
    return;
  }
  int layerNo = icon_layer_get_index(aLayer);
  // APP_LOG(APP_LOG_LEVEL_DEBUG, "layer[%d] = (%d, %d, %d, %d)",
  //   layerNo, r.origin.x, r.origin.y, r.size.w, r.size.h);
  GRect finalRect = icon_layer_get_to_frame(aLayer);
  bool animating = !grect_equal(&r, &finalRect);

  GPoint center = GPoint(r.size.w / 2,
                         r.size.h / 2-1);
  uint16_t radius = r.size.w / 2 - 1;
  if (layerNo == CLOCK_LAYER) {
    // 背景
    if (animating) {
      graphics_context_set_stroke_color(aCtx, GColorWhite);
      graphics_draw_circle(aCtx, center, radius);
      graphics_context_set_fill_color(aCtx, GColorWhite);
      graphics_context_set_stroke_color(aCtx, GColorWhite);
    } else {
      graphics_context_set_fill_color(aCtx, GColorWhite);
      graphics_fill_circle(aCtx, center, radius);
      graphics_context_set_fill_color(aCtx, GColorBlack);
      graphics_context_set_stroke_color(aCtx, GColorBlack);
    }

    // 時針
    int32_t hourLength = radius * 12 / 20;
    int32_t hourAngle = TRIG_MAX_ANGLE * (h + m / 60.f) / 12.f;
    GPoint hourHand;
    hourHand.y = (-cos_lookup(hourAngle) * hourLength / TRIG_MAX_RATIO) + center.y;
    hourHand.x = ( sin_lookup(hourAngle) * hourLength / TRIG_MAX_RATIO) + center.x;
    draw_bold_line(aCtx, center, hourHand);
    
    // 分針
    int32_t minLength = radius * 18 / 20;
    int32_t minAngle = TRIG_MAX_ANGLE * (m + s / 60.f) / 60.f;
    GPoint minHand;
    minHand.y = (-cos_lookup(minAngle) * minLength / TRIG_MAX_RATIO) + center.y;
    minHand.x = ( sin_lookup(minAngle) * minLength / TRIG_MAX_RATIO) + center.x;
    draw_bold_line(aCtx, center, minHand);

    
    // 秒針
    int32_t secLength = radius * 18 / 20;
    int32_t secAngle = TRIG_MAX_ANGLE * s / 60.f;
    GPoint secHand;
    secHand.y = (-cos_lookup(secAngle) * secLength / TRIG_MAX_RATIO) + center.y;
    secHand.x = ( sin_lookup(secAngle) * secLength / TRIG_MAX_RATIO) + center.x;
    graphics_draw_line(aCtx, center, secHand);
  } else {
    if (animating) {
      // アニメーション中は枠線だけ描画する
      graphics_context_set_stroke_color(aCtx, GColorWhite);
      graphics_draw_circle(aCtx, center, radius);
    } else {
      fill_dithered_circle(aCtx, center, radius,
        icon_layer_get_color(aLayer));
    }
  }
}


IconLayer *icon_layer_create(int32_t aIndex, GRect aFromFrame, GRect aToFrame) {
	IconLayer *layer = layer_create_with_data(aFromFrame, sizeof(icon_layer_data));
  layer_set_update_proc(layer, update_layer);

	icon_layer_data *data = icon_layer_data_get(layer);
  data->mIndex = aIndex;
	data->mColor = rand() % 3;
	data->mToFrame = aToFrame;
  
	return layer;
}

void icon_layer_destroy(IconLayer *aLayer) {
	layer_destroy(aLayer);
}

int32_t icon_layer_get_index(IconLayer *aLayer) {
  return icon_layer_data_get(aLayer)->mIndex;
}

DitheringPattern icon_layer_get_color(IconLayer *aLayer) {
	return icon_layer_data_get(aLayer)->mColor;
}

GRect icon_layer_get_to_frame(IconLayer *aLayer) {
	return icon_layer_data_get(aLayer)->mToFrame;
}
