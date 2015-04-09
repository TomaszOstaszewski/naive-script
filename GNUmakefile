.DEFAULT_GOAL:=all
.SECONDEXPANSION:

CPPFLAGS	=-I~/include -MP -MMD -MF $(@D)/$(*).d -MT '$(@D)/$(*).d $(@D)/$(*).o $(@D)/$(*).S $(@D)/$(*).C'
CFLAGS		:=-Wall -Wextra -Werror -O0 -ggdb
LDFLAGS		:=-lutil -levent -Wl,-rpath=$(shell readlink -f ~/lib) -L ~/lib

BUILD_ROOT:=$(shell $(CC) -dumpmachine)/

SOURCES:=pseudoshell.c
OBJECTS:=$(addprefix $(BUILD_ROOT),$(SOURCES:%.c=%.o))

.PHONY: all
all: $(BUILD_ROOT)pseudoshell pseudoshell.tags

.PHONY: app
app: $(BUILD_ROOT)pseudoshell

.PHONY: dox
dox: pseudoshell.tags

$(BUILD_ROOT)pseudoshell: $(OBJECTS)
	$(CC) -o $(@) $(^) $(CFLAGS) $(LDFLAGS)

pseudoshell.tags: pseudoshell.doxygen
	doxygen $(<)

%/.:
	mkdir -pv $(@)

.PRECIOUS: %/.

$(BUILD_ROOT)%.o: %.c | $$(@D)/.
	$(CC) -o $(@) -c $(<) $(CFLAGS) $(CPPFLAGS)

$(BUILD_ROOT)%.C: %.c
	$(CC) -E -o $(@) $(CPPFLAGS) $(<)

$(BUILD_ROOT)%.S: %.c
	$(CC) -S -o $(@) $(CPPFLAGS) $(<)

-include (

.PHONY: clean
clean:
	$(RM) -r $(BUILD_ROOT)
