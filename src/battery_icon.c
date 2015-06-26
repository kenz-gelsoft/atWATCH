#include "battery_icon.h"
#include "common.h"


#define RING_WIDTH              0.2f
#define BATTERY_FONT            FONT_KEY_GOTHIC_18_BOLD
#define BATTERY_FONT_SIZE       18
#define BATTERY_TEXT_ADJUSTMENT 26


static battery_icon_data *battery_icon_data_get(BatteryIcon *aIcon) {
    return (battery_icon_data *)layer_get_data(aIcon);
}

static bool point_in_ring(int32_t x, int32_t y, int32_t radius) {
    float r2 = radius * (1.f-RING_WIDTH);
#ifdef PBL_COLOR
    radius++;
#endif
    return (x*x + y*y <= radius*radius) 
#ifndef PBL_COLOR
        && (x*x + y*y > r2*r2)
#endif
        ;
}

static bool point_in_arc(int32_t aPercent, int32_t x, int32_t y, int32_t radius) {
    int32_t a = atan2_lookup(x, -y);
    int32_t degree = TRIG_MAX_ANGLE * aPercent / 100;
    return (0 <= a && a < degree);
}

static void paint_battery_icon(BatteryIcon *aIcon, GContext *aCtx, GRect r, GPoint aCenter, int32_t aRadius, bool aZoomedIn) {
    graphics_context_set_stroke_color(aCtx, GColorWhite);
    uint8_t percent = battery_icon_get_percent(aIcon);
#ifdef PBL_COLOR
    graphics_context_set_fill_color(aCtx, GColorMalachite);
    graphics_fill_circle(aCtx, aCenter, aRadius);
#endif
    for (int32_t x = 0; x < r.size.w; ++x) {
        for (int32_t y = 0; y < r.size.h; ++y) {
            if (point_in_ring(x-aCenter.x, y-aCenter.y, aCenter.y)) {
                graphics_context_set_stroke_color(aCtx, GColorWhite);
                if (!point_in_arc(percent, x-aCenter.x, y-aCenter.y, aCenter.y)) {
#ifdef PBL_COLOR
                    graphics_context_set_stroke_color(aCtx, GColorDarkGray);
                    graphics_draw_pixel(aCtx, GPoint(x, y));
#else
                    if (pattern_25percent(x,y)) {
                        graphics_context_set_stroke_color(aCtx, GColorBlack);
                    }
#endif
                }
#ifndef PBL_COLOR
                graphics_draw_pixel(aCtx, GPoint(x, y));
#endif
            }
        }
    }
#ifdef PBL_COLOR
    graphics_context_set_fill_color(aCtx, GColorBlack);
    graphics_fill_circle(aCtx, aCenter, aRadius * (1.f-RING_WIDTH));
#endif

    // text
    static char buffer[] = "100";
    snprintf(buffer, sizeof(buffer)/sizeof(buffer[0]), "%d", percent);
    graphics_draw_text(aCtx, buffer,
        fonts_get_system_font(BATTERY_FONT),
        GRect(0, (r.size.h - BATTERY_TEXT_ADJUSTMENT) / 2,
            r.size.w, BATTERY_FONT_SIZE),
        GTextOverflowModeWordWrap,
        GTextAlignmentCenter,
        NULL);
}

BatteryIcon *battery_icon_create(GRect aFromFrame, GRect aToFrame) {
    BatteryIcon *icon = icon_create_with_data(aFromFrame, aToFrame,
        sizeof(battery_icon_data));
    icon_set_painter(icon, paint_battery_icon);
    return icon;
}

void battery_icon_destroy(BatteryIcon *aIcon) {
    icon_destroy(aIcon);
}

uint8_t battery_icon_get_percent(BatteryIcon *aIcon) {
    return battery_icon_data_get(aIcon)->mPercent;
}

void battery_icon_update(BatteryIcon *aIcon, uint8_t aPercent) {
    battery_icon_data *data = battery_icon_data_get(aIcon);
    data->mPercent = aPercent;
    layer_mark_dirty(aIcon);
}
