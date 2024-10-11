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