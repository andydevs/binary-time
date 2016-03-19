#include <pebble.h>

//--------------------------PROGRAM CONSTANTS--------------------------

/**
 * The number of bits in the hour
 */
#define HOUR_BINARY_LENGTH 5

/**
 * The number of bits in the minute
 */
#define MINUTE_BINARY_LENGTH 6

/**
 * The number of bits in the month
 */
#define MONTH_BINARY_LENGTH 4

/**
 * The number of bits in the day
 */
#define DAY_BINARY_LENGTH 6


/**
 * The left margin of the TextLayers if the watch is square
 */
#define LEFT_MARGIN_SQUARE 10

/**
 * The left margin of the TextLayers if the watch is round
 */
#define LEFT_MARGIN_ROUND 25


/**
 * The top margin of the time TextLayers if the watch is square
 */
#define TIME_TOP_MARGIN_SQUARE 10

/**
 * The top margin of the date TextLayers if the watch is round
 */
#define TIME_TOP_MARGIN_ROUND 24

/**
 * The height of the time TextLayers
 */
#define TIME_HEIGHT 32


/**
 * The top margin of the date TextLayers if the watch is square
 */
#define DATE_TOP_MARGIN_SQUARE 100

/**
 * The top margin of the date TextLayers if the watch is round
 */
#define DATE_TOP_MARGIN_ROUND 100

/**
 * The height of the date TextLayers
 */
#define DATE_HEIGHT 24


//--------------------------PROGRAM RESOURCES--------------------------

/**
 * The font of the time
 */
static GFont time_font;

/**
 * The font of the date
 */
static GFont date_font;



//--------------------------PROGRAM OBJECTS--------------------------

/**
 * The main window of the program
 */
static Window* main_window;

/**
 * Called when the given window is loaded
 *
 * @param window the window that was loaded
 */
static void on_main_window_load(Window* window);

/**
 * Called when the given window is unloaded
 *
 * @param window the window that was unloaded
 */
static void on_main_window_unload(Window* window);


/**
 * Displays the hours of the time
 */
static TextLayer* hour_layer;

/**
 * Displays the minutes of the time
 */
static TextLayer* minute_layer;

/**
 * Displays the month of the date
 */
static TextLayer* month_layer;

/**
 * Displays the day of the date
 */
static TextLayer* day_layer;


/**
 * Called on every tick
 *
 * @param tick_time     the current time
 * @param units_changed the amount of time since the last tick
 */
static void on_tick(struct tm* tick_time, TimeUnits units_changed);

/**
 * Displays the time on the watch
 *
 * @param tick_time the time to display on the watch
 */
static void display_time(struct tm* tick_time);

/**
 * Logs decimal and binary representations of time to console
 *
 * @param tick_time the time being displayed
 */
static void debug_time(struct tm* tick_time);

/**
 * Returns the hours of the tick_time formatted correctly
 *
 * @param tick_time the tick_time to get the hours from
 *
 * @return the hours formatted correctly
 */
static int tm_get_hours(struct tm* tick_time);

/**
 * Returns the months of the tick_time formatted correctly
 *
 * @param tick_time the tick_time to get the months from
 *
 * @return the months formatted correctly
 */
static int tm_get_months(struct tm* tick_time);

/**
 * Fills the given buffer with the given value represented in binary
 *
 * @param buffer the buffer to write to
 * @param length the length of the buffer to write to
 * @param value  the value to write as binary
 */
static void format_as_binary(char* buffer, size_t length, int value);

/**
 * Raises the given base to the given exponent
 *
 * @param base   the base to raise to the given exponent
 * @param expont the exponent to raise the base to
 *
 * @return the given base raised to the given exponent
 */
static int power(int base, int expont);



//--------------------------BINARY BUFFERS--------------------------

/**
 * Stores the binary representation of the hours
 */
static char hour_binary_buffer[HOUR_BINARY_LENGTH + 1];

/**
 * Stores the binary representation of the minutes
 */ 
static char minute_binary_buffer[MINUTE_BINARY_LENGTH + 1];

/**
 * Stores the binary representation of the month
 */
static char month_binary_buffer[MONTH_BINARY_LENGTH + 1];

/**
 * Stores the binary representation of the day
 */
static char day_binary_buffer[DAY_BINARY_LENGTH + 1];


//--------------------------DEBUG BUFFERS--------------------------

/**
 * String representation of hour value for debug purposes
 */
static char hour_debug_buffer[32];

/**
 * String representation of minute value for debug purposes
 */
static char minute_debug_buffer[32];

/**
 * String representation of month value for debug purposes
 */
static char month_debug_buffer[32];

/**
 * String representation of day value for debug purposes
 */
static char day_debug_buffer[32];



//--------------------------BINARY TIME FUNCTIONS--------------------------

/**
 * Displays the time and date on the watch
 *
 * @param tick_time the time to display on the watch
 */
