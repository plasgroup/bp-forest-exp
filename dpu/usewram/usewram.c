#include "usewram.h"
#include "../../common/common.h"
#include <stdio.h>

#define true 1
#define false 0
//#define USE_LINEAR_SEARCH
__mram BPTreeNode nodes[MAX_NODE_NUM];
int free_node_index_stack_head = -1;
__mram int free_node_index_stack[MAX_NODE_NUM];
int height = 1;

#ifdef DEBUG_ON
	typedef struct Queue { 	// queue for showing all nodes by BFS
		int tail;
		int head;
		MBPTptr ptrs[MAX_NODE_NUM];
	} Queue_t;

	__mram_ptr Queue_t *queue;
	
	void initQueue(__mram_ptr Queue_t *queue){
		queue->head = 0;
		queue->tail = -1;
		//printf("queue is initialized\n");
	}

	void enqueue(__mram_ptr Queue_t *queue, MBPTptr input){
		if((queue->tail + 2) % MAX_NODE_NUM == queue->head){
			printf("queue is full\n");
			return;
		}
		queue->ptrs[(queue->tail + 1) % MAX_NODE_NUM] = input;
		queue->tail = (queue->tail + 1) % MAX_NODE_NUM;
		//printf("%p is enqueued\n",input);
	}

	MBPTptr dequeue(__mram_ptr Queue_t *queue){
		MBPTptr ret;
		if((queue->tail + 1) % MAX_NODE_NUM == queue->head){
			printf("queue is empty\n");
			return NULL;
		}
		ret = queue->ptrs[queue->head];
		queue->head = (queue->head + 1) % MAX_NODE_NUM;
		//printf("%p is dequeued\n",ret);
		return ret;
	}

	void showNode(MBPTptr, int);
#endif

int max_node_index = -1;

MBPTptr root;

int NumOfNodes = 0;

MBPTptr newBPTreeNode() {
    MBPTptr p;
    if (free_node_index_stack_head >= 0) { // if there is gap in nodes array
        p = &nodes[free_node_index_stack[free_node_index_stack_head--]];
    } else p = &nodes[++max_node_index];

    p->isRoot = false;
    p->isLeaf = false;
    p->numKeys = 0;
    p->children[0] = NULL;
    p->parent = NULL;
    p->right = NULL;
    p->left = NULL;
    NumOfNodes++;
    return p;
}

// binary search
int findKeyPos(MBPTptr n, key_t key) {
	int l = 0, r = n->numKeys;
	if (key < n->key[l]) return l;
	if (n->key[r - 1] <= key) return r;
	while (l < r - 1) {
		int mid = (l + r) >> 1;
		if (n->key[mid-1] > key)
			r = mid;
		else
			l = mid;
	}
	return l;
}

#ifdef USE_LINEAR_SEARCH
	// linear search
	int findKeyPos(MBPTptr n, key_t key) {
		int ret = 0;
		for(int ret = 0; ret < n->numKeys; ret++) {
			if (n->key[ret] <= key) return ret;
		}
		return ret;
	}
#endif
MBPTptr findLeaf(key_t key) {
	MBPTptr n = root;
	while (true) {
		if (n->isLeaf == true)
			break;
		if (key < n->key[0]) {
			n = n->children[0];
		} else {
			int i = findKeyPos(n, key);
			#ifdef DEBUG_ON
				//printf("findLeaf:Node = %p, key = %d, i = %d\n",n, key,i);
			#endif
			n = n->children[i];
		}
	}
	return n;
}
void insert(MBPTptr cur, key_t, __mram_ptr void*, MBPTptr);
void split(MBPTptr cur) {
	// cur splits into cur and n
	// copy cur[Mid+1 .. MAX_CHILD] to n[0 .. n->key_num-1]
	MBPTptr n = newBPTreeNode();
	int Mid = (MAX_CHILD + 1) >> 1;
	n->isLeaf = cur->isLeaf;
	n->numKeys = MAX_CHILD - Mid;
	if (!n->isLeaf) { // n is InternalNode
		for (int i = Mid; i < MAX_CHILD; i++) {
			n->children[i - Mid] = cur->children[i];
			n->key[i - Mid] = cur->key[i];
			n->children[i - Mid]->parent = n;
			cur->numKeys = Mid-1;
		}
		n->children[MAX_CHILD - Mid] = cur->children[MAX_CHILD];
		n->children[MAX_CHILD - Mid]->parent = n;
	} else { // n is LeafNode
		for (int i = Mid; i < MAX_CHILD; i++) {
			n->value[i - Mid] = cur->value[i];
			n->key[i - Mid] = cur->key[i];
			cur->numKeys = Mid;
		}
	}
	if (cur->isRoot) {// root Node splits
		// Create a new root
		root = newBPTreeNode();
		root->numKeys = 1;
		root->isRoot = true;
		root->children[0] = cur;
		root->children[1] = n;
		cur->parent = n->parent = root;
		cur->isRoot = false;
		if (cur->isLeaf) {
			cur->right = n;
			n->left = cur;
			root->key[0] = n->key[0];
		} else {
			root->key[0] = cur->key[Mid-1];
		}
		height++;
	} else {// insert n to cur->parent
		n->parent = cur->parent;
		if (cur->isLeaf) {
			insert(n->parent, n->key[0], NULL, n);
		} else {
			insert(cur->parent, cur->key[Mid-1], NULL, n);
		}
	}
}

