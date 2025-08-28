#ifndef MOUSE_H
#define MOUSE_H

// Mouse button definitions
#define MOUSE_LEFT_BUTTON   0x01
#define MOUSE_RIGHT_BUTTON  0x02
#define MOUSE_MIDDLE_BUTTON 0x04

// Mouse state structure
typedef struct {
    int x;                  // X position
    int y;                  // Y position
    int delta_x;            // X movement delta
    int delta_y;            // Y movement delta
    unsigned char buttons;  // Button state bitmask
    unsigned char wheel;    // Wheel movement
    int visible;           // Cursor visibility
} mouse_state_t;

// USB HID mouse report structure
typedef struct {
    unsigned char buttons;
    signed char delta_x;
    signed char delta_y;
    signed char wheel;
} __attribute__((packed)) hid_mouse_report_t;

// Function prototypes
void mouse_init();
void mouse_update();
void mouse_set_position(int x, int y);
void mouse_get_position(int *x, int *y);
unsigned char mouse_get_buttons();
void mouse_show_cursor();
void mouse_hide_cursor();
void mouse_draw_cursor();
int mouse_is_button_pressed(unsigned char button);
void mouse_handle_hid_report(hid_mouse_report_t *report);

// USB HID related functions
void usb_init();
void usb_scan_devices();
int usb_is_mouse_connected();

// Internal functions
void mouse_process_movement(signed char delta_x, signed char delta_y);
void mouse_process_buttons(unsigned char button_state);

// Global mouse state (extern declaration)
extern mouse_state_t mouse_state;

// Cursor appearance constants
#define CURSOR_WIDTH  16
#define CURSOR_HEIGHT 16

// Cursor bitmap (simple arrow cursor)
extern const unsigned char cursor_bitmap[CURSOR_HEIGHT][CURSOR_WIDTH];

#endif // MOUSE_H
