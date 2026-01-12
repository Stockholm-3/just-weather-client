# Just Weather Client - Команди

## Швидка довідка

### Початок роботи
```bash
make install-lib    # Налаштувати бібліотеки (перший раз)
make                # Скомпілювати
make test           # Перевірити роботу
make help           # Показати всі команди
```

### Компіляція
```bash
make                # Debug збірка
make debug          # Debug збірка
# Just Weather Client - Commands

## Quick reference

### Getting started
```bash
make install-lib    # Set up libraries (first time)
make                # Build
make test           # Run a quick test
make help           # Show all make targets
```

### Build
```bash
make                # Debug build
make debug          # Debug build
make release        # Release build (optimized)
make clean          # Remove build artifacts
make clean-all      # Remove build artifacts + cache
```

### Testing
```bash
make test           # Quick test (Stockholm)
make test-city      # Test by city name
make test-search    # Test city search
make test-all       # Run all tests sequentially
make demo           # Interactive demo
```

### Run
```bash
make run            # Show help
make interactive    # Start interactive mode

# Or run directly:
./build/debug/just-weather-client current 59.33 18.07
./build/debug/just-weather-client weather Stockholm SE
./build/debug/just-weather-client cities Stock
./build/debug/just-weather-client interactive
```

### Cache
```bash
make show-cache     # List cache contents
make clear-cache    # Clear cache
```

### Debugging
```bash
make asan           # AddressSanitizer (find memory issues)
make gdb            # Run in GDB
make valgrind       # Run under Valgrind
```

### Code quality
```bash
make format         # Check formatting
make format-fix     # Fix formatting
make lint           # Run clang-tidy
make lint-fix       # Fix lint issues
make lint-ci        # CI lint (strict)
```

## Examples

### Get weather for Kyiv
```bash
./build/debug/just-weather-client weather Kyiv UA
```

### Find cities matching "Stockholm"
```bash
./build/debug/just-weather-client cities Stockholm
```

### Run all tests
```bash
make test-all
```

### Memory checks
```bash
make asan
make test
```

### Debugging with GDB
```bash
make gdb
# In GDB:
(gdb) run current 59.33 18.07
(gdb) break main
```

## Build layout

```
build/
├── debug/              # Debug build (with debug info)
│   └── just-weather-client
└── release/            # Release build (optimized)
    └── just-weather-client
```

## Useful combinations

```bash
# Full rebuild
make clean && make

# Test after build
make && make test

# Release build and test
make release && ./build/release/just-weather-client weather Stockholm SE

# Inspect cache after tests
make test && make show-cache
```