static void display_time(struct tm* tick_time) {
    // Fill binary buffers with binaries of the time and date values
    format_as_binary(hour_binary_buffer, sizeof(hour_binary_buffer), tm_get_hours(tick_time));
    format_as_binary(minute_binary_buffer, sizeof(minute_binary_buffer), tick_time->tm_min);
    format_as_binary(month_binary_buffer, sizeof(month_binary_buffer), tm_get_months(tick_time));
    format_as_binary(day_binary_buffer, sizeof(day_binary_buffer), tick_time->tm_mday);
    
    // Log time, along with new binary values, to console
    debug_time(tick_time);
    
    // Set text layers with time and date buffers
    text_layer_set_text(hour_layer, hour_binary_buffer);
    text_layer_set_text(minute_layer, minute_binary_buffer);
    text_layer_set_text(month_layer, month_binary_buffer);
    text_layer_set_text(day_layer, day_binary_buffer);
}

/**
 * Returns the hours of the tick_time formatted correctly
 *
 * @param tick_time the tick_time to get the hours from
 *
 * @return the hours formatted correctly
 */
static int tm_get_hours(struct tm* tick_time) {
    // Get hours from tick_time
    int hours = tick_time->tm_hour;
    
    // If clock is 24 hour style, return hours
    // Else if hour mod 12 is 0, return 12 
    // (as 0 is represented by 12 in 12 hour clock)
    // Else return hours mod 12
    return clock_is_24h_style() ? hours 
                                : (hours % 12 == 0 ? 12 
                                                   : hours % 12);
}

/**
 * Returns the months of the tick_time formatted correctly
 *
 * @param tick_time the tick_time to get the months from
 *
 * @return the months formatted correctly
 */
static int tm_get_months(struct tm* tick_time) {
    return tick_time->tm_mon + 1;
}

/**
 * Raises the given base to the given exponent
 *
 * @param base   the base to raise to the given exponent
 * @param expont the exponent to raise the base to
 *
 * @return the given base raised to the given exponent
 */
static int power(int base, int expont) {
    // Initialize power value
    int p = 1;
    
    // Multiply by base exponent number of times
    for (int i = 0; i < expont; i++) {
        p *= base;
    }
    
    // Return final value
    return p;
}

/**
 * Fills the given buffer with the given value represented in binary
 *
 * @param buffer the buffer to write to
 * @param length the length of the buffer to write to
 * @param value  the value to write as binary
 */
static void format_as_binary(char* buffer, size_t length, int value) {
    // Initialize store and end integer values
    int store = value, end = (int)length - 1;
    
    // For each location from start to end of string
    for (int i = 0; i < end; i++) {
        // If the store is greater than the binary value 
        // represented by the digit at the location
        if (store >= power(2, end - 1 - i)) {
            // Write a 1 to the buffer at the location
            // And subtract value from store
            buffer[i] = '1';
            store -= power(2, end - 1 - i);
        } else {
            // Write a 0 to the buffer at the location
            buffer[i] = '0';
        }
    }
    
    // Append null terminator character
    buffer[end] = '\0';
}



//--------------------------DEBUG FUNCTION--------------------------

/**
 * Logs decimal and binary representations of time to console
 *
 * @param tick_time the time being displayed
 */
static void debug_time(struct tm* tick_time) {
    // Prints hour and minute values to buffers
    snprintf(hour_debug_buffer, sizeof(hour_debug_buffer), "Hours %i --> %s", tm_get_hours(tick_time), hour_binary_buffer);
    snprintf(minute_debug_buffer, sizeof(minute_debug_buffer), "Minutes %i --> %s", tick_time->tm_min, minute_binary_buffer);
    snprintf(month_debug_buffer, sizeof(month_debug_buffer), "Month %i --> %s", tm_get_months(tick_time), month_binary_buffer);
    snprintf(day_debug_buffer, sizeof(day_debug_buffer), "Day %i --> %s", tick_time->tm_mday, day_binary_buffer);
    
    // Log values to console
    APP_LOG(APP_LOG_LEVEL_INFO, "%s", hour_debug_buffer);
    APP_LOG(APP_LOG_LEVEL_INFO, "%s", minute_debug_buffer);
    APP_LOG(APP_LOG_LEVEL_INFO, "%s", month_debug_buffer);
    APP_LOG(APP_LOG_LEVEL_INFO, "%s", day_debug_buffer);
}



//--------------------------TICK TIMER HANDLER--------------------------

/**
 * Called on every tick
 *
 * @param tick_time     the current time
 * @param units_changed the amount of time since the last tick
 */
static void on_tick(struct tm* tick_time, TimeUnits units_changed) {
    display_time(tick_time);
}



//--------------------------WINDOW HANDLERS--------------------------

/**
 * Called when the given window is loaded
 *
 * @param window the window that was loaded
 */
