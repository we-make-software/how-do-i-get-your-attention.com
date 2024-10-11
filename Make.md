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
This function displays the kernel log (`dmesg`), which is useful for checking messages related to the module.

## make push
This function adds all changes, commits them with the message "Auto commit," and pushes them to the `main` branch of the repository.

## make pull
This function pulls the latest changes from the `main` branch of the repository.

## make clear
This function clears the kernel logs (`dmesg`) to ensure a clean slate before running the module.
