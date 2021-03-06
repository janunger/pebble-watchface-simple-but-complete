#include <pebble.h>

static Window *main_window;
static TextLayer *time_layer, *seconds_layer, *date_layer, *battery_text_layer;
static GFont time_font, seconds_font, date_font, battery_font;
static char battery_level_text[8];
static BitmapLayer *bt_icon_layer, *battery_icon_layer;
static GBitmap *bt_icon_bitmap, *battery_0_bitmap, *battery_1_bitmap, *battery_2_bitmap, *battery_3_bitmap, *battery_4_bitmap, *battery_5_bitmap, *battery_loading_bitmap;

static void update_time() {
    time_t temp = time(NULL);
    struct tm *tick_time = localtime(&temp);
    static char time_buffer[] = "00:00";
    if (clock_is_24h_style() == true) {
        strftime(time_buffer, sizeof("00:00"), "%H:%M", tick_time);
    } else {
        strftime(time_buffer, sizeof("00:00"), "%I:%M", tick_time);
    }
    if (time_buffer[0] == '0') {
        memmove(time_buffer, time_buffer + 1, strlen(time_buffer));
    }
    text_layer_set_text(time_layer, time_buffer);

    static char seconds_buffer[8];
    strftime(seconds_buffer, sizeof(seconds_buffer), "%S", tick_time);
    text_layer_set_text(seconds_layer, seconds_buffer);

    // setLocale is currently broken, so we format the date ourselves.
    static char fw[2], weekday[3];
    strftime(fw, sizeof(fw), "%w", tick_time);
    if (strcmp("0", fw) == 0) {
        strcpy(weekday, "So");
    } else if (strcmp("1", fw) == 0) {
        strcpy(weekday, "Mo");
    } else if (strcmp("2", fw) == 0) {
        strcpy(weekday, "Di");
    } else if (strcmp("3", fw) == 0) {
        strcpy(weekday, "Mi");
    } else if (strcmp("4", fw) == 0) {
        strcpy(weekday, "Do");
    } else if (strcmp("5", fw) == 0) {
        strcpy(weekday, "Fr");
    } else if (strcmp("6", fw) == 0) {
        strcpy(weekday, "Sa");
    } else {
        strcpy(weekday, "??");
    }

    static char day[11];
    strftime(day, sizeof(day), "%d.%m.%y", tick_time);

    static char date_buffer[16];
    strcpy(date_buffer, weekday);
    strcat(date_buffer, " ");
    strcat(date_buffer, day);
    text_layer_set_text(date_layer, date_buffer);
}

static void bluetooth_callback(bool connected) {
    layer_set_hidden(bitmap_layer_get_layer(bt_icon_layer), !connected);
    if (!connected) {
        vibes_double_pulse();
    }
}

