# Halo OS - Style Guide

* **Version:** 1.0
* **Enforcement:** Strict. Inconsistent code will be rejected during review.

---

## 1. General Rules (All Languages)
* **Indentation:** 4 Spaces. No Tabs. (Configure your editor to `Expand Tabs`).
* **Line Length:** Try to keep under 100 characters.
* **Braces:** K&R Style (Opening brace on the same line).
    ```c
    if (condition) {
        do_something();
    } else {
        do_other();
    }
    ```
* **Comments:** Use `//` for single lines and `/* ... */` for multi-line.
* **Documentation:** All public functions must have Doxygen-style headers.

---

## 2. The C Kernel Style (Strict C11)
The kernel code must be clean, flat, and obvious.

### Naming Conventions
* **Variables:** `snake_case`. Lowercase with underscores.
    * Good: `page_directory`, `current_process`
    * Bad: `PageDirectory`, `CurrentProcess`
* **Functions:** `snake_case`. Prefix with the subsystem name.
    * Good: `vmm_map_page()`, `kprint_string()`, `pci_scan_bus()`
    * Bad: `MapPage()`, `kPrintString`
* **Types/Structs:** `snake_case` with `_t` suffix.
    * Good: `process_t`, `header_t`
    * Bad: `Process`, `struct Header`
* **Macros:** `UPPER_SNAKE_CASE`.
    * Good: `PAGE_SIZE`, `MAX_THREADS`

### Pointer Syntax
* The `*` attaches to the **Type**, not the Variable.
    * Good: `void* ptr;`
    * Bad: `void *ptr;`

### Error Handling
* Functions usually return `int` (0 for Success, negative for Error Code).
* Do not use `goto` unless strictly necessary for cleanup at the end of a function.

---

## 3. The C++ Userland Style (Modern C++20)
The userland code embraces object-oriented design.

### Naming Conventions
* **Variables:** `camelCase`.
    * Good: `windowWidth`, `buttonState`
* **Classes:** `PascalCase`.
    * Good: `Window`, `TcpSocket`
* **Methods:** `camelCase`.
    * Good: `Window::draw()`, `String::length()`
* **Private Members:** `m_` prefix + `camelCase`.
    * Good: `m_width`, `m_buffer`
    * *Why?* Instantly tells you if a variable is local or part of the class state.

### File Structure
* **Header Files (`.h`):** Contains class declarations.
* **Source Files (`.cpp`):** Contains implementation.
* Use `#pragma once` at the top of all headers.

### Modern Features
* Use `auto` only when the type is obvious (e.g., iterators).
* Use `nullptr` instead of `NULL`.
* Use `constexpr` for constants instead of `#define`.

---

## 4. Doxygen Documentation Format
Every public function must follow this template:

```c
/**
 * @brief Short description of what the function does.
 *
 * Detailed explanation if necessary. Mention edge cases
 * or locking requirements.
 *
 * @param[in] size  How many bytes to allocate.
 * @param[in] flags Permission flags (READ/WRITE).
 * @return void* Pointer to memory or NULL on failure.
 */
void* kmalloc(size_t size, uint32_t flags);

---
