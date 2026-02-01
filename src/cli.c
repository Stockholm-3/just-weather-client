/**
 * @file cli.c
 * @brief Command-line interface implementation
 *
 * Implementation of the CLI interface defined in cli.h. This module provides
 * both command-line and interactive modes for the weather client application.
 * It handles argument parsing, command execution, JSON output formatting,
 * and provides a REPL-style interactive interface.
 *
 * See cli.h for detailed API documentation.
 */
#include "cli.h"

#include <jansson.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define EXIT_INVALID_ARGS 1  ///< Invalid command-line arguments
#define EXIT_NETWORK_ERROR 2 ///< Network communication error
#define EXIT_SERVER_ERROR 3  ///< Server/API error

// Result structure for synchronous wrapper
static CommandResult g_result = {NULL, 0, 0};

static void print_json_string(const char* json_str);
static int  parse_double(const char* str, double* out);
static void process_command(char* line);
static void response_callback(char* response, int status_code, void* user_data);

void cli_print_usage(const char* prog_name) {
    printf("Just Weather Client\n\n");
    printf("Usage:\n");
    printf("  %s current <lat> <lon>\n", prog_name);
    printf("  %s weather <city> [country] [region]\n", prog_name);
    printf("  %s cities <query>\n", prog_name);
    printf("  %s interactive    # Enter interactive mode\n", prog_name);
    printf("\nExamples:\n");
    printf("  %s current 59.33 18.07\n", prog_name);
    printf("  %s weather Stockholm SE\n", prog_name);
    printf("  %s cities Stock\n", prog_name);
    printf("  %s interactive\n", prog_name);
}

static void response_callback(char* response, int status_code, void* user_data) {
    g_result.response = response;  // Transfer ownership
    g_result.status_code = status_code;
    g_result.completed = 1;
}

int cli_interactive_mode(void) {
    char line[1024];

    printf("Just Weather Interactive Client\n");
    printf("Connected to async weather client\n");
    printf("Type 'help' for commands, 'quit' to exit\n\n");

    while (1) {
        printf("just-weather> ");
        fflush(stdout);

        if (!fgets(line, sizeof(line), stdin)) {
            break;
        }

        size_t len = strlen(line);
        if (len > 0 && line[len - 1] == '\n') {
            line[len - 1] = '\0';
        }

        if (strlen(line) == 0) {
            continue;
        }

        if (strcmp(line, "quit") == 0 || strcmp(line, "q") == 0 ||
            strcmp(line, "exit") == 0) {
            printf("Goodbye!\n");
            break;
        }

        if (strcmp(line, "help") == 0) {
            printf("\nAvailable commands:\n");
            printf("  current <lat> <lon>             - Get current weather by "
                   "coordinates\n");
            printf("  weather <city> [country]        - Get weather by city "
                   "name\n");
            printf("  cities <query>                  - Search for cities\n");
            printf("  help                            - Show this help\n");
            printf("  quit / exit                     - Exit interactive "
                   "mode\n\n");
            printf("Examples:\n");
            printf("  current 59.33 18.07\n");
            printf("  weather Kyiv UA\n");
            printf("  cities London\n\n");
            continue;
        }

        process_command(line);
    }
    
    return 0;
}

