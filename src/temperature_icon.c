#include "temperature_icon.h"
#include "common.h"


#define TEMPERATURE_FONT              FONT_KEY_GOTHIC_18_BOLD
#define TEMPERATURE_FONT_SIZE         18
#define TEMPERATURE_TEXT_ADJUSTMENT_X 2
#define TEMPERATURE_TEXT_ADJUSTMENT_Y 26
#define TEMPERATURE_CELCIUS_CONSTANT  273.15


static temperature_icon_data *temperature_icon_data_get(TemperatureIcon *aIcon) {
    return (temperature_icon_data *)layer_get_data(aIcon);
}

static void draw_degree(GContext *aCtx, GRect r, const char *buffer, int32_t dx, int32_t dy) {
    graphics_draw_text(aCtx, buffer,
        fonts_get_system_font(TEMPERATURE_FONT),
        GRect(dx, (r.size.h - TEMPERATURE_TEXT_ADJUSTMENT_Y) / 2 + dy,
            r.size.w + TEMPERATURE_TEXT_ADJUSTMENT_X, TEMPERATURE_FONT_SIZE),
        GTextOverflowModeWordWrap,
        GTextAlignmentCenter,
        NULL);
}

static void paint_temperature_icon(TemperatureIcon *aIcon, GContext *aCtx, GRect r, GPoint aCenter, int32_t aRadius, bool aZoomedIn) {
    int32_t temp = temperature_icon_get_temp(aIcon);
    
#ifdef PBL_COLOR
    graphics_context_set_fill_color(aCtx, GColorDukeBlue);
    graphics_fill_circle(aCtx, aCenter, aRadius);
#else
    graphics_context_set_stroke_color(aCtx, GColorWhite);
    fill_dithered_circle(aCtx, aCenter, aRadius, PATTERN_75_2);
#endif

    // text
    static char buffer[] = "-000°";
    snprintf(buffer, sizeof(buffer)/sizeof(buffer[0]), "%ld°", temp);

#ifdef PBL_COLOR
    graphics_context_set_text_color(aCtx, GColorElectricBlue);
#else
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
#endif
    draw_degree(aCtx, r, buffer, 0, 0);
}

TemperatureIcon *temperature_icon_create(GRect aFromFrame, GRect aToFrame) {
    TemperatureIcon *icon = icon_create_with_data(aFromFrame, aToFrame,
        sizeof(temperature_icon_data));
    icon_set_painter(icon, paint_temperature_icon);
    
    temperature_icon_data *data = temperature_icon_data_get(icon);
    data->mTemperature = TEMPERATURE_CELCIUS_CONSTANT;
    
    return icon;
}

void temperature_icon_destroy(TemperatureIcon *aIcon) {
    icon_destroy(aIcon);
}

int32_t temperature_icon_get_temp(TemperatureIcon *aIcon) {
    int32_t temp = temperature_icon_data_get(aIcon)->mTemperature;

    // Kelvin => Celcius Degree
    temp -= TEMPERATURE_CELCIUS_CONSTANT;

    return temp;
}

void temperature_icon_update(TemperatureIcon *aIcon, int32_t aTemperature) {
    temperature_icon_data *data = temperature_icon_data_get(aIcon);
    data->mTemperature = aTemperature;
    layer_mark_dirty(aIcon);
}
