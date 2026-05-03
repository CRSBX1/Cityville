#include "header.hpp"

//RUN THIS AT TERMINAL: 
//g++ -std=c++14 -o program main.cpp memory.cpp utils.cpp data.cpp analysis.cpp display.cpp sort.cpp search.cpp menu.cpp && ./program
int main() {
    Node* cityA = NULL;
    Node* cityB = NULL;
    Node* cityC = NULL;

    loadData(cityA, "CityA.txt");
    loadData(cityB, "CityB.txt");
    loadData(cityC, "CityC.txt");

    mainMenu(cityA, cityB, cityC);

    //cleanup using freeList instead of manual loop
    freeList(cityA);
    freeList(cityB);
    freeList(cityC);

    return 0;
}
