#ifndef INCLUDE_GUARD_96306AD8_172A_4085_84AA_8C2D7183F8A3
#define INCLUDE_GUARD_96306AD8_172A_4085_84AA_8C2D7183F8A3

#include <pebble.h>

typedef enum {
	PATTERN_20,
	PATTERN_25,
	PATTERN_25_2,
	PATTERN_25_3,
	PATTERN_50,
	PATTERN_75,
	PATTERN_75_2,
	PATTERN_75_3,
} DitheringPattern;
#define PATTERN_COUNT ((uint8_t)(PATTERN_75_3 + 1))


bool pattern20(int32_t x, int32_t y);
bool pattern25(int32_t x, int32_t y);
bool pattern25_2(int32_t x, int32_t y);
bool pattern25_3(int32_t x, int32_t y);
bool pattern50(int32_t x, int32_t y);
bool pattern75(int32_t x, int32_t y);
bool pattern75_2(int32_t x, int32_t y);
bool pattern75_3(int32_t x, int32_t y);

void fill_dithered_circle(GContext *ctx, GPoint center,
	int32_t radius, DitheringPattern pattern);

#endif