#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "gui_main.h"
#include "cli_main.h"
#include "ng_main.h"

void show_usage(void) {
    fprintf(stdout, "Usage:\
                        \n\tOppai_benchmark command\n\
                    \nCOMMANDS:\
                        \n\t-c   run on cli\
                        \n\t-g   run on gui\
                        \n\t-ng  run without graphic output\
                        \n\t-h  Shows a list of commands or help for one command\n"\
            );
}

int main(int argc, char *argv[])
{
    fprintf(stdout, "Oppai_benchmark - A benchmark tool.\n\n");
    if (argc < 2) {
        fprintf(stderr, "Wrong option!\n");
        show_usage();
        exit(EXIT_FAILURE);
    }
    else if (strcmp(argv[1], "-g") == 0) {
        gui_main();
    }
    else if (strcmp(argv[1], "-c") == 0) {
        cli_main();
    }
    else if (strcmp(argv[1], "-h") == 0) {
        show_usage();
        exit(EXIT_SUCCESS);
    }
    else if (strcmp(argv[1], "-ng") == 0) {
        ng_main();
    }
    else {
        fprintf(stderr, "Wrong option!\n");
        show_usage();
        exit(EXIT_FAILURE);
    }

    return 0;
}
