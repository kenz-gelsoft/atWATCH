#include <pebble.h>

#include "icon_layer.h"


IconLayer *icon_layer_create(GRect aFrame) {
	IconLayer *layer = layer_create_with_data(aFrame, sizeof(icon_layer_data));
	icon_layer_data *data = (icon_layer_data *)layer_get_data(layer);
	data->mColor = rand() % 3;
	return layer;
}

void icon_layer_destroy(IconLayer *aLayer) {
	layer_destroy(aLayer);
}

DitheringPattern icon_layer_get_color(IconLayer *aLayer) {
	icon_layer_data *data = (icon_layer_data *)layer_get_data(aLayer);
	return data->mColor;
}