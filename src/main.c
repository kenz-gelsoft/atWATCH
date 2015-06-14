#include <pebble.h>
#include "dithering/src/dithered_rects.h"

#define LAYER_COUNT 17

static Window *s_main_window;
static Layer  *s_layer[LAYER_COUNT];
static int h=0, m=0, s=0;

static GPath *sHourHandPath1x = NULL; // 20基準
static GPath *sHourHandPath2x = NULL;
static GPath *sHourHandPath4x = NULL;
static GPath *sMinHandPath1x  = NULL; // 20基準
static GPath *sMinHandPath2x  = NULL;
static GPath *sMinHandPath4x  = NULL;

#define PATH_DEFINITIONS(scale) \
  static const GPathInfo HOUR_HAND_PATH_INFO_##scale##X = { \
    .num_points = 5, \
    .points = (GPoint []) {{0,0}, {-3, -4}, {-3, -13*scale}, {2, -13*scale}, {2, -4}} \
  }; \
  static const GPathInfo MIN_HAND_PATH_INFO_##scale##X = { \
    .num_points = 5, \
    .points = (GPoint []) {{0,0}, {-3, -4}, {-3, -18*scale}, {2, -18*scale}, {2, -4}} \
  };

PATH_DEFINITIONS(1)
PATH_DEFINITIONS(2)
PATH_DEFINITIONS(4)

/*
      (19, 3) (58, 1) (98, 3)
  ( 1,38) (34,32) (76,32) (116,38)
      (12,68) (52,66) (98,68)
( 1,106) (34,106) (76,106) (116,106)
     (19,141) (58,144) (98,141)
*/

#define RECT(x,y,w,h) (x),(y),(w),(h)
static int16_t sIconAreas[] = {
            RECT(19,3,28,28), RECT(58,0,28,28), RECT(98,3,28,28),
  RECT(1,37,28,28), RECT(34,31,35,35), RECT(76,31,35,35), RECT(116,37,28,28),
            RECT(12,67,35,35), RECT(52,65,40,40), RECT(98,67,35,35),
  RECT(1,105,28,28), RECT(34,105,35,35), RECT(76,105,35,35), RECT(116,105,28,28),
            RECT(19,138,28,28), RECT(58,141,28,28), RECT(98,138,28,28),
};

static void update_layer(Layer *layer, GContext *ctx) {
  GRect r = layer_get_frame(layer);
  if (r.origin.x + r.size.w < 0 || 144 < r.origin.x ||
      r.origin.y + r.size.h < 0 || 168 < r.origin.y) {
    // 不可視
    return;
  }
  int layerNo = 0;
  for (int i = 0; i < LAYER_COUNT; ++i) {
    if (layer == s_layer[i]) {
      layerNo = i;
      break;
    }
  }
  // APP_LOG(APP_LOG_LEVEL_DEBUG, "layer[%d] = (%d, %d, %d, %d)",
  //   layerNo, r.origin.x, r.origin.y, r.size.w, r.size.h);
  GRect finalRect = GRect(
    sIconAreas[layerNo*4],
    sIconAreas[layerNo*4+1],
    sIconAreas[layerNo*4+2],
    sIconAreas[layerNo*4+3]);
  bool animating = !grect_equal(&r, &finalRect);

  GPoint center = GPoint(r.size.w / 2,
                         r.size.h / 2-1);
  uint16_t radius = r.size.w / 2 - 1;
  if (layerNo == 8) {
    // 背景
    if (animating) {
      graphics_context_set_stroke_color(ctx, GColorWhite);
      graphics_draw_circle(ctx, center, radius);
      graphics_context_set_fill_color(ctx, GColorWhite);
      graphics_context_set_stroke_color(ctx, GColorWhite);
    } else {
      graphics_context_set_fill_color(ctx, GColorWhite);
      graphics_fill_circle(ctx, center, radius);
      graphics_context_set_fill_color(ctx, GColorBlack);
      graphics_context_set_stroke_color(ctx, GColorBlack);
    }

    GPath *hourHandPath = NULL;
    GPath *minHandPath  = NULL;
    if (radius > 80) {
      hourHandPath = sHourHandPath4x;
      minHandPath  = sMinHandPath4x;
    } else if (radius > 40) {
      hourHandPath = sHourHandPath2x;
      minHandPath  = sMinHandPath2x;
    } else {
      hourHandPath = sHourHandPath1x;
      minHandPath  = sMinHandPath1x;
    }
    
    // 時針
    int32_t hourAngle = TRIG_MAX_ANGLE * (h + m / 60.f) / 12.f;
    gpath_move_to(hourHandPath, GPoint(radius, radius));
    gpath_rotate_to(hourHandPath, hourAngle);
    gpath_draw_filled(ctx, hourHandPath);
    
    // 分針
    int32_t minAngle = TRIG_MAX_ANGLE * m / 60.f;
    gpath_move_to(minHandPath, GPoint(radius, radius));
    gpath_rotate_to(minHandPath, minAngle);
    gpath_draw_filled(ctx, minHandPath);
    
    // 秒針
    int32_t secLength = radius * 18 / 20;
    int32_t secAngle = TRIG_MAX_ANGLE * s / 60.f;
    GPoint secHand;
    secHand.y = (-cos_lookup(secAngle) * secLength / TRIG_MAX_RATIO) + center.y;
    secHand.x = ( sin_lookup(secAngle) * secLength / TRIG_MAX_RATIO) + center.x;
    graphics_draw_line(ctx, center, secHand);
  } else {
    if (animating) {
      // アニメーション中は枠線だけ描画する
      graphics_context_set_stroke_color(ctx, GColorWhite);
      graphics_draw_circle(ctx, center, radius);
    } else {
      DitherPercentage p = layerNo % 2
        ? DITHER_50_PERCENT
        : DITHER_75_PERCENT;
      draw_dithered_circle(ctx, center.x, center.y, radius, GColorBlack, GColorWhite, p);
    }
  }
}

