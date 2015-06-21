#include "clock_layer.h"


static clock_layer_data *clock_layer_data_get(ClockLayer *aLayer) {
    return (clock_layer_data *)layer_get_data(aLayer);
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
            GPoint tickOuter;
            tickOuter.y = (-cos_lookup(angle) * len1 / TRIG_MAX_RATIO) + center.y;
            tickOuter.x = ( sin_lookup(angle) * len1 / TRIG_MAX_RATIO) + center.x;
            GPoint tickInner;
            tickInner.y = (-cos_lookup(angle) * len2 / TRIG_MAX_RATIO) + center.y;
            tickInner.x = ( sin_lookup(angle) * len2 / TRIG_MAX_RATIO) + center.x;
            if (i % 5 == 0) {
                draw_bold_line(aCtx, tickInner, tickOuter);                
            } else {
                graphics_draw_line(aCtx, tickInner, tickOuter);
            }
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
