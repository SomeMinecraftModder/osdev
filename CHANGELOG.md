# Changelog
All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## [Unreleased]
### Added
- Paging support.
- Heap support.
- Ordered array support (used for the heap).
- ClangFormat file for formatting.
- More standard string functions.
- CMake build system.
- Strict ANSI C support (not C90).
- Standard headers.
- (P)ATA hard disk driver.
- VFS (Virtual File System) support.

### Changed
- Corrected indent.
- Use Switch for keyboard (instead of "if else-if").
- ISR exception messages.
- ACPI FADT table.
- [printf](https://github.com/mpaland/printf) replaces old printf.
- [SSFN](https://gitlab.com/bztsrc/scalable-font2) replaces old text renderer.
- Use \<cpuid.h\> instead of inline assembly.

### Removed
- Make build system.
- FPU initialization code (VirtualBox doesn't like it).

## [0.03] - 2021-04-25
### Added
- GCC compatibility list.
- Standard memory and string functions.
- ctype.h standard library.
- printf-like PANIC message.
- PS/2 ports setup.
- Random numbers generator.
- FPU initialization code.
- Timer since boot.

### Changed
- Improve help command.
- Use scancode set 2 for keyboard.

### Removed
- Legacy shell code.
- \-m32 compiler flag.
- Some redundant code.

## [0.02] - 2021-04-06
### Added
- Build instructions.
- AZERTY keyboard layout support.
- ACPI initial support.
- Command history.
- Release type rules in Makefile.

### Changed
- Rename memory functions.
- Clean CPU detection code.
- Fix sleep() function.

## [0.01] - 2021-03-28
### Added
- Initial release.

[Unreleased]: https://github.com/Sebastian-byte/osdev/compare/v0.03...HEAD
[0.0.3]: https://github.com/Sebastian-byte/osdev/compare/v0.02...v0.03
[0.0.2]: https://github.com/Sebastian-byte/osdev/compare/v0.01...v0.02
[0.0.1]: https://github.com/Sebastian-byte/osdev/releases/tag/v0.01
