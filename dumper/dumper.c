#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <stdlib.h>
#include <time.h>
#include "serial_linux.h"

#define DEFAULT_TTY "/dev/ttyACM0"
#define MAX_TTY_LEN 17
#define DEFAULT_SAMPLING_RATE_MS 15
#define DEFAULT_SAMPLING_DURATION_S 5

void flush_stdin(void) {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

void fgets_stdin_flush(char *buf, int len) {
    fgets(buf, len, stdin);
    size_t buf_len = strlen(buf);
    if (buf[buf_len - 1] != '\n') {
        flush_stdin();
    }
}

void get_device_name(char *name, int len) {
    printf("Enter device tty name (default: %s): ", DEFAULT_TTY);
    fgets_stdin_flush(name, len);

    if (name[0] == '\n')
        strcpy(name, DEFAULT_TTY);
    else {
        // remove trailing newline
        name[strlen(name) - 1] = '\0';
    }
}

uint16_t get_sampling_rate_ms() {
    uint16_t sampling_rate_ms = DEFAULT_SAMPLING_RATE_MS;
    printf("Enter sampling rate in ms (default: %d): ", DEFAULT_SAMPLING_RATE_MS);
    char input[4];
    fgets_stdin_flush(input, 4);
    if (input[0] != '\n')
        sampling_rate_ms = strtoul(input, NULL, 10);

    return sampling_rate_ms;
}

uint16_t get_sampling_duration_s() {
    uint16_t sampling_duration_s = DEFAULT_SAMPLING_DURATION_S;
    printf("Enter sampling duration in seconds (default: %d): ", DEFAULT_SAMPLING_DURATION_S);
    char input[4];
    fgets_stdin_flush(input, 4);
    if (input[0] != '\n')
        sampling_duration_s = strtoul(input, NULL, 10);

    return sampling_duration_s;
}

unsigned char get_pin() {
    unsigned char pin = '0';
    printf("Enter pin number (default: 0): ");
    char input[3];
    fgets_stdin_flush(input, 3);
    if (input[0] != '\n')
        pin = input[0];

    return pin;
}

int open_serial_port(char *serial_device_name) {
    int fd = serial_open(serial_device_name);
    if (fd < 0) {
        perror("serial_open");
        exit(1);
    }
    serial_set_interface_attribs(fd, 19200, 0);
    serial_set_blocking(fd, 1);

    return fd;
}

FILE *create_dump_file(void) {
    time_t rawtime;
    struct tm * timeinfo;
    char filename[80];
    time(&rawtime);
    timeinfo = localtime(&rawtime);
    strftime(filename, sizeof(filename), "%d-%m-%Y_%H-%M-%S.txt",timeinfo);

    FILE *file = fopen(filename, "w");
    if (file == NULL) {
        perror("fopen");
        exit(1);
    }

    char linkname[] = "latest.txt";
    unlink(linkname);
    symlink(filename, linkname);

    return file;
}

void wait_for_arduino_to_start(int serial_fd) {
    char buf[10];
    ssize_t bytes_read = 0;
    while (1) {
        ssize_t ret = read(serial_fd, buf + bytes_read, 1);
        if (ret < 0) {
            perror("read");
            exit(1);
        }
        bytes_read += ret;

        if (buf[bytes_read - 1] == '\n') {
            buf[bytes_read] = '\0';
            if (strncmp(buf, "start", 5) == 0) {
                break;
            }
            bytes_read = 0;
        } else if (bytes_read >= 10) {
            bytes_read = 0;
        }
    }
}

void send_pin(const int serial_fd, unsigned char pin) {
    unsigned char buf[] = {'a', pin, '\n'};
    ssize_t len = write(serial_fd, buf, 3);
    if (len < 0) {
        perror("write");
        exit(1);
    }
}

void send_sampling_rate(const int serial_fd, uint16_t sampling_rate_ms) {
    char buf[8];
    buf[0] = 'f';
    sprintf(buf + 1, "%d\n", sampling_rate_ms);

    ssize_t len = write(serial_fd, buf, strlen(buf));
    if (len < 0) {
        perror("write");
        exit(1);
    }
}

void wait_for_frequency_to_change(int serial_fd) {
    char c;
    while (1) {
        ssize_t bytes_read = read(serial_fd, &c, 1);
        if (bytes_read < 0) {
            perror("read");
            exit(1);
        }
        if (c == 'f') {
            break;
        }
    }
}

void sample(int serial_fd, FILE *file, uint32_t estimated_samples) {
    uint32_t samples = 0;
    char buffer[10];
    uint8_t buffer_idx = 0;
    while (samples < estimated_samples) {
        ssize_t bytes_read = read(serial_fd, buffer + buffer_idx, 1);
        if (bytes_read < 0) {
            perror("read");
            exit(1);
        }
        buffer_idx += bytes_read;
        if (buffer[buffer_idx - 1] == '\n') {
            samples++;
            buffer[buffer_idx] = '\0';
            buffer_idx = 0;
            uint16_t value = strtoul(buffer, NULL, 10);
            double voltage = value * 5.0 / 1023.0;
            fprintf(file, "%.4f\n", voltage);
        }
    }
}

void debug_print(const char *str) {
#ifdef DEBUG
    printf("%s\n", str);
#endif
}

int main() {
    char serial_device_name[MAX_TTY_LEN + 1];
    get_device_name(serial_device_name, MAX_TTY_LEN);
    printf("Using serial @ %s\n", serial_device_name);

    uint16_t sampling_rate_ms = get_sampling_rate_ms();
    printf("Using sampling rate %d ms\n", sampling_rate_ms);

    uint16_t sampling_duration_s = get_sampling_duration_s();
    printf("Using sampling duration %d s\n", sampling_duration_s);

    uint32_t estimated_samples = sampling_duration_s * 1000 / sampling_rate_ms;
    uint32_t estimated_file_size_kb = estimated_samples * 7 / 1024 + 1;

    printf("Estimated file size: %d kB.\n", estimated_file_size_kb);

    unsigned char pin = get_pin();
    printf("Using pin A%c\n", pin);

    printf("Press enter to start sampling or ctrl+c to exit.\n");
    getchar();

    int serial_fd = open_serial_port(serial_device_name);
    FILE *file = create_dump_file();

    printf("Preparing...\n");

    wait_for_arduino_to_start(serial_fd);
    debug_print("Arduino started.");

    send_pin(serial_fd, pin);
    debug_print("Pin sent.");

    send_sampling_rate(serial_fd, sampling_rate_ms);
    debug_print("Sampling rate sent.");

    wait_for_frequency_to_change(serial_fd);
    debug_print("Frequency changed.");


    printf("Sampling...\n");

    sample(serial_fd, file, estimated_samples);

    printf("Done.\n");

    debug_print("Closing file...");
    fclose(file);

    debug_print("Finished.");

    return 0;
}
