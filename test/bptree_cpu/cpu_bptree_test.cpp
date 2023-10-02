#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif
#include "../../bp-forest/external/cmdline/cmdline.h"
#include "bplustree.h"
#include <assert.h>
#include <fstream>
#include <iostream>
#include <pthread.h>
#include <sched.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <time.h>

#define GET_AND_PRINT_TIME(CODES, LABEL) \
    gettimeofday(&start, NULL);          \
    CODES                                \
    gettimeofday(&end, NULL);            \
    printf("time spent for %s: %0.8f sec\n", #LABEL, time_diff(&start, &end));

// #define WRITE_ON
#ifndef THREAD_NUM
#define THREAD_NUM (1)
#endif
#define NUM_REQ_PER_THREAD (10000000)
#define W50R50 (0)
#define W95R05 (1)
request_t* requests[THREAD_NUM];
int num_reqs[THREAD_NUM];
BPlusTree* bptrees[THREAD_NUM];
int total_num_reqs;
int workload;
float time_total;
int req_skew_limit = 10;
BPlusTree* bplustrees[THREAD_NUM];
constexpr key_t_ RANGE
    = std::numeric_limits<uint64_t>::max() / (THREAD_NUM);
float time_diff(struct timeval* start, struct timeval* end)
{
    return (end->tv_sec - start->tv_sec) + 1e-6 * (end->tv_usec - start->tv_usec);
}

void generate_requests(int n)
{
    srand((unsigned int)time(NULL));
    total_num_reqs = 0;
    for (int i = 0; i < THREAD_NUM; i++) {
        requests[i] = (request_t*)malloc(sizeof(request_t) * req_skew_limit * NUM_REQ_PER_THREAD);
        num_reqs[i] = 0;
    }
    int range = RAND_MAX / THREAD_NUM;  // range of keys for each threads  ex)RAND_MAX=103,THREAD_NUM=4 -> RANGE=25
    while (total_num_reqs < n) {
        key_t_ key = (key_t_)rand();
        int which_thread = key / range;    // to which thread the query send ex)key=54->which_thread=2
        if (which_thread >= THREAD_NUM) {  // actual rand_max=range*THREAD_NUM ex)RAND_MAX=103,THREAD_NUM=4 -> actual rand_max=100
            continue;
            // printf("[debug_info]thread remainder: key = %ld\n", key);
        }
        requests[which_thread][num_reqs[which_thread]].key = key;
        requests[which_thread][num_reqs[which_thread]].read_or_write = WRITE;
        requests[which_thread][num_reqs[which_thread]].val = key;
        num_reqs[which_thread]++;
        total_num_reqs++;
        if (num_reqs[which_thread] > req_skew_limit * NUM_REQ_PER_THREAD) {
            fprintf(stderr, "error: the num of reqs exceed the limit\n");
            assert(0);
        }
    }
}

void generate_requests_fromfile(int n, std::ifstream& fs)
{
    char* file_name;
    /* read workload file */
    key_t_* batch_keys = (key_t_*)malloc(n * sizeof(key_t_));
    fs.read(reinterpret_cast<char*>(batch_keys), sizeof(batch_keys) * n);

    total_num_reqs = 0;
    for (int i = 0; i < THREAD_NUM; i++) {
        requests[i] = (request_t*)malloc(sizeof(request_t) * req_skew_limit * NUM_REQ_PER_THREAD);
        num_reqs[i] = 0;
    }
    key_t_ key;
    while (total_num_reqs < n) {
        key = batch_keys[total_num_reqs];
        int which_thread = key / RANGE;    // to which thread the query send ex)key=54->which_thread=2
        if (which_thread >= THREAD_NUM) {  // actual rand_max=RANGE*THREAD_NUM ex)RAND_MAX=103,THREAD_NUM=4 -> actual rand_max=100
            continue;
            // printf("[debug_info]thread remainder: key = %ld\n", key);
        }
        requests[which_thread][num_reqs[which_thread]].key = key;
        requests[which_thread][num_reqs[which_thread]].read_or_write = WRITE;
        requests[which_thread][num_reqs[which_thread]].val = key;
        num_reqs[which_thread]++;
        total_num_reqs++;
        if (num_reqs[which_thread] > req_skew_limit * NUM_REQ_PER_THREAD) {
            fprintf(stderr, "error: the num of reqs exceed the limit\n");
            assert(0);
        }
    }
}

void* local_bptree_init(void* thread_id)
{
    srand((unsigned int)time(NULL));
    int tid = *((int*)thread_id);
    request_t* reqs = (request_t*)malloc(sizeof(request_t) * 100000);
    for (int i = 0; i < 100000; i++) {
        key_t_ key = (key_t_)rand() % RANGE + tid * RANGE;  // tid*RANGE ~ (tid+1)*RANGE
        reqs[i].key = key;
        reqs[i].read_or_write = WRITE;
        reqs[i].val = key;
    }
    // printf("thread %d: initialize B+ tree with 100000 keys\n", tid);
    bplustrees[tid] = init_BPTree();
    for (int i = 0; i < 100000; i++) {
        BPTreeInsert(bplustrees[tid], reqs[i].key, reqs[i].val);
    }
    free((void*)reqs);
    return NULL;
    // printf("thread %d: num of nodes = %d, height = %d\n", tid, BPTree_GetNumOfNodes(bplustrees[tid]) ,BPTree_GetHeight(bplustrees[tid]));
}

void* execute_queries(void* thread_id)
{
    int tid = *((int*)thread_id);
    // thread->core map
    cpu_set_t aff_mask, get_mask;
    uint32_t cpu_id = tid;  // the cpu the thread want to use
    CPU_ZERO(&aff_mask);
    CPU_SET(cpu_id, &aff_mask);
    // printf("thread %d:setting mask to %08lx\n",tid,*(unsigned long*)aff_mask.__bits);
    assert(0 == sched_setaffinity(0, sizeof(cpu_set_t), &aff_mask));
    assert(0 == sched_getaffinity(0, sizeof(cpu_set_t), &get_mask));
    // printf("thread %d:set mask to %08lx\n",tid,*(unsigned long*)get_mask.__bits);
    value_ptr_t_ volatile getval;
    // printf("thread %d: %d times of insertion\n", tid, num_reqs[tid]);
    if (workload == W50R50) {
        for (int i = 0; i < num_reqs[tid]; i++) {
            BPTreeInsert(bplustrees[tid], requests[tid][i].key, requests[tid][i].val);
        }
        // printf("thread %d: search\n", tid);
        for (int i = 0; i < num_reqs[tid]; i++) {
            getval = BPTreeGet(bplustrees[tid], requests[tid][i].key);
        }
    } else if (workload == W95R05) {
        for (int i = 0; i < num_reqs[tid] / 10; i++) {
            BPTreeInsert(bplustrees[tid], requests[tid][i].key, requests[tid][i].val);
        }
        // printf("thread %d: search\n", tid);
        for (int i = 0; i < num_reqs[tid]; i++) {
            getval = BPTreeGet(bplustrees[tid], requests[tid][i].key);
        }
        for (int i = 0; i < num_reqs[tid] * 9 / 10; i++) {
            getval = BPTreeGet(bplustrees[tid], requests[tid][i].key);
        }
    }

    getval++;
    return NULL;
    // printf("thread %d: num of nodes = %d, height = %d\n", tid, BPTree_GetNumOfNodes(bplustrees[tid]) ,BPTree_GetHeight(bplustrees[tid]));
}

int main(int argc, char* argv[])
{
    // printf("number of request = %d\n", 2*THREAD_NUM*NUM_REQ_PER_THREAD);
    // printf("sizeof a Node = %ld\n", sizeof(BPTreeNode));
    struct timeval start, end;
    pthread_t threads[THREAD_NUM];

    cmdline::parser a;
    a.add<int>("keynum", 'n', "maximum num of keys for the experiment", false, 100000000);
    a.add<std::string>("zipfianconst", 'a', "zipfianconst", false, "0");
    a.add<int>("workload", 'w', "0->write_intensive,1->read_intensive", false, 0);
    a.parse_check(argc, argv);
    std::string zipfian_const = a.get<std::string>("zipfianconst");
    workload = a.get<int>("workload");
    int max_key_num = a.get<int>("keynum");
    std::string file_name = ("./bp-forest/workload/zipf_const_" + zipfian_const + ".bin");
    std::ifstream file_input(file_name, std::ios_base::binary);
    std::cout << file_name << std::endl;
    if (!file_input) {
        printf("cannot open file\n");
        return 1;
    }
    // init bplusstrees
    int thread_ids[THREAD_NUM];
    for (int i = 0; i < THREAD_NUM; i++) {
        thread_ids[i] = i;
    }
    for (int i = 0; i < THREAD_NUM; i++) {
        pthread_create(&threads[i], NULL, local_bptree_init, thread_ids + i);
    }
    for (int i = 0; i < THREAD_NUM; i++) {
        pthread_join(threads[i], NULL);
    }
    // printf("generating requests...\n");
    generate_requests_fromfile(THREAD_NUM * NUM_REQ_PER_THREAD, file_input);
    // printf("generated %d requests\n", total_num_reqs);
    // execute queries
    gettimeofday(&start, NULL);
    for (int i = 0; i < THREAD_NUM; i++) {
        pthread_create(&threads[i], NULL, execute_queries, thread_ids + i);
    }
    for (int i = 0; i < THREAD_NUM; i++) {
        pthread_join(threads[i], NULL);
    }
    gettimeofday(&end, NULL);
    for (int i = 0; i < THREAD_NUM; i++) {
        free(requests[i]);
    }
    time_total = time_diff(&start, &end);
    double throughput = (double)total_num_reqs * 2 / time_total;
    // printf("time: %0.8f[s] for %d requests, %d threads\n",time_total, total_num_reqs, THREAD_NUM);
    // printf("throughput: %0.8lf\n",throughput);
    printf("%d,%d,%d,%0.5f,%0.1lf\n", MAX_CHILD, THREAD_NUM, total_num_reqs, time_total, throughput);
#ifdef DEBUG_ON
    BPTreePrintAll();
#endif
// write results to a csv file
#ifdef WRITE_ON
    char fname[50];
    sprintf(fname, "data/%d_req_cpu_throughput.csv", REQ_NUM);
    FILE* fp;
    fp = fopen(fname, "a");
    if (fp == NULL) {
        printf("file %s cannot be open¥n", fname);
        return -1;
    }
    if (MAX_CHILD == 4) {
        fprintf(fp, "branching factor, throughput_get, throughput_insert, throughput[OPS/s], total_time[s]\n");
    }
    fprintf(fp, "%d, %lf, %lf, %lf, %0.8f\n", MAX_CHILD, throughput_get, throughput_ins, throughput, time_total);
    fclose(fp);
#endif
    return 0;
}