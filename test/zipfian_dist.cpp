#include "cmdline.h"
#include <assert.h>
#include <algorithm>
#include <fstream>
#include <iostream>
#include <limits>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <string>

#ifndef NR_DPUS
#define NR_DPUS (1920)
#endif
#ifndef NR_TASKLETS
#define NR_TASKLETS (1)
#endif
#ifndef NUM_BPTREE_IN_DPU
#define NUM_BPTREE_IN_DPU (NR_TASKLETS)
#endif
#ifndef NUM_BPTREE_IN_CPU
#define NUM_BPTREE_IN_CPU (128)
#endif
/* the size for a request(default:17B) */
#define REQUEST_SIZE (17)
/* buffer size for request in a DPU(default:20MB/64MB) */
#ifndef MRAM_REQUEST_BUFFER_SIZE
#define MRAM_REQUEST_BUFFER_SIZE (1024 * 1024)
#endif
/* default:1,233,618 requests / DPU / batch */
#ifndef MAX_REQ_NUM_IN_A_DPU
#define MAX_REQ_NUM_IN_A_DPU (MRAM_REQUEST_BUFFER_SIZE / REQUEST_SIZE)
#endif
#define NUM_REQUESTS_PER_BATCH (1000000)
#define READ (1)
#define WRITE (0)

#define PRINT_DEBUG

/* Structure used by both the host and the dpu to communicate information */
#include <stdint.h>

typedef uint64_t key_int64_t;
typedef uint64_t value_ptr_t;
/* one request */
typedef struct
{
    key_int64_t key;           // key of each request
    value_ptr_t write_val_ptr; // write pointer to the value if request is write
    uint8_t operation;         // 1→read or 0→write
} each_request_t;

/* requests for a DPU in a batch */
typedef struct
{
    int end_idx[NUM_BPTREE_IN_DPU]; // number of requests
    each_request_t requests[MAX_REQ_NUM_IN_A_DPU];
} dpu_requests_t;

#define ANSI_COLOR_RED "\x1b[31m"
#define ANSI_COLOR_GREEN "\x1b[32m"
#define ANSI_COLOR_RESET "\x1b[0m"

#define NUM_TOTAL_TREES (NR_DPUS * NUM_BPTREE_IN_DPU + NUM_BPTREE_IN_CPU)
#define NR_DPUS_REDUNDANT (64)

dpu_requests_t *dpu_requests;
typedef std::vector<std::vector<each_request_t>> cpu_requests_t;
cpu_requests_t cpu_requests(NUM_BPTREE_IN_CPU);

int send_size;
uint64_t num_requests[NR_DPUS];
int num_reqs_for_cpus[NUM_BPTREE_IN_CPU];
uint64_t total_num_keys;
uint64_t total_num_keys_cpu;
uint64_t total_num_keys_dpu;
constexpr key_int64_t RANGE = std::numeric_limits<uint64_t>::max() / (NUM_TOTAL_TREES);

