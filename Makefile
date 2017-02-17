# C Project Makefile

#========= Debug mode setup ========#
# debug.h macros.
DEBUG := -DDEBUG -DVERBOSE -UTRACE
NDEBUG := -UDEBUG -DVERBOSE -UTRACE

#===== Compiler / linker setup =====#
# gcc with MinGW setup.
CC := gcc
CFLAGS := -g -O3 -Wall -Wpedantic -Wextra -std=gnu99
DFLAGS := -MP -MMD
LFLAGS := -s -lm
INCLUDE := 
LIBRARY := 
IMPORTANT :=

#======== Source code setup ========#
# Directory for all project files and
# the main.c file.
SRC_DIR := ./src
INCLUDE += -I$(SRC_DIR)

# Source files
CFILES := $(wildcard $(SRC_DIR)/*.c)
HFILES := $(wildcard $(SRC_DIR)/*.h)
IMPORTANT += $(SRC_DIR)

# Important files
MAKEFILE := Makefile
IMPORTANT += $(MAKEFILE) README.md

#========== Allegro Setup ==========#
ALLEGRO_DIR := C:/lib/allegro/allegro
INCLUDE += -I$(ALLEGRO_DIR)/include
LIBRARY += -L$(ALLEGRO_DIR)/lib
LFLAGS += -lallegro
LFLAGS += -lallegro_audio
LFLAGS += -lallegro_font
LFLAGS += -lallegro_image
LFLAGS += -lallegro_primitives
LFLAGS += -lallegro_ttf

#=========== Build setup ===========#
# Directory for built files.
BUILD_DIR := build
OFILES := $(CFILES:$(SRC_DIR)/%.c=$(BUILD_DIR)/%.o)
DFILES := $(OFILES:%.o=%.d)

#============ Main files ===========#
# Standalone text executable sources
# to link with the rest of the code.
MAIN_DIR := main
MCFILES := $(wildcard $(MAIN_DIR)/*.c)
MOFILES := $(MCFILES:$(MAIN_DIR)/%.c=$(BUILD_DIR)/$(MAIN_DIR)/%.o)
MDFILES := $(MOFILES:%.o=%.d)
IMPORTANT += $(MCFILES)

ALL_EXECUTABLES := $(MCFILES:$(MAIN_DIR)/%.c=%.exe)
TESTS := $(filter test_%.exe,$(ALL_EXECUTABLES))
EXECUTABLES := $(filter-out test_%.exe,$(ALL_EXECUTABLES))

#========== Documentation ==========#
# Doxygen documentation setup
DOC_DIR := docs
DOXYFILE := Doxyfile
DOXFILES := $(wildcard doxygen/*)
IMPORTANT += $(DOXYFILE) $(DOXFILES)

#============== Rules ==============#
# Default: just make executables
.PHONY: default
default: $(BUILD_DIR) $(EXECUTABLES)

# Make just the tests
.PHONY: tests
tests: $(BUILD_DIR) $(TESTS)

# Default - make the executable
.PHONY: all
all: default tests

# Put all the .o files in the build directory
$(BUILD_DIR):
	-mkdir $@
	-mkdir $@/$(MAIN_DIR)

# Compile the source files
.SECONDARY: $(DFILES)
.SECONDARY: $(OFILES)
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c $(MAKEFILE)
	$(CC) $(CFLAGS) $(DFLAGS) $(DEBUG) $(INCLUDE) -c $< -o $@
	
# Compile the main method executables
.SECONDARY: $(MDFILES)
.SECONDARY: $(MOFILES)
$(BUILD_DIR)/$(MAIN_DIR)/%.o: $(MAIN_DIR)/%.c $(MAKEFILE)
	$(CC) $(CFLAGS) $(DFLAGS) $(DEBUG) $(INCLUDE) -c $< -o $@

# Automatic dependency files
-include $(DFILES)
-include $(MDFILES)

# Documentation
.PHONY: documentation
documentation: $(DOC_DIR)
$(DOC_DIR): $(DOXFILES) $(CFILES) $(HFILES) $(MCFILES)
	doxygen Doxyfile

# Make executable for each driver
%.exe: $(BUILD_DIR)/$(MAIN_DIR)/%.o $(OFILES)
	$(CC) -o $@ $^ $(LIBRARY) $(LFLAGS)

#============== Clean ==============#
# Clean up build files and executable
.PHONY: clean
clean:
	-rm -rf $(BUILD_DIR) $(EXECUTABLES) $(TESTS)

#===================================#
