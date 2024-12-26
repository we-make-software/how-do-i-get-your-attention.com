BRANCH = main

ModuleWeMakeSoftware = WeMakeSoftware
obj-m += $(ModuleWeMakeSoftware).o
all: build

build:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules

clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean

insert: build
	sudo insmod $(ModuleWeMakeSoftware).ko

remove:
	sudo rmmod $(ModuleWeMakeSoftware)  || echo "Nothing to remove"
clear: 
	sudo dmesg -C

log:
	sudo dmesg -w | grep "WMS"


deploy: insert log

stop: remove clean clear

reset: remove clear deploy

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
