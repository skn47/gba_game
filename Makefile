PROGNAME = Game

OFILES := $(shell find . -name '*.c' ! -name 'client.c' | sed 's/\.c/\.o/g')

.PHONY: all
all: CFLAGS += $(CDEBUG) -I../shared
all: LDFLAGS += $(LDDEBUG)
all: $(PROGNAME).gba
	@echo "[FINISH] Created $(PROGNAME).gba"

include /opt/cs2110-tools/GBAVariables.mak

INCLUDE_DIR = -Iinclude

LDFLAGS += --specs=nosys.specs

# Adjust default compiler warnings and errors
CFLAGS += -Wstrict-prototypes -Wold-style-definition -Werror=vla $(INCLUDE_DIR)

.PHONY: debug
debug: CFLAGS += $(CDEBUG) -I../shared
debug: LDFLAGS += $(LDDEBUG)
debug: $(PROGNAME).gba
	@echo "[FINISH] Created $(PROGNAME).gba"

$(PROGNAME).gba: clean $(PROGNAME).elf client
	@echo "[LINK] Linking objects together to create $(PROGNAME).gba"
	@$(OBJCOPY) -O binary $(PROGNAME).elf $(PROGNAME).gba

$(PROGNAME).elf: crt0.o $(GCCLIB)/crtbegin.o $(GCCLIB)/crtend.o $(GCCLIB)/crti.o $(GCCLIB)/crtn.o $(OFILES) libc_sbrk.o
	$(CC) -o $(PROGNAME).elf $^ $(LDFLAGS)

.PHONY: client
client: client.c
	@gcc -o client client.c

.PHONY: mgba
mgba: debug
	@./client ${PROGNAME}.gba
	@echo "[EXECUTE] Running emulator mGBA"

.PHONY: gdb
gdb: debug
	@./client ${PROGNAME}.gba gdb
	@sleep 1
	@echo "[EXECUTE] Attempting to connect to GDB server."
	@echo "          Make sure mGBA is already running on the host machine!"
	@gdb-multiarch -ex 'file $(PROGNAME).elf' -ex 'target remote host.docker.internal:2345'

.PHONY: clean
clean:
	@echo "[CLEAN] Removing all compiled files"
	rm -f *.o *.elf *.gba *.log */*.o *.sav */*/*.sav client

