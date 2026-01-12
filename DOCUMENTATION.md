# API Documentation

This project includes comprehensive API documentation generated with Doxygen.

## Generating Documentation

To generate the HTML documentation:

```bash
doxygen
```

The generated documentation will be available in `docs/html/index.html`.

## Documented Modules

### Network Layer
- **[client_tcp.h](src/network/client_tcp.h)** - Low-level TCP client
  - Socket management with timeout support
  - IPv4/IPv6 support
  - Non-blocking connections
  - Reliable data transmission

- **[http_client.h](src/network/http_client.h)** - HTTP/1.1 client
  - GET request support
  - URL parsing
  - Chunked transfer encoding
  - Response parsing

### API Layer
- **[weather_client.h](src/api/weather_client.h)** - Weather API client
  - Current weather by coordinates
  - Weather by city name
  - City search
  - Automatic response caching
  - JSON response handling

### User Interface
- **[cli.h](src/cli.h)** - Command-line interface
  - Command-line mode
  - Interactive mode (REPL)
  - JSON output formatting
  - Error handling

## Documentation Features

- Complete function documentation with parameters and return values
- Usage examples for all public functions
- Cross-references between related functions
- Call graphs showing function relationships
- Detailed error handling documentation
- Memory management guidelines

## Viewing Documentation

Open `docs/html/index.html` in your web browser to browse the documentation.

The documentation includes:
- Module overview pages
- File documentation
- Function reference
- Data structure descriptions
- Code examples
- Navigation index

## Documentation Style

All documentation follows Doxygen format with:
- `@brief` - Short description
- `@param` - Parameter descriptions
- `@return` / `@retval` - Return value documentation
- `@note` - Important notes and warnings
- `@see` - Cross-references
- `@par Example` - Code examples
- `@warning` - Critical warnings

## Cache TTL Values

Weather client uses different cache TTL values:
- Weather data: 5 minutes (300 seconds)
- City search: 1 hour (3600 seconds)
- Homepage: 24 hours (86400 seconds)
- Echo: Not cached

## Exit Codes

CLI exit codes:
- 0: Success
- 1: Invalid arguments
- 2: Network error
- 3: Server error
- -1: Interactive mode requested (internal use)