#inlcude <linux/bpf.h>
#include <bpf/bpf_helpers.h>

// Using map to store the counter , let us can read it in userspace
struct {
    __uint(type, BPF_MAP_TYPE_ARRAY);
    __type(key, int);
    __type(value, long);
    __uint(max_entries, 1);
} counter_map SEC(".maps");

SEC("cgroup/bind4")
int hello_bind(void *ctx)
{
    int key = 0;
    long *count = bpf_map_lookup_elem(&counter_map, &key);

    if (count) {
        *count += 1;
    } else {
        long *initial = 1;
        bpf_map_update_elem(&counter_map, &key, &initial, BPF_ANY);
    }

    return 1; // 1 = allow
}

char LICENSE[] SEC("license") = "GPL";
