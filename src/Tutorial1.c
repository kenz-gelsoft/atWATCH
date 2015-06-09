#include <pebble.h>

static Window *s_main_window;
static TextLayer *s_time_layer;
static Layer *s_layer;

static void update_time() {
  time_t temp = time(NULL);
  struct tm *tick_time = localtime(&temp);
  
  static char buffer[] = "00:00";
  if (clock_is_24h_style()) {
    strftime(buffer, sizeof("00:00"), "%H:%M", tick_time);
  } else {
    strftime(buffer, sizeof("00:00"), "%I:%M", tick_time);
  }
  
  text_layer_set_text(s_time_layer, buffer);
}

static void update_layer(Layer *layer, GContext *ctx) {
  graphics_context_set_stroke_color(ctx, GColorBlack);
  GRect r = layer_get_bounds(layer);
  GPoint center = GPoint(
    (r.size.w - r.origin.x) / 2,
    (r.size.h - r.origin.y) / 2);
  graphics_draw_circle(ctx, center, r.size.w / 2);
}

static void main_window_load(Window *window) {
  s_time_layer = text_layer_create(GRect(0, 55, 144, 50));
  text_layer_set_background_color(s_time_layer, GColorClear);
  text_layer_set_text_color(s_time_layer, GColorBlack);
  text_layer_set_text(s_time_layer, "00:00");
  
  text_layer_set_font(s_time_layer, fonts_get_system_font(FONT_KEY_BITHAM_42_BOLD));
  text_layer_set_text_alignment(s_time_layer, GTextAlignmentCenter);
  
  layer_add_child(
    window_get_root_layer(window),
    text_layer_get_layer(s_time_layer));
  
  s_layer = layer_create(GRect(0, 14, 144, 144));
  layer_set_update_proc(s_layer, update_layer);
  layer_add_child(
    window_get_root_layer(window),
    s_layer);
}

static void main_window_unload(Window *window) {
  layer_destroy(s_layer);
  text_layer_destroy(s_time_layer);
}

static void tick_handler(struct tm *tick_time, TimeUnits units_changed) {
  update_time();
}

static void init() {
  s_main_window = window_create();
  
  window_set_window_handlers(s_main_window, (WindowHandlers) {
    .load   = main_window_load,
    .unload = main_window_unload
  });
  
  window_stack_push(s_main_window, true);
  
  update_time();
  tick_timer_service_subscribe(MINUTE_UNIT, tick_handler);
}

static void deinit() {
  window_destroy(s_main_window);
  s_main_window = NULL;
}

int main(void) {
  init();
  app_event_loop();
  deinit();
}