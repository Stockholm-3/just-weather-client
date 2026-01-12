# Just Weather Client

A weather client application written in C.

## Build

### Requirements
- `gcc`
- `make`
- The `jansson` library (used via a symlink at `proj/lib/jansson`)

### Prepare libraries

Before the first build, set up the symlink for the `jansson` library:

```bash
make install-lib
```

This command will create:
- Symlink: `lib/jansson -> ../../lib/jansson`
- Header file: `includes/jansson_config.h`

### Build the project

```bash
make              # Debug build
make release      # Release build
make clean        # Remove build artifacts
```

### Run

```bash
# Show help
./build/debug/just-weather-client

# Get weather by coordinates
./build/debug/just-weather-client current 59.33 18.07

# Get weather by city name
./build/debug/just-weather-client weather Stockholm SE

# Search cities
./build/debug/just-weather-client cities Stock

# Interactive mode
./build/debug/just-weather-client interactive
```

## Make targets

### Setup and build
```bash
make help         # Show all available commands
make install-lib  # Create symlink for jansson
make              # Debug build
make debug        # Debug build
make release      # Release build
make clean        # Clean build artifacts
make clean-all    # Clean build artifacts and cache
```

### Run and test
```bash
make run              # Run client (shows help)
make test             # Quick test using Stockholm coordinates
make test-city        # Test by city name
make test-search      # Test city search
make test-homepage    # Test homepage endpoint
make test-echo        # Test echo endpoint
make test-all         # Run all tests sequentially
make interactive      # Start interactive mode
make demo             # Interactive demo
```

### Cache
```bash
make show-cache       # Show cache contents
make clear-cache      # Clear cache via client
```

### Debugging
```bash
make asan         # Build with AddressSanitizer
make gdb          # Start client in GDB
make valgrind     # Run under Valgrind
```

### Code quality
```bash
make format       # Check formatting
make format-fix   # Fix formatting
make lint         # Run clang-tidy
make lint-fix     # Fix lint issues
make lint-ci      # CI lint (strict mode)
```

### Documentation
```bash
doxygen           # Generate API documentation
```

The generated documentation will be available in `documentation/html/index.html`.

## Quick reference

To view all available targets:
```bash
make help
```

## Usage examples

### Quick start
```bash
# Build and run tests
make
make test

# Run all tests
make test-all

# Or run individual tests
make test-city        # Weather for Stockholm
make test-search      # Search for cities matching "Stock"
```

### Interactive mode
```bash
make interactive
# Or directly
./build/debug/just-weather-client interactive
```

### Debugging with GDB
```bash
make gdb
# In GDB:
# (gdb) run current 59.33 18.07
# (gdb) break main
```

### Memory leak checks
```bash
make asan             # Build with AddressSanitizer
make run              # Run

# Or use Valgrind
make valgrind
```

## Project structure

```
just-weater-client/
├── src/                # Client source code
│   ├── api/            # API integration
│   ├── network/        # Network modules
│   ├── utils/          # Utilities and caching
│   └── cache/          # Cache files
├── lib/                # Libraries
│   └── jansson -> ../../lib/jansson  # Symlink to jansson
├── includes/           # Header files
│   └── jansson_config.h
├── build/              # Build artifacts
│   ├── debug/          # Debug build
│   └── release/        # Release build
└── Makefile            # Top-level Makefile
```

## Client features

- **current** - Get weather by coordinates
- **weather** - Get weather by city name
- **cities** - Search for cities
- **homepage** - Test the API homepage endpoint
- **echo** - Test the echo endpoint
- **interactive** - Interactive mode
- **clear-cache** - Clear client cache

For detailed information, run the client without arguments:
```bash
./build/debug/just-weather-client
```