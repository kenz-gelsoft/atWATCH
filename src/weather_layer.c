#include "weather_layer.h"
#include "dithering.h"


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

static weather_layer_data *weather_layer_data_get(WeatherLayer *aLayer) {
    return (weather_layer_data *)layer_get_data(aLayer);
}

static void update_layer(WeatherLayer *aLayer, GContext *aCtx) {
    GRect r = layer_get_frame(aLayer);
    if (r.origin.x + r.size.w < 0 || 144 < r.origin.x ||
        r.origin.y + r.size.h < 0 || 168 < r.origin.y) {
        // 不可視
        return;
    }
    GRect finalRect = icon_layer_get_to_frame(aLayer);
    bool animating = !grect_equal(&r, &finalRect);
    GRect fromFrame = icon_layer_get_from_frame(aLayer);
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
        // 背景
        fill_dithered_circle(aCtx, center, radius, Dithering50Percent);
            
        GBitmap* mask = weather_layer_get_mask(aLayer);
        graphics_context_set_compositing_mode(aCtx, GCompOpClear);
        graphics_draw_bitmap_in_rect(aCtx, mask, GRect(
            1 + (r.size.w - 24) / 2,
            (r.size.h - 24) / 2,
            24, 24));
            
        GBitmap* weather = weather_layer_get_weather(aLayer);
        graphics_context_set_compositing_mode(aCtx, GCompOpSet);
        graphics_draw_bitmap_in_rect(aCtx, weather, GRect(
            1 + (r.size.w - 24) / 2,
            (r.size.h - 24) / 2,
            24, 24));
    }
}

WeatherLayer *weather_layer_create(GRect aFromFrame, GRect aToFrame) {
    WeatherLayer *layer = icon_layer_create_with_data(aFromFrame, aToFrame,
        sizeof(weather_layer_data));
    layer_set_update_proc(layer, update_layer);
    
    weather_layer_data *data = weather_layer_data_get(layer);
    data->mWeather = bitmap_for_weather(1);
    data->mMask    = mask_for_weather(1);
    return layer;
}

void weather_layer_destroy(WeatherLayer *aLayer) {
    weather_layer_data *data = weather_layer_data_get(aLayer);
    if (data->mWeather) {
        gbitmap_destroy(data->mWeather);
    }
    if (data->mMask) {
        gbitmap_destroy(data->mMask);
    }
    icon_layer_destroy(aLayer);
}

GBitmap *weather_layer_get_weather(WeatherLayer *aLayer) {
    return weather_layer_data_get(aLayer)->mWeather;
}
GBitmap *weather_layer_get_mask(WeatherLayer *aLayer) {
    return weather_layer_data_get(aLayer)->mMask;
}
