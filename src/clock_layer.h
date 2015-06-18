#ifndef INCLUDE_GUARD_C9909392_C859_4CE1_9899_FAB3645CFAB9
#define INCLUDE_GUARD_C9909392_C859_4CE1_9899_FAB3645CFAB9

#include "icon_layer.h"


#define CLOCK_LAYER 9
void update_time2();
  // TODO 本来であればこの中にあるべきではない

typedef struct {
	icon_layer_data mBase;
	int32_t h;
	int32_t m;
	int32_t s;
} clock_layer_data;

typedef IconLayer ClockLayer;

ClockLayer *clock_layer_create(int32_t aIndex, GRect aFromFrame, GRect aToFrame);
void clock_layer_destroy(ClockLayer *aLayer);

void clock_layer_update_time(ClockLayer *aLayer);

#endif