static void main_window_load(Window *window) {
    time_layer = text_layer_create(GRect(0, 30, 144, 61));
    text_layer_set_background_color(time_layer, GColorClear);
    text_layer_set_text_color(time_layer, GColorBlack);
    time_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_LIBERATION_NARROW_BOLD_60));
    text_layer_set_font(time_layer, time_font);
    text_layer_set_text_alignment(time_layer, GTextAlignmentCenter);
    layer_add_child(window_get_root_layer(main_window), text_layer_get_layer(time_layer));

    seconds_layer = text_layer_create(GRect(0, 90, 144, 34));
    text_layer_set_background_color(seconds_layer, GColorClear);
    text_layer_set_text_color(seconds_layer, GColorBlack);
    seconds_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_LIBERATION_NARROW_BOLD_30));
    text_layer_set_font(seconds_layer, seconds_font);
    text_layer_set_text_alignment(seconds_layer, GTextAlignmentCenter);
    layer_add_child(window_get_root_layer(main_window), text_layer_get_layer(seconds_layer));

    date_layer = text_layer_create(GRect(0, 125, 144, 34));
    text_layer_set_background_color(date_layer, GColorClear);
    text_layer_set_text_color(date_layer, GColorBlack);
    date_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_LIBERATION_NARROW_BOLD_30));
    text_layer_set_font(date_layer, date_font);
    text_layer_set_text_alignment(date_layer, GTextAlignmentCenter);
    layer_add_child(window_get_root_layer(main_window), text_layer_get_layer(date_layer));
    
    battery_text_layer = text_layer_create(GRect(32, 0, 67, 34));
    text_layer_set_background_color(battery_text_layer, GColorClear);
    text_layer_set_text_color(battery_text_layer, GColorBlack);
    battery_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_LIBERATION_NARROW_BOLD_30));
    text_layer_set_font(battery_text_layer, battery_font);
    text_layer_set_text_alignment(battery_text_layer, GTextAlignmentLeft);
    layer_add_child(window_get_root_layer(main_window), text_layer_get_layer(battery_text_layer));

    battery_0_bitmap = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_BATTERY_0);
    battery_1_bitmap = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_BATTERY_1);
    battery_2_bitmap = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_BATTERY_2);
    battery_3_bitmap = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_BATTERY_3);
    battery_4_bitmap = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_BATTERY_4);
    battery_5_bitmap = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_BATTERY_5);
    battery_loading_bitmap = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_BATTERY_LOADING);
    battery_icon_layer = bitmap_layer_create(GRect(10, 7, 15, 24));
    bitmap_layer_set_bitmap(battery_icon_layer, battery_0_bitmap);
    layer_add_child(window_get_root_layer(main_window), bitmap_layer_get_layer(battery_icon_layer));

    bt_icon_bitmap = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_BT_ICON);
    bt_icon_layer = bitmap_layer_create(GRect(116, 7, 18, 24));
    bitmap_layer_set_bitmap(bt_icon_layer, bt_icon_bitmap);
    layer_add_child(window_get_root_layer(main_window), bitmap_layer_get_layer(bt_icon_layer));

    bluetooth_callback(bluetooth_connection_service_peek());
}

static void main_window_unload(Window *window) {
    fonts_unload_custom_font(time_font);
    fonts_unload_custom_font(seconds_font);
    fonts_unload_custom_font(date_font);
    fonts_unload_custom_font(battery_font);
    text_layer_destroy(time_layer);
    text_layer_destroy(seconds_layer);
    text_layer_destroy(date_layer);
    text_layer_destroy(battery_text_layer);
    gbitmap_destroy(bt_icon_bitmap);
    bitmap_layer_destroy(bt_icon_layer);
}

static void tick_handler(struct tm *tick_time, TimeUnits units_changed) {
    update_time();
}

static void battery_callback(BatteryChargeState state) {
    snprintf(battery_level_text, 8, "%d%%", state.charge_percent);
    if (state.is_charging == true) {
        bitmap_layer_set_bitmap(battery_icon_layer, battery_loading_bitmap);
    } else if (state.charge_percent >= 90) {
        bitmap_layer_set_bitmap(battery_icon_layer, battery_5_bitmap);
    } else if (state.charge_percent >= 70) {
        bitmap_layer_set_bitmap(battery_icon_layer, battery_4_bitmap);
    } else if (state.charge_percent >= 50) {
        bitmap_layer_set_bitmap(battery_icon_layer, battery_3_bitmap);
    } else if (state.charge_percent >= 30) {
        bitmap_layer_set_bitmap(battery_icon_layer, battery_2_bitmap);
    } else if (state.charge_percent >= 10) {
        bitmap_layer_set_bitmap(battery_icon_layer, battery_1_bitmap);
    } else {
        bitmap_layer_set_bitmap(battery_icon_layer, battery_0_bitmap);
    }
    text_layer_set_text(battery_text_layer, battery_level_text);
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
    bluetooth_connection_service_subscribe(bluetooth_callback);
}

static void deinit(void) {
    window_destroy(main_window);
}

int main(void) {
    init();
    app_event_loop();
    deinit();
}
