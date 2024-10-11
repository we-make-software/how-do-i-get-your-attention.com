# Makefile Targets Documentation

## make play
This function pushes the changes to the repository, builds the kernel modules (`Memory.o` and `wms.o`), inserts both kernel modules (`Memory.ko` and `wms.ko`), and displays the kernel logs.

## make stop
This function stops the project by removing the `wms.ko` kernel module (and ignoring errors if it's not loaded) and cleaning up the build environment.

## make reload
This function removes both kernel modules (`wms.ko` and `Memory.ko`), cleans the build, rebuilds the modules, inserts them again, and displays the logs to ensure a fresh start.

## make insert
This function inserts both `Memory.ko` and `wms.ko` kernel modules into the system.

## make remove
This function removes the `wms.ko` kernel module first, followed by the `Memory.ko` module.

## make clean
This function cleans up the module build directory to ensure a fresh compilation.

## make all
This function builds both kernel modules (`Memory.o` and `wms.o`) by calling the kernel’s build system.

## make log
This function displays the kernel log (`dmesg -w`) in real-time, allowing you to monitor logs live.

## make push
This function adds all changes, commits them with the message "Auto commit," and pushes them to the `main` branch of the repository.

## make pull
This function pulls the latest changes from the `main` branch of the repository.

## make clear
This function clears the kernel logs (`dmesg -C`) to ensure a clean slate before running the module.

## make h
This function generates a header file (`filename.h`) with the standard structure (`#pragma once`, `#include "Memory.h"`) and an empty Markdown file (`filename.h.md`) with a documentation link appended to `Readme.md`.

## make c
This function generates a C source file (`filename.c`) and a corresponding empty Markdown file (`filename.c.md`), also appending the appropriate documentation link to `Readme.md`.

## make ch
This function combines the operations of `make h` and `make c`, generating both header and source files along with their Markdown files, and updating `Readme.md` with the relevant documentation links.
