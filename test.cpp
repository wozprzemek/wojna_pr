#include <iostream>
#include <vector>
#include <algorithm>
#include <functional>


int main(int argc, char **argv) {
    std::vector<std::pair<int, int>> v;
    v.push_back(std::make_pair(2,32));
    v.push_back(std::make_pair(4,6));
    v.push_back(std::make_pair(2,2));
    v.push_back(std::make_pair(2,122));
    v.push_back(std::make_pair(2,1));
    v.push_back(std::make_pair(-2,32));

    std::sort(v.begin(), v.end(), std::greater<>());

    for (int i=0; i<v.size(); i++) {
        std::cout << v[i].second << std::endl;
        if (v[i].second == 1) {
            std::cout << "A " << v[i].second << std::endl;
            v.erase(v.begin() + i);
        }
    }
    std::cout  << std::endl;
    for (int i=0; i<v.size(); i++) {
        std::cout << v[i].first << " " << v[i].second << std::endl;
    }
}