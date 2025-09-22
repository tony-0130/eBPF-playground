# eBPF Playground

This project is my personal playground for learning and experimenting with eBPF (Extended Berkeley Packet Filter) programming. Currently focused on understanding the basics and getting familiar with the eBPF ecosystem.

## Current Status
🎯 **Phase 1: Learning Fundamentals** (In Progress)
- Environment setup and toolchain configuration
- Basic eBPF program compilation and loading
- Understanding BPF maps and program lifecycle

## Future Roadmap
- [ ] Advanced eBPF program types (networking, tracing, security)
- [ ] Performance monitoring and profiling tools
- [ ] Custom eBPF applications and use cases
- [ ] Integration with container orchestration platforms

## Environment Setup

> **Note**: This setup is specific to cross-compilation for ARM64 targets using NXP's IMX95 toolchain. Adapt paths and toolchain names for your specific hardware platform.

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
sudo apt-get install -y \
    clang \
    llvm \
    libbpf-dev \
```

## Target Machine

### Kernel config
Use the command `zgrep CONFIG_BPF /proc/config.gz` to verify these necessary configurations are enabled:

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

## Basic eBPF Workflow

### Building eBPF Programs
Example: Compiling `hello.bpf.c` to `hello.bpf.o`
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
- Pin the program to the file system (/sys/fs/bpf/hello)
- Allocate program ID
```shell
$ sudo bpftool prog load hello.bpf.o /sys/fs/bpf/hello
```

### Check registered programs
```shell
$ sudo bpftool prog list | grep hello
```

### Check eBPF Maps
```shell
$ sudo bpftool map list
```

## eBPF program Activation

### Activation 
Below command will
- attach the program to cgroup hook
- activate program (start listening)
- Set the trigger condition (bind4 event)
```shell
$ sudo bpftool cgroup attach /sys/fs/cgroup/ bind4 pinned /sys/fs/bpf/hello
```

## Test with bind4 syscall

### Run python to call bind
```python
python3 -c "
import socket
s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
s.bind(('127.0.0.1', 8888))
s.listen(1)
print('Server listening on port 8888')
input('Press Enter to exit...')
s.close()
"
```

### Dump Maps to check call
```shell
$ sudo bpftool map dump name counter_map
```
```shell
root@imx95aom5521a2:~# sudo bpftool map dump name counter_map
[
    {
        "key": 0,
        "value": 2
    }
]

```

---

## Next Steps
- [ ] Explore different eBPF program types (XDP, TC, kprobes, uprobes)
- [ ] Learn about BPF maps usage patterns
- [ ] Implement custom tracing applications
- [ ] Study eBPF security implications and best practices

## Resources
- [eBPF Official Documentation](https://ebpf.io/)
- [Linux Kernel BPF Documentation](https://docs.kernel.org/bpf/)
- [BPF Compiler Collection (BCC)](https://github.com/iovisor/bcc)
