#include <assert.h>
extern "C" {
    #include <dpu.h>
    #include <dpu_log.h>
}
#include <stdio.h>
#include <stdint.h>

typedef uint64_t key_int64_t;
typedef uint64_t value_ptr_t;

typedef struct MBPTptr{
    uint64_t x:48;
} __attribute__((packed)) MBPTptr;

#define MAX_CHILD 12
#define MAX_NODE_NUM 10000
extern MBPTptr root;

typedef struct InternalNodePtrs {
    MBPTptr children[MAX_CHILD + 1];

} InternalNodePtrs;
typedef struct LeafNodePtrs {
    value_ptr_t value[MAX_CHILD];
    MBPTptr right;
    MBPTptr left;
} LeafNodePtrs;

typedef struct BPTreeNode {
    int isRoot : 8;
    int isLeaf : 8;
    int numKeys : 16;
    key_int64_t key[MAX_CHILD];
    MBPTptr parent;
    union {
        InternalNodePtrs inl;
        LeafNodePtrs lf;
    } ptrs;
} BPTreeNode;

#ifndef DPU_BINARY1
#define DPU_BINARY1 "./build/tree_migration_from"
#endif

#ifndef DPU_BINARY2
#define DPU_BINARY2 "./build/tree_migration_to"
#endif

BPTreeNode Nodes[MAX_NODE_NUM];
uint64_t node_num;
int main(void) {
  struct dpu_set_t set, dpu;
  int each_dpu;

  DPU_ASSERT(dpu_alloc(2, NULL, &set));
  DPU_FOREACH(set, dpu, each_dpu) {
    if (each_dpu == 0) {
      DPU_ASSERT(dpu_load(dpu, DPU_BINARY1, NULL));
      DPU_ASSERT(dpu_launch(dpu, DPU_SYNCHRONOUS));
      DPU_ASSERT(dpu_copy_from(dpu, "nodes_transfer_num", 0, nodes_num, sizeof(uint64_t)));
      DPU_ASSERT(dpu_copy_from(dpu, "nodes_transfer_buffer", 0, nodes, nodes_num*sizeof(BPTreeNode)));
    }
    else {
      DPU_ASSERT(dpu_load(dpu, DPU_BINARY2, NULL));
      DPU_ASSERT(dpu_launch(dpu, DPU_SYNCHRONOUS));
    }
      DPU_ASSERT(dpu_log_read(dpu, stdout));
  }

  DPU_ASSERT(dpu_free(set));

  return 0;
}
