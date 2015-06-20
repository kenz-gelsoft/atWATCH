#ifndef INCLUDE_GUARD_96306AD8_172A_4085_84AA_8C2D7183F8A3
#define INCLUDE_GUARD_96306AD8_172A_4085_84AA_8C2D7183F8A3

#include <pebble.h>

typedef enum {
	Dithering20Percent,
	Dithering25Percent,
	Dithering50Percent,
} DitheringPattern;

bool pattern_20percent(int32_t x, int32_t y);
bool pattern_25percent(int32_t x, int32_t y);
bool pattern_50percent(int32_t x, int32_t y);

void fill_dithered_circle(GContext *ctx, GPoint center,
	int32_t radius, DitheringPattern pattern);

#endif