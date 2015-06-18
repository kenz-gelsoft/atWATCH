#include <pebble.h>

#include "icon_layer.h"


static icon_layer_data *icon_layer_data_get(IconLayer *aLayer) {
	return (icon_layer_data *)layer_get_data(aLayer);
}


IconLayer *icon_layer_create(GRect aFromFrame, GRect aToFrame) {
	IconLayer *layer = layer_create_with_data(aFromFrame, sizeof(icon_layer_data));
	icon_layer_data *data = icon_layer_data_get(layer);
	data->mColor = rand() % 3;
	data->mToFrame = aToFrame;
	return layer;
}

void icon_layer_destroy(IconLayer *aLayer) {
	layer_destroy(aLayer);
}

DitheringPattern icon_layer_get_color(IconLayer *aLayer) {
	return icon_layer_data_get(aLayer)->mColor;
}

GRect icon_layer_get_to_frame(IconLayer *aLayer) {
	return icon_layer_data_get(aLayer)->mToFrame;
}
