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

    Node* lists[3] = { cityA, cityB, cityC };
    for(int l = 0; l < 3; l++) {
        Node* current = lists[l];
        while(current != NULL) {
            Node* temp = current;
            current = current->next;
            delete temp;
        }
    }

    return 0;
}
