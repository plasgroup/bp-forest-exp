#!/bin/bash
mkdir -p build
for i in 0 0.4 0.6 0.99 1.2
do
g++ ./ycsb-c/zipfian.cpp -DZIPFIAN_CONST=$i -o build/zipfian
build/zipfian > workload/zipfianconst$i.csv
done