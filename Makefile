CFILES = $(wildcard *.c)
OFILES = $(CFILES:.c=.o)
LLVMPATH = /opt/homebrew/opt/llvm/bin
LLDPATH = /opt/homebrew/opt/lld/bin
CLANGFLAGS = -Wall -O2 -ffreestanding -nostdinc -nostdlib -mcpu=cortex-a72+nosimd

# QEMU Einstellungen für Raspberry Pi 4
QEMU = qemu-system-aarch64
QEMU_FLAGS = -M raspi4b \
             -cpu cortex-a72 \
             -m 2G \
             -serial stdio \
             -kernel kernel8.img

# Kleinere Fenstergröße - 800x600 statt Standard
QEMU_DISPLAY_FLAGS = -display sdl,window-close=off,gl=off -device usb-mouse -device usb-kbd

all: clean kernel8.img run

boot.o: boot.S
	$(LLVMPATH)/clang --target=aarch64-elf $(CLANGFLAGS) -c boot.S -o boot.o

%.o: %.c
	$(LLVMPATH)/clang --target=aarch64-elf $(CLANGFLAGS) -c $< -o $@

kernel8.img: boot.o $(OFILES)
	$(LLDPATH)/ld.lld -m aarch64elf -nostdlib boot.o $(OFILES) -T link.ld -o kernel8.elf
	$(LLVMPATH)/llvm-objcopy -O binary kernel8.elf kernel8.img

# QEMU starten (automatische Display-Erkennung mit kleinerem Fenster)
run: kernel8.img
	@echo "Starte Raspberry Pi 4 OS in QEMU..."
	@echo "Zum Beenden: Ctrl+A, dann X drücken"
	@if $(QEMU) $(QEMU_FLAGS) -display sdl,window-close=off,gl=off,zoom-to-fit=off 2>/dev/null; then \
		echo "SDL Display verwendet (kleine Fenstergröße)"; \
	elif $(QEMU) $(QEMU_FLAGS) -display cocoa,zoom-to-fit=off 2>/dev/null; then \
		echo "Cocoa Display verwendet (macOS, kleine Fenstergröße)"; \
	else \
		echo "Fallback auf VNC - Verbinde mit VNC-Client auf localhost:5900"; \
		$(QEMU) $(QEMU_FLAGS) -display vnc=:0; \
	fi

run-cocoa: kernel8.img
	@echo "Starte mit Cocoa Display (macOS, kleine Fenstergröße)..."
	$(QEMU) $(QEMU_FLAGS) -display cocoa,zoom-to-fit=off

run-small: kernel8.img
	@echo "Starte mit explizit kleiner Auflösung..."
	$(QEMU) $(QEMU_FLAGS) -display sdl,window-close=off,gl=off,zoom-to-fit=off

run-gui: kernel8.img
	@echo "Starte Raspberry Pi 4 OS in QEMU mit GUI (kleines Fenster)..."
	@$(QEMU) $(QEMU_FLAGS) -display sdl,window-close=off,gl=off,zoom-to-fit=off 2>/dev/null || \
	 $(QEMU) $(QEMU_FLAGS) -display cocoa,zoom-to-fit=off 2>/dev/null || \
	 $(QEMU) $(QEMU_FLAGS) -display vnc=:0

# Neues Target für wirklich minimale Fenstergröße
run-tiny: kernel8.img
	@echo "Starte mit minimaler Fenstergröße..."
	$(QEMU) $(QEMU_FLAGS) -display sdl,window-close=off,gl=off,zoom-to-fit=off,grab-on-hover=off

clean:
	/bin/rm kernel8.elf *.o *.img > /dev/null 2> /dev/null || true

.PHONY: all clean run run-headless run-sdl run-cocoa run-vnc run-gui run-small run-tiny debug install-qemu qemu-info
