#include "temperature_icon.h"
#include "common.h"


#define TEMPERATURE_FONT              FONT_KEY_GOTHIC_18_BOLD
#define TEMPERATURE_FONT_SIZE         18
#define TEMPERATURE_TEXT_ADJUSTMENT_X 1
#define TEMPERATURE_TEXT_ADJUSTMENT_Y 26


static temperature_icon_data *temperature_icon_data_get(TemperatureIcon *aIcon) {
    return (temperature_icon_data *)layer_get_data(aIcon);
}

static void draw_degree(GContext *aCtx, GRect r, const char *buffer, int32_t dx, int32_t dy) {
    graphics_draw_text(aCtx, buffer,
        fonts_get_system_font(TEMPERATURE_FONT),
        GRect(dx + TEMPERATURE_TEXT_ADJUSTMENT_X, (r.size.h - TEMPERATURE_TEXT_ADJUSTMENT_Y) / 2 + dy,
            r.size.w, TEMPERATURE_FONT_SIZE),
        GTextOverflowModeWordWrap,
        GTextAlignmentCenter,
        NULL);
}

static void paint_temperature_icon(TemperatureIcon *aIcon, GContext *aCtx, GRect r, GPoint aCenter, int32_t aRadius, bool aZoomedIn) {
    int32_t temp = temperature_icon_get_temp(aIcon);
    // Kelvin => Celcius Degree
    temp -= 273.15;
    
// #ifdef PBL_COLOR
//     graphics_context_set_fill_color(aCtx, GColorMalachite);
//     graphics_fill_circle(aCtx, aCenter, aRadius);
// #else
    graphics_context_set_stroke_color(aCtx, GColorWhite);
    fill_dithered_circle(aCtx, aCenter, aRadius, PATTERN_75_2);
// #endif

    // text
    static char buffer[] = "-00°";
    snprintf(buffer, sizeof(buffer)/sizeof(buffer[0]), "%ld°", temp);

    graphics_context_set_text_color(aCtx, GColorBlack);
    for (int dy = -1; dy <= +1; ++dy) {
        for (int dx = -1; dx <= +1; ++dx) {
            if (dx == 0 && dy == 0) {
                continue;
            }
            draw_degree(aCtx, r, buffer, dx, dy);
        }
    }
    graphics_context_set_text_color(aCtx, GColorWhite);
    draw_degree(aCtx, r, buffer, 0, 0);
}

TemperatureIcon *temperature_icon_create(GRect aFromFrame, GRect aToFrame) {
    TemperatureIcon *icon = icon_create_with_data(aFromFrame, aToFrame,
        sizeof(temperature_icon_data));
    icon_set_painter(icon, paint_temperature_icon);
    return icon;
}

void temperature_icon_destroy(TemperatureIcon *aIcon) {
    icon_destroy(aIcon);
}

int32_t temperature_icon_get_temp(TemperatureIcon *aIcon) {
    return temperature_icon_data_get(aIcon)->mTemperature;
}

void temperature_icon_update(TemperatureIcon *aIcon, int32_t aTemperature) {
    temperature_icon_data *data = temperature_icon_data_get(aIcon);
    data->mTemperature = aTemperature;
    layer_mark_dirty(aIcon);
}
