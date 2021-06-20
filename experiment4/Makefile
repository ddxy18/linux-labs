
obj-$(CONFIG_ROMFS_FS) += hack_romfs.o

hack_romfs-y := storage.o super.o

all:
	make -C /lib/modules/$(shell uname -r)/build M=$(shell pwd) modules

clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(shell pwd) clean
	rm test.img