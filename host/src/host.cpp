#include <assert.h>
#include <dpu.h>
#include <dpu_log.h>
#include <stdio.h>
#include <fstream>
#include <stdlib.h>
#include <sys/time.h>
#include <math.h>
#include <string>

#include "../../common/common.h"
#include "../inc/cmdline.h"
using namespace std;

#define ANSI_COLOR_RED "\x1b[31m"
#define ANSI_COLOR_GREEN "\x1b[32m"
#define ANSI_COLOR_RESET "\x1b[0m"

#ifndef DPU_BINARY
#define DPU_BINARY "../../build/dpu_task"
#endif

#ifndef FILE_NAME
#define FILE_NAME "./workload/zipfianconst0.99.csv"
#endif

#define GET_AND_PRINT_TIME(CODES,LABEL)  gettimeofday(&_time, NULL); \
                               CODES \
                               gettimeofday(&end_time, NULL); \
                               printf("time spent for %s: %0.8f sec\n", #LABEL, time_diff(&_time,&end_time)); \
                               
//dpu_request_t dpu_requests[NR_DPUS];
dpu_request_t* dpu_requests;
#ifdef VARY_REQUESTNUM
  dpu_experiment_var_t expvars;
  dpu_stats_t stats[NR_DPUS][NUM_VARS];
#endif
uint64_t nb_cycles_insert[NR_DPUS];
uint64_t nb_cycles_get[NR_DPUS];
uint64_t total_cycles_insert;
float total_time_sendrequests;
float total_time_dpu_execution;
float total_time;
int each_dpu;
uint64_t num_requests[NR_DPUS];
uint64_t total_num_keys;
uint32_t nr_of_dpus;  
struct timeval start_time;
struct timeval end_time;
struct timeval start_total;

uint64_t generate_requests(){
  dpu_requests = (dpu_request_t*)malloc(NR_DPUS*NR_TASKLETS*sizeof(dpu_request_t));
  if(dpu_requests == NULL){
    printf("[" ANSI_COLOR_RED "ERROR" ANSI_COLOR_RESET"]heap size is not enough\n");
    return 0;
  }
  for(int i = 0; i < NR_DPUS*NR_TASKLETS; i++){
        dpu_requests[i].num_req = 0;
  }
  int range = NR_ELEMS_PER_TASKLET;
  for (int i = 0; i < 10000*NR_DPUS*NR_TASKLETS; i++){
    //printf("%d\n",i);
    intkey_t key = (intkey_t)rand();
    //intkey_t key = (intkey_t)rand();
    int which_tasklet = key/range;
    //printf("key:%ld,DPU:%d,tasklet:%d\n", key, which_DPU, which_tasklet);
    if(which_tasklet >= NR_TASKLETS*NR_DPUS){
      continue;
      //printf("[debug_info]tasklet remainder: key = %ld\n", key);
    } 
    int idx = dpu_requests[which_tasklet].num_req;
    dpu_requests[which_tasklet].key[idx] = key;
    dpu_requests[which_tasklet].read_or_write[idx] = WRITE;
    dpu_requests[which_tasklet].write_val_ptr[idx] = key;
    //printf("[debug_info]request %d,key:%ld,which_tasklet:%d,dpu_requests[which_tasklet].num_req:%d\n",i,key,which_tasklet,dpu_requests[which_tasklet].num_req);
    dpu_requests[which_tasklet].num_req++;
    if(dpu_requests[which_tasklet].num_req > NUM_REQUESTS_IN_BATCH){
      printf("[" ANSI_COLOR_RED "ERROR" ANSI_COLOR_RESET"] request buffer size exceeds the limit because of skew\n");
      assert(false);
    }
    
  }
  for(int i = 0; i < NR_DPUS*NR_TASKLETS; i++){
    //num_requests[batch_num][i/NR_TASKLETS] += (uint64_t)dpu_requests[i].num_req;
    //printf("[debug_info]num_req[%d][%d] = %d\n", i/NR_TASKLETS,i%NR_TASKLETS,dpu_requests[i].num_req);
  }
  uint64_t ret = 0;
  for(int i = 0; i < NR_DPUS; i++){
    // ret += num_requests[batch_num][i];
  }
  return ret;
  //printf("[debug_info]MAX_REQ_NUM_PER_TASKLET_IN_BATCH = %d\n",MAX_REQ_NUM_PER_TASKLET_IN_BATCH);
}

void generate_requests_fromfile(string f){
    ifstream file(f);
        if (file.fail()) {
        cerr << "Cannot open file\n";
        exit(0);
    }
  dpu_requests = (dpu_request_t*)malloc(NR_DPUS*NR_TASKLETS*sizeof(dpu_request_t));
  if(dpu_requests == NULL){
    printf("[" ANSI_COLOR_RED "ERROR" ANSI_COLOR_RESET"]heap size is not enough\n");
    return;
  }
  for(int i = 0; i < NR_DPUS*NR_TASKLETS; i++){
        dpu_requests[i].num_req = 0;
  }
  int range = NR_ELEMS_PER_TASKLET;
  //printf("%d\n",i);
  intkey_t key;
  int key_count = 0;
  string l;
  while (getline(file, l)){
    key_count++;
    key = stoi(l);
    int which_tasklet = key/range;
    //printf("key:%ld,DPU:%d,tasklet:%d\n", key, which_DPU, which_tasklet);
    if(which_tasklet >= NR_TASKLETS*NR_DPUS){
      which_tasklet = NR_TASKLETS*NR_DPUS - 1;
      //printf("[debug_info]tasklet remainder: key = %ld\n", key);
    } 
    int idx = dpu_requests[which_tasklet].num_req;
    dpu_requests[which_tasklet].key[idx] = key;
    dpu_requests[which_tasklet].read_or_write[idx] = WRITE;
    dpu_requests[which_tasklet].write_val_ptr[idx] = key;
    //printf("[debug_info]request %d,key:%ld,which_tasklet:%d,dpu_requests[which_tasklet].num_req:%d\n",i,key,which_tasklet,dpu_requests[which_tasklet].num_req);
    dpu_requests[which_tasklet].num_req++;
    if(dpu_requests[which_tasklet].num_req > NUM_REQUESTS_IN_BATCH){
      printf("[" ANSI_COLOR_RED "ERROR" ANSI_COLOR_RESET"] request buffer size exceeds the limit because of skew\n");
      assert(false);
    }
  }
  for(int i = 0; i < NR_DPUS*NR_TASKLETS; i++){
    num_requests[i/NR_TASKLETS] += (uint64_t)dpu_requests[i].num_req;
    //printf("[debug_info]num_req[%d][%d] = %d\n", i/NR_TASKLETS,i%NR_TASKLETS,dpu_requests[i].num_req);
  }
  //printf("[debug_info]MAX_REQ_NUM_PER_TASKLET_IN_BATCH = %d\n",MAX_REQ_NUM_PER_TASKLET_IN_BATCH);
}

// void generate_requests_same(){
//   dpu_requests.num_req = 0;
//   for (int i = 0; i < NUM_REQUESTS/NR_DPUS; i++){
//     intkey_t key = (intkey_t)rand();
//     dpu_requests.key[i] = key;
//     dpu_requests.read_or_write[i] = WRITE;
//     sprintf(dpu_requests.write_val[i], "%ld", key);
//     dpu_requests.num_req++;
//   }
// }
#ifdef VARY_REQUESTNUM
  void send_experiment_vars(struct dpu_set_t set){
    DPU_ASSERT(dpu_broadcast_to(set, "expvars", 0, &expvars, sizeof(dpu_experiment_var_t), DPU_XFER_DEFAULT));
  }

  void receive_stats(struct dpu_set_t set){
    struct dpu_set_t dpu;
    uint32_t each_dpu;
    DPU_FOREACH(set, dpu, each_dpu) {
      DPU_ASSERT(dpu_prepare_xfer(dpu, &stats[each_dpu]));
    }
    DPU_ASSERT(dpu_push_xfer(set, DPU_XFER_FROM_DPU, "stats", 0, NUM_VARS*sizeof(dpu_stats_t), DPU_XFER_DEFAULT));
  }
#endif

void show_requests(int i){
    if(i >= NR_DPUS*NR_TASKLETS) {
      printf("[invalid argment]i must be less than NR_DPUS*NR_TASKLETS");
      return;
    }
    printf("[debug_info]DPU:%d,tasklet:%d\n",i/NR_DPUS,i%NR_DPUS);
    for (int j = 0; j < dpu_requests[i].num_req; j++){
      printf("[debug_info][key:%ld, ",dpu_requests[i].key[j]);
      //if (dpu_requests[i].read_or_write[j] == WRITE) printf("type:write]\n");
      //else printf("type:read]\n");
    }
}

void send_requests(struct dpu_set_t set, struct dpu_set_t dpu){
  DPU_FOREACH(set, dpu, each_dpu) {
    DPU_ASSERT(dpu_prepare_xfer(dpu, &dpu_requests[each_dpu*NR_TASKLETS]));
  }
  DPU_ASSERT(dpu_push_xfer(set, DPU_XFER_TO_DPU, "request_buffer", 0, sizeof(dpu_request_t)*NR_TASKLETS, DPU_XFER_DEFAULT));
}

void send_requests_same(struct dpu_set_t set){
  DPU_ASSERT(dpu_broadcast_to(set, "request_buffer", 0, &dpu_requests, sizeof(dpu_request_t), DPU_XFER_DEFAULT));
}


float time_diff(struct timeval *_time, struct timeval *end_time) {
    float timediff=(end_time->tv_sec - _time->tv_sec) + 1e-6*(end_time->tv_usec - _time->tv_usec);
    return timediff;
}

void execute_one_batch(struct dpu_set_t set, struct dpu_set_t dpu){
  //printf("\n");
  //printf("======= batch %d =======\n",batch_num);
  total_num_keys += 100000;
  //show_requests(0);
  gettimeofday(&start_total, NULL);
  //CPU→DPU
  //printf("sending %d requests for %d DPUS...\n", NUM_REQUESTS_IN_BATCH, nr_of_dpus);
  gettimeofday(&start_time, NULL);
  send_requests(set,dpu);
  gettimeofday(&end_time, NULL);
  total_time+=time_diff(&start_time,&end_time);
  total_time_sendrequests+=time_diff(&start_time,&end_time);

  //DPU execution
  gettimeofday(&start_time, NULL);
  DPU_ASSERT(dpu_launch(set, DPU_SYNCHRONOUS));
  gettimeofday(&end_time, NULL);

  total_time+=time_diff(&start_time,&end_time);
  total_time_dpu_execution+=time_diff(&start_time,&end_time);

  //DPU→CPU
  #ifdef STATS_ON
  GET_AND_PRINT_TIME(
    {
      #ifdef  VARY_REQUESTNUM
        each_dpu = 0;
        receive_stats(set);
      #endif
        each_dpu = 0;
        DPU_FOREACH(set, dpu, each_dpu) {
          DPU_ASSERT(dpu_prepare_xfer(dpu, &nb_cycles_get[each_dpu]));
        }
        DPU_ASSERT(dpu_push_xfer(set, DPU_XFER_FROM_DPU, "nb_cycles_get", 0, sizeof(uint64_t), DPU_XFER_DEFAULT));
        DPU_FOREACH(set, dpu, each_dpu) {
          DPU_ASSERT(dpu_prepare_xfer(dpu, &nb_cycles_insert[each_dpu]));
        }
        DPU_ASSERT(dpu_push_xfer(set, DPU_XFER_FROM_DPU, "nb_cycles_insert", 0, sizeof(uint64_t), DPU_XFER_DEFAULT));     
    },receive results
  )
  for(int each_dpu = 0; each_dpu < NR_DPUS;each_dpu++) {
    #ifdef VARY_REQUESTNUM
    for(int x=0; x<NUM_VARS;x++){
      printf("[DPU#%u]requestnum:around_%d cycles/request:[insert:%dcycles, get:%dcycles]\n", each_dpu, stats[each_dpu][x].x,stats[each_dpu][x].cycle_insert/(expvars.gap*2),stats[each_dpu][x].cycle_get/(expvars.gap*2));
    }
    #endif
    printf("[DPU#%u]nb_cycles_insert=%ld(average %ld cycles)\n",each_dpu,nb_cycles_insert[each_dpu],nb_cycles_insert[each_dpu]/num_requests[each_dpu]);
    printf("[DPU#%u]nb_cycles_get=%ld(average %ld cycles)\n",each_dpu,nb_cycles_get[each_dpu],nb_cycles_get[each_dpu]/num_requests[each_dpu]);
    printf("\n");
  }
  #endif
  //print results
  //printf("total time spent: %0.8f sec\n", time_diff(&start_total,&end_time));
  //#ifdef DEBUG_ON
    //printf("results from DPUs: batch %d\n",batch_num);
    //DPU_FOREACH(set, dpu) {
      //DPU_ASSERT(dpu_log_read(dpu, stdout));
    //}
  //#endif
}



int main(void) {
    cmdline::parser a;
    a.add<string>("workload", 'w', "workload file name", true, "./workload/test.csv");
  //printf("\n");
  //printf("size of dpu_request_t:%lu,%lu\n", sizeof(dpu_request_t), sizeof(dpu_requests[0]));
  //printf("total num of requests:%ld\n",(uint64_t)NUM_REQUESTS);
  struct dpu_set_t set, dpu;
  string file_name = a.get<string>("workload");
  DPU_ASSERT(dpu_alloc(NR_DPUS, NULL, &set));
  DPU_ASSERT(dpu_load(set, DPU_BINARY, NULL));
  DPU_ASSERT(dpu_get_nr_dpus(set, &nr_of_dpus));
  //printf("Allocated %d DPU(s)\n", nr_of_dpus);

  //set expvars
  #ifdef VARY_REQUESTNUM
    expvars.gap=50;
    int init = 100;
    int var = 1000; //vars = {100,1000,2000,4000,8000,16000,32000,64000,...}
    expvars.vars[0] = init;
    for(int i = 1; i < NUM_VARS; i++){
      expvars.vars[i] = var;
      var = var << 1;
    }
    printf("max_expvars:%d\n",expvars.vars[NUM_VARS-1]);
    printf("NUM_REQUESTS_PER_DPU:%d\n",NUM_REQUESTS_PER_DPU);
    if(expvars.vars[NUM_VARS-1] >= NUM_REQUESTS_PER_DPU){
      printf(ANSI_COLOR_RED "please reduce NUM_VARS in common.h to ceil(%f).\n", (log2(NUM_REQUESTS_PER_DPU/1000)+1));
      return 1;
    }

    send_experiment_vars(set);
  #endif
  //printf("initializing trees (10000 keys average)\n");
  generate_requests();
  send_requests(set,dpu);
  DPU_ASSERT(dpu_launch(set, DPU_SYNCHRONOUS));
  free(dpu_requests);
  //printf("initialization finished\n");
  for(int i = 0; i < NUM_BATCH; i++){
      generate_requests_fromfile(file_name);
      execute_one_batch(set,dpu);
      free(dpu_requests);
  }

  double throughput = 2*total_num_keys/total_time;
  printf("%d, %ld, %0.8f, %0.8f, %0.8f, %0.0f\n", NR_TASKLETS, (long int)2*NUM_REQUESTS, total_time_sendrequests, total_time_dpu_execution, total_time, throughput);
  DPU_ASSERT(dpu_free(set));
#ifdef WRITE_CSV
  // write results to a csv file
  char *fname = "data/taskletnum_upmem_pointerdram_1thread.csv";
  FILE *fp;
  fp = fopen( fname, "a");
  if( fp == NULL ){
    printf( "file %s cannot be open¥n", fname );
    return -1;
  }
  if(NR_TASKLETS == 1){
      fprintf(fp,"num of tasklets, total_num_requests[s], time_sendrequests[s], time_dpu_execution[s], total_time[s], throughput[OPS/s]\n");
  }
  fprintf(fp,"%d, %ld, %0.8f, %0.8f, %0.8f, %0.0f\n", NR_TASKLETS, 2*total_num_keys, total_time_sendrequests, total_time_dpu_execution, total_time, throughput);
  fclose(fp);
#endif
  return 0;
}