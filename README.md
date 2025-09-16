# EBPF TEST
This project is my personal study record of the eBPF program

## 1. Environment preparement

## Host Machine

- Prepare kernel-headers and download bpf libraries

### Generate kernel-headers

1. Download your kernel source and kernel toolchain

2. Use toolchain to set up environment

```shell
$ sudo ./fsl-imx-xwayland-glibc-x86_64-meta-toolchain-armv8a-imx95aom5521a2-toolchain-6.12-walnascar.sh

$ source /opt/fsl-imx-xwayland/6.12-walnascar/environment-setup-armv8a-poky-linux
```

3. Build kernel-headers

```shell
$ make headers_install ARCH=arm64 INSTALL_HDR_PATH=~/kernel-headers
```

### Package requirement

```shell
sudo apt-get update
sudo apt-get intall -y \
    clang \
    llvm \
    libbpf-dev \
```

## Target Machine

### Kernel config
Can using the command `zgrep CONFIG_BPF /proc/config.gz` to check we have enable these neccessary configurations.

```conf
CONFIG_BPF=y
CONFIG_BPF_SYSCALL=y
CONFIG_BPF_JIT=y
CONFIG_HAVE_EBPF_JIT=y
CONFIG_BPF_EVENTS=y
```

### Install libbpf.a (optional)
```
$ dpkg -i libbpf-staticdev_1.5.0-r0_arm64.deb
```

## How to build eBPF program (ex: hello.bpf.o)
```shell
make

make clean
```

## eBPF program Registration & Verification

### Registration & Verification
if need detail info, can use `-d` or `-dd` option
Below command will
- Load program to the kernel memory
- Verify through the BPF validator
- Create BPF Maps
- Pin the program toe the file system (/sys/fs/bpf/hello)
- Allocate program ID
```shell
$ sudo bpftool prog load hello.bpf.o /sys/fs/bpf/hello
```

### Check registerd programs
```shell
$ sudo bpftool prog list | grep hello
```

### Check eBPF Maps
```shell
$ sudo bpftool map list
```

## eBPF program Activation

### Active 
Below command will
- attach the program to cgroup hook
- active program (start listening)
- Set the trigger condition (bind4 event)
```shell
$ sudo bpftool cgroup attach /sys/fs/cgroup/ bind4 pinned /sys/fs/bpf/hello
```
