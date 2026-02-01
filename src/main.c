#include "api/weather_client.h"
#include "cli.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define EXIT_INVALID_ARGS 1
#define EXIT_NETWORK_ERROR 2

int main(int argc, char* argv[]) {
    if (argc < 2) {
        cli_print_usage(argv[0]);
        return EXIT_INVALID_ARGS;
    }

    // Initialize async weather client
    if (weather_client_init("http://localhost:10680/v1") != 0) {
        fprintf(stderr, "Failed to initialize weather client\n");
        return EXIT_NETWORK_ERROR;
    }

    const char* command   = argv[1];
    int         exit_code = 0;

    if (strcmp(command, "interactive") == 0 || strcmp(command, "-i") == 0) {
        exit_code = cli_interactive_mode();
    } else {
        exit_code = cli_execute_command(argc, argv);
        if (exit_code == EXIT_INVALID_ARGS) {
            cli_print_usage(argv[0]);
        }
    }

    weather_client_cleanup();
    return exit_code;
}
