# 02-process-monitor

## What it does
Monitors process creation events using the `sched_process_fork` tracepoint. Captures PID, PPID, and command name for each new process, storing events in a ring buffer and maintaining a process counter.

## Files
- `process_monitor.bpf.c` - eBPF program that tracks process fork events
- `process_monitor.c` - Userspace program to read ring buffer events
- `Makefile` - Build configuration for both kernel and userspace programs

## Prerequisites

### Kernel Requirements
This exercise requires tracepoint support in your kernel:
```bash
# Check if the tracepoint exists
ls /sys/kernel/debug/tracing/events/sched/sched_process_fork

# If debugfs is not mounted:
sudo mount -t debugfs none /sys/kernel/debug
```

### Required Kernel Configurations
```bash
# Verify these are enabled in your kernel
zgrep CONFIG_TRACEPOINTS /proc/config.gz    # Should show =y
zgrep CONFIG_SCHED_TRACER /proc/config.gz   # Should show =y
zgrep CONFIG_BPF_EVENTS /proc/config.gz     # Should show =y
```

## How to build
```bash
make
```

## How to run

1. Load the eBPF program
```shell
sudo bpftool prog load process_monitor.bpf.o /sys/fs/bpf/process_monitor
```

2. Check the process counter (should be 0 initially)
```shell
sudo bpftool map dump name process_count
```

3. Attach to the tracepoint
```shell
sudo bpftool prog attach pinned /sys/fs/bpf/process_monitor tracepoint/sched/sched_process_fork
```

4. Test by creating new processes
```shell
# Run some commands to trigger process creation
ls /tmp
ps aux | head
echo "testing process creation"
```

5. Check the process counter again
```shell
sudo bpftool map dump name process_count
```

6. Run the userspace program to view ring buffer events
```shell
sudo ./process_monitor
```

7. In another terminal, trigger process creation
```shell
ls /tmp
ps aux | head
echo "testing process creation"
```

## Expected output

**process_count map:**
```bash
$ sudo bpftool map dump name process_count
[{"key": 0, "value": 3}]
```

**Ring buffer events (from userspace program):**
```
Monitoring process creation events... Press Ctrl-C to exit
PID      PPID     COMM
-------- -------- ----------------
12345    8888     ls
12346    8888     ps
12347    8888     head
```

## How it works
- Uses `SEC("tracepoint/sched/sched_process_fork")` to hook into process creation
- Stores detailed events in `BPF_MAP_TYPE_RINGBUF` for userspace consumption
- Maintains a simple counter in `BPF_MAP_TYPE_ARRAY` for quick statistics
- Captures process metadata using `bpf_get_current_pid_tgid()` and `bpf_get_current_comm()`

## Status
✅ **Complete** - Full implementation with both kernel and userspace components

## Troubleshooting

### Common Issues

**1. Tracepoint not found**
```bash
# Error: could not attach program
# Check if tracepoint exists:
ls /sys/kernel/debug/tracing/events/sched/
```

**2. Permission denied**
```bash
# Make sure debugfs is mounted and accessible
sudo mount -t debugfs none /sys/kernel/debug
sudo chmod 755 /sys/kernel/debug/tracing
```

**3. Program load fails**
```bash
# Check kernel logs for BPF verifier errors
dmesg | grep -i bpf
```

**4. No events captured**
```bash
# Verify the tracepoint is enabled
cat /sys/kernel/debug/tracing/events/sched/sched_process_fork/enable
# Should show "1" when program is attached
```

### Alternative approaches if tracepoint fails
- Try kprobe approach: `SEC("kprobe/do_fork")`
- Use syscall tracing: `SEC("tracepoint/syscalls/sys_enter_clone")`

## Detailed Information
Walk through exactly what happens when you run a command and how the eBPF program captures it.

### Example: Running `ls /tmp`

**System call chain:**
```bash
$ ls /tmp
```
1. Shell (bash) calls `fork()` syscall
2. Kernel creates new process with new PID
3. New process calls `execve()` to run `/bin/ls`
4. `ls` process runs and exits

**What the eBPF program sees:**

When the `fork()` happens, kernel triggers the `sched_process_fork` tracepoint:
```c
SEC("tracepoint/sched/sched_process_fork")
int trace_process_fork(void *ctx)
```

**Data captured:**

For `ls /tmp` command:
```c
// What gets stored in the ring buffer:
struct process_event {
    pid: 12345,        // New process ID for 'ls'
    ppid: 8888,        // Parent process ID (bash shell)
    comm: "ls"         // Command name (truncated to 16 chars)
}
```

**Process counter increments:** `0 → 1`

### Complex Example: `ps aux | head`

This creates **3 processes:**

```
bash → fork() → ps     (event 1: pid=12346, ppid=8888, comm="ps")
bash → fork() → head   (event 2: pid=12347, ppid=8888, comm="head")
bash → fork() → bash   (event 3: pid=12348, ppid=8888, comm="bash") [for pipe]
```

- **Ring buffer contains:** 3 separate events
- **Process counter:** `1 → 4`

### Observable Results

**Before running commands:**
```bash
$ sudo bpftool map dump name process_count
[{"key": 0, "value": 0}]
```

**After running `ls /tmp; ps aux | head`:**
```bash
$ sudo bpftool map dump name process_count
[{"key": 0, "value": 4}]
```

> **Note**: The ring buffer contains the detailed events, but you need a userspace program to read those (that's the "to be implemented" part).
