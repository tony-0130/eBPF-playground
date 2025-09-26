# 01-syscall-counter

## What it does
Counts how many times the `bind4` syscall is invoked by attaching to the cgroup/bind4 hook. Uses an eBPF map to store the counter value that can be read from userspace.

## Files
- `syscall-counter.bpf.c` - eBPF program that increments counter on each bind4 call
- `Makefile` - Build configuration for compiling the eBPF program

## How to build
```bash
make
```

## How to run
Currently only compiles the eBPF object file. To actually use it, you need:
1. Load up the eBPF program
```shell
sudo bpftool prog load syscall-counter.bpf.o /sys/fs/bpf/counter
```

2. Check the counter first
```shell
sudo bpftool map dump name counter_map
```

3. Proper cgroup setup for the bind4 hook
```shell
sudo bpftool cgroup attach /sys/fs/cgroup/ bind4 pinned /sys/fs/bpf/counter
```

4. Test by running a program that calls bind()
```shell
python3 -c "import socket; s=socket.socket(); s.bind(('127.0.0.1', 8888))"
```

4. Check the counter again
```shell
sudo bpftool map dump name counter_map
```

## Expected output
When properly loaded and attached, the program will count bind4 syscalls. Each time a process calls bind() on an IPv4 socket, the counter increments.

## How it works
- Uses `SEC("cgroup/bind4")` to attach to IPv4 bind operations
- Stores counter in `BPF_MAP_TYPE_ARRAY` map for userspace access
- Returns 1 to allow the bind operation to proceed

## Notes
- Works well as a basic eBPF syscall counting example
- Alternative approach to tracepoint-based syscall monitoring
- Requires userspace loader program to be fully functional