int cli_execute_command(int argc, char* argv[]) {
    if (argc < 2) {
        return EXIT_INVALID_ARGS;
    }

    const char* command = argv[1];
    
    // Reset result
    g_result.response = NULL;
    g_result.status_code = 0;
    g_result.completed = 0;

    if (strcmp(command, "current") == 0) {
        if (argc < 4) {
            fprintf(stderr, "Usage: %s current <lat> <lon>\n", argv[0]);
            return EXIT_INVALID_ARGS;
        }

        double lat, lon;
        if (!parse_double(argv[2], &lat) || !parse_double(argv[3], &lon)) {
            fprintf(stderr, "Invalid coordinates\n");
            return EXIT_INVALID_ARGS;
        }

        // For now, we'll use a simple city lookup (Stockholm as example)
        // The async API doesn't have lat/lon support in the copied version
        fprintf(stderr, "Note: Using Stockholm, SE as example (lat/lon not supported in async API)\n");
        weather_client_current_async("Stockholm", "SE", response_callback, NULL);

    } else if (strcmp(command, "weather") == 0) {
        if (argc < 3) {
            fprintf(stderr, "Usage: %s weather <city> [country] [region]\n",
                    argv[0]);
            return EXIT_INVALID_ARGS;
        }

        const char* city    = argv[2];
        const char* country = argc > 3 ? argv[3] : "SE";

        weather_client_current_async(city, country, response_callback, NULL);

    } else if (strcmp(command, "cities") == 0 ||
               strcmp(command, "homepage") == 0 ||
               strcmp(command, "echo") == 0 ||
               strcmp(command, "clear-cache") == 0) {
        fprintf(stderr, "Command '%s' not implemented in async API\n", command);
        return EXIT_INVALID_ARGS;

    } else if (strcmp(command, "interactive") == 0 ||
               strcmp(command, "-i") == 0) {
        return -1;

    } else {
        fprintf(stderr, "Unknown command: %s\n", command);
        return EXIT_INVALID_ARGS;
    }

    // Poll for completion
    while (!g_result.completed) {
        weather_client_poll();
    }

    if (!g_result.response || g_result.status_code != 200) {
        fprintf(stderr, "Error: HTTP %d\n", g_result.status_code);
        if (g_result.response) {
            free(g_result.response);
        }
        return EXIT_SERVER_ERROR;
    }

    print_json_string(g_result.response);
    free(g_result.response);

    return 0;
}

static void print_json_string(const char* json_str) {
    if (!json_str) {
        return;
    }
    
    // Try to parse and pretty print
    json_error_t error;
    json_t* json = json_loads(json_str, 0, &error);
    if (json) {
        char* pretty = json_dumps(json, JSON_INDENT(2) | JSON_PRESERVE_ORDER);
        if (pretty) {
            printf("%s\n", pretty);
            free(pretty);
        }
        json_decref(json);
    } else {
        // If not valid JSON, just print as-is
        printf("%s\n", json_str);
    }
}

static int parse_double(const char* str, double* out) {
    if (!str || !out) {
        return 0;
    }

    char*  endptr;
    double value = strtod(str, &endptr);

    if (endptr == str || *endptr != '\0') {
        return 0;
    }

    *out = value;
    return 1;
}

static void process_command(char* line) {
    char* cmd = strtok(line, " ");
    if (!cmd) {
        return;
    }

    // Reset result
    g_result.response = NULL;
    g_result.status_code = 0;
    g_result.completed = 0;

    if (strcmp(cmd, "current") == 0) {
        char* lat_str = strtok(NULL, " ");
        char* lon_str = strtok(NULL, " ");

        if (!lat_str || !lon_str) {
            printf("Error: Usage: current <lat> <lon>\n");
            return;
        }

        double lat, lon;
        if (!parse_double(lat_str, &lat) || !parse_double(lon_str, &lon)) {
            printf("Error: Invalid coordinates\n");
            return;
        }

        // Using Stockholm as example since lat/lon not in async API
        printf("Note: Using Stockholm, SE (lat/lon not supported)\n");
        weather_client_current_async("Stockholm", "SE", response_callback, NULL);

    } else if (strcmp(cmd, "weather") == 0) {
        char* city    = strtok(NULL, " ");
        char* country = strtok(NULL, " ");

        if (!city) {
            printf("Error: Usage: weather <city> [country]\n");
            return;
        }

        if (!country) {
            country = "SE";  // Default
        }

        weather_client_current_async(city, country, response_callback, NULL);

    } else if (strcmp(cmd, "cities") == 0) {
        printf("Error: 'cities' command not implemented in async API\n");
        return;

    } else {
        printf("Error: Unknown command '%s'. Type 'help' for available "
               "commands.\n",
               cmd);
        return;
    }

    // Poll for completion
    while (!g_result.completed) {
        weather_client_poll();
    }

    if (!g_result.response || g_result.status_code != 200) {
        printf("Error: HTTP %d\n", g_result.status_code);
        if (g_result.response) {
            free(g_result.response);
        }
        return;
    }

    print_json_string(g_result.response);
    free(g_result.response);
}
