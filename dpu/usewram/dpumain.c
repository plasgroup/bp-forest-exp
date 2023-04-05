#include <perfcounter.h>
#include <stdio.h>
#include <mram.h>
#include "bplustree.h"
#include "../../common/common.h"

//#define INSERTION_TIMES (10000)
#define GET_TIMES (10000)



__mram char value_mram[MAXIMAM_LENGTH];
__mram dpu_request_t request_buffer;
__mram dpu_result_t result;
__mram_ptr void *ptr;
#ifdef DEBUG_ON
    __mram_ptr void* getval;
#endif
char wram_str[MAXIMAM_LENGTH];
int main() {
    printf("\n");
    printf("\n");
    printf("using up to %d MB for node data, maximum node number = %d\n", NODE_DATA_SIZE,MAX_NODE_NUM);
    #ifdef STATS_ON
        int nb_cycles_get = 0;
        int nb_cycles_insert = 0;
    #endif
    printf("initializing BPTree\n");
    init_BPTree();
    uint32_t cnt = 0;
    printf("%d times insertion\n",request_buffer.num_req);
    #ifdef STATS_ON
        perfcounter_config(COUNT_CYCLES, true);
    #endif

    while(cnt < request_buffer.num_req){
        //memcpy(value_mram,"test",MAXIMAM_LENGTH);
        //ptr = &value_mram[cnt];
        //BPTreeInsert(cnt, ptr);
        BPTreeInsert(request_buffer.key[cnt], request_buffer.write_val[cnt]);
        cnt++;
    }
    #ifdef STATS_ON
        nb_cycles_insert = perfcounter_get();
    #endif
    printf("insertion finished\n");
    #ifdef DEBUG_ON
        BPTreePrintAll();
    #endif
    printf("%d times search\n",request_buffer.num_req);
    #ifdef STATS_ON
        perfcounter_config(COUNT_CYCLES, true);
    #endif

    for(uint32_t i = 0; i < request_buffer.num_req; i++) {

        memcpy(result.val, BPTreeGet(request_buffer.key[i]), MAXIMAM_LENGTH);
        #ifdef DEBUG_ON
        //     if(getval!= NULL){
        //         memcpy(wram_str, getval, MAXIMAM_LENGTH);
        //         printf(", value: \"%s\"\n", wram_str);
        //     } else {printf(", value: NULL\n");}

        #endif
    }

    #ifdef STATS_ON
        nb_cycles_get = perfcounter_get();
    #endif
    printf("search finished\n");
    #ifdef STATS_ON
        printf("total num of nodes = %d\n", BPTree_GetNumOfNodes());
        printf("height = %d\n", BPTree_GetHeight());
        printf("nb_cycles_insert = %d\n", nb_cycles_insert);
        printf("cycles/insert = %d\n", nb_cycles_insert/request_buffer.num_req);
        printf("nb_cycles_get = %d\n", nb_cycles_get);
        printf("cycles/get = %d\n", nb_cycles_get/request_buffer.num_req);
    #endif
    return 0;
}