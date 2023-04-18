#!/bin/bash

dpu-upmem-dpurte-clang -g -O3 -Wall -Werror -Wextra -flto=thin -Idpu/inc -Icommon/inc -DNR_DPUS=1 -DNUM_REQUESTS=2700 -DNR_TASKLETS=1 -DSTACK_SIZE_DEFAULT=256 dpu/usewram/usewram.c dpu/usewram/dpumain.c -o build/dpu_task
cc -o build/host_app host/src/pim_host.c `dpu-pkg-config --libs dpu` -fopenmp -g -Wall -Werror -Wextra -O3 -DNR_DPUS=1 -DNUM_REQUESTS=2700 -std=c99 `dpu-pkg-config --cflags dpu` -Ihost/inc -Icommon/inc -DNR_TASKLETS=1 -DDPU_BINARY=\"/home/okudahi/PIM_masstree/build/dpu_task\"
build/host_app > log/dpu/eval_wramandmram/wram.txt

dpu-upmem-dpurte-clang -g -O3 -Wall -Werror -Wextra -flto=thin -Idpu/inc -Icommon/inc -DNR_DPUS=1 -DNUM_REQUESTS=2700 -DNR_TASKLETS=1 -DSTACK_SIZE_DEFAULT=256 dpu/src/bplustree.c dpu/src/dpumain.c -o build/dpu_task
cc -o build/host_app host/src/pim_host.c `dpu-pkg-config --libs dpu` -fopenmp -g -Wall -Werror -Wextra -O3 -DNR_DPUS=1 -DNUM_REQUESTS=2700 -std=c99 `dpu-pkg-config --cflags dpu` -Ihost/inc -Icommon/inc -DNR_TASKLETS=1 -DDPU_BINARY=\"/home/okudahi/PIM_masstree/build/dpu_task\"
build/host_app > log/dpu/eval_wramandmram/mram.txt