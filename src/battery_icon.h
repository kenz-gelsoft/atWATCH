#ifndef INCLUDE_GUARD_B4024748_FAC3_461F_933C_5B997B6A075F
#define INCLUDE_GUARD_B4024748_FAC3_461F_933C_5B997B6A075F

#include <pebble.h>
#include "icon.h"


#define BATTERY_ICON 5


typedef struct {
	icon_data mBase;
	uint8_t mPercent;
} battery_icon_data;

typedef Icon BatteryIcon;

BatteryIcon *battery_icon_create(GRect aFromFrame, GRect aToFrame);
void battery_icon_destroy(BatteryIcon *aLayer);

uint8_t battery_icon_get_percent(BatteryIcon *aLayer);

void battery_icon_update(BatteryIcon *aLayer, uint8_t aPercent);

#endif
