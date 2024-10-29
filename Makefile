BRANCH = main

# Define the module name
MODULE_NAME = WeMakeSoftware

obj-m += $(MODULE_NAME).o

all: build

build:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules

clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean

insert: build
	sudo insmod $(MODULE_NAME).ko

remove:
	sudo rmmod $(MODULE_NAME) 

clear: 
	sudo dmesg -C

log:
	sudo dmesg -w
	
deploy:insert log	

reset: remove clear deploy

login:
	git remote set-url origin https://github.com/we-make-software/how-to-get-your-attention.com.git

push:
	git add .
	-git commit -m "Auto commit" || echo "Nothing to commit"
	git push origin $(BRANCH)


pull:
	git pull origin $(BRANCH)