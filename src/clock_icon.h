#ifndef INCLUDE_GUARD_C9909392_C859_4CE1_9899_FAB3645CFAB9
#define INCLUDE_GUARD_C9909392_C859_4CE1_9899_FAB3645CFAB9

#include "icon.h"


#define CLOCK_ICON 9


typedef struct {
	icon_data mBase;
	int32_t h;
	int32_t m;
	int32_t s;
} clock_icon_data;

typedef Icon ClockIcon;

ClockIcon *clock_icon_create(GRect aFromFrame, GRect aToFrame);
void clock_icon_destroy(ClockIcon *aLayer);

void clock_icon_update_time(ClockIcon *aLayer);

#endif
