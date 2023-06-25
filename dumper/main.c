#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <stdlib.h>
#include "serial_linux.h"

#define DEFAULT_TTY "/dev/ttyACM0"
#define MAX_TTY_LEN 15
#define DEFAULT_SAMPLING_RATE_MS 15
#define DEFAULT_SAMPLING_DURATION_S 5

int main() {
    char name[MAX_TTY_LEN + 1];
    printf("Enter device tty name (default: %s): ", DEFAULT_TTY);
    fgets(name, MAX_TTY_LEN, stdin);

    if (name[0] == '\n')
        strcpy(name, DEFAULT_TTY);
    else {
        // remove trailing newline
        name[strlen(name) - 1] = '\0';
    }

    printf("Using device %s\n", name);

    uint16_t sampling_rate_ms = DEFAULT_SAMPLING_RATE_MS;
    printf("Enter sampling rate in ms (default: %d): ", DEFAULT_SAMPLING_RATE_MS);
    char input[4];
    fgets(input, 4, stdin);

    if (input[0] != '\n') {
        sampling_rate_ms = strtoul(input, NULL, 10);
    }

    printf("Using sampling rate %d ms\n", sampling_rate_ms);


    uint16_t sampling_duration_s = DEFAULT_SAMPLING_DURATION_S;
    printf("Enter sampling duration in seconds (default: %d): ", DEFAULT_SAMPLING_DURATION_S);
    fgets(input, 4, stdin);

    if (input[0] != '\n') {
        sampling_duration_s = strtoul(input, NULL, 10);
    }

    printf("Using sampling duration %d s\n", sampling_duration_s);

    uint32_t estimated_samples = sampling_duration_s * 1000 / sampling_rate_ms;
    uint32_t estimated_file_size_kb = estimated_samples * 4 / 1024 + 1;

    printf("Estimated file size: %d kB.\n", estimated_file_size_kb);

    uint8_t pin = 0;
    printf("Enter pin number (default: 0): ");
    fgets(input, 2, stdin);
    if (input[0] != '\n') {
        pin = strtoul(input, NULL, 10);
    }
    printf("Using pin %d\n", pin);

    printf("Press enter to start sampling.\n");
    getchar();

    int fd = serial_open(name);
    if (fd < 0) {
        perror("serial_open");
        exit(1);
    }
    serial_set_interface_attribs(fd, 19200, 0);
    serial_set_blocking(fd, 1);

    // send pin
    ssize_t len = write(fd, &pin, 1);
    if (len < 0) {
        perror("write");
        exit(1);
    }

    FILE* file = fopen("data.csv", "w");
    if (file == NULL) {
        perror("fopen");
        exit(1);
    }

    uint32_t samples = 0;
    char cur_byte;
    while (samples < estimated_samples) {
        ssize_t bytes_read = read(fd, &cur_byte, 1);
        if (bytes_read < 0) {
            perror("read");
            exit(1);
        }
        if (cur_byte == '\n') {
            samples++;
        }
        fputc(cur_byte, file);
    }

    printf("Done.\n");

    return 0;
}
