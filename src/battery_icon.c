#include "battery_icon.h"


static battery_icon_data *battery_icon_data_get(BatteryIcon *aIcon) {
    return (battery_icon_data *)layer_get_data(aIcon);
}

static bool point_in_ring(int32_t x, int32_t y, int32_t radius) {
  float r2 = radius * 0.8f;
  return (x*x + y*y <= radius*radius) &&
         (x*x + y*y > r2*r2);
}

static bool point_in_arc(int32_t aPercent, int32_t x, int32_t y, int32_t radius) {
  int32_t a = atan2_lookup(x, -y);
  int32_t degree = TRIG_MAX_ANGLE * aPercent / 100;
  return (0 <= a && a < degree);
}

static void update_layer(BatteryIcon *aIcon, GContext *aCtx) {
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
    graphics_context_set_stroke_color(aCtx, GColorWhite);
    if (animating) {
        if (!zoomedIn) {
            graphics_draw_circle(aCtx, center, radius);
        }
    } else {
        uint8_t percent = battery_icon_get_percent(aIcon);
        for (int32_t x = 0; x < r.size.w; ++x) {
            for (int32_t y = 0; y < r.size.h; ++y) {
                if (point_in_ring(x-center.x, y-center.y, center.y)) {
                    graphics_context_set_stroke_color(aCtx, GColorWhite);
                    if (!point_in_arc(percent, x-center.x, y-center.y, center.y)) {
                        if (pattern_25percent(x,y)) {
                            graphics_context_set_stroke_color(aCtx, GColorBlack);
                        }
                    }
                    graphics_draw_pixel(aCtx, GPoint(x, y));
                }
            }
        }
    
        // text
        static char buffer[] = "100";
        snprintf(buffer, sizeof(buffer)/sizeof(buffer[0]), "%d", percent);
        graphics_draw_text(aCtx, buffer,
            fonts_get_system_font(FONT_KEY_GOTHIC_18_BOLD),
            GRect(0, (r.size.h - 26) / 2, r.size.w, 18),
            GTextOverflowModeWordWrap,
            GTextAlignmentCenter,
            NULL);
    }
}

BatteryIcon *battery_icon_create(GRect aFromFrame, GRect aToFrame) {
    BatteryIcon *icon = icon_create_with_data(aFromFrame, aToFrame,
        sizeof(battery_icon_data));
    layer_set_update_proc(icon, update_layer);
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
