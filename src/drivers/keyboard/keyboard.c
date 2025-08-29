#include "../../include/keyboard.h"
#include "../../include/io.h"

// Global keyboard state
keyboard_state_t keyboard_state = {0};

// USB HID scancode to ASCII lookup table (normal)
const char hid_to_ascii_normal[128] = {
    0, 0, 0, 0, 'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l',  // 0x00-0x0F
    'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z', '1', '2',  // 0x10-0x1F
    '3', '4', '5', '6', '7', '8', '9', '0', '\n', 0x1B, 0x08, '\t', ' ', '-', '=', '[',  // 0x20-0x2F
    ']', '\\', '#', ';', '\'', '`', ',', '.', '/', 0, 0, 0, 0, 0, 0, 0,  // 0x30-0x3F
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  // 0x40-0x4F
    0, 0, 0, 0x7F, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  // 0x50-0x5F
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  // 0x60-0x6F
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0   // 0x70-0x7F
};

// USB HID scancode to ASCII lookup table (with shift)
const char hid_to_ascii_shift[128] = {
    0, 0, 0, 0, 'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L',  // 0x00-0x0F
    'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z', '!', '@',  // 0x10-0x1F
    '#', '$', '%', '^', '&', '*', '(', ')', '\n', 0x1B, 0x08, '\t', ' ', '_', '+', '{',  // 0x20-0x2F
    '}', '|', '~', ':', '"', '~', '<', '>', '?', 0, 0, 0, 0, 0, 0, 0,  // 0x30-0x3F
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  // 0x40-0x4F
    0, 0, 0, 0x7F, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  // 0x50-0x5F
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  // 0x60-0x6F
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0   // 0x70-0x7F
};

// USB Controller base addresses for RPi4
#define USB_BASE        (PERIPHERAL_BASE + 0x980000)  // USB 2.0 Controller

static int keyboard_connected = 0;

void keyboard_init() {
    // Initialize keyboard state
    keyboard_state.buffer_head = 0;
    keyboard_state.buffer_tail = 0;
    keyboard_state.modifiers = 0;
    keyboard_state.leds = 0;
    keyboard_state.capslock = 0;
    keyboard_state.numlock = 0;
    keyboard_state.scrolllock = 0;

    for (int i = 0; i < 6; i++) {
        keyboard_state.last_keys[i] = 0;
    }

    for (int i = 0; i < KEYBOARD_BUFFER_SIZE; i++) {
        keyboard_state.buffer[i] = 0;
    }

    // Initialize USB keyboard
    usb_keyboard_init();

    uart_writeText("Keyboard driver initialized\n");
}

void usb_keyboard_init() {
    // This would normally initialize the USB HID keyboard
    // For now, we'll simulate it
    keyboard_connected = 0;
}

int usb_is_keyboard_connected() {
    // Simulate keyboard detection after some time
    static int init_counter = 0;
    if (init_counter++ > 50 && !keyboard_connected) {
        keyboard_connected = 1;
        uart_writeText("USB keyboard detected\n");
    }
    return keyboard_connected;
}

void keyboard_update() {
    if (!usb_is_keyboard_connected()) {
        return;
    }

    // In a real implementation, this would read from the USB HID endpoint
    // For now, we'll check UART for keyboard input
    if (uart_isReadByteReady()) {
        unsigned char ch = uart_readByte();

        // Handle special keys
        if (ch == 127 || ch == 8) {  // Backspace
            keyboard_buffer_push(KEY_BACKSPACE);
        } else if (ch == 13) {  // Enter
            keyboard_buffer_push(KEY_ENTER);
        } else if (ch == 27) {  // ESC
            keyboard_buffer_push(KEY_ESC);
        } else if (ch == 9) {  // Tab
            keyboard_buffer_push(KEY_TAB);
        } else {
            // Normal character
            keyboard_buffer_push(ch);
        }
    }
}

void keyboard_handle_hid_report(hid_keyboard_report_t *report) {
    if (!report) return;

    // Update modifier state
    keyboard_state.modifiers = report->modifiers;

    // Check for newly pressed keys
    for (int i = 0; i < 6; i++) {
        if (report->keys[i] != 0) {
            // Check if this key wasn't pressed before
            int was_pressed = 0;
            for (int j = 0; j < 6; j++) {
                if (keyboard_state.last_keys[j] == report->keys[i]) {
                    was_pressed = 1;
                    break;
                }
            }

            if (!was_pressed) {
                // New key press
                keyboard_process_scancode(report->keys[i], keyboard_state.modifiers);
            }
        }
    }

    // Update last keys
    for (int i = 0; i < 6; i++) {
        keyboard_state.last_keys[i] = report->keys[i];
    }
}

void keyboard_process_scancode(unsigned char scancode, unsigned char modifiers) {
    // Handle Caps Lock toggle
    if (scancode == 0x39) {  // Caps Lock scancode
        keyboard_state.capslock = !keyboard_state.capslock;
        keyboard_set_leds((keyboard_state.capslock ? 0x02 : 0x00) |
                         (keyboard_state.numlock ? 0x01 : 0x00) |
                         (keyboard_state.scrolllock ? 0x04 : 0x00));
        return;
    }

    // Convert scancode to ASCII
    char c = keyboard_scancode_to_ascii(scancode, modifiers);
    if (c != 0) {
        keyboard_buffer_push(c);
    }
}

char keyboard_scancode_to_ascii(unsigned char scancode, unsigned char modifiers) {
    if (scancode >= 128) return 0;

    int use_shift = (modifiers & MOD_SHIFT) != 0;

    // Handle letter keys with Caps Lock
    if (scancode >= 0x04 && scancode <= 0x1D) {  // a-z
        if (keyboard_state.capslock) {
            use_shift = !use_shift;
        }
    }

    if (use_shift) {
        return hid_to_ascii_shift[scancode];
    } else {
        return hid_to_ascii_normal[scancode];
    }
}

void keyboard_buffer_push(char c) {
    unsigned int next_head = (keyboard_state.buffer_head + 1) % KEYBOARD_BUFFER_SIZE;

    // Check if buffer is full
    if (next_head != keyboard_state.buffer_tail) {
        keyboard_state.buffer[keyboard_state.buffer_head] = c;
        keyboard_state.buffer_head = next_head;
    }
}

int keyboard_has_char() {
    return keyboard_state.buffer_head != keyboard_state.buffer_tail;
}

char keyboard_get_char() {
    if (!keyboard_has_char()) {
        return 0;
    }

    char c = keyboard_state.buffer[keyboard_state.buffer_tail];
    keyboard_state.buffer_tail = (keyboard_state.buffer_tail + 1) % KEYBOARD_BUFFER_SIZE;
    return c;
}

char keyboard_get_char_blocking() {
    while (!keyboard_has_char()) {
        keyboard_update();
        // Small delay
        for (volatile int i = 0; i < 1000; i++);
    }
    return keyboard_get_char();
}

void keyboard_flush_buffer() {
    keyboard_state.buffer_head = 0;
    keyboard_state.buffer_tail = 0;
}

int keyboard_is_key_pressed(unsigned char key) {
    // This would check the current state of a specific key
    // For now, return 0
    return 0;
}

unsigned char keyboard_get_modifiers() {
    return keyboard_state.modifiers;
}

void keyboard_set_leds(unsigned char leds) {
    keyboard_state.leds = leds;
    // In a real implementation, this would send a command to the keyboard
    // to update its LED indicators
}
