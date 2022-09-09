#include <iostream>
#include <vector>

int main(int argc, char **argv) {
    std::vector<int> v;
    v.resize(10);

    v[5] = 1;
    for (int i=0; i<v.size(); i++) {
        std::cout << v[i] << std::endl;
    }
}