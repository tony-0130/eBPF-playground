CLANG ?= clang
LLVM_OBJCOPY ?= llvm-objcopy
LLVM_STRIP ?= llvm-strip

KERNEL_SRC=/home/adv/kernel-headers/include
BPF_SRC=/usr/include/bpf

.PHONY: all clean

all:
	$(CLANG) -g -O2 -c -I${KERNEL_SRC} -I${BPF_SRC} hello.bpf.c -o hello.bpf.o 

clean:
	rm -f *.o
