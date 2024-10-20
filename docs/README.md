
# File hierarchy

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

## src/boot/:
Bootloader code and all assembly files related to the boot process, including GRUB-related configurations.

## src/kernel/:
The core C code of the kernel, including main functions and high-level logic. Primarily the code that manages drivers and memory management.

## src/drivers/:
Handles drivers for devices like serial ports, I/O ports, keyboard, mouse, etc. For example, you can create files like `serial.c` or `ports.c`, which handle functions related to serial ports and other ports. This structure keeps hardware-specific logic separate from the kernel's core logic.

## src/lib/:
General helper functions, such as mathematical operations, bit manipulation, or memory management utilities that are needed in various places.

## src/arch/x86_64/:
All architecture-specific code, including GDT, IDT, and paging (memory management). This allows easier future expansion of the project to support other architectures.

## docs/:
Documentation such as `README.md` or notes related to setting up or extending the code.

