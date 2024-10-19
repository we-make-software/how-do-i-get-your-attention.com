REPO_NAME = how-to-get-your-attention.com
BRANCH = main
COMMIT_MESSAGE = "Auto commit"
SERVICE_PATH = /etc/systemd/system/reload-modules.service
SCRIPT_PATH = /path/to/reload_modules.sh

# Check if WeMakeSoftware.modules exists. If it does, read from it; if not, default to WeMakeSoftware.
modules := $(if $(wildcard WeMakeSoftware.modules),$(shell cat WeMakeSoftware.modules),WeMakeSoftware)

obj-m += $(modules:=.o)

all: build check_service

build:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules

clean: remove_modules
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean

check_service:
	@if [ ! -f $(SERVICE_PATH) ]; then \
		echo "Service file not found. Creating reload-modules.service..."; \
		$(MAKE) insert_modules; \
		$(MAKE) create_service; \
		echo "Enabling and starting the reload-modules service..."; \
		sudo systemctl daemon-reload; \
		sudo systemctl enable reload-modules.service; \
		sudo systemctl start reload-modules.service; \
	else \
		echo "Service file already exists. Inserting modules..."; \
		$(MAKE) insert_modules; \
	fi

insert_modules:
	@echo "Inserting kernel modules..."
	@for module in $(modules); do \
		sudo insmod $$module.ko || true; \
	done

remove_modules:
	@echo "Removing kernel modules..."
	@for module in $(modules); do \
		sudo rmmod $$module || true; \
	done

create_service:
	@echo "Creating the service file..."
	@sudo bash -c 'echo "[Unit]" > $(SERVICE_PATH)'
	@sudo bash -c 'echo "Description=Reload kernel modules" >> $(SERVICE_PATH)'
	@sudo bash -c 'echo "DefaultDependencies=no" >> $(SERVICE_PATH)'
	@sudo bash -c 'echo "Before=shutdown.target reboot.target halt.target" >> $(SERVICE_PATH)'
	@sudo bash -c 'echo "After=multi-user.target" >> $(SERVICE_PATH)'
	@sudo bash -c 'echo "" >> $(SERVICE_PATH)'
	@sudo bash -c 'echo "[Service]" >> $(SERVICE_PATH)'
	@sudo bash -c 'echo "Type=oneshot" >> $(SERVICE_PATH)'
	@sudo bash -c 'echo "RemainAfterExit=true" >> $(SERVICE_PATH)'
	@sudo bash -c 'echo "ExecStart=$(SCRIPT_PATH) start" >> $(SERVICE_PATH)'
	@sudo bash -c 'echo "ExecStop=$(SCRIPT_PATH) stop" >> $(SERVICE_PATH)'
	@sudo bash -c 'echo "" >> $(SERVICE_PATH)'
	@sudo bash -c 'echo "[Install]" >> $(SERVICE_PATH)'
	@sudo bash -c 'echo "WantedBy=multi-user.target" >> $(SERVICE_PATH)'

log:
	sudo dmesg -w

login:
	git remote set-url origin https://github.com/we-make-software/how-to-get-your-attention.com.git

push:
	git add .
	-git commit -m "Auto commit" || echo "Nothing to commit"
	git push origin $(BRANCH)

clear: 
	sudo dmesg -C

pull:
	git pull origin $(BRANCH)

# Command to generate .h and .h.md files
h:
	@if [ -z "$(filename)" ]; then \
		echo "Please provide a filename. Usage: make h filename=<filename>"; \
	else \
		HEADER_FILE=$$(head -n 1 WeMakeSoftware.modules); \
		echo "#pragma once\n#include \"$$HEADER_FILE.h\"" > $(filename).h; \
		echo "# $(filename).h based on $$HEADER_FILE.h" > $(filename).h.md; \
		echo "\n# [$(filename) h Documentation](https://github.com/we-make-software/how-to-get-your-attention.com/blob/main/$(filename).h.md)" >> Readme.md; \
		echo "Header file $(filename).h, $(filename).h.md, and documentation link added."; \
	fi

c:
	@if [ -z "$(filename)" ]; then \
		echo "Please provide a filename. Usage: make c filename=<filename>"; \
	else \
		HEADER_FILE=$$(head -n 1 WeMakeSoftware.modules); \
		echo "#include \"$$HEADER_FILE.h\"\n\n// Implementation of functions for $(filename) module." > $(filename).c; \
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

unload:
	@echo "Unloading modules in reverse order..."
	@for module in $(shell echo $(modules) | tr ' ' '\n' | tac); do \
		echo "Removing $$module..."; \
		sudo rmmod $$module || true; \
	done
	@echo "Cleaning the build directory..."
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean

load:
	@echo "Building and loading modules..."
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules
	@for module in $(modules); do \
		echo "Inserting $$module.ko..."; \
		sudo insmod $$module.ko; \
	done
	@echo "Monitoring dmesg output..."
	sudo dmesg -w
