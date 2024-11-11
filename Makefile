BRANCH = main

ModuleWeMakeSoftware = WeMakeSoftware
ModuleIEE802_3 = IEE802_3Software
ModuleRFC791 = RFC791Software
ModuleRFC826 = RFC826Software
ModuleRFC8200 = RFC8200Software

obj-m += $(ModuleWeMakeSoftware).o
obj-m += $(ModuleIEE802_3).o
obj-m += $(ModuleRFC791).o
obj-m += $(ModuleRFC826).o
obj-m += $(ModuleRFC8200).o

all: build

build:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules

clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean

insert: build
	sudo insmod $(ModuleRFC8200).ko
	sudo insmod $(ModuleRFC826).ko
	sudo insmod $(ModuleRFC791).ko
	sudo insmod $(ModuleIEE802_3).ko
	sudo insmod $(ModuleWeMakeSoftware).ko reboot=0

remove:
	sudo rmmod $(ModuleWeMakeSoftware)
	sudo rmmod $(ModuleIEE802_3)
	sudo rmmod $(ModuleRFC791)
	sudo rmmod $(ModuleRFC826)
	sudo rmmod $(ModuleRFC8200)

clear: 
	sudo dmesg -C

log:
	sudo dmesg -w 

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
