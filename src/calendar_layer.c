#include "calendar_layer.h"


static calendar_layer_data *calendar_layer_data_get(CalendarLayer *aLayer) {
    return (calendar_layer_data *)layer_get_data(aLayer);
}

static void update_layer(CalendarLayer *aLayer, GContext *aCtx) {
    GRect r = layer_get_frame(aLayer);
    if (r.origin.x + r.size.w < 0 || 144 < r.origin.x ||
        r.origin.y + r.size.h < 0 || 168 < r.origin.y) {
        // 不可視
        return;
    }
    GRect finalRect = icon_layer_get_to_frame(aLayer);
    bool animating = !grect_equal(&r, &finalRect);
    
    GPoint center = GPoint(r.size.w / 2,
                           r.size.h / 2-1);
    uint16_t radius = r.size.w / 2 - 1;
    if (animating) {
        graphics_context_set_stroke_color(aCtx, GColorWhite);
        graphics_draw_circle(aCtx, center, radius);
    } else {
        // 背景
        graphics_context_set_fill_color(aCtx, GColorWhite);
        graphics_fill_circle(aCtx, center, radius);
        graphics_context_set_text_color(aCtx, GColorBlack);
        
        uint8_t date = calendar_layer_get_date(aLayer);
    
        // text
        static char buffer[] = "00";
        snprintf(buffer, sizeof(buffer)/sizeof(buffer[0]), "%d", date);
        graphics_draw_text(aCtx, buffer,
            fonts_get_system_font(FONT_KEY_ROBOTO_CONDENSED_21),
            GRect(1, (r.size.h - 28) / 2, r.size.w, 21),
            GTextOverflowModeWordWrap,
            GTextAlignmentCenter,
            NULL);
    }
}

CalendarLayer *calendar_layer_create(GRect aFromFrame, GRect aToFrame) {
    CalendarLayer *layer = icon_layer_create_with_data(aFromFrame, aToFrame,
        sizeof(calendar_layer_data));
    layer_set_update_proc(layer, update_layer);
    return layer;
}

void calendar_layer_destroy(CalendarLayer *aLayer) {
    icon_layer_destroy(aLayer);
}

uint8_t calendar_layer_get_date(CalendarLayer *aLayer) {
    return calendar_layer_data_get(aLayer)->mDate;
}

void calendar_layer_update(CalendarLayer *aLayer) {
    time_t temp = time(NULL);
    struct tm *tick_time = localtime(&temp);

    calendar_layer_data *data = calendar_layer_data_get(aLayer);
    uint8_t oldDate = data->mDate;
    if (oldDate != tick_time->tm_mday) {
        data->mDate = tick_time->tm_mday;
    }
    layer_mark_dirty(aLayer);
}
