# eBPF Playground

This project is my personal playground for learning and experimenting with eBPF (Extended Berkeley Packet Filter) programming. Currently focused on understanding the basics and getting familiar with the eBPF ecosystem.

## Exercises

This repository contains a series of eBPF learning exercises, each in its own folder:

- **01-syscall-counter/** - Count bind4 syscall invocations using cgroup hooks
- **02-process-monitor/** - Track process creation and lifecycle (planned)

Each exercise folder contains:
- eBPF program source code
- Makefile for building
- Exercise-specific README (e.g., `syscall-counter-README.md`) with detailed instructions and notes

## eBPF Concepts Covered

### Program Types
- **cgroup/bind4** - Attach to IPv4 bind operations in cgroups
- **tracepoints** - Hook into kernel tracepoint events (various exercises)
- **kprobes** - Dynamic kernel function tracing (future exercises)

### BPF Maps
- **BPF_MAP_TYPE_ARRAY** - Simple array-based storage for counters and data
- Map operations: lookup, update, and userspace access patterns

### Attachment Methods
- **bpftool** - Manual program loading and attachment
- **cgroup attachment** - Attaching programs to cgroup hooks for process monitoring

## Current Status
🎯 **Phase 1: Learning Fundamentals** (In Progress)
- ✅ Environment setup and toolchain configuration
- ✅ Basic eBPF program compilation and loading
- ✅ Understanding BPF maps and program lifecycle
- ✅ Working syscall counter with cgroup/bind4 hook

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
    libbpf-dev
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
```shell
make          # Build the eBPF program
make clean    # Clean build artifacts
```

### General eBPF Operations
Common `bpftool` operations for managing eBPF programs:
- Load and verify programs in kernel
- Attach programs to hooks/events
- Monitor program execution and maps
- Debug with detailed output (`-d` or `-dd` flags)

> **Note**: Each exercise folder contains specific commands and examples for that particular eBPF program.

---

## Next Steps
- [ ] Explore different eBPF program types (XDP, TC, kprobes, uprobes)
- [ ] Learn about BPF maps usage patterns
- [ ] Implement custom tracing applications
- [ ] Study eBPF security implications and best practices
- [ ] Add userspace programs for reading eBPF maps
- [ ] Implement more complex syscall monitoring

## Resources
- [eBPF Official Documentation](https://ebpf.io/)
- [Linux Kernel BPF Documentation](https://docs.kernel.org/bpf/)
- [BPF Compiler Collection (BCC)](https://github.com/iovisor/bcc)
