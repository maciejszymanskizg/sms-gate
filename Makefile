CC ?= gcc
CXX ?= g++
LD ?= ld
STRIP ?= strip
ECHO = @

OUTPUT=sms-gate

CSRCS_CONFIGURATION=$(wildcard configuration/*.c)
CSRCS_CORE=$(wildcard core/*.c)
CSRCS_UTILS=$(wildcard utils/*.c)
CPPSRCS_CONFIGURATION=$(wildcard configuration/*.cpp)
CPPSRCS_CORE=$(wildcard core/*.cpp)
CPPSRCS_UTILS=$(wildcard utils/*.cpp)

CSRCS := $(CSRCS_CONFIGURATION) $(CSRCS_CORE) $(CSRCS_UTILS)
CPPSRCS += $(CPPSRCS_CONFIGURATION) $(CPPSRCS_CORE) $(CPPSRCS_UTILS)

OBJS := $(patsubst %.c,%.o,$(CSRCS)) $(patsubst %.cpp,%.o,$(CPPSRCS))

CFLAGS=-I./configuration -I./core -I./utils -Wall
LDFLAGS=-pthread -Wl,-Map,$(OUTPUT).map

all: $(OUTPUT)

%.o: %.c
	@echo "Compiling \033[1;32m$@\033[0m from \033[32m$<\033[0m ..."
	$(ECHO)$(CC) $(CFLAGS) -c $< -o $@

%.o: %.cpp
	@echo "Compiling \033[1;32m$@\033[0m from \033[32m$<\033[0m ..."
	$(ECHO)$(CXX) $(CFLAGS) -c $< -o $@

$(OUTPUT): $(OBJS)
	@echo "Linking output \033[1;33m$@\033[0m from \033[34m$^\033[0m ..."
	$(ECHO)$(CXX) $(LDFLAGS) $^ -o $@
	@echo "Striping output \033[1;33m$@\033[0m from \033[34m$^\033[0m ..."
	$(ECHO)$(STRIP) $@

clean:
	@echo "Cleaning."
	@rm -rf $(OUTPUT) $(OUTPUT).map $(OBJS)
