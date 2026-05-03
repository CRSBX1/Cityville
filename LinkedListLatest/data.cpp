#include "header.hpp"

//insert function, to insert at the end of linked list
void insert(Node*& head, Resident r) {
    Node* newNode = new Node();
    newNode->data = r;
    newNode->next = NULL;
    if(head == NULL) {
        head = newNode; //first node becomes head
    } else {
        Node* temp = head;
        while(temp->next != NULL) {
            temp = temp->next; //traverse to last node
        }
        temp->next = newNode; //link new node at the end
    }
}

//load dataset from file into linked list
void loadData(Node*& head, string filename) {
    ifstream file(filename);
    if(!file) {
        cout << "File not exist" << endl;
        return;
    }
    string line;
    getline(file, line); //skip header row
    //read each data line
    while(getline(file, line)) {
        stringstream ss(line);
        Resident r;
        string temp;

        getline(ss, r.ResidentId, ',');
        getline(ss, temp, ',');
        r.Age = stoi(temp);
        getline(ss, r.ModeOfTransport, ',');
        getline(ss, temp, ',');
        r.DailyDistance = stoi(temp);
        getline(ss, temp, ',');
        r.CarbonEmissionFactor = stof(temp);
        getline(ss, temp, ',');
        r.AverageDayPerMonth = stoi(temp);
        insert(head, r); //add to linked list
    }
    file.close();
}

//creates and returns a copy of linked list so the original is not modified
Node* copyList(Node* head){
    Node* newHead = NULL;
    Node* current = head;
    while(current!=NULL){
        insert(newHead, current->data);
        current= current->next;
    }
    return newHead;
}

//frees all memory used by linked list to prevent memory leaks
void freeList(Node* head){
    while(head!=NULL){
        Node* temp = head;
        head = head->next;
        delete temp;
    }
}

//convert linked list to a temporary array 
Resident* listToArray(Node* head, int& size){
    size = 0;
    Node* cur = head;
    while(cur != NULL){
        size++;
        cur = cur->next;
    }
    if(size == 0) return NULL;

    Resident *arr = new Resident[size];
    cur = head;
    for(int i=0; i<size; i++){
        arr[i] = cur->data;
        cur = cur->next;
    }
    return arr;
}

//count nodes in a linked list
int countNodes(Node* head) {
    int n = 0;
    for(Node* c = head; c != NULL; c = c->next) n++;
    return n;
}

//merges multiple linked lists into one combined copy
//the original lists are not modified
Node* combineLists(Node* lists[], int count) {
    Node* combined = NULL;
    for(int l = 0; l < count; l++) {
        Node* cur = lists[l];
        while(cur != NULL) {
            insert(combined, cur->data);
            cur = cur->next;
        }
    }
    return combined;
}
