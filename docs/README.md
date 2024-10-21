# PotatOS
A small little 64bit hobby kernel. Nothing special.

## Roadmap

```
✔️ = Done
🛠️ = In Progress
❌ = Not Done
```

### 1. Bootloader
- [✔️] GRUB installed and configured
- [✔️] Initial assembly code complete
- [✔️] Transition to protected mode

### 2. Kernel Core Architecture
- [✔️] Kernel main code initialized
- [✔️] GDT and IDT set up
- [✔️] Basic interrupt handling complete
- [🛠️] Kernel panic handling and debugging

### 3. Memory Management
- [✔️] Physical memory management (PMM)
- [✔️] Virtual memory management (VMM)
- [🛠️] Heap allocator development

### 4. Drivers
- [✔️] Serial port driver
- [✔️] Keyboard driver
- [🛠️] Framebuffer support for display
- [❌] Mouse driver

### 5. Process Management
- [🛠️] Multitasking support
- [🛠️ ] Process scheduling
- [❌] Process creation and management

### 6. System Calls
- [❌] System call implementation (syscalls)
- [❌] Interrupt-driven I/O

## Additional Features
- [❌] Graphics support and BMP image handling
- [❌] Basic file system development
- [❌] Security features


## File hierarchy

```
project-root/
├── Makefile
├── build/                  # Compiled files (object files, binaries, etc.)
├── iso/                    # GRUB ISO files
│   └── grub/               # GRUB configuration
├── src/
│   ├── boot/               # Bootloader code, e.g., assembly files
│   ├── kernel/             # Main kernel code and higher-level logic
│   ├── mem/                # Memory management (bitmap, mmap, vmap, paging, etc.)
│   ├── drivers/            # Drivers (e.g., ports, serial port, etc.)
│   ├── lib/                # Shared libraries and helper functions
│   ├── utils/              # General utility functions
│   └── arch/               # Architecture and platform-specific code
│       └── x86_64/         # Architecture-specific for x86_64
│           ├── gdt/        # Code related to GDT
│           ├── idt/        # Code related to IDT and interrupts
│           └── paging/     # Code related to paging
└── docs/                   # Documentation, such as installation and usage guides
```

### src/boot/:
Bootloader code and all assembly files related to the boot process, including GRUB-related configurations.

### src/kernel/:
The core C code of the kernel, including main functions and high-level logic. Primarily the code that manages drivers and memory management.

### src/drivers/:
Handles drivers for devices like serial ports, I/O ports, keyboard, mouse, etc. For example, you can create files like `serial.c` or `ports.c`, which handle functions related to serial ports and other ports. This structure keeps hardware-specific logic separate from the kernel's core logic.

### src/lib/:
General helper functions, such as mathematical operations, bit manipulation, or memory management utilities that are needed in various places.

### src/arch/x86_64/:
All architecture-specific code, including GDT, IDT, and paging (memory management). This allows easier future expansion of the project to support other architectures.

### docs/:
Documentation such as `README.md` or notes related to setting up or extending the code.


# Compiling and running
## Cross compiler
Before building the os, we need crosscompiler. `prepare/prepare.sh` download binutils and gcc source codes and build-essential, bison, flex, libgmp-3, libmpfr, texinfo, wget, grub-common, grub-pc-bin and xorriso and then the build cross compiler for us.

Then you can use `source prepare/start.sh` or `export PATH="`pwd`/prepare/opt/cross/bin/:$PATH"` to use new cross compiler! \o/

Finally, just run `make` or tweak it.