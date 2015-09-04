#include <pebble.h>

static Window *main_window;
static TextLayer *time_layer, *seconds_layer, *date_layer, *battery_layer;
static GFont time_font, seconds_font, date_font, battery_font;
static char battery_level_text[8];

static void update_time() {
    time_t temp = time(NULL);
    struct tm *tick_time = localtime(&temp);
    static char time_buffer[] = "00:00";
    if (clock_is_24h_style() == true) {
        strftime(time_buffer, sizeof("00:00"), "%H:%M", tick_time);
    } else {
        strftime(time_buffer, sizeof("00:00"), "%I:%M", tick_time);
    }
    text_layer_set_text(time_layer, time_buffer);

    static char seconds_buffer[8];
    strftime(seconds_buffer, sizeof(seconds_buffer), "%S", tick_time);
    text_layer_set_text(seconds_layer, seconds_buffer);

    static char date_buffer[16];
    strftime(date_buffer, sizeof(date_buffer), "%a %d %b", tick_time);
    text_layer_set_text(date_layer, date_buffer);
}

static void main_window_load(Window *window) {
    time_layer = text_layer_create(GRect(0, 30, 144, 61));
    text_layer_set_background_color(time_layer, GColorClear);
    text_layer_set_text_color(time_layer, GColorBlack);
    time_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_ARIAL_NARROW_BOLD_60));
    text_layer_set_font(time_layer, time_font);
    text_layer_set_text_alignment(time_layer, GTextAlignmentCenter);
    layer_add_child(window_get_root_layer(main_window), text_layer_get_layer(time_layer));

    seconds_layer = text_layer_create(GRect(0, 90, 144, 34));
    text_layer_set_background_color(seconds_layer, GColorClear);
    text_layer_set_text_color(seconds_layer, GColorBlack);
    seconds_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_ARIAL_NARROW_BOLD_30));
    text_layer_set_font(seconds_layer, seconds_font);
    text_layer_set_text_alignment(seconds_layer, GTextAlignmentCenter);
    layer_add_child(window_get_root_layer(main_window), text_layer_get_layer(seconds_layer));

    date_layer = text_layer_create(GRect(0, 125, 144, 34));
    text_layer_set_background_color(date_layer, GColorClear);
    text_layer_set_text_color(date_layer, GColorBlack);
    date_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_ARIAL_NARROW_BOLD_30));
    text_layer_set_font(date_layer, date_font);
    text_layer_set_text_alignment(date_layer, GTextAlignmentCenter);
    layer_add_child(window_get_root_layer(main_window), text_layer_get_layer(date_layer));
    
    battery_layer = text_layer_create(GRect(5, 5, 67, 34));
    text_layer_set_background_color(battery_layer, GColorClear);
    text_layer_set_text_color(battery_layer, GColorBlack);
    battery_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_ARIAL_NARROW_BOLD_24));
    text_layer_set_font(battery_layer, battery_font);
    text_layer_set_text_alignment(battery_layer, GTextAlignmentLeft);
    layer_add_child(window_get_root_layer(main_window), text_layer_get_layer(battery_layer));
}

static void main_window_unload(Window *window) {
    fonts_unload_custom_font(time_font);
    fonts_unload_custom_font(seconds_font);
    fonts_unload_custom_font(date_font);
    fonts_unload_custom_font(battery_font);
    text_layer_destroy(time_layer);
    text_layer_destroy(seconds_layer);
    text_layer_destroy(date_layer);
    text_layer_destroy(battery_layer);
}

static void tick_handler(struct tm *tick_time, TimeUnits units_changed) {
    update_time();
}

static void battery_callback(BatteryChargeState state) {
    snprintf(battery_level_text, 8, "B %d%%", state.charge_percent);
    text_layer_set_text(battery_layer, battery_level_text);
}

static void init(void) {
    main_window = window_create();
    window_set_window_handlers(main_window, (WindowHandlers) {
        .load = main_window_load,
        .unload = main_window_unload
    });
    window_stack_push(main_window, true);
    update_time();
    tick_timer_service_subscribe(SECOND_UNIT, tick_handler);
    battery_callback(battery_state_service_peek());
    battery_state_service_subscribe(battery_callback);
}

static void deinit(void) {
    window_destroy(main_window);
}

int main(void) {
    init();
    app_event_loop();
    deinit();
}
