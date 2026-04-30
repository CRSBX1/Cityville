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
    Node* newHead = NULL; //start the new list as empty
    Node* current = head; //start from the original list's head
    while(current!=NULL){ //traverse until the end of original list
        insert(newHead, current->data); //insert each node's data into the new list
        current= current->next; //move to the next node
    }
    return newHead; //return the head of the copied list
}

//frees all memory used by linked list to prevent memory leaks
void freeList(Node* head){
    while(head!=NULL){
        Node* temp = head;
        head = head->next;
        delete temp;
    }
}

//BINARY SEARCH - convert linked list to a temporary array 
Resident* listToArray(Node* head, int& size){
    size = 0;
    Node* cur = head;

    //count total number of residents in the list
    while(cur != NULL){
        size++;
        cur = cur-> next;
    }
    //if list is empty, return NULL
    if(size == 0){
        return NULL;
    }
    //creates an array with enough space size for all residents
    Resident *arr = new Resident[size];
    //copy each residents data from the list into the array
    cur = head;
    for(int i=0; i<size; i++){
        arr[i] = cur->data; //copy full resident data
        cur = cur->next; //move to the next node in the list
    }
    return arr; //return the filled array
}

//count nodes in a linked list
int countNodes(Node* head) {
    int n = 0;
    for(Node* c = head; c != NULL; c = c->next) n++;
    return n;
}
