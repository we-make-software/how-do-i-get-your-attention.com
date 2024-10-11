	REPO_NAME = how-to-get-your-attention.com
BRANCH = main
COMMIT_MESSAGE = "Auto commit"
obj-m += Memory.o

obj-m += wms.o 

all:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules

clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean

insert:
	sudo insmod wms.ko

remove:
	sudo rmmod wms

log: 
	dmesg

reload: remove clean all insert log

play: all insert log

stop: 
	sudo rmmod wms || true && make clean

clear: 
	sudo dmesg -C



# Target to push changes
push:
	git add .
	git commit -m $(COMMIT_MESSAGE)
	git push origin $(BRANCH)

# Target to pull changes
pull:
	git pull origin $(BRANCH)