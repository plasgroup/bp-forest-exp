# B+-Forest
## Directory
- /dpu
  - Source codes and header files for the DPUs (including B+tree implementation)
- /host
  - Source codes and headers files the host CPU 
- /common
  - Common header files
  
## パラメータの変更
- common/common.h
  - NR_DPUS
    - the number of DPUs(1~2560)
  - NR_TASKLETS
    - the number of threads in each DPU (1~24)
  - NUM_REQUESTS
    - the number of requests
  - NUM_REQUESTS_IN_BATCH
    - The number of requests in a batch
