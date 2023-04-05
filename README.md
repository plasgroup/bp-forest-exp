# B+-Forest
B+-Forest is an ordered index for Processing-In-Memory (PIM) architectures. It is implemented for [UPMEM](), a practical PIM architecture.  B+-Forest is based on many B+-trees and aims to handle data skew by migrating hot B+-trees from DPU to CPU.
## Directory
- /dpu
  - Source codes and header files for the DPUs (including B+tree implementation)
- /host
  - Source codes and headers files the host CPU 
- /common
  - Common header files
  
## Parameters
- common/common.h
  - NR_DPUS
    - the number of DPUs(1~2560)
  - NR_TASKLETS
    - the number of threads in each DPU (1~24)
  - NUM_REQUESTS
    - the number of requests
  - NUM_REQUESTS_IN_BATCH
    - The number of requests in a batch
