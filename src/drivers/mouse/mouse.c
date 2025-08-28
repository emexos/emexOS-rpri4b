#include "../../include/mouse.h"
#include "../../include/fb.h"
#include "../../include/io.h"

// Global mouse state
mouse_state_t mouse_state = {0};

// Simple arrow cursor bitmap (16x16)
const unsigned char cursor_bitmap[CURSOR_HEIGHT][CURSOR_WIDTH] = {
    {1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {1,2,1,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {1,2,2,1,0,0,0,0,0,0,0,0,0,0,0,0},
    {1,2,2,2,1,0,0,0,0,0,0,0,0,0,0,0},
    {1,2,2,2,2,1,0,0,0,0,0,0,0,0,0,0},
    {1,2,2,2,2,2,1,0,0,0,0,0,0,0,0,0},
    {1,2,2,2,2,2,2,1,0,0,0,0,0,0,0,0},
    {1,2,2,2,2,2,2,2,1,0,0,0,0,0,0,0},
    {1,2,2,2,2,2,2,2,2,1,0,0,0,0,0,0},
    {1,2,2,2,2,2,2,2,2,2,1,0,0,0,0,0},
    {1,2,2,2,2,2,1,1,1,1,1,0,0,0,0,0},
    {1,2,2,1,2,2,1,0,0,0,0,0,0,0,0,0},
    {1,2,1,0,1,2,2,1,0,0,0,0,0,0,0,0},
    {1,1,0,0,1,2,2,1,0,0,0,0,0,0,0,0},
    {1,0,0,0,0,1,2,2,1,0,0,0,0,0,0,0},
    {0,0,0,0,0,1,1,1,0,0,0,0,0,0,0,0}
};

// Screen dimensions (assuming 1920x1080 from fb.c)
static int screen_width = 1920;
static int screen_height = 1080;

// USB Controller base addresses for RPi4
#define USB_BASE        (PERIPHERAL_BASE + 0x980000)  // USB 2.0 Controller
#define USB3_BASE       (PERIPHERAL_BASE + 0x9A0000)  // USB 3.0 Controller

// Basic USB HID constants
#define USB_HID_MOUSE_PROTOCOL 2
#define USB_REQ_GET_REPORT     0x01
#define USB_REQ_SET_IDLE       0x0A

// Simple USB device detection (simplified for this example)
static int mouse_connected = 0;
static unsigned char usb_mouse_buffer[8];

void mouse_init() {
    // Initialize mouse state
    mouse_state.x = screen_width / 2;
    mouse_state.y = screen_height / 2;
    mouse_state.delta_x = 0;
    mouse_state.delta_y = 0;
    mouse_state.buttons = 0;
    mouse_state.wheel = 0;
    mouse_state.visible = 1;

    // Initialize USB subsystem
    usb_init();

    uart_writeText("Mouse driver initialized\n");
}

void usb_init() {
    // Basic USB controller initialization
    // This is a simplified version - full USB initialization is quite complex

    // Enable USB power
    mmio_write(USB_BASE + 0x00, 0x00000001);

    // Wait for power up
    for(volatile int i = 0; i < 1000000; i++);

    // Initialize USB host controller
    mmio_write(USB_BASE + 0x04, 0x00000000);  // Reset
    mmio_write(USB_BASE + 0x08, 0x00000001);  // Enable

    uart_writeText("USB controller initialized\n");
}

void usb_scan_devices() {
    // Simplified device scanning
    // In a real implementation, this would enumerate USB devices
    // and check for HID mouse devices

    static int scan_counter = 0;
    scan_counter++;

    // Simulate mouse detection after some scans
    if (scan_counter > 100 && !mouse_connected) {
        mouse_connected = 1;
        uart_writeText("USB mouse detected\n");
    }
}

int usb_is_mouse_connected() {
    return mouse_connected;
}

void mouse_update() {
    // Scan for USB devices periodically
    usb_scan_devices();

    if (!mouse_connected) {
        return;
    }

    // Simulate reading HID mouse report
    // In a real implementation, this would read from USB interrupt endpoint
    static int update_counter = 0;
    update_counter++;

    // Simulate some mouse movement for testing
    if (update_counter % 1000 == 0) {
        hid_mouse_report_t test_report;
        test_report.buttons = 0;
        test_report.delta_x = (update_counter / 1000) % 3 - 1;  // -1, 0, 1
        test_report.delta_y = ((update_counter / 1000) / 3) % 3 - 1;
        test_report.wheel = 0;

        mouse_handle_hid_report(&test_report);
    }

    // Draw cursor if visible
    if (mouse_state.visible) {
        mouse_draw_cursor();
    }
}

void mouse_handle_hid_report(hid_mouse_report_t *report) {
    if (!report) return;

    // Process movement
    mouse_process_movement(report->delta_x, report->delta_y);

    // Process buttons
    mouse_process_buttons(report->buttons);

    // Process wheel
    mouse_state.wheel = report->wheel;
}

void mouse_process_movement(signed char delta_x, signed char delta_y) {
    // Store deltas
    mouse_state.delta_x = delta_x;
    mouse_state.delta_y = delta_y;

    // Update absolute position
    mouse_state.x += delta_x;
    mouse_state.y += delta_y;

    // Clamp to screen boundaries
    if (mouse_state.x < 0) mouse_state.x = 0;
    if (mouse_state.x >= screen_width) mouse_state.x = screen_width - 1;
    if (mouse_state.y < 0) mouse_state.y = 0;
    if (mouse_state.y >= screen_height) mouse_state.y = screen_height - 1;
}

void mouse_process_buttons(unsigned char button_state) {
    mouse_state.buttons = button_state;
}

void mouse_set_position(int x, int y) {
    if (x >= 0 && x < screen_width && y >= 0 && y < screen_height) {
        mouse_state.x = x;
        mouse_state.y = y;
    }
}

void mouse_get_position(int *x, int *y) {
    if (x) *x = mouse_state.x;
    if (y) *y = mouse_state.y;
}

unsigned char mouse_get_buttons() {
    return mouse_state.buttons;
}

int mouse_is_button_pressed(unsigned char button) {
    return (mouse_state.buttons & button) != 0;
}

void mouse_show_cursor() {
    mouse_state.visible = 1;
}

void mouse_hide_cursor() {
    mouse_state.visible = 0;
}

void mouse_draw_cursor() {
    int x = mouse_state.x;
    int y = mouse_state.y;

    // Draw cursor bitmap
    for (int cy = 0; cy < CURSOR_HEIGHT; cy++) {
        for (int cx = 0; cx < CURSOR_WIDTH; cx++) {
            int screen_x = x + cx;
            int screen_y = y + cy;

            // Check bounds
            if (screen_x >= 0 && screen_x < screen_width &&
                screen_y >= 0 && screen_y < screen_height) {

                unsigned char pixel = cursor_bitmap[cy][cx];

                if (pixel == 1) {
                    // Black outline
                    drawPixel(screen_x, screen_y, 0x00);  // Black
                } else if (pixel == 2) {
                    // White fill
                    drawPixel(screen_x, screen_y, 0x0F);  // White
                }
                // pixel == 0 means transparent, don't draw
            }
        }
    }
}
