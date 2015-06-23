#ifndef INCLUDE_GUARD_90C0D390_D69F_444F_8287_570404DF8A65
#define INCLUDE_GUARD_90C0D390_D69F_444F_8287_570404DF8A65

#include <pebble.h>
#include "icon.h"


#define CALENDAR_ICON 14


typedef struct {
	icon_data mBase;
	uint8_t mDate;
	GBitmap *mWeekday;
} calendar_icon_data;

typedef Icon CalendarIcon;

CalendarIcon *calendar_icon_create(GRect aFromFrame, GRect aToFrame);
void calendar_icon_destroy(CalendarIcon *aLayer);

uint8_t calendar_icon_get_date(CalendarIcon *aLayer);
GBitmap *calendar_icon_get_weekday(CalendarIcon *aLayer);

void calendar_icon_update(CalendarIcon *aLayer);

#endif
