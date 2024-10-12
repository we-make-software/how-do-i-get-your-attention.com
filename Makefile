REPO_NAME = how-to-get-your-attention.com
BRANCH = main
COMMIT_MESSAGE = "Auto commit"
obj-m += MemoryManager.o
obj-m += Memory.o
obj-m += wms.o 
all:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules

clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean

insert:
	sudo rmmod wms || true
	sudo rmmod Memory || true
	sudo rmmod MemoryManager || true
	sudo insmod MemoryManager.ko
	sudo insmod Memory.ko
	sudo insmod wms.ko

remove:
	sudo rmmod wms
	sudo rmmod Memory
	sudo rmmod MemoryManager
log:
	sudo dmesg -w

push:
	git add .
	git commit -m $(COMMIT_MESSAGE)
	git push origin $(BRANCH)
clear: 
	sudo dmesg -C


# Target to pull changes
pull:
	git pull origin $(BRANCH)

reload: remove clean all insert log

play:all insert log

stop: 
	sudo rmmod wms || true && make clean


# Command to generate .h and .h.md files
h:
	@if [ -z "$(filename)" ]; then \
		echo "Please provide a filename. Usage: make h filename=<filename>"; \
	else \
		echo "#pragma once\n#include \"Memory.h\"" > $(filename).h; \
		echo "# $(filename).h" > $(filename).h.md; \
		echo "\n# [$(filename) h Documentation](https://github.com/we-make-software/how-to-get-your-attention.com/blob/main/$(filename).h.md)" >> Readme.md; \
		echo "Header file $(filename).h, $(filename).h.md, and documentation link added."; \
	fi

# Command to generate .c and .c.md files
c:
	@if [ -z "$(filename)" ]; then \
		echo "Please provide a filename. Usage: make c filename=<filename>"; \
	else \
		echo "#include \"Memory.h\"\n\n// Implementation of functions for $(filename) module." > $(filename).c; \
		echo "# $(filename).c" > $(filename).c.md; \
		echo "\n# [$(filename) c Documentation](https://github.com/we-make-software/how-to-get-your-attention.com/blob/main/$(filename).c.md)" >> Readme.md; \
		echo "Source file $(filename).c, $(filename).c.md, and documentation link added."; \
	fi

# Command to generate both .h and .c files
ch:
	@if [ -z "$(filename)" ]; then \
		echo "Please provide a filename. Usage: make ch filename=<filename>"; \
	else \
		$(MAKE) h filename=$(filename); \
		$(MAKE) c filename=$(filename); \
		echo "Both header and source files for $(filename) have been created."; \
	fi



# List of modules
modules = Memory MemoryManager wms

# Unload modules in reverse order
unload:
	@echo "Unloading modules in reverse order..."
	@for module in $(shell echo $(modules) | tr ' ' '\n' | tac); do \
		echo "Removing $$module..."; \
		sudo rmmod $$module || true; \
	done
	@echo "Cleaning the build directory..."
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean

# Build and load modules in correct order
load:
	@echo "Building and loading modules..."
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules
	@for module in $(modules); do \
		echo "Inserting $$module.ko..."; \
		sudo insmod $$module.ko; \
	done
	@echo "Monitoring dmesg output..."
	sudo dmesg -w

# Combine both unload and load
reload: unload load
	@echo "Modules reloaded successfully."
