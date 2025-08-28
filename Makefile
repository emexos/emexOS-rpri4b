SRCDIR = src
BOOTDIR = boot
BUILDDIR = build
KERNELDIR = $(SRCDIR)/kernel
LIBDIR = $(SRCDIR)/lib
INCDIR = $(SRCDIR)/include
DRIVERDIR = $(SRCDIR)/drivers

BUILDBOTDIR = $(BUILDDIR)/boot
BUILDKERNELDIR = $(BUILDDIR)/kernel
BUILDLIBDIR = $(BUILDDIR)/lib
BUILDDRIVERDIR = $(BUILDDIR)/drivers

CFILES = $(wildcard $(KERNELDIR)/*.c) $(wildcard $(LIBDIR)/*.c) $(wildcard $(DRIVERDIR)/**/*.c)

KERNEL_OFILES = $(patsubst $(KERNELDIR)/%.c,$(BUILDKERNELDIR)/%.o,$(wildcard $(KERNELDIR)/*.c))
LIB_OFILES = $(patsubst $(LIBDIR)/%.c,$(BUILDLIBDIR)/%.o,$(wildcard $(LIBDIR)/*.c))
DRIVER_OFILES = $(patsubst $(DRIVERDIR)/%.c,$(BUILDDRIVERDIR)/%.o,$(wildcard $(DRIVERDIR)/**/*.c))
OFILES = $(KERNEL_OFILES) $(LIB_OFILES) $(DRIVER_OFILES)

LLVMPATH = /opt/homebrew/opt/llvm/bin
LLDPATH = /opt/homebrew/opt/lld/bin
CLANGFLAGS = -Wall -O2 -ffreestanding -nostdinc -nostdlib -mcpu=cortex-a72+nosimd -I$(INCDIR)

QEMU = qemu-system-aarch64
QEMU_FLAGS = -M raspi4b -cpu cortex-a72 -m 2G -serial stdio -kernel kernel8.img

all: clean kernel8.img run

$(BUILDDIR):
	@mkdir -p $@

$(BUILDBOTDIR)/boot.o: $(BOOTDIR)/boot.S | $(BUILDDIR)
	@mkdir -p $(BUILDBOTDIR)
	$(LLVMPATH)/clang --target=aarch64-elf $(CLANGFLAGS) -c $< -o $@

$(BUILDKERNELDIR)/%.o: $(KERNELDIR)/%.c | $(BUILDDIR)
	@mkdir -p $(dir $@)
	$(LLVMPATH)/clang --target=aarch64-elf $(CLANGFLAGS) -c $< -o $@

$(BUILDLIBDIR)/%.o: $(LIBDIR)/%.c | $(BUILDDIR)
	@mkdir -p $(dir $@)
	$(LLVMPATH)/clang --target=aarch64-elf $(CLANGFLAGS) -c $< -o $@

$(BUILDDRIVERDIR)/%.o: $(DRIVERDIR)/%.c | $(BUILDDIR)
	@mkdir -p $(dir $@)
	$(LLVMPATH)/clang --target=aarch64-elf $(CLANGFLAGS) -c $< -o $@

kernel8.img: $(BUILDBOTDIR)/boot.o $(OFILES)
	$(LLDPATH)/ld.lld -m aarch64elf -nostdlib $(BUILDBOTDIR)/boot.o $(OFILES) -T $(BOOTDIR)/link.ld -o kernel8.elf
	$(LLVMPATH)/llvm-objcopy -O binary kernel8.elf kernel8.img

run: kernel8.img
	$(QEMU) $(QEMU_FLAGS) -display cocoa,zoom-to-fit=on

clean:
	@rm -f kernel8.elf kernel8.img
	@rm -rf $(BUILDDIR)

debug-files:
	@echo "C files: $(CFILES)"
	@echo "Object files: $(OFILES)"

create-structure:
	@mkdir -p $(BOOTDIR) $(SRCDIR)/drivers $(INCDIR) $(KERNELDIR) $(LIBDIR) $(BUILDDIR)

.PHONY: all clean run debug-files create-structure
