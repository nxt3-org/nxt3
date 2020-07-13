# variables
#TARGET  ?= arm-none-linux-gnueabi-
TARGET  ?= armv5-linux-musleabi-
CC       = $(TARGET)gcc
OBJCOPY  = $(TARGET)objcopy
STRIP    = $(TARGET)strip

# files
OBJDIR = .objs
SRCS     = $(wildcard *.c)
OBJS     = $(patsubst %.c,$(OBJDIR)/%.o,$(SRCS))
DEPFILES = $(patsubst %.c,$(OBJDIR)/%.d,$(SRCS))

# flags
override CFLAGS += -std=c99 -fno-strict-aliasing
override CFLAGS += -Wall -Wextra -Wno-unused-parameter -Wno-missing-field-initializers
override CXXFLAGS += -D_GNU_SOURCE=1
override LDFLAGS += -static -static-libgcc $(CFLAGS)
override OPTIMIZE = -O0 -ggdb3

# default target
all: nxt3.elf

# compile object files
$(OBJDIR)/%.o: %.c $(OBJDIR)/%.d
	@echo "  [CC] " $(@F)
	@mkdir -p $(@D)
	@$(CC) -MMD -MP $(OPTIMIZE) $(CXXFLAGS) $(CFLAGS) -c $< -o $@

$(DEPFILES):

-include $(DEPFILES)

nxt3.elf: $(OBJS)
	@echo "  [CCLD] " $@
	@mkdir -p $(@D)
	@$(CC) -lrt -lm $(OPTIMIZE) $(LDFLAGS) $^ -o $@
	@$(OBJCOPY) --only-keep-debug "$@" "$@.dbg"
	@$(STRIP)   --strip-debug --strip-unneeded "$@"
	@$(OBJCOPY) --add-gnu-debuglink="$@.dbg" "$@"

# remove built files
clean:
	$(RM) -r $(OBJDIR) nxt3.elf

.PHONY: all clean
