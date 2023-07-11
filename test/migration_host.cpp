#include <assert.h>
extern "C" {
#include <dpu.h>
#include <dpu_log.h>
}
#include <stdint.h>
#include <stdio.h>

typedef uint64_t key_int64_t;
typedef uint64_t value_ptr_t;

typedef struct BPTptr {
    uint64_t x;
} BPTptr;

#define MAX_CHILD 126
#define MAX_NODE_NUM 10000
extern BPTptr root;

typedef struct InternalNodePtrs {
    BPTptr children[MAX_CHILD + 1];
} InternalNodePtrs;

typedef struct LeafNodePtrs {
    value_ptr_t value[MAX_CHILD];
    BPTptr right;
    BPTptr left;
} LeafNodePtrs;

typedef struct BPTreeNode {
    int isRoot : 8;
    int isLeaf : 8;
    int numKeys : 16;
    key_int64_t key[MAX_CHILD];
    BPTptr parent;
    union {
        InternalNodePtrs inl;
        LeafNodePtrs lf;
    } ptrs;
} BPTreeNode;

#ifndef DPU_BINARY
#define DPU_BINARY "./build/migration_dpu"
#endif

extern BPTreeNode nodes_buffer[MAX_NODE_NUM];
extern uint64_t nodes_num;
int each_dpu;
uint64_t task_from = 0;
uint64_t task_to = 1;
void send_nodes_from_dpu_to_dpu(int from, int to, dpu_set_t set, dpu_set_t dpu)
{
    DPU_FOREACH(set, dpu, each_dpu)
    {
        printf("ed = %d\n", each_dpu);
        if (each_dpu == from) {
            DPU_ASSERT(dpu_prepare_xfer(dpu, &task_from));
            DPU_ASSERT(dpu_push_xfer(dpu, DPU_XFER_TO_DPU, "task_no", 0, sizeof(uint64_t), DPU_XFER_DEFAULT));
            DPU_ASSERT(dpu_launch(dpu, DPU_SYNCHRONOUS));
            DPU_ASSERT(dpu_copy_from(dpu, "nodes_transfer_num", 0, &nodes_num, sizeof(uint64_t)));
            DPU_ASSERT(dpu_copy_from(dpu, "nodes_transfer_buffer", 0, &nodes_buffer, nodes_num * sizeof(BPTreeNode)));
            break;
            printf("%lu\n", nodes_num);
        }
    }
    DPU_FOREACH(set, dpu, each_dpu)
    {
        printf("ed = %d\n", each_dpu);
        if (each_dpu == to) {
            DPU_ASSERT(dpu_prepare_xfer(dpu, &nodes_num));
            DPU_ASSERT(dpu_push_xfer(set, DPU_XFER_TO_DPU, "nodes_transfer_num", 0, sizeof(uint64_t), DPU_XFER_DEFAULT));
            DPU_ASSERT(dpu_prepare_xfer(dpu, &nodes_buffer));
            DPU_ASSERT(dpu_push_xfer(set, DPU_XFER_TO_DPU, "nodes_transfer_buffer", 0, nodes_num * sizeof(BPTreeNode), DPU_XFER_DEFAULT));
            DPU_ASSERT(dpu_prepare_xfer(dpu, &task_to));
            DPU_ASSERT(dpu_push_xfer(set, DPU_XFER_TO_DPU, "task_no", 0, sizeof(uint64_t), DPU_XFER_DEFAULT));
            DPU_ASSERT(dpu_launch(dpu, DPU_SYNCHRONOUS));
            break;
        }
    }
}
int main(void)
{
    struct dpu_set_t set, dpu;
    printf("size:%ld\n", sizeof(BPTreeNode));

    DPU_ASSERT(dpu_alloc(2, NULL, &set));
    DPU_ASSERT(dpu_load(set, DPU_BINARY, NULL));
    send_nodes_from_dpu_to_dpu(0, 1, set, dpu);
    DPU_FOREACH(set, dpu, each_dpu)
    {
        DPU_ASSERT(dpu_log_read(dpu, stdout));
    }
    DPU_ASSERT(dpu_free(set));
    return 0;
}