#include "calendar_icon.h"
#include "common.h"


#define WEEKDAY_CENTER_Y(h) ((h) / 5)
#define WEEKDAY_ICON_WIDTH  15
#define WEEKDAY_ICON_HEIGHT 7

#define CALENDAR_CENTER_Y(h)        ((h) * 3 / 5)
#define CALENDAR_FONT               FONT_KEY_ROBOTO_CONDENSED_21
#define CALENDAR_FONT_SIZE          21
#define CALENDAR_TEXT_ADJUSTMENT    14


#define WEEKDAY_IMAGE(n) \
    case n: return gbitmap_create_with_resource(RESOURCE_ID_WEEKDAY##n);
static GBitmap *bitmap_for_weekday(uint8_t aWeekday) {
    switch (aWeekday) {
    WEEKDAY_IMAGE(0);
    WEEKDAY_IMAGE(1);
    WEEKDAY_IMAGE(2);
    WEEKDAY_IMAGE(3);
    WEEKDAY_IMAGE(4);
    WEEKDAY_IMAGE(5);
    WEEKDAY_IMAGE(6);
    }
    return NULL;
}

static calendar_icon_data *calendar_icon_data_get(CalendarIcon *aIcon) {
    return (calendar_icon_data *)layer_get_data(aIcon);
}

static void update_layer(CalendarIcon *aIcon, GContext *aCtx) {
    GRect r = layer_get_frame(aIcon);
    if (r.origin.x + r.size.w < 0 || SCREEN_WIDTH  < r.origin.x ||
        r.origin.y + r.size.h < 0 || SCREEN_HEIGHT < r.origin.y) {
        // invisible
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
        if (!zoomedIn) {
            graphics_context_set_stroke_color(aCtx, GColorWhite);
            graphics_draw_circle(aCtx, center, radius);
        }
    } else {
        // background
        graphics_context_set_fill_color(aCtx, GColorWhite);
        graphics_fill_circle(aCtx, center, radius);
        graphics_context_set_text_color(aCtx, GColorBlack);
        
        uint8_t date = calendar_icon_get_date(aIcon);
        
        GBitmap* weekday = calendar_icon_get_weekday(aIcon);
        graphics_draw_bitmap_in_rect(aCtx, weekday, GRect(
            1 + (r.size.w - WEEKDAY_ICON_WIDTH) / 2,
            WEEKDAY_CENTER_Y(r.size.h) - WEEKDAY_ICON_HEIGHT / 2,
            WEEKDAY_ICON_WIDTH, WEEKDAY_ICON_HEIGHT));
    
        // text
        static char buffer[] = "00";
        snprintf(buffer, sizeof(buffer)/sizeof(buffer[0]), "%d", date);
        graphics_draw_text(aCtx, buffer,
            fonts_get_system_font(CALENDAR_FONT),
            GRect(1, 1 + CALENDAR_CENTER_Y(r.size.h) - CALENDAR_TEXT_ADJUSTMENT,
                r.size.w, CALENDAR_FONT_SIZE),
            GTextOverflowModeWordWrap,
            GTextAlignmentCenter,
            NULL);
    }
}

CalendarIcon *calendar_icon_create(GRect aFromFrame, GRect aToFrame) {
    CalendarIcon *icon = icon_create_with_data(aFromFrame, aToFrame,
        sizeof(calendar_icon_data));
    layer_set_update_proc(icon, update_layer);
    return icon;
}

void calendar_icon_destroy(CalendarIcon *aIcon) {
    calendar_icon_data *data = calendar_icon_data_get(aIcon);
    if (data->mWeekday) {
        gbitmap_destroy(data->mWeekday);
    }
    icon_destroy(aIcon);
}

uint8_t calendar_icon_get_date(CalendarIcon *aIcon) {
    return calendar_icon_data_get(aIcon)->mDate;
}
GBitmap *calendar_icon_get_weekday(CalendarIcon *aIcon) {
    return calendar_icon_data_get(aIcon)->mWeekday;
}

void calendar_icon_update(CalendarIcon *aIcon) {
    time_t temp = time(NULL);
    struct tm *tick_time = localtime(&temp);

    calendar_icon_data *data = calendar_icon_data_get(aIcon);
    uint8_t oldDate = data->mDate;
    if (oldDate != tick_time->tm_mday) {
        data->mDate = tick_time->tm_mday;
        if (data->mWeekday) {
            gbitmap_destroy(data->mWeekday);
        }
        data->mWeekday = bitmap_for_weekday(tick_time->tm_wday);
        layer_mark_dirty(aIcon);
    }
}
