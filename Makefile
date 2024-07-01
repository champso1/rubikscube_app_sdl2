CC = gcc
CFLAGS = -Wall -Iinclude -MMD `pkg-config --cflags sdl2 SDL2_image SDL2_ttf sqlite3`
RM = rm -rf

LDFLAGS = -g
LDLIBS = `pkg-config --libs sdl2 SDL2_image SDL2_ttf sqlite3` -lm

SRCDIR = src
BUILDDIR = build
TARGET = bin/main

SRCEXT = c
SOURCES = $(shell find $(SRCDIR) -type f -name *.$(SRCEXT))
OBJS = $(patsubst $(SRCDIR)/%,$(BUILDDIR)/%,$(SOURCES:.$(SRCEXT)=.o))
DEPS = $(patsubst $(SRCDIR)/%,$(BUILDDIR)/%,$(SOURCES:.$(SRCEXT)=.d))


$(TARGET): $(OBJS)
	@echo "Linking ..."
	@echo "$(CC) $(LDFLAGS) $^ -o $(TARGET) $(LDLIBS)"; $(CC) $(LDFLAGS) $^ -o $(TARGET) $(LDLIBS)


-include $(DEPS)

$(BUILDDIR)/%.o: $(SRCDIR)/%.$(SRCEXT)
	@mkdir -p $(BUILDDIR)
	@echo "$(CC) $(CFLAGS) -c $< -o $@"; $(CC) $(CFLAGS) -MMD -c $< -o $@

clean:
	@echo "Cleaning ..."
	@echo "$(RM) $(BUILDDIR) $(TARGET)"; $(RM) $(BUILDDIR) $(TARGET)

.PHONY: clean
