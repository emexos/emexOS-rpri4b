#include "../include/io.h"
#include "../include/fb.h"
#include "../include/mouse.h"

void main() {
    uart_init();
    fb_init();
    mouse_init();

    // Draw initial interface
    drawRect(90,90,200,145,0x03,0);

    drawString(100,100,"emexOS rpi4b",0x0f);
    drawString(100,110,"Hello world!",0x0f);
    drawString(100,120,"Shell loaded",0x0f);
    drawString(100,130,"Mouse driver active",0x0e);
    drawString(100,140," >",0x0f);

    // Draw bottom bar
    drawRect(-1,1040,1921,1080,0xFF,1);

    // Draw some interactive elements for mouse testing
    drawRect(300,300,400,350,0x0c,0);  // Red box
    drawString(310,320,"Click me!",0x0f);

    drawRect(500,300,600,350,0x0a,0);  // Green box
    drawString(510,320,"Or me!",0x0f);

    drawRect(700,300,800,350,0x09,0);  // Blue box
    drawString(710,320,"Test box",0x0f);

    int last_mouse_x = -1, last_mouse_y = -1;
    unsigned char last_buttons = 0;

    uart_writeText("Mouse demo ready. Move your USB mouse!\n");

    while (1) {
        // Update mouse
        mouse_update();

        // Get current mouse state
        int mouse_x, mouse_y;
        mouse_get_position(&mouse_x, &mouse_y);
        unsigned char buttons = mouse_get_buttons();

        // Check if mouse position changed
        if (mouse_x != last_mouse_x || mouse_y != last_mouse_y) {
            // Clear previous status
            drawRect(100,160,500,170,0x00,1);

            // Show current position
            char pos_str[50];
            // Simple integer to string conversion
            char x_str[10], y_str[10];
            int temp_x = mouse_x, temp_y = mouse_y;
            int i = 0;

            // Convert X coordinate
            if (temp_x == 0) {
                x_str[0] = '0';
                x_str[1] = '\0';
            } else {
                while (temp_x > 0 && i < 9) {
                    x_str[i++] = '0' + (temp_x % 10);
                    temp_x /= 10;
                }
                x_str[i] = '\0';
                // Reverse string
                for (int j = 0; j < i/2; j++) {
                    char temp = x_str[j];
                    x_str[j] = x_str[i-1-j];
                    x_str[i-1-j] = temp;
                }
            }

            // Convert Y coordinate
            i = 0;
            if (temp_y == 0) {
                y_str[0] = '0';
                y_str[1] = '\0';
            } else {
                temp_y = mouse_y;
                while (temp_y > 0 && i < 9) {
                    y_str[i++] = '0' + (temp_y % 10);
                    temp_y /= 10;
                }
                y_str[i] = '\0';
                // Reverse string
                for (int j = 0; j < i/2; j++) {
                    char temp = y_str[j];
                    y_str[j] = y_str[i-1-j];
                    y_str[i-1-j] = temp;
                }
            }

            drawString(100,160,"Mouse: X=",0x0e);
            drawString(180,160,x_str,0x0f);
            drawString(220,160," Y=",0x0e);
            drawString(250,160,y_str,0x0f);

            last_mouse_x = mouse_x;
            last_mouse_y = mouse_y;
        }

        // Check for button presses
        if (buttons != last_buttons) {
            // Clear previous button status
            drawRect(100,180,400,190,0x00,1);

            if (mouse_is_button_pressed(MOUSE_LEFT_BUTTON)) {
                drawString(100,180,"Left Click!",0x0c);

                // Check if clicking on test boxes
                if (mouse_x >= 300 && mouse_x <= 400 && mouse_y >= 300 && mouse_y <= 350) {
                    drawRect(300,300,400,350,0x0f,1);  // Highlight red box
                    drawString(310,320,"Clicked!",0x00);
                } else if (mouse_x >= 500 && mouse_x <= 600 && mouse_y >= 300 && mouse_y <= 350) {
                    drawRect(500,300,600,350,0x0f,1);  // Highlight green box
                    drawString(510,320,"Got it!",0x00);
                } else if (mouse_x >= 700 && mouse_x <= 800 && mouse_y >= 300 && mouse_y <= 350) {
                    drawRect(700,300,800,350,0x0f,1);  // Highlight blue box
                    drawString(710,320,"Success!",0x00);
                }
            }

            if (mouse_is_button_pressed(MOUSE_RIGHT_BUTTON)) {
                drawString(200,180,"Right Click!",0x0a);
            }

            if (mouse_is_button_pressed(MOUSE_MIDDLE_BUTTON)) {
                drawString(320,180,"Middle Click!",0x09);
            }

            if (buttons == 0 && last_buttons != 0) {
                // Button released - restore original box colors
                drawRect(300,300,400,350,0x0c,0);
                drawString(310,320,"Click me!",0x0f);

                drawRect(500,300,600,350,0x0a,0);
                drawString(510,320,"Or me!",0x0f);

                drawRect(700,300,800,350,0x09,0);
                drawString(710,320,"Test box",0x0f);
            }

            last_buttons = buttons;
        }

        // Small delay to prevent excessive updates
        for (volatile int i = 0; i < 10000; i++);
    }
}
