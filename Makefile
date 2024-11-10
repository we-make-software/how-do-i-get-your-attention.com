BRANCH = main

MODULE_NAME1 = WeMakeSoftware
MODULE_NAME2 = IEE802_3Software
USER_SERVICE_PATH = /etc/systemd/system/$(MODULE_NAME1).service
EXECUTABLE_PATH = /root/how-to-get-your-attention.com/User

obj-m += $(MODULE_NAME1).o
obj-m += $(MODULE_NAME2).o

all: build

build:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules

clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean

insert: build
	sudo insmod $(MODULE_NAME2).ko
	sudo insmod $(MODULE_NAME1).ko

remove:
	sudo rmmod $(MODULE_NAME1)
	sudo rmmod $(MODULE_NAME2)

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