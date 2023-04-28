#include "../inc/utils.hpp"
#include <vector>
#include <iostream>
#include <string>
#include "../inc/cmdline.hpp"

using namespace std;
int main(int argc, char *argv[]){
    cmdline::parser a;
    a.add<int>("numkeys", 'n', "number of keys", false, 10);
    a.add("64bit", 'b', "if use 64bit_random_generator(if false, 32bit)");
    a.parse_check(argc, argv);
    auto numkeys = a.get<int>("numkeys");
    vector<uint64_t> keys64;
    vector<uint32_t> keys32;
    rand_generator::init();
    for(int i = 0; i < numkeys; i++){
        keys64.push_back(rand_generator::rand64());
        keys32.push_back(rand_generator::rand32());
    }
    if(a.exist("64bit")) {
        cout << "===== " << numkeys << " 64bit random integers =====" << endl;
        for(int i = 0; i < numkeys; i++){
            cout << keys64[i] << endl;
        }
    } else {
        cout << endl << "===== " << numkeys << " 32bit random integers =====" << endl;
        for(int i = 0; i < numkeys; i++){
            cout << keys32[i] << endl;
        }
    }


    return 0;
}