static void anim_stopped(Animation *animation, bool finished, void *context) {
  property_animation_destroy((PropertyAnimation *)animation);
}

static void make_circle_layer(Layer **p_layer, GRect *from, GRect *to) {
  GRect initR = *from;
  *p_layer = layer_create(initR);
  layer_set_update_proc(*p_layer, update_layer);
  layer_add_child(window_get_root_layer(s_main_window), *p_layer);
  
  PropertyAnimation *animation = property_animation_create_layer_frame(*p_layer, NULL, to);
  animation_set_duration((Animation *)animation, 500);
  animation_set_delay((Animation *)animation, 300);
  animation_set_curve((Animation *)animation, AnimationCurveEaseInOut);
  animation_set_handlers((Animation *)animation, (AnimationHandlers) {
    .stopped = anim_stopped
  }, NULL);
  animation_schedule((Animation *)animation);
}

static void update_time() {
  time_t temp = time(NULL);
  struct tm *tick_time = localtime(&temp);
  h = tick_time->tm_hour % 12;
  m = tick_time->tm_min;
  s = tick_time->tm_sec;
  APP_LOG(APP_LOG_LEVEL_DEBUG, "%d:%d", h, m);
  layer_mark_dirty(s_layer[8]);
}

static void tick_handler(struct tm *tick_time, TimeUnits units_changed) {
  update_time();
}

static void main_window_load(Window *window) {
  float scale = 168.f / 40.f;
  for (int i = 0; i < LAYER_COUNT; ++i) {
    GRect to_rect = GRect(sIconAreas[i*4],sIconAreas[i*4+1],sIconAreas[i*4+2],sIconAreas[i*4+3]);
    int16_t cx = 144 / 2;
    int16_t cy = 168 / 2;
    GRect r = to_rect;
    r.origin.x = cx + (r.origin.x - cx) * scale;
    r.origin.y = cy + (r.origin.y - cy) * scale;
    r.size.w *= scale;
    r.size.h *= scale;
    // APP_LOG(APP_LOG_LEVEL_DEBUG, "from_rect = (%d, %d, %d, %d)",
    //   r.origin.x, r.origin.y, r.size.w, r.size.h);
    make_circle_layer(&s_layer[i], &r, &to_rect);
  }
  
  update_time();
}

static void main_window_unload(Window *window) {
  animation_unschedule_all();
  for (int i = 0; i < LAYER_COUNT; ++i) {
    layer_destroy(s_layer[i]);
  }
}

static void init() {
  sHourHandPath1x = gpath_create(&HOUR_HAND_PATH_INFO_1X);
  sHourHandPath2x = gpath_create(&HOUR_HAND_PATH_INFO_2X);
  sHourHandPath4x = gpath_create(&HOUR_HAND_PATH_INFO_4X);
  sMinHandPath1x  = gpath_create(&MIN_HAND_PATH_INFO_1X);
  sMinHandPath2x  = gpath_create(&MIN_HAND_PATH_INFO_2X);
  sMinHandPath4x  = gpath_create(&MIN_HAND_PATH_INFO_4X);
  
  s_main_window = window_create();
  window_set_window_handlers(s_main_window, (WindowHandlers) {
    .load   = main_window_load,
    .unload = main_window_unload
  });
  window_set_background_color(s_main_window, GColorBlack);
  window_stack_push(s_main_window, true);
  
  tick_timer_service_subscribe(SECOND_UNIT, tick_handler);
}

static void deinit() {
  window_destroy(s_main_window);
  gpath_destroy(sMinHandPath4x);
  gpath_destroy(sMinHandPath2x);
  gpath_destroy(sMinHandPath1x);
  gpath_destroy(sHourHandPath4x);
  gpath_destroy(sHourHandPath2x);
  gpath_destroy(sHourHandPath1x);
}
  
int main(void) {
  init();
  app_event_loop();
  deinit();
}