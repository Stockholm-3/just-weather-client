/**
 * @file cli.h
 * @brief Command-line interface for the weather client
 *
 * This header provides the CLI functionality for the weather client
 * application. It supports both command-line mode (one-shot commands) and
 * interactive mode (REPL-style interface). The CLI handles command parsing,
 * validation, output formatting, and error reporting.
 *
 * Supported commands:
 * - current - Get weather by coordinates
 * - weather - Get weather by city name
 * - cities - Search for cities
 * - homepage - Get API homepage
 * - echo - Test server connectivity
 * - clear-cache - Clear response cache
 * - interactive - Enter interactive mode
 *
 * Exit codes:
 * - 0: Success
 * - 1: Invalid arguments
 * - 2: Network error
 * - 3: Server error
 */
#ifndef CLI_H
#define CLI_H

#include "api/weather_client.h"

/**
 * @brief Command result structure
 */
typedef struct {
    char* response;
    int status_code;
    int completed;
} CommandResult;

/**
 * @brief Prints usage information and available commands
 *
 * Displays a comprehensive help message showing all available commands,
 * their syntax, and usage examples. Called when the program is run without
 * arguments or with invalid arguments.
 *
 * @param prog_name The program name (typically argv[0]) to use in usage
 * examples
 *
 * @note This function writes to stdout and does not return any error status.
 *
 * @par Example:
 * @code
 * if (argc < 2) {
 *     cli_print_usage(argv[0]);
 *     return 1;
 * }
 * @endcode
 *
 * @par Output example:
 * @code
 * Just Weather Client
 *
 * Usage:
 *   ./just-weather-client current <lat> <lon>
 *   ./just-weather-client weather <city> [country] [region]
 *   ./just-weather-client cities <query>
 *   ...
 * @endcode
 */
void cli_print_usage(const char* prog_name);

/**
 * @brief Starts the interactive mode (REPL)
 *
 * Enters an interactive Read-Eval-Print Loop (REPL) where the user can
 * execute multiple commands without restarting the program. The interactive
 * mode provides a persistent session with command history and maintains
 * cache between commands for better performance.
 *
 * Interactive commands:
 * - All standard commands (current, weather, cities, homepage, echo,
 * clear-cache)
 * - help - Show available commands
 * - quit/exit/q - Exit interactive mode
 *
 * The function runs until the user types 'quit', 'exit', 'q', or EOF (Ctrl+D).
 *
 * @note This function is blocking and will not return until the user exits
 *       interactive mode or an unrecoverable error occurs.
 *
 * @note All JSON responses are formatted with 2-space indentation for
 *       better readability.
 *
 * @see cli_execute_command()
 *
 * @par Example:
 * @code
 * weather_client_init("http://localhost:10680/v1");
 * cli_interactive_mode();
 * weather_client_cleanup();
 * @endcode
 *
 * @par Interactive session example:
 * @code
 * Just Weather Interactive Client
 * Connected to: localhost:10680
 * Type 'help' for commands, 'quit' to exit
 *
 * just-weather> current 59.33 18.07
 * {
 *   "success": true,
 *   "data": { ... }
 * }
 * just-weather> quit
 * Goodbye!
 * @endcode
 */
int cli_interactive_mode(void);

/**
 * @brief Executes a command based on command-line arguments
 *
 * Parses and executes a single command from the command-line arguments.
 * This is the main entry point for non-interactive (one-shot) command
 * execution. The function validates arguments, calls the appropriate
 * weather client API function, and formats the output.
 *
 * Supported commands:
 * - current \<lat\> \<lon\> - Get weather by coordinates
 * - weather \<city\> [country] [region] - Get weather by city
 * - cities \<query\> - Search for cities
 * - homepage - Get API homepage information
 * - echo - Test server connectivity
 * - clear-cache - Clear the response cache
 * - interactive / -i - Returns -1 to signal interactive mode request
 *
 * @param client Pointer to the WeatherClient to use for the request.
 *               Must be a valid, initialized client.
 * @param argc Argument count (from main)
 * @param argv Argument vector (from main)
 *
 * @return Exit code indicating the result:
 * @retval 0 Command executed successfully
 * @retval 1 Invalid arguments or unknown command
 * @retval 2 Network error (connection failed, timeout, etc.)
 * @retval 3 Server error (API error, invalid response, etc.)
 * @retval -1 Special case: interactive mode requested (not an error)
 *
 * @note If argc < 2, the function returns EXIT_INVALID_ARGS (1) without
 *       printing usage. The caller should handle usage printing.
 *
 * @note JSON responses are formatted with 2-space indentation and written
 *       to stdout. Errors are written to stderr.
 *
 * @note The function handles JSON object cleanup (json_decref) and error
 *       string cleanup automatically.
 *
 * @see cli_print_usage(), cli_interactive_mode()
 *
 * @par Example:
 * @code
 * int main(int argc, char *argv[]) {
 *     if (argc < 2) {
 *         cli_print_usage(argv[0]);
 *         return 1;
 *     }
 *
 *     WeatherClient *client = weather_client_create("localhost", 10680);
 *     if (!client) {
 *         fprintf(stderr, "Failed to create client\n");
 *         return 2;
 *     }
 *
 *     int exit_code = cli_execute_command(client, argc, argv);
 *
 *     if (exit_code == -1) {
 *         // Interactive mode requested
 *         cli_interactive_mode(client);
 *         exit_code = 0;
 *     }
 *
 *     weather_client_destroy(client);
 *     return exit_code;
 * }
 * @endcode
 *
 * @par Command examples:
 * @code
 * // Get weather by coordinates
 * ./just-weather-client current 59.33 18.07
 *
 * // Get weather by city
 * ./just-weather-client weather Stockholm SE
 *
 * // Search cities
 * ./just-weather-client cities London
 *
 * // Test connectivity
 * ./just-weather-client echo
 * @endcode
 */
int cli_execute_command(int argc, char* argv[]);

#endif
