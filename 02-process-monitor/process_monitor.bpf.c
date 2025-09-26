#include <linux/bpf.h>
#include <bpf/bpf_helpers.h>

// Map to store process creation events
struct process_event {
    __u32 pid;
    __u32 ppid;
    char comm[16];  // Process name (TASK_COMM_LEN)
};

struct {
    __uint(type, BPF_MAP_TYPE_RINGBUF);
    __uint(max_entries, 1024 * 1024); // 1MB ring buffer
} events SEC(".maps");

// Map to count total processes created
struct {
    __uint(type, BPF_MAP_TYPE_ARRAY);
    __type(key, int);
    __type(value, long);
    __uint(max_entries, 1);
} process_count SEC(".maps");

SEC("tracepoint/sched/sched_process_fork")
int trace_process_fork(void *ctx)
{
    struct process_event *event;
    int key = 0;
    long *count;

    // Reserve space in ring buffer
    event = bpf_ringbuf_reserve(&events, sizeof(*event), 0);
    if (!event)
        return 0;

    // Get current task info
    event->pid = bpf_get_current_pid_tgid() >> 32;
    event->ppid = bpf_get_current_pid_tgid() & 0xFFFFFFFF;
    bpf_get_current_comm(event->comm, sizeof(event->comm));

    // Submit event to userspace
    bpf_ringbuf_submit(event, 0);

    // Increment process counter
    count = bpf_map_lookup_elem(&process_count, &key);
    if (count) {
        *count += 1;
    } else {
        long initial = 1;
        bpf_map_update_elem(&process_count, &key, &initial, BPF_ANY);
    }

    return 0;
}

char LICENSE[] SEC("license") = "GPL";
