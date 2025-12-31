# Halo OS - Development Tools

* **Host System:** Debian 13 (Trixie) or compatible Linux.
* **Target Architecture:** x86_64 (64-bit).

## 1. Required Packages
Install the following dependencies to build the toolchain and the OS image:

```bash
sudo apt update
sudo apt install build-essential bison flex libgmp3-dev \
    libmpc-dev libmpfr-dev texinfo nasm qemu-system-x86 \
    xorriso grub-pc-bin grub-common mtools git gdb

```

## 2. The Cross-Compiler (Toolchain)

**Do not** use the system `gcc`. You must build a specific cross-compiler.

* **Target:** `x86_64-elf`
* **Install Prefix:** `$HOME/osdev/cross`
* **Binutils Version:** 2.43
* **GCC Version:** 14.2.0

### setup Environment

Add this to your `.bashrc` or `.zshrc`:

```bash
export PATH="$HOME/osdev/cross/bin:$PATH"

```

## 3. Build Commands

The `Makefile` manages the entire lifecycle:

* **`make`**: Compiles the kernel and links the binary (`build/kernel.bin`).
* **`make iso`**: Generates the bootable image (`distro/halo-os.iso`) using `grub-mkrescue`.
* **`make run`**: Launches the ISO in QEMU with serial logging enabled.
* **`make clean`**: Removes all compiled object files and binaries.

## 4. Debugging

We use QEMU's GDB stub to debug the kernel while it runs.

1. **Launch QEMU in Debug Mode:**
```bash
make debug
# Or manually: qemu-system-x86_64 -s -S -cdrom distro/halo-os.iso

```


2. **Connect GDB:**
```bash
gdb build/kernel.bin
(gdb) target remote localhost:1234
(gdb) continue

```
---