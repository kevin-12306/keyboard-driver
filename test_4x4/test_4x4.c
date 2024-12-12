#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>

#define DEVICE_NAME "/dev/keypad_4x4"

const char *key_map[16] = {
    "1", "2", "3", "A",
    "4", "5", "6", "B",
    "7", "8", "9", "C",
    "*", "0", "#", "D"
};

int main(void) {
    int fd;
    unsigned char key_value;
    unsigned char prev_value = 0xFF;

    fd = open(DEVICE_NAME, O_RDONLY);
    if (fd < 0) {
        perror("Failed to open device file");
        return -1;
    }

    printf("4x4 Keypad Test Program\n");
    printf("Press keys on the 4x4 keypad...\n");

    while (1) {
        if (read(fd, &key_value, 1) > 0) {
            if (key_value != 0xFF && key_value != prev_value) {
                printf("Key pressed: %s\n", key_map[key_value]);
                prev_value = key_value;
            }
        }
        usleep(50000);
    }

    close(fd);
    return 0;
}
