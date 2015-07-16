#ifndef INCLUDE_GUARD_41D02389_3360_49DC_B09B_65191D0E9DA8
#define INCLUDE_GUARD_41D02389_3360_49DC_B09B_65191D0E9DA8

#include <pebble.h>
#include "icon.h"


#define TEMPERATURE_ICON 13


typedef struct {
	icon_data mBase;
	int32_t mTemperature;
} temperature_icon_data;

typedef Icon TemperatureIcon;

TemperatureIcon *temperature_icon_create(GRect aFromFrame, GRect aToFrame);
void temperature_icon_destroy(TemperatureIcon *aIcon);

int32_t temperature_icon_get_temp(TemperatureIcon *aIcon);

void temperature_icon_update(TemperatureIcon *aIcon, int32_t aTemperature);

#endif
