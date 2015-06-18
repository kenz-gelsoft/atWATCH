#include <pebble.h>
#include "dithering.h"
#include "icon_layer.h"

#define LAYER_COUNT 19

static Window *sMainWindow;
static IconLayer  *sLayers[LAYER_COUNT];
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

static void draw_bold_line(GContext *aCtx, GPoint aPt1, GPoint aPt2) {
  int32_t x1 = aPt1.x;
  int32_t y1 = aPt1.y;
  int32_t x2 = aPt2.x;
  int32_t y2 = aPt2.y;
  graphics_draw_line(aCtx, aPt1, aPt2);
  graphics_draw_line(aCtx, GPoint(x1 + 1, y1),     GPoint(x2 + 1, y2));
  graphics_draw_line(aCtx, GPoint(x1,     y1 + 1), GPoint(x2    , y2 + 1));
  graphics_draw_line(aCtx, GPoint(x1 + 1, y1 + 1), GPoint(x2 + 1, y2 + 1));
}

static void update_layer(IconLayer *aLayer, GContext *aCtx) {
  GRect r = layer_get_frame(aLayer);
  if (r.origin.x + r.size.w < 0 || 144 < r.origin.x ||
      r.origin.y + r.size.h < 0 || 168 < r.origin.y) {
    // 不可視
    return;
  }
  int layerNo = 0;
  for (int i = 0; i < LAYER_COUNT; ++i) {
    if (aLayer == sLayers[i]) {
      layerNo = i;
      break;
    }
  }
  // APP_LOG(APP_LOG_LEVEL_DEBUG, "layer[%d] = (%d, %d, %d, %d)",
  //   layerNo, r.origin.x, r.origin.y, r.size.w, r.size.h);
  GRect finalRect = icon_layer_get_to_frame(aLayer);
  bool animating = !grect_equal(&r, &finalRect);

  GPoint center = GPoint(r.size.w / 2,
                         r.size.h / 2-1);
  uint16_t radius = r.size.w / 2 - 1;
  if (layerNo == CLOCK_LAYER) {
    // 背景
    if (animating) {
      graphics_context_set_stroke_color(aCtx, GColorWhite);
      graphics_draw_circle(aCtx, center, radius);
      graphics_context_set_fill_color(aCtx, GColorWhite);
      graphics_context_set_stroke_color(aCtx, GColorWhite);
    } else {
      graphics_context_set_fill_color(aCtx, GColorWhite);
      graphics_fill_circle(aCtx, center, radius);
      graphics_context_set_fill_color(aCtx, GColorBlack);
      graphics_context_set_stroke_color(aCtx, GColorBlack);
    }

    // 時針
    int32_t hourLength = radius * 12 / 20;
    int32_t hourAngle = TRIG_MAX_ANGLE * (h + m / 60.f) / 12.f;
    GPoint hourHand;
    hourHand.y = (-cos_lookup(hourAngle) * hourLength / TRIG_MAX_RATIO) + center.y;
    hourHand.x = ( sin_lookup(hourAngle) * hourLength / TRIG_MAX_RATIO) + center.x;
    draw_bold_line(aCtx, center, hourHand);
    
    // 分針
    int32_t minLength = radius * 18 / 20;
    int32_t minAngle = TRIG_MAX_ANGLE * (m + s / 60.f) / 60.f;
    GPoint minHand;
    minHand.y = (-cos_lookup(minAngle) * minLength / TRIG_MAX_RATIO) + center.y;
    minHand.x = ( sin_lookup(minAngle) * minLength / TRIG_MAX_RATIO) + center.x;
    draw_bold_line(aCtx, center, minHand);

    
    // 秒針
    int32_t secLength = radius * 18 / 20;
    int32_t secAngle = TRIG_MAX_ANGLE * s / 60.f;
    GPoint secHand;
    secHand.y = (-cos_lookup(secAngle) * secLength / TRIG_MAX_RATIO) + center.y;
    secHand.x = ( sin_lookup(secAngle) * secLength / TRIG_MAX_RATIO) + center.x;
    graphics_draw_line(aCtx, center, secHand);
  } else {
    if (animating) {
      // アニメーション中は枠線だけ描画する
      graphics_context_set_stroke_color(aCtx, GColorWhite);
      graphics_draw_circle(aCtx, center, radius);
    } else {
      fill_dithered_circle(aCtx, center, radius,
        icon_layer_get_color(aLayer));
    }
  }
}

static void anim_stopped(Animation *aAnimation, bool aFinished, void *aCtx) {
  property_animation_destroy((PropertyAnimation *)aAnimation);
}

static void make_circle_layer(IconLayer **aOutLayer, GRect *aFromRect, GRect *aToRect) {
  GRect initR = *aFromRect;
  *aOutLayer = icon_layer_create(initR, *aToRect);
  layer_set_update_proc(*aOutLayer, update_layer);
  layer_add_child(window_get_root_layer(sMainWindow), *aOutLayer);
  
  PropertyAnimation *animation = property_animation_create_layer_frame(*aOutLayer, NULL, aToRect);
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
  layer_mark_dirty(sLayers[CLOCK_LAYER]);
}

static void tick_handler(struct tm *aTickTime, TimeUnits aUnitsChanged) {
  update_time();
}

static void main_window_load(Window *aWindow) {
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
    make_circle_layer(&sLayers[i], &r, &to_rect);
  }
  
  update_time();
}

static void main_window_unload(Window *aWindow) {
  animation_unschedule_all();
  for (int i = 0; i < LAYER_COUNT; ++i) {
    icon_layer_destroy(sLayers[i]);
  }
}

static void init() {
  srand(time(NULL));

  sMainWindow = window_create();
  window_set_window_handlers(sMainWindow, (WindowHandlers) {
    .load   = main_window_load,
    .unload = main_window_unload
  });
  window_set_background_color(sMainWindow, GColorBlack);
  window_stack_push(sMainWindow, true);
  
  tick_timer_service_subscribe(SECOND_UNIT, tick_handler);
}

static void deinit() {
  window_destroy(sMainWindow);
}
  
int main(void) {
  init();
  app_event_loop();
  deinit();
}