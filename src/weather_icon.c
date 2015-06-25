#include "common.h"
#include "dithering.h"
#include "weather_icon.h"


#define WEATHER_ICON_SIZE   24


typedef enum {
    WeatherClearSky,
    WeatherFewClouds,
    WeatherScatteredClouds,
    WeatherRain,
} WeatherImage;

#define WEATHER_IMAGE(n) \
    case n: return gbitmap_create_with_resource(RESOURCE_ID_WEATHER##n);
static GBitmap *bitmap_for_weather(uint8_t aWeather) {
    switch (aWeather) {
    WEATHER_IMAGE(0);
    WEATHER_IMAGE(1);
    WEATHER_IMAGE(2);
    WEATHER_IMAGE(3);
    }
    return NULL;
}

#define WEATHER_MASK(n) \
    case n: return gbitmap_create_with_resource(RESOURCE_ID_WEATHER##n##_MASK);
static GBitmap *mask_for_weather(uint8_t aWeather) {
    switch (aWeather) {
    WEATHER_MASK(0);
    WEATHER_MASK(1);
    WEATHER_MASK(2);
    WEATHER_MASK(3);
    }
    return NULL;
}

WeatherImage weather_for_id(int32_t aWeatherId) {
    // see about weather IDs at
    // http://openweathermap.org/weather-conditions#Weather-Condition-Codes-2
    if (200 <= aWeatherId && aWeatherId < 700) {
        return WeatherRain;
    }
    if (700 <= aWeatherId && aWeatherId < 800) {
        return WeatherFewClouds;
    }
    switch (aWeatherId) {
        case 800: return WeatherClearSky;
        case 801: return WeatherFewClouds;
        case 802:
        case 803:
        case 804: return WeatherScatteredClouds;
    }
    // TODO: error handling
    return WeatherFewClouds;
}

static weather_icon_data *weather_icon_data_get(WeatherIcon *aIcon) {
    return (weather_icon_data *)layer_get_data(aIcon);
}

static void update_layer(WeatherIcon *aIcon, GContext *aCtx) {
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
        fill_dithered_circle(aCtx, center, radius, Dithering50Percent);
            
        GBitmap* mask = weather_icon_get_mask(aIcon);
        graphics_context_set_compositing_mode(aCtx, GCompOpClear);
        for (int dx = -1; dx <= +1; ++dx) {
            for (int dy = -1; dy <= +1; ++dy) {
                graphics_draw_bitmap_in_rect(aCtx, mask, GRect(
                    dx + 1 + (r.size.w - WEATHER_ICON_SIZE) / 2,
                    dy + (r.size.h - WEATHER_ICON_SIZE) / 2,
                    WEATHER_ICON_SIZE, WEATHER_ICON_SIZE));
            }
        }
            
        GBitmap* weather = weather_icon_get_weather(aIcon);
        graphics_context_set_compositing_mode(aCtx, GCompOpSet);
        graphics_draw_bitmap_in_rect(aCtx, weather, GRect(
            1 + (r.size.w - WEATHER_ICON_SIZE) / 2,
            (r.size.h - WEATHER_ICON_SIZE) / 2,
            WEATHER_ICON_SIZE, WEATHER_ICON_SIZE));
    }
}

WeatherIcon *weather_icon_create(GRect aFromFrame, GRect aToFrame) {
    WeatherIcon *icon = icon_create_with_data(aFromFrame, aToFrame,
        sizeof(weather_icon_data));
    layer_set_update_proc(icon, update_layer);
    
    weather_icon_data *data = weather_icon_data_get(icon);
    data->mWeather = bitmap_for_weather(WeatherFewClouds);
    data->mMask    = mask_for_weather(WeatherFewClouds);
    return icon;
}

void weather_icon_destroy(WeatherIcon *aIcon) {
    weather_icon_data *data = weather_icon_data_get(aIcon);
    if (data->mWeather) {
        gbitmap_destroy(data->mWeather);
    }
    if (data->mMask) {
        gbitmap_destroy(data->mMask);
    }
    icon_destroy(aIcon);
}

GBitmap *weather_icon_get_weather(WeatherIcon *aIcon) {
    return weather_icon_data_get(aIcon)->mWeather;
}
GBitmap *weather_icon_get_mask(WeatherIcon *aIcon) {
    return weather_icon_data_get(aIcon)->mMask;
}

void weather_icon_update(WeatherIcon *aIcon, int32_t aWeatherId) {
    weather_icon_data *data = weather_icon_data_get(aIcon);
    if (data->mWeather) {
        gbitmap_destroy(data->mWeather);
    }
    if (data->mMask) {
        gbitmap_destroy(data->mMask);
    }
    WeatherImage w = weather_for_id(aWeatherId);
    data->mWeather = bitmap_for_weather(w);
    data->mMask    = mask_for_weather(w);
}