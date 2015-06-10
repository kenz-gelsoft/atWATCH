#include <pebble.h>

static Window *s_main_window;
static Layer  *s_layer[7];

static void update_layer(Layer *layer, GContext *ctx) {
  GRect r = layer_get_frame(layer);
  graphics_context_set_fill_color(ctx, GColorBlack);
  GPoint center = GPoint(r.size.w / 2,
                         r.size.h / 2);
  uint16_t radius = r.size.w / 2 - 1;
  APP_LOG(APP_LOG_LEVEL_DEBUG, "update_layer: (%d, %d), %d",
          center.x, center.y, radius);
  graphics_fill_circle(ctx, center, radius);
}

static void anim_stopped(Animation *animation, bool finished, void *context) {
  property_animation_destroy((PropertyAnimation *)animation);
}

static void make_circle_layer(Layer **p_layer, GRect *from, GRect *to) {
  *p_layer = layer_create(GRect(0, 0, 144, 168));
  layer_set_update_proc(*p_layer, update_layer);
  layer_add_child(window_get_root_layer(s_main_window), *p_layer);
  
  PropertyAnimation *animation = property_animation_create_layer_frame(*p_layer, from, to);
  animation_set_duration((Animation *)animation, 500);
  animation_set_delay((Animation *)animation, 300);
  animation_set_curve((Animation *)animation, AnimationCurveEaseInOut);
  animation_set_handlers((Animation *)animation, (AnimationHandlers) {
    .stopped = anim_stopped
  }, NULL);
  animation_schedule((Animation *)animation);
}

static void main_window_load(Window *window) {
  // 144 / 3 = 48
  static const uint16_t to_size = 41;
  static const uint16_t smaller = 36;
  static const uint16_t radius = 144/2;
  static const int16_t to_distance = 42;
  static const int16_t from_distance = 144 + (to_distance-to_size)*3; // gap = 48 - 40 = 8
  
  // sqrt(3)/2 * edge = height
  // 1.732*edge/2
  GPoint center = GPoint(144/2, 168/2);  
  GRect from_rect = GRect(0, 0, 144, 168);
  GRect to_rect = GRect((144 - to_size) / 2,
                        (168 - to_size) / 2,
                        to_size, to_size);
  make_circle_layer(&s_layer[0], &from_rect, &to_rect);

  GRect from_rect2 = GRect(-from_distance,
                           0, 144, 168);
  GRect to_rect2 = GRect(center.x-to_distance-smaller/2,
                        (168 - smaller) / 2,
                        smaller, smaller);
  make_circle_layer(&s_layer[1], &from_rect2, &to_rect2);

  GRect from_rect3 = GRect(from_distance,
                           0, 144, 168);
  GRect to_rect3 = GRect(center.x+to_distance-smaller/2,
                         (168 - smaller) / 2,
                         smaller, smaller);
  make_circle_layer(&s_layer[2], &from_rect3, &to_rect3);
  
  GRect from_rect4 = GRect(-center.x,
                           -from_distance*1.732/2,
                           144, 168);
  GRect to_rect4 = GRect(center.x-to_distance/2-smaller/2,
                         center.y-to_distance*1.732/2-smaller/2,
                         smaller, smaller);
  make_circle_layer(&s_layer[3], &from_rect4, &to_rect4);

  GRect from_rect5 = GRect(from_distance/2,
                           -from_distance*1.732/2,
                           144, 168);
  GRect to_rect5 = GRect(center.x+to_distance/2-smaller/2,
                         center.y-to_distance*1.732/2-smaller/2,
                         smaller, smaller);
  make_circle_layer(&s_layer[4], &from_rect5, &to_rect5);

  GRect from_rect6 = GRect(-center.x,
                           from_distance*1.732/2,
                           144, 168);
  GRect to_rect6 = GRect(center.x-to_distance/2-smaller/2,
                         center.y+to_distance*1.732/2-smaller/2,
                         smaller, smaller);
  make_circle_layer(&s_layer[5], &from_rect6, &to_rect6);

  GRect from_rect7 = GRect(from_distance/2,
                           from_distance*1.732/2,
                           144, 168);
  GRect to_rect7 = GRect(center.x+to_distance/2-smaller/2,
                         center.y+to_distance*1.732/2-smaller/2,
                         smaller, smaller);
  make_circle_layer(&s_layer[6], &from_rect7, &to_rect7);
}

static void main_window_unload(Window *window) {
  animation_unschedule_all();
  layer_destroy(s_layer[0]);
  layer_destroy(s_layer[1]);
}

static void init() {
  s_main_window = window_create();
  window_set_window_handlers(s_main_window, (WindowHandlers) {
    .load   = main_window_load,
    .unload = main_window_unload
  });
  window_stack_push(s_main_window, true);
}

static void deinit() {
  window_destroy(s_main_window);
}
  
int main(void) {
  init();
  app_event_loop();
  deinit();
}