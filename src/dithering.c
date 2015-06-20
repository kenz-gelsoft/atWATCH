#include "dithering.h"

#define FILL_DITHERING_CIRCLE(percentage) \
	void fill_##percentage##percent_circle(GContext *ctx, GPoint center, int32_t radius) { \
		for (int32_t y = -radius; y < radius; ++y) { \
			for (int32_t x = -radius; x < radius; ++x) { \
				if (pattern_##percentage##percent(x, y)) { \
					graphics_context_set_stroke_color(ctx, GColorWhite); \
				} else { \
					graphics_context_set_stroke_color(ctx, GColorBlack); \
				} \
				if (x*x + y*y <= radius*radius) { \
					graphics_draw_pixel(ctx, GPoint(center.x + x, center.y + y)); \
				} \
			} \
		} \
	}

bool pattern_20percent(int32_t x, int32_t y) {
	return (y % 4 && x % 4) || ((y+2) % 4 && (x+2) % 4);
}
bool pattern_25percent(int32_t x, int32_t y) {
	return (y % 4 && x % 2) || ((y+2) % 4 && (x+1) % 2);
}
bool pattern_50percent(int32_t x, int32_t y) {
	return (x + y) % 2;
}

FILL_DITHERING_CIRCLE(20);
FILL_DITHERING_CIRCLE(25);
FILL_DITHERING_CIRCLE(50);

void fill_dithered_circle(GContext *ctx, GPoint center, int32_t radius, DitheringPattern pattern) {
	switch (pattern) {
	case Dithering20Percent:
		fill_20percent_circle(ctx, center, radius); break;
	case Dithering25Percent:
		fill_25percent_circle(ctx, center, radius); break;
	case Dithering50Percent:
		fill_50percent_circle(ctx, center, radius); break;
	}
}
