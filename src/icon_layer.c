#include <pebble.h>

#include "icon_layer.h"


IconLayer *icon_layer_create(GRect frame) {
	IconLayer *layer = layer_create_with_data(frame, sizeof(icon_layer_data));
	return layer;
}

void icon_layer_destroy(IconLayer *layer) {
	layer_destroy(layer);
}
