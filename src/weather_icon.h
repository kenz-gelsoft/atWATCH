#ifndef INCLUDE_GUARD_AFBDD6C8_B9EE_4079_B442_EC0C70C15510
#define INCLUDE_GUARD_AFBDD6C8_B9EE_4079_B442_EC0C70C15510

#include <pebble.h>
#include "icon.h"


#define WEATHER_ICON 8


typedef struct {
	icon_data mBase;
	GBitmap *mWeather;
	GBitmap *mMask;
} weather_icon_data;

typedef Icon WeatherIcon;

WeatherIcon *weather_icon_create(GRect aFromFrame, GRect aToFrame);
void weather_icon_destroy(WeatherIcon *aLayer);

GBitmap *weather_icon_get_weather(WeatherIcon *aLayer);
GBitmap *weather_icon_get_mask(WeatherIcon *aLayer);

void weather_icon_update(WeatherIcon *aLayer, int32_t aWeatherId);

#endif
