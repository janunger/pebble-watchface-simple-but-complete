#include <pebble.h>

static Window *main_window;
static TextLayer *time_layer;
static GFont time_font;

static void update_time() {
    time_t temp = time(NULL);
    struct tm *tick_time = localtime(&temp);
    static char buffer[] = "00:00";
    if (clock_is_24h_style() == true) {
        strftime(buffer, sizeof("00:00"), "%H:%M", tick_time);
    } else {
        strftime(buffer, sizeof("00:00"), "%I:%M", tick_time);
    }
    text_layer_set_text(time_layer, buffer);
}

static void main_window_load(Window *window) {
    time_layer = text_layer_create(GRect(0, 30, 144, 61));
    text_layer_set_background_color(time_layer, GColorClear);
    text_layer_set_text_color(time_layer, GColorBlack);
    time_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_ARIAL_NARROW_BOLD_60));
    text_layer_set_font(time_layer, time_font);
    text_layer_set_text_alignment(time_layer, GTextAlignmentCenter);
    layer_add_child(window_get_root_layer(main_window), text_layer_get_layer(time_layer));
}

static void main_window_unload(Window *window) {
    text_layer_destroy(time_layer);
}

static void tick_handler(struct tm *tick_time, TimeUnits units_changed) {
    update_time();
}

static void init(void) {
    main_window = window_create();
    window_set_window_handlers(main_window, (WindowHandlers) {
        .load = main_window_load,
        .unload = main_window_unload
    });
    window_stack_push(main_window, true);
    update_time();
    tick_timer_service_subscribe(MINUTE_UNIT, tick_handler);
}

static void deinit(void) {
    window_destroy(main_window);
}

int main(void) {
    init();
    app_event_loop();
    deinit();
}
