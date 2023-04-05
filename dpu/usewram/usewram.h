#ifndef __usewram_H__
#define __usewram_H__

#define MAX_CHILD (12) //split occurs if numKeys >= MAX_CHILD

#define NODE_DATA_SIZE (48) //maximum node data size, MB
#define MAX_NODE_NUM ((NODE_DATA_SIZE << 20) / sizeof(BPTreeNode)) // 48MB for Node data 
#include <stdint.h>
#include <mram.h>
#include <string.h>
#include "../../common/common.h"

typedef struct BPTreeNode* MBPTptr;
extern MBPTptr root;
typedef struct BPTreeNode {
	int isRoot : 8;
    int isLeaf : 8;
	int numKeys : 16;
	key__t key[MAX_CHILD];
	void* children[MAX_CHILD];
	struct BPTreeNode* parent;
	struct BPTreeNode* right;
	struct BPTreeNode* left;
} BPTreeNode;


extern void init_BPTree();

/**
*    @param key key to insert
*    @param pos pos
*    @param value value to insert
**/
extern int BPTreeInsert(key_t, __mram_ptr void*);
/**
*    @param key key to search
**/
extern __mram_ptr void* BPTreeGet(key_t);
extern void BPTreeGetRange(key_t, int);
extern void BPTreeDelete(key_t);
extern int BPTree_GetNumOfNodes();
extern void BPTreePrintLeaves();
extern void BPTreePrintRoot();
extern void BPTreePrintAll();
extern int BPTree_GetHeight();

#endif