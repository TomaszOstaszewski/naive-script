.DEFAULT_GOAL:=all
.SECONDEXPANSION:

CPPFLAGS	=-MP -MMD -MF $(@D)/$(*).d -MT '$(@D)/$(*).d $(@D)/$(*).o $(@D)/$(*).S $(@D)/$(*).i'
CPPFLAGS	+=-DNDEBUG
CFLAGS		:=-Wall -Wextra -O0 -ggdb
LDFLAGS		:=-lutil -levent -L/usr/local/lib

CPPFLAGS	+=-I/usr/local/include
BUILD_ROOT:=$(shell $(CC) -dumpmachine)/

SOURCES:=pseudoshell.c yandu_log.c nt-vis.c nt-bitmap.c
OBJECTS:=$(addprefix $(BUILD_ROOT),$(SOURCES:%.c=%.o))
DEPENDS:=$(OBJECTS:%.o=%.d)

-include $(DEPENDS)

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


.PHONY: clean
clean:
	$(RM) -r $(BUILD_ROOT)