static void on_main_window_load(Window* window) {
    // Load resources
    time_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_KEY_PERFECT_DOS_32));
    date_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_KEY_PERFECT_DOS_24));
    
    // Get window information
    Layer *window_layer = window_get_root_layer(window);
    GRect window_bounds = layer_get_bounds(window_layer);
    
    // Create time and date layers
    hour_layer = text_layer_create(GRect(PBL_IF_ROUND_ELSE(LEFT_MARGIN_ROUND, LEFT_MARGIN_SQUARE),
                                         PBL_IF_ROUND_ELSE(TIME_TOP_MARGIN_ROUND, TIME_TOP_MARGIN_SQUARE),
                                         window_bounds.size.w - PBL_IF_ROUND_ELSE(LEFT_MARGIN_ROUND, LEFT_MARGIN_SQUARE),
                                         TIME_HEIGHT));
    minute_layer = text_layer_create(GRect(PBL_IF_ROUND_ELSE(LEFT_MARGIN_ROUND, LEFT_MARGIN_SQUARE),
                                           PBL_IF_ROUND_ELSE(TIME_TOP_MARGIN_ROUND, TIME_TOP_MARGIN_SQUARE) + TIME_HEIGHT,
                                           window_bounds.size.w - PBL_IF_ROUND_ELSE(LEFT_MARGIN_ROUND, LEFT_MARGIN_SQUARE),
                                           TIME_HEIGHT));
    month_layer = text_layer_create(GRect(PBL_IF_ROUND_ELSE(LEFT_MARGIN_ROUND, LEFT_MARGIN_SQUARE),
                                          PBL_IF_ROUND_ELSE(DATE_TOP_MARGIN_ROUND, DATE_TOP_MARGIN_SQUARE),
                                          window_bounds.size.w - PBL_IF_ROUND_ELSE(LEFT_MARGIN_ROUND, LEFT_MARGIN_SQUARE),
                                          DATE_HEIGHT));
    day_layer = text_layer_create(GRect(PBL_IF_ROUND_ELSE(LEFT_MARGIN_ROUND, LEFT_MARGIN_SQUARE),
                                        PBL_IF_ROUND_ELSE(DATE_TOP_MARGIN_ROUND, DATE_TOP_MARGIN_SQUARE) + DATE_HEIGHT,
                                        window_bounds.size.w - PBL_IF_ROUND_ELSE(LEFT_MARGIN_ROUND, LEFT_MARGIN_SQUARE),
                                        DATE_HEIGHT));
    
    // Set style of hour layer
    text_layer_set_background_color(hour_layer, GColorClear);
    text_layer_set_text_color(hour_layer, GColorGreen);
    text_layer_set_text_alignment(hour_layer, GTextAlignmentLeft);
    text_layer_set_font(hour_layer, time_font);
    
    // Set style of minute layer
    text_layer_set_background_color(minute_layer, GColorClear);
    text_layer_set_text_color(minute_layer, GColorGreen);
    text_layer_set_text_alignment(minute_layer, GTextAlignmentLeft);
    text_layer_set_font(minute_layer, time_font);
    
    // Set style of month layer
    text_layer_set_background_color(month_layer, GColorClear);
    text_layer_set_text_color(month_layer, GColorGreen);
    text_layer_set_text_alignment(month_layer, GTextAlignmentLeft);
    text_layer_set_font(month_layer, date_font);
    
    // Set style of day layer
    text_layer_set_background_color(day_layer, GColorClear);
    text_layer_set_text_color(day_layer, GColorGreen);
    text_layer_set_text_alignment(day_layer, GTextAlignmentLeft);
    text_layer_set_font(day_layer, date_font);
    
    // Append text layers to window
    layer_add_child(window_layer, text_layer_get_layer(hour_layer));
    layer_add_child(window_layer, text_layer_get_layer(minute_layer));
    layer_add_child(window_layer, text_layer_get_layer(month_layer));
    layer_add_child(window_layer, text_layer_get_layer(day_layer));
    
    // Display time on init
    time_t init_time = time(NULL);
    struct tm* init_tick_time = localtime(&init_time);
    display_time(init_tick_time);
}

/**
 * Called when the given window is unloaded
 *
 * @param window the window that was unloaded
 */
static void on_main_window_unload(Window* window) {
    // Destroy layers
    text_layer_destroy(hour_layer);
    text_layer_destroy(minute_layer);
    text_layer_destroy(month_layer);
    text_layer_destroy(day_layer);
}



//--------------------------MAIN PROGRAM--------------------------

/**
 * Called at the start of the program
 */
static void start(void) {
    // Register with the tick timer service
    tick_timer_service_subscribe(MINUTE_UNIT, on_tick);
    
    // Create main window
    main_window = window_create();
    
    // Set window handlers
    window_set_window_handlers(main_window, (WindowHandlers) {
        .load = on_main_window_load,
        .unload = on_main_window_unload
    });
    
    // Set window color
    window_set_background_color(main_window, GColorBlack);
    
    // Display main window
    window_stack_push(main_window, true);
}

/**
 * Called at the end of the program
 */
static void end(void) {
    // Destroy main window
    window_destroy(main_window);
    
    // Unsubscribe from the tick timer service
    tick_timer_service_unsubscribe();
}

/**
 * The main method of the program
 */
int main(void) {
  start();
  app_event_loop();
  end();
}