int generate_requests_fromfile(std::ifstream &fs, int n)
{
    dpu_requests = (dpu_requests_t *)malloc(
        (NR_DPUS) * sizeof(dpu_requests_t));
    if (dpu_requests == NULL)
    {
        printf("[" ANSI_COLOR_RED "ERROR" ANSI_COLOR_RESET
               "] heap size is not enough\n");
        return 0;
    }

    /* read workload file */
    key_int64_t *batch_keys = (key_int64_t *)malloc(n * sizeof(key_int64_t));
    if (dpu_requests == NULL)
    {
        printf("[" ANSI_COLOR_RED "ERROR" ANSI_COLOR_RESET
               "] heap size is not enough\n");
        return 0;
    }
    // std::cout << "malloc batch_keys" << std::endl;
    int key_count = 0;
    fs.read(reinterpret_cast<char *>(batch_keys), sizeof(batch_keys) * n);
    key_count = fs.tellg() / sizeof(key_int64_t) - total_num_keys;
    // std::cout << "key_count: " << key_count << std::endl;
    for (int i = 0; i < key_count; i++)
    {
        // std::cout << batch_keys[i] << std::endl;
    }
    std::cout << std::endl;
    /* sort by which tree the requests should be processed */
    std::sort(batch_keys, batch_keys + key_count, [](auto a, auto b)
              { return a / RANGE < b / RANGE; });
    /* count the number of requests for each tree */
    int num_keys[NUM_TOTAL_TREES] = {};
    int num_keys_sum[NUM_TOTAL_TREES + 1] = {};
    int num_keys_for_each_dpu[NR_DPUS] = {};
    for (int i = 0; i < key_count; i++)
    {
        num_keys[batch_keys[i] / RANGE]++;
        // std::cout << batch_keys[i] << std::endl;
    }
    std::cout << "range: " << RANGE << std::endl;
    num_keys_sum[0] = num_keys[0];
    for (int i = 0; i < 20; i++)
    {
        std::cout << "elem" << i << ": " << num_keys[i] << ", " << 100 * num_keys[i] / key_count << "%" << std::endl;
        if (!(num_keys_sum[i] == 0))
            std::cout << "max elem of " << i << ": " << batch_keys[num_keys_sum[i] - 1] << std::endl;
        num_keys_sum[i + 1] = num_keys_sum[i] + num_keys[i];
    }
    /* count the number of requests for each DPU, determine the send size */
    send_size = 0;
    for (int dpu_i = 0; dpu_i < NR_DPUS; dpu_i++)
    {
        for (int tree_i = 0; tree_i < NUM_BPTREE_IN_DPU; tree_i++)
        {
            num_keys_for_each_dpu[dpu_i] += num_keys[NUM_BPTREE_IN_CPU + dpu_i * NUM_BPTREE_IN_DPU + tree_i];
            dpu_requests[dpu_i].end_idx[tree_i] = num_keys_for_each_dpu[dpu_i];
#ifdef PRINT_DEBUG
            // printf("dpu_%d's end_idx of tree%d = %d\n", dpu_i, tree_i, dpu_requests[dpu_i].end_idx[tree_i]);
#endif
        }
        total_num_keys_dpu += num_keys_for_each_dpu[dpu_i];
        /* send size: maximum number of requests to a DPU */
        if (num_keys_for_each_dpu[dpu_i] > send_size)
            send_size = num_keys_for_each_dpu[dpu_i];
    }
    int current_idx = 0;
    /* make cpu requests */
    for (int cpu_i = 0; cpu_i < NUM_BPTREE_IN_CPU; cpu_i++)
    {
        cpu_requests.at(cpu_i).clear();
        for (int i = 0; i < num_keys[cpu_i]; i++)
        {
            cpu_requests.at(cpu_i).push_back({batch_keys[current_idx + i], batch_keys[current_idx + i], WRITE});
        }
        current_idx += num_keys[cpu_i];
        num_reqs_for_cpus[cpu_i] = num_keys[cpu_i];
        total_num_keys_cpu += num_reqs_for_cpus[cpu_i];
    }
    /* make dpu requests */
    for (int dpu_i = 0; dpu_i < NR_DPUS; dpu_i++)
    {
        if (num_keys_for_each_dpu[dpu_i] >= MAX_REQ_NUM_IN_A_DPU)
        {
            printf("[" ANSI_COLOR_RED "ERROR" ANSI_COLOR_RESET
                   "] request buffer size %d exceeds the limit %d because of skew\n",
                   num_keys_for_each_dpu[dpu_i], MAX_REQ_NUM_IN_A_DPU);
            assert(false);
        }
        for (int i = 0; i < num_keys_for_each_dpu[dpu_i]; i++)
        {
            dpu_requests[dpu_i].requests[i].key = batch_keys[current_idx + i];
            dpu_requests[dpu_i].requests[i].write_val_ptr = batch_keys[current_idx + i];
            dpu_requests[dpu_i].requests[i].operation = WRITE;
        }
        current_idx += num_keys_for_each_dpu[dpu_i];
    }
#ifdef PRINT_DEBUG
    // for (key_int64_t x : num_keys) {
    //     std::cout << x << std::endl;
    // }
#endif
    free(batch_keys);
    return key_count;
}

void show_requests(int i)
{
    if (i >= NR_DPUS)
    {
        printf("[invalid argment]i must be less than NR_DPUS");
        return;
    }
    printf("[debug_info]DPU:%d\n", i);
    for (int tree_i = 0; tree_i < NUM_BPTREE_IN_DPU; tree_i++)
    {
        if (dpu_requests[i].end_idx[tree_i] != 0)
        {
            printf("tree %d first req:%ld,WRITE\n", tree_i, dpu_requests[i].requests[0].key);
        }
    }
}

int main(int argc, char *argv[])
{
    cmdline::parser a;
    a.add<int>("keynum", 'n', "maximum num of keys for the experiment", false, 1000000);
    a.add<std::string>("zipfianconst", 'a', "zipfianconst", false, "1.2");
    a.parse_check(argc, argv);
    std::string zipfian_const = a.get<std::string>("zipfianconst");
    int max_key_num = a.get<int>("keynum");
    std::string file_name = ("./bp-forest/workload/zipf_const_" + zipfian_const + ".bin");
#ifdef PRINT_DEBUG
    std::cout << "zipf_const:" << zipfian_const << ", file:" << file_name << std::endl;
#endif
#ifdef PRINT_DEBUG
    std::cout << "num trees in CPU:" << NUM_BPTREE_IN_CPU << std::endl;
    std::cout << "num trees in DPU:" << NUM_BPTREE_IN_DPU << std::endl;
    std::cout << "num total trees:" << NUM_TOTAL_TREES << std::endl;
#endif
    /* load workload file */
    std::ifstream file_input(file_name, std::ios_base::binary);
    if (!file_input)
    {
        printf("cannot open file\n");
        return 1;
    }
    /* main routine */
    int batch_num = 0;
    int num_keys = 0;
    while (total_num_keys < max_key_num)
    {
        // printf("%d\n", num_keys);
        if (max_key_num - total_num_keys >= NUM_REQUESTS_PER_BATCH)
        {
            num_keys = generate_requests_fromfile(file_input, NUM_REQUESTS_PER_BATCH);
        }
        else
        {
            num_keys = generate_requests_fromfile(file_input, max_key_num - total_num_keys);
        }
        if (num_keys == 0)
            break;
        total_num_keys += num_keys;
#ifdef PRINT_DEBUG
        std::cout << std::endl
                  << "===== batch " << batch_num << " =====" << std::endl;
        std::cout << "[1/4] " << num_keys << " requests generated" << std::endl;
#endif
        free(dpu_requests);
        batch_num++;
    }
    return 0;
}
