#!/bin/bash
mkdir -p build
for i in 0 0.5 0.99 1.2 1.5 2
do
g++ ./ycsb-c/zipfian.cpp -DZIPFIAN_CONST=$i -o build/zipfian
build/zipfian > workload/zipfianconst$i.csv
done