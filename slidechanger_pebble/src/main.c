#include "pebble.h"

#define AppKeyRequestData 0
#define ACTION_NEXT 1
#define ACTION_PREV 0
static Window *s_main_window;

static ActionBarLayer *s_action_bar;
static TextLayer *s_header_layer, *s_label_layer;
static GBitmap *s_icon_next, *s_icon_prev;


static void inbox_received_callback(DictionaryIterator *iterator, void *context) {
  APP_LOG(APP_LOG_LEVEL_ERROR, "Message received!");
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


static void increment_click_handler(ClickRecognizerRef recognizer, void *context) {
// Declare the dictionary's iterator
DictionaryIterator *out_iter;

// Prepare the outbox buffer for this message
AppMessageResult result = app_message_outbox_begin(&out_iter);
if(result == APP_MSG_OK) {
  // Add an item to ask for weather data
  int value = ACTION_NEXT;
  dict_write_int(out_iter, AppKeyRequestData, &value, sizeof(int), true);

  // Send this message
  result = app_message_outbox_send();
  if(result != APP_MSG_OK) {
    APP_LOG(APP_LOG_LEVEL_ERROR, "Error sending the outbox: %d", (int)result);
  }
} else {
  // The outbox cannot be used right now
  APP_LOG(APP_LOG_LEVEL_ERROR, "Error preparing the outbox: %d", (int)result);
}
  
}

static void decrement_click_handler(ClickRecognizerRef recognizer, void *context) {
// Declare the dictionary's iterator
DictionaryIterator *out_iter;

// Prepare the outbox buffer for this message
AppMessageResult result = app_message_outbox_begin(&out_iter);
if(result == APP_MSG_OK) {
  // Add an item to ask for weather data
  int value = ACTION_PREV;
  dict_write_int(out_iter, AppKeyRequestData, &value, sizeof(int), true);

  // Send this message
  result = app_message_outbox_send();
  if(result != APP_MSG_OK) {
    APP_LOG(APP_LOG_LEVEL_ERROR, "Error sending the outbox: %d", (int)result);
  }
} else {
  // The outbox cannot be used right now
  APP_LOG(APP_LOG_LEVEL_ERROR, "Error preparing the outbox: %d", (int)result);
}
  
}

static void click_config_provider(void *context) {
  window_single_click_subscribe(BUTTON_ID_UP, increment_click_handler);
  window_single_click_subscribe(BUTTON_ID_DOWN, decrement_click_handler);
}

static void main_window_load(Window *window) {
  Layer *window_layer = window_get_root_layer(window);

  s_action_bar = action_bar_layer_create();
  action_bar_layer_add_to_window(s_action_bar, window);
  action_bar_layer_set_click_config_provider(s_action_bar, click_config_provider);

  action_bar_layer_set_icon(s_action_bar, BUTTON_ID_UP, s_icon_next);
  action_bar_layer_set_icon(s_action_bar, BUTTON_ID_DOWN, s_icon_prev);

  int width = layer_get_frame(window_layer).size.w - ACTION_BAR_WIDTH - 3;

  s_header_layer = text_layer_create(GRect(4, PBL_IF_RECT_ELSE(0, 30), width, 60));
  text_layer_set_font(s_header_layer, fonts_get_system_font(FONT_KEY_GOTHIC_24));
  text_layer_set_background_color(s_header_layer, GColorClear);
  text_layer_set_text(s_header_layer, "Slides Changer");
  text_layer_set_text_alignment(s_header_layer, PBL_IF_ROUND_ELSE(GTextAlignmentCenter, GTextAlignmentLeft));
  layer_add_child(window_layer, text_layer_get_layer(s_header_layer));

  s_label_layer = text_layer_create(GRect(4, PBL_IF_RECT_ELSE(44, 60), width, 60));
  text_layer_set_font(s_label_layer, fonts_get_system_font(FONT_KEY_GOTHIC_18));
  text_layer_set_background_color(s_label_layer, GColorClear);
  text_layer_set_text(s_label_layer, "Slides at your fingertips.");
  text_layer_set_text_alignment(s_label_layer, PBL_IF_ROUND_ELSE(GTextAlignmentCenter, GTextAlignmentLeft));
  layer_add_child(window_layer, text_layer_get_layer(s_label_layer));

}

static void main_window_unload(Window *window) {
  text_layer_destroy(s_header_layer);
  text_layer_destroy(s_label_layer);

  action_bar_layer_destroy(s_action_bar);
}

static void init() {
  s_icon_next = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_ACTION_ICON_NEXT);
  s_icon_prev = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_ACTION_ICON_PREV);

  s_main_window = window_create();
  window_set_window_handlers(s_main_window, (WindowHandlers) {
    .load = main_window_load,
    .unload = main_window_unload,
  });
  window_stack_push(s_main_window, true);
  
  // Register callbacks
  app_message_register_inbox_received(inbox_received_callback);
  app_message_register_inbox_dropped(inbox_dropped_callback);
  app_message_register_outbox_failed(outbox_failed_callback);
  app_message_register_outbox_sent(outbox_sent_callback);
  
  
  app_message_open(app_message_inbox_size_maximum(), app_message_outbox_size_maximum());

}

static void deinit() {
  window_destroy(s_main_window);

  gbitmap_destroy(s_icon_next);
  gbitmap_destroy(s_icon_prev);
}

int main(void) {
  init();
  app_event_loop();
  deinit();
}
