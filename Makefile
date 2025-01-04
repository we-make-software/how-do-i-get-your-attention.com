BRANCH = main

# The main module name
obj-m += WeMakeSoftware.o
obj-m += Router.o

all: build

build:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules

clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean

insert: build
	sudo insmod WeMakeSoftware.ko
	sudo insmod Router.ko
remove:
	sudo rmmod Router || echo "Nothing to remove"
	sudo rmmod WeMakeSoftware || echo "Nothing to remove"

clear: 
	sudo dmesg -C

log:
	sudo dmesg -w | grep "WeMakeSoftware\|Router"

login:
	git remote set-url origin https://github.com/we-make-software/how-to-get-your-attention.com.git

push:
	git add .
	-git commit -m "Auto commit" || echo "Nothing to commit"
	git push origin $(BRANCH)

pull:
	git pull origin $(BRANCH)

reboot:
	sudo reboot
