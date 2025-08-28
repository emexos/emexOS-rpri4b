# Verzeichnisse definieren
SRCDIR = src
BOOTDIR = boot
KERNELDIR = $(SRCDIR)/kernel
LIBDIR = $(SRCDIR)/lib
INCDIR = $(SRCDIR)/include

# Source-Dateien aus verschiedenen Verzeichnissen
CFILES = $(wildcard $(KERNELDIR)/*.c) $(wildcard $(LIBDIR)/*.c)
OFILES = $(CFILES:.c=.o)

# Tools
LLVMPATH = /opt/homebrew/opt/llvm/bin
LLDPATH = /opt/homebrew/opt/lld/bin
CLANGFLAGS = -Wall -O2 -ffreestanding -nostdinc -nostdlib -mcpu=cortex-a72+nosimd -I$(INCDIR)

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

$(BOOTDIR)/boot.o: $(BOOTDIR)/boot.S
	$(LLVMPATH)/clang --target=aarch64-elf $(CLANGFLAGS) -c $< -o $@

$(KERNELDIR)/%.o: $(KERNELDIR)/%.c
	$(LLVMPATH)/clang --target=aarch64-elf $(CLANGFLAGS) -c $< -o $@

$(LIBDIR)/%.o: $(LIBDIR)/%.c
	$(LLVMPATH)/clang --target=aarch64-elf $(CLANGFLAGS) -c $< -o $@

kernel8.img: $(BOOTDIR)/boot.o $(OFILES)
	$(LLDPATH)/ld.lld -m aarch64elf -nostdlib $(BOOTDIR)/boot.o $(OFILES) -T $(BOOTDIR)/link.ld -o kernel8.elf
	$(LLVMPATH)/llvm-objcopy -O binary kernel8.elf kernel8.img


run: kernel8.img
	@echo "Starte Raspberry Pi 4 OS in QEMU..."
	@echo "Zum Beenden: Ctrl+A, dann X drücken"
	@if $(QEMU) $(QEMU_FLAGS) -display sdl,window-close=off,gl=off,zoom-to-fit=on 2>/dev/null; then \
		echo "SDL Display verwendet (kleine Fenstergröße)"; \
	elif $(QEMU) $(QEMU_FLAGS) -display cocoa,zoom-to-fit=on 2>/dev/null; then \
		echo "Cocoa Display verwendet (macOS, kleine Fenstergröße)"; \
	else \
		echo "Fallback auf VNC - Verbinde mit VNC-Client auf localhost:5900"; \
		$(QEMU) $(QEMU_FLAGS) -display vnc=:0; \
	fi

clean:
	@echo "Bereinige Build-Dateien..."
	@rm -f kernel8.elf kernel8.img
	@find . -name "*.o" -type f -delete
	@echo "Bereinigung abgeschlossen."

debug-files:
	@echo "Boot-Verzeichnis: $(BOOTDIR)"
	@echo "Source-Verzeichnis: $(SRCDIR)"
	@echo "Include-Verzeichnis: $(INCDIR)"
	@echo "Gefundene C-Dateien:"
	@echo "$(CFILES)"
	@echo "Generierte Object-Dateien:"
	@echo "$(OFILES)"

create-structure:
	@echo "Erstelle Verzeichnisstruktur..."
	@mkdir -p $(BOOTDIR)
	@mkdir -p $(SRCDIR)/drivers
	@mkdir -p $(INCDIR)
	@mkdir -p $(KERNELDIR)
	@mkdir -p $(LIBDIR)
	@echo "Verzeichnisstruktur erstellt."

.PHONY: all clean run debug-files create-structure
