#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifndef __ANDROID__
// Non-Android code
#include "gui_main.h"
#endif
#include "cli_main.h"
#include "ng_main.h"

void show_usage(void) {
  fprintf(stdout, "Usage:\
                        \n\tOppai_benchmark command\n\
                    \nCOMMANDS:\
                        \n\t-c   run on cli\
                       \n\t-g   run on gui\
                       \n\t-s   run on single thread\
                        \n\t-ng  run without graphic output\
                        \n\t-h  Shows a list of commands or help for one command\n");
}

int main(int argc, char *argv[]) {
  fprintf(stdout, "Oppai_benchmark - A benchmark tool.\n\n");
  if (argc < 2) {
    fprintf(stderr, "Wrong option!\n");
    show_usage();
    exit(EXIT_FAILURE);
  } else if (strcmp(argv[1], "-g") == 0) {
#ifndef __ANDROID__
    // Non-Android code
    if (argc > 2 && strcmp(argv[2], "-s") == 0) {
      gui_main(0);
    } else {
      gui_main(1);
    }
#else
    fprintf(stderr, "NO GUI support on Android!\n");
#endif
  } else if (strcmp(argv[1], "-c") == 0) {
    if (argc > 2 && strcmp(argv[2], "-s") == 0) {
      cli_main(0);
    } else {

      cli_main(1);
    }
  } else if (strcmp(argv[1], "-h") == 0) {
    show_usage();
    exit(EXIT_SUCCESS);
  } else if (strcmp(argv[1], "-ng") == 0) {
    if (argc > 2 && strcmp(argv[2], "-s") == 0) {
      ng_main(0);
    } else {

      ng_main(1);
    }
  } else if (strcmp(argv[1], "-s") == 0) {
   if (argc > 2) {
	if (strcmp(argv[2], "-ng") == 0) {
		ng_main(0);
	} else if (strcmp(argv[2], "-g") == 0) {
#ifndef __ANDROID__
		gui_main(0);
#else
    fprintf(stderr, "NO GUI support on Android!\n");
#endif
	} else if (strcmp(argv[2], "-c") == 0) {
		cli_main(0);
	};
    } else {
    	fprintf(stderr, "Wrong option!\n");
    }

  } else {
    fprintf(stderr, "Wrong option!\n");
    show_usage();
    exit(EXIT_FAILURE);
  }

  return 0;
}
