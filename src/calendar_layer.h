#ifndef INCLUDE_GUARD_90C0D390_D69F_444F_8287_570404DF8A65
#define INCLUDE_GUARD_90C0D390_D69F_444F_8287_570404DF8A65

#include <pebble.h>
#include "icon.h"


#define CALENDAR_LAYER 14


typedef struct {
	icon_data mBase;
	uint8_t mDate;
	GBitmap *mWeekday;
} calendar_layer_data;

typedef Icon CalendarLayer;

CalendarLayer *calendar_layer_create(GRect aFromFrame, GRect aToFrame);
void calendar_layer_destroy(CalendarLayer *aLayer);

uint8_t calendar_layer_get_date(CalendarLayer *aLayer);
GBitmap *calendar_layer_get_weekday(CalendarLayer *aLayer);

void calendar_layer_update(CalendarLayer *aLayer);

#endif
