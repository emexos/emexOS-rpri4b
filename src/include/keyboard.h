#ifndef KEYBOARD_H
#define KEYBOARD_H

// Key codes for special keys
#define KEY_ESC         0x1B
#define KEY_BACKSPACE   0x08
#define KEY_TAB         0x09
#define KEY_ENTER       0x0D
#define KEY_CTRL        0x11
#define KEY_SHIFT       0x10
#define KEY_ALT         0x12
#define KEY_CAPSLOCK    0x14
#define KEY_F1          0x70
#define KEY_F2          0x71
#define KEY_F3          0x72
#define KEY_F4          0x73
#define KEY_F5          0x74
#define KEY_F6          0x75
#define KEY_F7          0x76
#define KEY_F8          0x77
#define KEY_F9          0x78
#define KEY_F10         0x79
#define KEY_F11         0x7A
#define KEY_F12         0x7B
#define KEY_HOME        0x24
#define KEY_END         0x23
#define KEY_PAGEUP      0x21
#define KEY_PAGEDOWN    0x22
#define KEY_INSERT      0x2D
#define KEY_DELETE      0x2E
#define KEY_LEFT        0x25
#define KEY_UP          0x26
#define KEY_RIGHT       0x27
#define KEY_DOWN        0x28

// Modifier key flags
#define MOD_SHIFT       0x01
#define MOD_CTRL        0x02
#define MOD_ALT         0x04
#define MOD_CAPSLOCK    0x08
#define MOD_NUMLOCK     0x10
#define MOD_SCROLLLOCK  0x20

// Keyboard buffer size
#define KEYBOARD_BUFFER_SIZE 256

// USB HID keyboard report structure
typedef struct {
    unsigned char modifiers;
    unsigned char reserved;
    unsigned char keys[6];
} __attribute__((packed)) hid_keyboard_report_t;

// Keyboard state structure
typedef struct {
    unsigned char buffer[KEYBOARD_BUFFER_SIZE];
    unsigned int buffer_head;
    unsigned int buffer_tail;
    unsigned char modifiers;
    unsigned char leds;
    int capslock;
    int numlock;
    int scrolllock;
    unsigned char last_keys[6];
} keyboard_state_t;

// Function prototypes
void keyboard_init();
void keyboard_update();
int keyboard_has_char();
char keyboard_get_char();
char keyboard_get_char_blocking();
void keyboard_flush_buffer();
int keyboard_is_key_pressed(unsigned char key);
unsigned char keyboard_get_modifiers();
void keyboard_set_leds(unsigned char leds);
void keyboard_handle_hid_report(hid_keyboard_report_t *report);

// USB HID related functions
int usb_is_keyboard_connected();
void usb_keyboard_init();

// Internal functions
void keyboard_process_scancode(unsigned char scancode, unsigned char modifiers);
char keyboard_scancode_to_ascii(unsigned char scancode, unsigned char modifiers);
void keyboard_buffer_push(char c);

// Global keyboard state (extern declaration)
extern keyboard_state_t keyboard_state;

// USB HID to ASCII lookup tables
extern const char hid_to_ascii_normal[128];
extern const char hid_to_ascii_shift[128];

#endif // KEYBOARD_H
