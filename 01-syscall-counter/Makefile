CLANG ?= clang

KERNEL_SRC=${HOME}/kernel-headers/include
BPF_SRC=/usr/include/bpf

.PHONY: all clean

all:
	$(CLANG) -g -O2 -c -I${KERNEL_SRC} -I${BPF_SRC} syscall-counter.bpf.c -o syscall-counter.bpf.o 

clean:
	rm -f *.o
