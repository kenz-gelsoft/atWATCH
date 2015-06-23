#include <pebble.h>

#include "battery_icon.h"
#include "calendar_icon.h"
#include "clock_icon.h"
#include "dithering.h"
#include "icon.h"
#include "weather_layer.h"


#define LAYER_COUNT 19

static Window *sMainWindow;
static Icon *sLayers[LAYER_COUNT];

#define CLOCK_SIZE 41

enum {
  KEY_WEATHER_ID = 1
};

#define RECT(x,y,w,h) (x),(y),(w),(h)
static int16_t sIconFrames[] = {
                   RECT(19,1,29,29), RECT(57,-1,30,30), RECT(98,1,29,29),
           RECT(0,35,29,29), RECT(33,28,36,36), RECT(75,28,36,36), RECT(117,35,29,29),
RECT(0,79,10,10), RECT(12,65,36,36), RECT(52,63,CLOCK_SIZE,CLOCK_SIZE), RECT(97,65,36,36), RECT(135,79,10,10),
         RECT(0,104,29,29), RECT(33,102,36,36), RECT(75,102,36,36), RECT(116,104,29,29),
                RECT(19,138,29,29), RECT(57,140,30,30), RECT(98,138,29,29),
};

static void make_circle_layer(int32_t aIndex, Icon **aOutLayer, GRect *aFromRect, GRect *aToRect) {
  GRect initR = *aFromRect;
  if (aIndex == CLOCK_ICON) {
    *aOutLayer = clock_icon_create(initR, *aToRect);
  } else if (aIndex == BATTERY_ICON) {
    *aOutLayer = battery_icon_create(initR, *aToRect);
  } else if (aIndex == CALENDAR_ICON) {
    *aOutLayer = calendar_icon_create(initR, *aToRect);
  } else if (aIndex == WEATHER_LAYER) {
    *aOutLayer = weather_layer_create(initR, *aToRect);
  } else {
    *aOutLayer = icon_create(initR, *aToRect);
  }
  layer_add_child(window_get_root_layer(sMainWindow), *aOutLayer);
}

static void update_time() {
  ClockIcon *clock = sLayers[CLOCK_ICON];
  clock_icon_update_time(clock);

  CalendarIcon *calendar = sLayers[CALENDAR_ICON];
  calendar_icon_update(calendar);
}

static void tick_handler(struct tm *aTickTime, TimeUnits aUnitsChanged) {
  update_time();
}

static void update_battery(BatteryChargeState aCharge) {
  BatteryIcon *battery = sLayers[BATTERY_ICON];
  battery_icon_update(battery, aCharge.charge_percent);
}

static void battery_handler(BatteryChargeState aCharge) {
  update_battery(aCharge);
}

static void tap_handler(AccelAxisType aAxis, int32_t aDirection) {
    for (int32_t i = 0; i < LAYER_COUNT; ++i) {
      icon_zoom_in(sLayers[i]);
    }
}

static void inbox_received_callback(DictionaryIterator *iterator, void *context) {
  Tuple *t = dict_read_first(iterator);
  while (t != NULL) {
    switch (t->key) {
    case KEY_WEATHER_ID:
      weather_layer_update(sLayers[WEATHER_LAYER], t->value->int32);
      break;
    }
    t = dict_read_next(iterator);
  }
}

static void inbox_dropped_callback(AppMessageResult reason, void *context) {
  APP_LOG(APP_LOG_LEVEL_ERROR, "Message dropped!");
}

static void outbox_failed_callback(DictionaryIterator *iterator, AppMessageResult reason, void *context) {
  APP_LOG(APP_LOG_LEVEL_ERROR, "Outbox send failed!");
}

static void outbox_sent_callback(DictionaryIterator *iterator, void *context) {
  APP_LOG(APP_LOG_LEVEL_INFO, "Outbox send success!");
}

static void main_window_load(Window *aWindow) {
  float scale = 144.f / CLOCK_SIZE;
  for (int32_t i = 0; i < LAYER_COUNT; ++i) {
    GRect to_rect = GRect(sIconFrames[i*4],sIconFrames[i*4+1],sIconFrames[i*4+2],sIconFrames[i*4+3]);
    int16_t cx = 144 / 2;
    int16_t cy = 168 / 2;
    GRect r = to_rect;
    r.origin.x = cx + (r.origin.x - cx) * scale;
    r.origin.y = cy + (r.origin.y - cy) * scale;
    r.size.w *= scale;
    r.size.h *= scale;
    // APP_LOG(APP_LOG_LEVEL_DEBUG, "from_rect = (%d, %d, %d, %d)",
    //   r.origin.x, r.origin.y, r.size.w, r.size.h);
    make_circle_layer(i, &sLayers[i], &r, &to_rect);
  }
  
  update_time();
  BatteryChargeState charge = battery_state_service_peek();
  update_battery(charge);
}

static void main_window_unload(Window *aWindow) {
  animation_unschedule_all();
  for (int i = 0; i < LAYER_COUNT; ++i) {
    if (i == CLOCK_ICON) {
      clock_icon_destroy(sLayers[i]);
    } else if (i == BATTERY_ICON) { 
      battery_icon_destroy(sLayers[i]);
    } else if (i == CALENDAR_ICON) {
      calendar_icon_destroy(sLayers[i]);
    } else if (i == WEATHER_LAYER) {
      weather_layer_destroy(sLayers[i]);
    } else {
      icon_destroy(sLayers[i]);
    }
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
  battery_state_service_subscribe(battery_handler);
  accel_tap_service_subscribe(tap_handler);
  
  app_message_register_inbox_received(inbox_received_callback);
  app_message_register_inbox_dropped(inbox_dropped_callback);
  app_message_register_outbox_failed(outbox_failed_callback);
  app_message_register_outbox_sent(outbox_sent_callback);
  
  app_message_open(app_message_inbox_size_maximum(), app_message_outbox_size_maximum());
}

static void deinit() {
  window_destroy(sMainWindow);
}
  
int main(void) {
  init();
  app_event_loop();
  deinit();
}