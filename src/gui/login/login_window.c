    drawRect(0, 0, 1920, 1080, 0x88, 1); // z.B. dunkles Blau/Grau

    // Titel
    drawStringSized(400, 300, "emexos login screen", 0x09, 64);

    // Login Box (rounded rect, etwas heller)
    drawRoundedRect(710, 500, 1210, 600, 50, 0x77, 1, 0xFF, 3);

    // Placeholder: "Press Enter to continue..."
    drawStringSized(850, 560, "Press Enter to continue...", 0x00, 32);

    int last_mouse_x = -1, last_mouse_y = -1;
    unsigned char last_buttons = 0;

    // Warte auf Eingabe
    while (1) {
        mouse_update();

        // Get current mouse state
        int mouse_x, mouse_y;
        mouse_get_position(&mouse_x, &mouse_y);
        unsigned char buttons = mouse_get_buttons();

        // Check if mouse position changed
        if (mouse_x != last_mouse_x || mouse_y != last_mouse_y) {

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

            //drawString(180,160,x_str,0x0f);
            //drawString(250,160,y_str,0x0f);

            last_mouse_x = mouse_x;
            last_mouse_y = mouse_y;
        }
    }


this is the login logik from the kernel we can soon remove it from the kernel because we have it here
