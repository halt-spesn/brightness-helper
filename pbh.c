#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <linux/input.h>

#define INPUT_DEVICE "/dev/input/event0"
#define BRIGHTNESS_PATH "/sys/class/backlight/panel0-backlight/brightness"
#define BRIGHTNESS_COMMAND "echo 2047 > " BRIGHTNESS_PATH

int main() {
    struct input_event ev;
    int input_fd, counter = 0;

    // Open the input device file
    input_fd = open(INPUT_DEVICE, O_RDONLY);
    if (input_fd == -1) {
        perror("Error opening input device");
        return EXIT_FAILURE;
    }

    while (1) {
        // Read input events
        if (read(input_fd, &ev, sizeof(struct input_event)) != sizeof(struct input_event)) {
            perror("Error reading input event");
            break;
        }

        // Check for keypress event
        if (ev.type == EV_KEY && ev.value == 1) {
            counter++;

            // Execute the command on every 2nd keypress
            if (counter % 2 == 0) {
                if (system(BRIGHTNESS_COMMAND) == -1) {
                    perror("Error executing brightness command");
                    break;
                }
                printf("Brightness changed!\n");
            }
        }
    }

    // Close the input device file
    close(input_fd);
    return EXIT_SUCCESS;
}
