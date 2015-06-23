#ifndef INCLUDE_GUARD_AFBDD6C8_B9EE_4079_B442_EC0C70C15510
#define INCLUDE_GUARD_AFBDD6C8_B9EE_4079_B442_EC0C70C15510

#include <pebble.h>
#include "icon.h"


#define WEATHER_LAYER 8


typedef struct {
	icon_data mBase;
	GBitmap *mWeather;
	GBitmap *mMask;
} weather_layer_data;

typedef Icon WeatherLayer;

WeatherLayer *weather_layer_create(GRect aFromFrame, GRect aToFrame);
void weather_layer_destroy(WeatherLayer *aLayer);

GBitmap *weather_layer_get_weather(WeatherLayer *aLayer);
GBitmap *weather_layer_get_mask(WeatherLayer *aLayer);

void weather_layer_update(WeatherLayer *aLayer, int32_t aWeatherId);

#endif
