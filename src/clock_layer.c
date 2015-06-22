#include "clock_layer.h"


static clock_layer_data *clock_layer_data_get(ClockLayer *aLayer) {
    return (clock_layer_data *)layer_get_data(aLayer);
}

typedef enum {
    LineWidth1,
    LineWidth2,
    LineWidth3
} LineWidth;

static void draw_width_line(GContext *aCtx, GPoint aPt1, GPoint aPt2, LineWidth aLineWidth) {
  int32_t x1 = aPt1.x;
  int32_t y1 = aPt1.y;
  int32_t x2 = aPt2.x;
  int32_t y2 = aPt2.y;
  switch (aLineWidth) {
  case LineWidth3:
      // 3x3 very bold line
      graphics_draw_line(aCtx, GPoint(x1 - 1, y1 - 1), GPoint(x2 - 1, y2 - 1));
      graphics_draw_line(aCtx, GPoint(x1    , y1 - 1), GPoint(x2    , y2 - 1));
      graphics_draw_line(aCtx, GPoint(x1 + 1, y1 - 1), GPoint(x2 + 1, y2 - 1));
      graphics_draw_line(aCtx, GPoint(x1 - 1, y1),     GPoint(x2 - 1, y2));
      graphics_draw_line(aCtx, GPoint(x1 - 1, y1 + 1), GPoint(x2 - 1, y2 + 1));
  case LineWidth2:
      // 2x2 bold line
      graphics_draw_line(aCtx, GPoint(x1 + 1, y1),     GPoint(x2 + 1, y2));
      graphics_draw_line(aCtx, GPoint(x1    , y1 + 1), GPoint(x2    , y2 + 1));
      graphics_draw_line(aCtx, GPoint(x1 + 1, y1 + 1), GPoint(x2 + 1, y2 + 1));
  case LineWidth1:
      // 1x1 normal line
      graphics_draw_line(aCtx, aPt1, aPt2);
  }
}

static void draw_angle_line(GContext *aCtx, GPoint aCenter, int32_t aAngle,
        int32_t aRadiusFrom, int32_t aRadiusTo, LineWidth aLineWidth) {
    GPoint pt1;
    if (aRadiusFrom == 0) {
        pt1 = aCenter;
    } else {
        pt1.y = (-cos_lookup(aAngle) * aRadiusFrom / TRIG_MAX_RATIO) + aCenter.y;
        pt1.x = ( sin_lookup(aAngle) * aRadiusFrom / TRIG_MAX_RATIO) + aCenter.x;
    }
    GPoint pt2;
    if (aRadiusTo == 0) {
        pt2 = aCenter;
    } else {
        pt2.y = (-cos_lookup(aAngle) * aRadiusTo / TRIG_MAX_RATIO) + aCenter.y;
        pt2.x = ( sin_lookup(aAngle) * aRadiusTo / TRIG_MAX_RATIO) + aCenter.x;
    }
    draw_width_line(aCtx, pt1, pt2, aLineWidth);
}

static void draw_clock_hand(GContext *aCtx, GPoint aCenter, int32_t aRadius,
        bool aZoomedIn, int32_t aHandLen, int32_t aAngle) {
    int32_t handLen2 = aRadius * 4 / 20;
    if (aZoomedIn) {
        draw_angle_line(aCtx, aCenter, aAngle, 3, handLen2, LineWidth1);
        draw_angle_line(aCtx, aCenter, aAngle, handLen2, aHandLen, LineWidth3);
    } else {
        draw_angle_line(aCtx, aCenter, aAngle, 0, aHandLen, LineWidth2);
    }
}

static void update_layer(ClockLayer *aLayer, GContext *aCtx) {
    GRect r = layer_get_frame(aLayer);
    if (r.origin.x + r.size.w < 0 || 144 < r.origin.x ||
        r.origin.y + r.size.h < 0 || 168 < r.origin.y) {
        // 不可視
        return;
    }
    GRect toFrame   = icon_layer_get_to_frame(aLayer);
    bool animating = !grect_equal(&r, &toFrame);
    GRect fromFrame = icon_layer_get_from_frame(aLayer);
    bool zoomedIn  =  grect_equal(&r, &fromFrame);
    
    GPoint center = GPoint(r.size.w / 2,
                           r.size.h / 2-1);
    uint16_t radius = r.size.w / 2 - 1;
    // 背景
    if (animating) {
        graphics_context_set_stroke_color(aCtx, GColorWhite);
        for (int32_t i = 0; i < 60; ++i) {
            int32_t len1 = radius;
            int32_t len2 = radius * ((i % 5 == 0) ? 16 : 19) / 20;
            int32_t angle = TRIG_MAX_ANGLE * i / 60.f;
            draw_angle_line(aCtx, center, angle, len1, len2,
                (i % 5 == 0) ? LineWidth2 : LineWidth1);
        }
        graphics_context_set_fill_color(aCtx, GColorWhite);
        graphics_context_set_stroke_color(aCtx, GColorWhite);
    } else {
        graphics_context_set_fill_color(aCtx, GColorWhite);
        graphics_fill_circle(aCtx, center, radius);
        graphics_context_set_fill_color(aCtx, GColorBlack);
        graphics_context_set_stroke_color(aCtx, GColorBlack);
    }
    
    clock_layer_data *data = layer_get_data(aLayer);
    int32_t h = data->h;
    int32_t m = data->m;
    int32_t s = data->s;
    
    // 時針
    draw_clock_hand(aCtx, center, radius, zoomedIn,
        radius * 12 / 20,
        TRIG_MAX_ANGLE * (h + m / 60.f) / 12.f);
    
    // 分針
    draw_clock_hand(aCtx, center, radius, zoomedIn,
        radius * 18 / 20,
        TRIG_MAX_ANGLE * (m + s / 60.f) / 60.f);
    
    // 秒針
    int32_t secLength = radius * 18 / 20;
    int32_t secAngle = TRIG_MAX_ANGLE * s / 60.f;
    if (zoomedIn) {
        draw_angle_line(aCtx, center, secAngle, 3, secLength, LineWidth1);
        graphics_draw_circle(aCtx, center, 2);
    } else {
        draw_angle_line(aCtx, center, secAngle, 0, secLength, LineWidth1);
    }
}

ClockLayer *clock_layer_create(GRect aFromFrame, GRect aToFrame) {
    ClockLayer *layer = icon_layer_create_with_data(aFromFrame, aToFrame,
        sizeof(clock_layer_data));
    layer_set_update_proc(layer, update_layer);
    
    return layer;
}

void clock_layer_destroy(ClockLayer *aLayer) {
    icon_layer_destroy(aLayer);
}

void clock_layer_update_time(ClockLayer *aLayer) {
    time_t temp = time(NULL);
    struct tm *tick_time = localtime(&temp);
    
    clock_layer_data *data = clock_layer_data_get(aLayer);
    data->h = tick_time->tm_hour % 12;
    data->m = tick_time->tm_min;
    data->s = tick_time->tm_sec;
    //APP_LOG(APP_LOG_LEVEL_DEBUG, "%d:%d", h, m);
    layer_mark_dirty(aLayer);
}
