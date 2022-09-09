#include <iostream>
#include <vector>
#include <algorithm>

int main(int argc, char **argv) {
    std::vector<std::pair<int, int>> v;
    v.push_back(std::make_pair(1,2));
    v.push_back(std::make_pair(4,6));
    v.push_back(std::make_pair(2,32));
    v.push_back(std::make_pair(-2,32));

    std::sort(v.begin(), v.end());


    for (int i=0; i<v.size(); i++) {
        std::cout << v[i].first << " " << v[i].second << std::endl;
    }
}