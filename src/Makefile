/*
Using Makefile to build on windows however added Linux, can't see why it can't be built
on Linux. File paths and '/\' may need changing in output.c
*/

# Detect OS
ifeq ($(OS),Windows_NT)
    # Windows settings
    CC = gcc
    RM = del /Q
    EXE = .exe
    PREFIX = C:\Program Files\tcgen
    INSTALL = copy
    MKDIR = if not exist "$@" mkdir "$@"
    BIN = bin\\
    BUILD = build\\
else
    # Linux settings
    CC = gcc
    RM = rm -f
    EXE =
    PREFIX = /usr/local
    INSTALL = install -m 755
    MKDIR = mkdir -p $@
    BIN = bin/
    BUILD = build/
endif

# Common settings
CFLAGS = -Wall -O2 -I.
OBJECTS = $(BUILD)tcgen.o $(BUILD)patterns.o $(BUILD)output.o
TARGET = $(BIN)tcgen$(EXE)

# Default target
all: $(TARGET)

# Create bin directory and build target
$(TARGET): | bin $(OBJECTS)
	$(CC) -o $@ $(OBJECTS)

# Create directories if they don't exist
bin build:
	$(MKDIR)

# Object files
$(BUILD)tcgen.o: tcgen.c tcgen.h patterns.h output.h version.h | build
	$(CC) $(CFLAGS) -c tcgen.c -o $@

$(BUILD)patterns.o: patterns.c patterns.h | build
	$(CC) $(CFLAGS) -c patterns.c -o $@

$(BUILD)output.o: output.c output.h | build
	$(CC) $(CFLAGS) -c output.c -o $@

# Installation
install: $(TARGET)
ifeq ($(OS),Windows_NT)
	if not exist "$(PREFIX)" mkdir "$(PREFIX)"
	$(INSTALL) $(TARGET) "$(PREFIX)"
else
	install -d $(PREFIX)/bin
	$(INSTALL) $(TARGET) $(PREFIX)/bin/
endif

# Uninstallation
uninstall:
ifeq ($(OS),Windows_NT)
	if exist "$(PREFIX)\$(TARGET)" del "$(PREFIX)\$(TARGET)"
else
	$(RM) $(PREFIX)/bin/$(TARGET)
endif

# Clean build files
clean:
ifeq ($(OS),Windows_NT)
	if exist bin rmdir /s /q bin
	if exist build rmdir /s /q build
else
	rm -rf bin build
endif
	@echo "Build files cleaned"

# Help target
help:
	@echo "Available targets:"
	@echo "  all        - Build everything (default)"
	@echo "  clean      - Remove build files"
	@echo "  install    - Install to $(PREFIX)"
	@echo "  uninstall  - Remove from $(PREFIX)"
	@echo "  help       - Show this help"

.PHONY: all clean install uninstall help bin build
