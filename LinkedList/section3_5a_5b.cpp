#include <iostream>
#include <fstream> // read data from a file and write data into a file (gets the line)
#include <sstream> // cutting each sentence into words (breaks into value)
using namespace std;

// Define Structure
struct Resident {
    string ResidentId;
    int Age;
    string ModeOfTransport;
    int DailyDistance;
    float CarbonEmissionFactor;
    int AverageDayPerMonth;
};
// Define Structure
struct Node {
    Resident data;
    Node* next;
};

// Insert function, to insert at the end of linked list
void insert(Node*& head, Resident r){
    Node* newNode = new Node();
    newNode->data = r;
    newNode->next = NULL;
    if(head == NULL) {
        head = newNode; // first node becomes head
    } else {
        Node* temp = head;
        while(temp->next != NULL) {
            temp = temp->next; // traverse to last node
        }
        temp->next = newNode; // link new node at the end
    }
}

// Load dataset
void loadData(Node*& head, string filename) {
    ifstream file(filename);
    if(!file) {
        cout << "File not exist"<< endl;
        return;
    }
    string line;
    getline(file,line); // skip header row
    // read each data line
    while(getline(file,line)) {
        stringstream ss(line);
        Resident r;
        string temp;

        getline(ss,r.ResidentId,',');
        getline(ss,temp,',');
        r.Age = stoi(temp);
        getline(ss,r.ModeOfTransport,',');
        getline(ss,temp,',');
        r.DailyDistance = stoi(temp);
        getline(ss,temp,',');
        r.CarbonEmissionFactor = stof(temp);
        getline(ss,temp,',');
        r.AverageDayPerMonth = stoi(temp);
        insert(head,r); // add to linked list
    }
    file.close();
}

// testing
void printList(Node* head) {
    Node* temp = head;
    while (temp != NULL) {
        cout << temp->data.ResidentId<< "\t"
             << temp->data.Age << "\t"
             << temp->data.ModeOfTransport << "\t\t"
             << temp->data.DailyDistance << "\t\t"
             << temp->data.CarbonEmissionFactor << "\t"
             << temp->data.AverageDayPerMonth << endl;
        temp = temp->next;
    }
}


int main() {
    Node* cityA = NULL;
    Node* cityB = NULL;
    Node* cityC = NULL;
    loadData(cityA,"CityA.txt");
    loadData(cityB,"CityB.txt");
    loadData(cityC,"CityC.txt");
    printList(cityA);
    return 0;
}