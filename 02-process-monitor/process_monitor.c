#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/resource.h>
#include <bpf/libbpf.h>
#include <bpf/bpf.h>

// Must match the struct in the eBPF program
struct process_event {
    unsigned int pid;
    unsigned int ppid;
    char comm[16];
};

static volatile bool exiting = false;

static void sig_handler(int sig)
{
    exiting = true;
}

static int handle_event(void *ctx, void *data, size_t data_sz)
{
    const struct process_event *e = data;

    printf("PID: %-8u PPID: %-8u COMM: %s\n",
           e->pid, e->ppid, e->comm);

    return 0;
}

int main(int argc, char **argv)
{
    struct ring_buffer *rb = NULL;
    int map_fd, err;

    // Handle Ctrl-C
    signal(SIGINT, sig_handler);
    signal(SIGTERM, sig_handler);

    // Set rlimit for memlock (required for BPF)
    struct rlimit rlim = {
        .rlim_cur = RLIM_INFINITY,
        .rlim_max = RLIM_INFINITY,
    };
    if (setrlimit(RLIMIT_MEMLOCK, &rlim)) {
        perror("setrlimit");
        return 1;
    }

    // Get the ring buffer map file descriptor
    map_fd = bpf_obj_get("/sys/fs/bpf/process_monitor");
    if (map_fd < 0) {
        fprintf(stderr, "Failed to get map fd: %s\n", strerror(-map_fd));
        fprintf(stderr, "Make sure the eBPF program is loaded and pinned\n");
        return 1;
    }

    // Get the events map by name (we need to find it)
    // This is a simplified approach - in practice you'd use bpf_object__find_map_by_name
    int events_fd = -1;
    struct bpf_map_info info = {};
    __u32 info_len = sizeof(info);

    // Try to find the ring buffer map
    // In a real implementation, you'd iterate through maps to find the ring buffer
    // For now, we'll use a simple approach assuming it's map id 1
    for (int id = 1; id < 100; id++) {
        int fd = bpf_map_get_fd_by_id(id);
        if (fd < 0) continue;

        if (bpf_obj_get_info_by_fd(fd, &info, &info_len) == 0) {
            if (info.type == BPF_MAP_TYPE_RINGBUF) {
                events_fd = fd;
                break;
            }
        }
        close(fd);
    }

    if (events_fd < 0) {
        fprintf(stderr, "Failed to find ring buffer map\n");
        return 1;
    }

    // Create ring buffer manager
    rb = ring_buffer__new(events_fd, handle_event, NULL, NULL);
    if (!rb) {
        fprintf(stderr, "Failed to create ring buffer\n");
        close(events_fd);
        return 1;
    }

    printf("Monitoring process creation events... Press Ctrl-C to exit\n");
    printf("%-8s %-8s %s\n", "PID", "PPID", "COMM");
    printf("-------- -------- ----------------\n");

    // Poll for events
    while (!exiting) {
        err = ring_buffer__poll(rb, 100 /* timeout, ms */);
        if (err == -EINTR) {
            err = 0;
            break;
        }
        if (err < 0) {
            fprintf(stderr, "Error polling ring buffer: %d\n", err);
            break;
        }
    }

    ring_buffer__free(rb);
    close(events_fd);
    return err < 0 ? -err : 0;
}
