#include "dithering.h"


#define CASE_PATTERN(pattern_name) \
	case PATTERN_##pattern_name: \
	{ \
		for (int32_t y = -radius; y < radius; ++y) { \
			for (int32_t x = -radius; x < radius; ++x) { \
				if (pattern##pattern_name(x, y)) { \
					graphics_context_set_stroke_color(ctx, GColorWhite); \
				} else { \
					graphics_context_set_stroke_color(ctx, GColorBlack); \
				} \
				if (x*x + y*y <= radius*radius) { \
					graphics_draw_pixel(ctx, GPoint(center.x + x, center.y + y)); \
				} \
			} \
		} \
		break; \
	}


bool pattern20(int32_t x, int32_t y) {
	return (y % 4 && x % 4) || ((y+2) % 4 && (x+2) % 4);
}
bool pattern25(int32_t x, int32_t y) {
	return !(x % 2 && y % 2);
}
bool pattern25_2(int32_t x, int32_t y) {
	return (x % 4 && y % 2) || ((x+2) % 4 && (y+1) % 2);
}
bool pattern25_3(int32_t x, int32_t y) {
	return (y % 4 && x % 2) || ((y+2) % 4 && (x+1) % 2);
}
bool pattern50(int32_t x, int32_t y) {
	return (x + y) % 2;
}
bool pattern75(int32_t x, int32_t y) {
	return !pattern25(x, y);
}
bool pattern75_2(int32_t x, int32_t y) {
	return !pattern25_2(x, y);
}
bool pattern75_3(int32_t x, int32_t y) {
	return !pattern25_3(x, y);
}

void fill_dithered_circle(GContext *ctx, GPoint center, int32_t radius, DitheringPattern pattern) {
	switch (pattern) {
	CASE_PATTERN(20);
	CASE_PATTERN(25);
	CASE_PATTERN(25_2);
	CASE_PATTERN(25_3);
	CASE_PATTERN(50);
	CASE_PATTERN(75);
	CASE_PATTERN(75_2);
	CASE_PATTERN(75_3);
	}
}