void insert(MBPTptr cur, key_t key, __mram_ptr void* value, MBPTptr n) {
	int i, ins;
    ins = findKeyPos(cur, key);
	if (cur->isLeaf == true) { // inserted into a Leaf node
		if(ins != 0 && cur->key[ins-1] == key){ // key already exist
			cur->value[ins-1] = value;
		} else { // key doesn't already exist
			for (i = cur->numKeys; i > ins; i--) {
				cur->key[i] = cur->key[i - 1];
				cur->value[i] = cur->value[i - 1];
			}
			cur->key[ins] = key;
			cur->value[ins] = value;
			cur->numKeys++;
		}

	} else { // inserted into an internal node by split
		cur->children[cur->numKeys+1] = cur->children[cur->numKeys];
		for (i = cur->numKeys; i > ins; i--) {
			cur->children[i] = cur->children[i - 1];
			cur->key[i] = cur->key[i - 1];
		}
		cur->key[ins] = key;
		cur->children[ins+1] = n;
		cur->numKeys++;
		MBPTptr firstChild = cur->children[0];
		if (firstChild->isLeaf == true) { // the child is Leaf
			if (ins > 0) {
				MBPTptr prevChild;
				MBPTptr nextChild;
				prevChild = cur->children[ins];
				nextChild = prevChild->right;
				prevChild->right = n;
				n->right = nextChild;
				n->left = prevChild;
				if (nextChild != NULL) nextChild->left = n;
			} else { // do not have a prevChild
				n->right = cur->children[2];
				n->left = cur->children[0];
				firstChild->right = n;
			}
		}
	}
	if (cur->numKeys == MAX_CHILD) 	split(cur); // key is full
}

void init_BPTree() {
    NumOfNodes = 0;
	height = 1;
	root = newBPTreeNode();
	root->isRoot = true;
	root->isLeaf = true;
}

int BPTreeInsert(key_t key, __mram_ptr void* value) {
	if(root->numKeys == 0){ // if the tree is empty
		root->key[0] = key;
		root->numKeys++;
		root->value[0] = value;
		return true;
	}
	MBPTptr Leaf = findLeaf(key);
	//int i = findKeyPos(Leaf, key);
	//printf("key:%ld,pos:%d\n",key,i);
	insert(Leaf, key, value, NULL);
    //printf("inserted {key %d, value '%s'}.\n",key,(char*)value);
	return true;
}

__mram_ptr void *BPTreeGet(key_t key) {
	MBPTptr Leaf = findLeaf(key);
	int i;
	for (i = 0; i < Leaf->numKeys; i++) {
		if (Leaf->key[i] == key) {
			#ifdef DEBUG_ON
				//printf("[key = %ld: found]", Leaf->key[i]);
			#endif
            return Leaf->value[i];
		}
	}	
	#ifdef DEBUG_ON
    	//printf("[key = %ld: not found]", key);
	#endif
	return NULL;
}
#ifdef DEBUG_ON
	void showNode(MBPTptr cur, int nodeNo) { // show single node
		char wram_str[MAXIMAM_LENGTH];
		printf("[Node No. %d]\n",nodeNo);
		if(cur->isLeaf == true){
			cur->isRoot? printf("this is a Root LeafNode (addr %p)\n", cur) : printf("this is a LeafNode (addr %p)\n", cur) ;
			printf("0. parent: %p\n", cur->parent);
			printf("1. number of keys: %d\n", cur->numKeys);
			printf("2. keys:[ ");
			for(int i = 0; i < cur->numKeys; i++) {
				printf("%lu ", cur->key[i]);
			}
			printf("]\n");
			printf("3. values:[ ");
			for(int i = 0; i < cur->numKeys; i++) {
				memcpy(wram_str, cur->value[i], MAXIMAM_LENGTH);
				printf("%s ", wram_str);
			}
			printf("]\n");
			printf("4. leaf connections, left: %p right: %p\n", cur->left, cur->right);
		} else {
			cur->isRoot? printf("this is a Root InternalNode (addr %p)\n", cur) : printf("this is an InternalNode (addr %p)\n", cur) ;
			printf("0. parent: %p\n", cur->parent);
			printf("1. number of keys: %d\n", cur->numKeys);
			printf("2. keys:[ ");
			for(int i = 0; i < cur->numKeys; i++) {
				printf("%lu ", cur->key[i]);
			}
			printf("]\n");
			printf("3. children:[ ");
			for(int i = 0; i <= cur->numKeys; i++) {
				printf("%p ", cur->children[i]);
			}
			printf("]\n");
		}
		printf("\n");

	}

	void BPTreePrintLeaves() {
		MBPTptr Leaf = findLeaf(0);
		int cnt = 0;
		while (Leaf != NULL) {
			showNode(Leaf, cnt);
			Leaf = Leaf->right;
			cnt++;
		}
		printf("\n");
	}

	void BPTreePrintRoot() {
		printf("rootNode\n");
		showNode(root,0);
	}

	void BPTreePrintAll() { // show all node (BFS)
		int nodeNo = 0;
		initQueue(queue);
		enqueue(queue, root);
		while((queue->tail + 1) % MAX_NODE_NUM != queue->head) {
			MBPTptr cur = dequeue(queue);
			showNode(cur,nodeNo);
			nodeNo++;
			if(!cur->isLeaf){
				for(int i = 0; i <= cur->numKeys; i++){
					enqueue(queue, cur->children[i]);
				}
			}
		}
	}

#endif
	int BPTree_GetNumOfNodes() {
		return NumOfNodes;
	}

	int BPTree_GetHeight() {
		return height;
	}