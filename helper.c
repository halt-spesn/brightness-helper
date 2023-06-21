#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <linux/input.h>
#include <sys/time.h>

#define POWER_BUTTON_EVENT "/dev/input/event0"
#define BACKLIGHT_PATH "/sys/class/backlight/panel0-backlight/brightness"
#define POWER_KEYCODE KEY_POWER
#define DELAY_SECONDS 1

int main() {
    int input_fd = open(POWER_BUTTON_EVENT, O_RDONLY);
    if (input_fd == -1) {
        perror("Failed to open power button event");
        return 1;
    }

    struct input_event ev;
    int count = 0;
    struct timeval last_press_time;
    struct timeval current_time;

    while (1) {
        if (read(input_fd, &ev, sizeof(ev)) == -1) {
            perror("Failed to read power button event");
            close(input_fd);
            return 1;
        }

        if (ev.type == EV_KEY && ev.value == 1) {  // Key press event
            if (ev.code == POWER_KEYCODE) {
                count++;
                if (count % 2 == 0) {  // Every 2nd press
                    gettimeofday(&current_time, NULL);
                    double elapsed_time = (current_time.tv_sec - last_press_time.tv_sec) +
                                          (current_time.tv_usec - last_press_time.tv_usec) / 1000000.0;

                    if (elapsed_time >= DELAY_SECONDS) {
                        // Perform the desired action after the delay
                        sleep(DELAY_SECONDS);
                        system("echo 2047 > " BACKLIGHT_PATH);
                        printf("Brightness set to 2047\n");
                    }
                }

                gettimeofday(&last_press_time, NULL);
            }
        }
    }

    close(input_fd);
    return 0;
}
