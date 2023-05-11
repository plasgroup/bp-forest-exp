#include <algorithm>
#include <fstream>
#include <iostream>
#include <stdint.h>
#include <vector>
#define NR_DPUS (10)
#define NR_TASKLETS (1)
#define NUM_BPTREE_IN_CPU (0)
#define NUM_REQUESTS_PER_BATCH (100)
typedef uint64_t intkey_t;
constexpr intkey_t MAX_KEY = std::numeric_limits<intkey_t>::max();
constexpr int NUM_TOTAL_TREES(NR_DPUS* NR_TASKLETS + NUM_BPTREE_IN_CPU);
constexpr intkey_t RANGE = MAX_KEY / NUM_TOTAL_TREES;
int main()
{
    std::ifstream fs("./workload/zipf_const_1.2.bin", std::ios_base::binary);
    //FILE* fp = fopen("./workload/zipf_const_1.2.bin", "rb");
    // if (!fs) {

    //     // ファイルがオープンできなかった
    //     std::cout << L"ファイルオープン失敗" << std::endl;
    //     return (-1);
    // }
    intkey_t batch_keys[NUM_REQUESTS_PER_BATCH] = {1, 5, 4, 2, 1, 10, 9, 7, 6, 3, 1, 1};
    int key_count = 12;
    int total_num_keys = 0;
    for (int j = 0; j < 5; j++) {
        std::cout << "batch" << j << std::endl;
        fs.read(reinterpret_cast<char*>(&batch_keys), sizeof(batch_keys));
        key_count = fs.tellg() / sizeof(intkey_t) - total_num_keys;
        std::cout << "key_count: " << key_count << std::endl;
        //key_count = fread(&batch_keys, sizeof(intkey_t), NUM_REQUESTS_PER_BATCH, fp);
        std::sort(batch_keys, batch_keys + key_count, [](auto a, auto b) { return a / RANGE < b / RANGE; });
        int num_keys[NUM_TOTAL_TREES] = {};
        for (int i = 0; i < key_count; i++) {
            //std::cout << batch_keys[i] << std::endl;
            num_keys[(batch_keys[i] - 1) / RANGE]++;
        }
        for (intkey_t x : num_keys) {
            std::cout << x << std::endl;
        }
        total_num_keys += key_count;
    }

    //fclose(fp);
}
