#include <pebble.h>

static const int LAYER_COUNT = 17;

static Window *s_main_window;
static Layer  *s_layer[17];

/*
      (19, 3) (58, 1) (98, 3)
  ( 1,38) (34,32) (76,32) (116,38)
      (12,68) (52,66) (98,68)
( 1,106) (34,106) (76,106) (116,106)
     (19,141) (58,144) (98,141)
*/

#define RECT(x,y,w,h) (x),(y),(w),(h)
static int16_t sIconAreas[] = {
            RECT(19,3,27,27), RECT(58,1,27,27), RECT(98,3,27,27),
  RECT(1,38,27,27), RECT(34,32,35,35), RECT(76,32,35,35), RECT(116,38,27,27),
            RECT(12,68,35,35), RECT(52,66,40,40), RECT(98,68,35,35),
  RECT(1,106,27,27), RECT(34,106,35,35), RECT(76,106,35,35), RECT(116,106,27,27),
            RECT(19,141,27,27), RECT(58,144,27,27), RECT(98,141,27,27),
};

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
  float scale = 144.f / 40.f;
  for (int i = 0; i < LAYER_COUNT; ++i) {
    GRect to_rect = GRect(sIconAreas[i*4],sIconAreas[i*4+1],sIconAreas[i*4+2],sIconAreas[i*4+3]);
    int16_t cx = 144 / 2;
    int16_t cy = 168 / 2;
    GRect r = to_rect;
    r.origin.x += (r.origin.x - cx) * scale;
    r.origin.y += (r.origin.y - cy) * scale;
    r.size.w *= scale;
    r.size.h *= scale;
    make_circle_layer(&s_layer[i], &r, &to_rect);
  }
}                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                

static void main_window_unload(Window *window) {
  animation_unschedule_all();
  for (int i = 0; i < LAYER_COUNT; ++i) {
    layer_destroy(s_layer[i]);
  }
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