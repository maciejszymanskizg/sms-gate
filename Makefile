CC ?= gcc
CXX ?= g++
LD ?= ld
STRIP ?= strip
ECHO = @

SRCS_CONFIGURATION=$(wildcard configuration/*.c)
SRCS_CORE=$(wildcard core/*.c)
SRCS_UTILS=$(wildcard utils/*.c)

SRCS := $(SRCS_CONFIGURATION) $(SRCS_CORE) $(SRCS_UTILS)

OBJS_CONFIGURATION=$(patsubst configuration/%.c,configuration/%.o,$(SRCS_CONFIGURATION))
OBJS_CORE=$(patsubst core/%.c,core/%.o,$(SRCS_CORE))
OBJS_UTILS=$(patsubst utils/%.c,utils/%.o,$(SRCS_UTILS))

OBJS := $(OBJS_CONFIGURATION) $(OBJS_CORE) $(OBJS_UTILS)

OUTPUT=sms-gate

all: $(OUTPUT)

%.o: %.c
	@echo "Compiling \033[1;32m$@\033[0m from \033[32m$<\033[0m ..."
	$(ECHO)$(CC) -c $< -o $@

$(OUTPUT): $(OBJS_CONFIGURATION) $(OBJS_CORE) $(OBJS_UTILS)
	@echo "Linking output \033[1;33m$@\033[0m ..."
	$(ECHO)$(CC) $^ -o $@

clean:
	@echo "Cleaning."
	@rm -rf $(OUTPUT) $(OBJS)
