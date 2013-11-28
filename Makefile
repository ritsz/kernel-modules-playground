# Makefile – makefile of our first driver
 
# if KERNELRELEASE is defined, we've been invoked from the
# kernel build system and can use its language.
ifneq (${KERNELRELEASE},)
	obj-m ?= thread_tree.o
	#thread_tree.o is default module
# Otherwise we were called directly from the command line.
# Invoke the kernel build system.

#Use make default obj-m=<module_name.o>
else
	KERNEL_SOURCE := /lib/modules/$(shell uname -r)/build
	KERNEL_SOURCE2 := /home/ritsz/linux/
	PWD := $(shell pwd)
default:
	${MAKE} -C ${KERNEL_SOURCE2} SUBDIRS=${PWD} modules
 
clean:
	${MAKE} -C ${KERNEL_SOURCE2} SUBDIRS=${PWD} clean
endif