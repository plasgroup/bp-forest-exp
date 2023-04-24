#include <stdio.h>
#include "../../common/common.h"

int main(){
    const char *file_name = "./workload/zipf_const_1.5.bin";
    FILE *fp = fopen(file_name, "rb");
    if(!fp) {
        printf("cannot open file\n");
        return 1;
    }
    intkey_t key;
    int cnt = 0;
    while (fread(&key, sizeof(intkey_t), 1, fp) == 1){
        printf("%ld\n", key);
        cnt++;
    }
    printf("%d\n", cnt);
    return 0;
}