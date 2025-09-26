CLANG ?= clang
CC ?= gcc

KERNEL_SRC=${HOME}/kernel-headers/include
BPF_SRC=/usr/include/bpf

.PHONY: all clean

all: process_monitor.bpf.o process_monitor

# Build eBPF program
process_monitor.bpf.o: process_monitor.bpf.c
	$(CLANG) -g -O2 -c -I${KERNEL_SRC} -I${BPF_SRC} process_monitor.bpf.c -o process_monitor.bpf.o

# Build userspace program
process_monitor: process_monitor.c
	$(CC) -g -O2 -o process_monitor process_monitor.c -lbpf

clean:
	rm -f *.o process_monitor
