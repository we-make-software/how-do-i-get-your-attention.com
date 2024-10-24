BRANCH = main

# Define the module name
MODULE_NAME = WeMakeSoftware

# Makefile for building, inserting, and removing kernel module

# Build the kernel module
obj-m += $(MODULE_NAME).o

all: build

build:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules

clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean

# Insert the kernel module
insert: build
	sudo insmod $(MODULE_NAME).ko

# Remove the kernel module
remove:
	sudo rmmod $(MODULE_NAME) 

# Rebuild and reinsert the module (for convenience)
reinsert: remove insert


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