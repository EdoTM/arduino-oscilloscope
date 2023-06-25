#include <stdio.h>
#include <string.h>

#define DEFAULT_TTY "/dev/ttyACM0"
#define MAX_TTY_LEN 15

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



    return 0;
}
