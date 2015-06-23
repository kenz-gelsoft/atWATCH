#ifndef INCLUDE_GUARD_B4024748_FAC3_461F_933C_5B997B6A075F
#define INCLUDE_GUARD_B4024748_FAC3_461F_933C_5B997B6A075F

#include <pebble.h>
#include "icon.h"


#define BATTERY_LAYER 5


typedef struct {
	icon_data mBase;
	uint8_t mPercent;
} battery_layer_data;

typedef Icon BatteryLayer;

BatteryLayer *battery_layer_create(GRect aFromFrame, GRect aToFrame);
void battery_layer_destroy(BatteryLayer *aLayer);

uint8_t battery_layer_get_percent(BatteryLayer *aLayer);

void battery_layer_update(BatteryLayer *aLayer, uint8_t aPercent);

#endif
