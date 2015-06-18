#include <pebble.h>

#include "icon_layer.h"


static icon_layer_data *icon_layer_data_get(IconLayer *aLayer) {
	return (icon_layer_data *)layer_get_data(aLayer);
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
  if (animating) {
    // アニメーション中は枠線だけ描画する
    graphics_context_set_stroke_color(aCtx, GColorWhite);
    graphics_draw_circle(aCtx, center, radius);
  } else {
    fill_dithered_circle(aCtx, center, radius,
      icon_layer_get_color(aLayer));
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
