#include <pebble.h>
#include "dithering.h"

#define LAYER_COUNT 19

static Window *s_main_window;
static Layer  *s_layer[LAYER_COUNT];
static DitheringPattern sColors[LAYER_COUNT]; // TODO 構造体
static int h=0, m=0, s=0;

#define CLOCK_LAYER 9
#define CLOCK_SIZE 41

#define RECT(x,y,w,h) (x),(y),(w),(h)
static int16_t sIconAreas[] = {
                   RECT(19,1,29,29), RECT(57,-1,30,30), RECT(98,1,29,29),
           RECT(0,35,29,29), RECT(33,28,36,36), RECT(75,28,36,36), RECT(117,35,29,29),
RECT(0,79,10,10), RECT(12,65,36,36), RECT(52,63,CLOCK_SIZE,CLOCK_SIZE), RECT(97,65,36,36), RECT(135,79,10,10),
         RECT(0,104,29,29), RECT(33,102,36,36), RECT(75,102,36,36), RECT(116,104,29,29),
                RECT(19,138,29,29), RECT(57,140,30,30), RECT(98,138,29,29),
};

static void draw_bold_line(GContext *ctx, GPoint p1, GPoint p2) {
  int32_t x1 = p1.x;
  int32_t y1 = p1.y;
  int32_t x2 = p2.x;
  int32_t y2 = p2.y;
  graphics_draw_line(ctx, p1, p2);
  graphics_draw_line(ctx, GPoint(x1 + 1, y1),     GPoint(x2 + 1, y2));
  graphics_draw_line(ctx, GPoint(x1,     y1 + 1), GPoint(x2    , y2 + 1));
  graphics_draw_line(ctx, GPoint(x1 + 1, y1 + 1), GPoint(x2 + 1, y2 + 1));
}

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
  if (layerNo == CLOCK_LAYER) {
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

    // 時針
    int32_t hourLength = radius * 12 / 20;
    int32_t hourAngle = TRIG_MAX_ANGLE * (h + m / 60.f) / 12.f;
    GPoint hourHand;
    hourHand.y = (-cos_lookup(hourAngle) * hourLength / TRIG_MAX_RATIO) + center.y;
    hourHand.x = ( sin_lookup(hourAngle) * hourLength / TRIG_MAX_RATIO) + center.x;
    draw_bold_line(ctx, center, hourHand);
    
    // 分針
    int32_t minLength = radius * 18 / 20;
    int32_t minAngle = TRIG_MAX_ANGLE * (m + s / 60.f) / 60.f;
    GPoint minHand;
    minHand.y = (-cos_lookup(minAngle) * minLength / TRIG_MAX_RATIO) + center.y;
    minHand.x = ( sin_lookup(minAngle) * minLength / TRIG_MAX_RATIO) + center.x;
    draw_bold_line(ctx, center, minHand);

    
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
      fill_dithered_circle(ctx, center, radius, sColors[layerNo]);
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
  //APP_LOG(APP_LOG_LEVEL_DEBUG, "%d:%d", h, m);
  layer_mark_dirty(s_layer[CLOCK_LAYER]);
}

static void tick_handler(struct tm *tick_time, TimeUnits units_changed) {
  update_time();
}

static void main_window_load(Window *window) {
  float scale = 168.f / CLOCK_SIZE;
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
  srand(time(NULL));
  for (int i = 0; i < LAYER_COUNT; ++i) {
    sColors[i] = rand() % 3;
  }
  
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
}
  
int main(void) {
  init();
  app_event_loop();
  deinit();
}