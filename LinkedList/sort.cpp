#include "header.hpp"

bool shouldSwap(Node* a, Node* b, SortField field, SortOrder order){
    float value1, value2; // two empty values to compare
    switch(field){
        case sort_age:
        value1 = (float)a->data.Age;
        value2 = (float)b->data.Age;
        break;
        case sort_distance:
        value1 = (float)a->data.DailyDistance;
        value2 = (float)b->data.DailyDistance;
        break;
        case sort_carbem:
        value1 = a->data.CarbonEmissionFactor;
        value2 = b->data.CarbonEmissionFactor;
        break;
        default:
        return false; // invalid field
    }
    if(order == sort_asc){
        return value1 > value2; // ascending: swap if the left value is bigger than right value
    } else {
        return value1 < value2; // descending: swap if left value is smaller than right value
    }
}

void bubbleSort(Node* head, SortField field, SortOrder order){
    if(head == NULL || head->next ==NULL){ // empty list / only one node, it is already sorted
        return;
    }
    bool swapped;
    do{
        swapped = false; // nothing was swapped yet
        Node* current = head;
        while(current->next!=NULL){
            if(shouldSwap(current, current->next, field, order)){ // to decide should current and current->next be swapped
                Resident temp = current->data;
                current->data = current->next->data;
                current->next->data = temp;
                swapped = true; // something was swapped and it is not fully sorted yet
            }
            current = current->next;
        } 
    } while (swapped); // swapped = false, list is sorted
}

void insertionSort(Node* head, SortField field, SortOrder order){
    if(head == NULL || head->next == NULL){ //no sorting needed if list is empty or has only 1 node
        return;
    }
    Node* sortedEnd = head; //marks where the sorted section ends, starts as the first node

    while(sortedEnd->next != NULL){
        Node* key_node = sortedEnd->next; //first node of the unsorted section, to be placed in the correct position
        Resident key = key_node->data; //copy its data so we can safely overwrite nodes while shifting

        Node tempNode; //temporary node to wrap 'key' so shouldSwap() can compare it
        tempNode.data = key; 
        tempNode.next = NULL; 

        Node* insertBefore = NULL; //stays null if 'key' belong at the end (no movement needed)
        Node * cur = head; //walks through sorted section from beginning to find the right location for 'key'

        while(cur!=sortedEnd->next){ //scan every node in the sorted section 
            if(shouldSwap(cur, &tempNode, field, order)){ //if 'cur' comes after 'key', then 'key' is in front of 'cur'
                insertBefore = cur; //mark node as insertion point
                break; 
            }
            cur = cur->next; //move to the next node in the sorted section
        }

        if(insertBefore!=NULL){
            Node* shift = sortedEnd; //starts at sortedEnd and moves backward to shift data forward one by one
            while(shift!=insertBefore){ //keep shifting until we reach insertion point
                shift->next->data = shift->data; //push this node's data one step forward to open a gap
                //singly linked list can't go backward directly
                //so we walk from head to find the node just before 'shift'
                Node* prev = head;
                while(prev->next!=shift){
                    prev=prev->next; //move forward until prev is right behind 'shift'
                }
                shift=prev; //step 'shift' one node backward
            }
            insertBefore->data = key; //gap is open and we can now place 'key' here
        }
        sortedEnd = sortedEnd->next; //expand the sorted section by one node
    }
}

//converts SortField enum to a readable string for display
string fieldName(SortField field){
    switch(field){
        case sort_age:
            return "Age";
        case sort_distance:
            return "Daily Distance";
        case sort_carbem:
            return "Carbon Emission Factor";
        default:
            return "Unknown";
    }
}

//convert SortOrder enum to a readable string for display
string orderName(SortOrder order){
    return (order==sort_asc) ? "Ascending" : "Descending";
}

void displaySortResult(Node* originalHead, const string& cityName, const string& algoName, SortField field, SortOrder order, bool useInsertion){
    //reset memory tracking before copying so the copy's allocations are counted
    MemoryMetrics::reset();
    Node* copy = copyList(originalHead); //make a copy so the original list stays unchanged

    //start timer just before the sort begins
    auto timer = startTimer();

    if(useInsertion){
        insertionSort(copy, field, order); //sort the copy using insertion sort
    }else{
        bubbleSort(copy, field, order); //sort the copy using bubble sort
    }

    //stop timer immediately after the sort finishes
    double elapsedMs = stopTimer(timer);    
    size_t memUsed = MemoryMetrics::trackMemoryUsage();

    int width = 82;
    printSeparator(width, '=');
    cout<<algoName << " | " << cityName << " | Sorted by: " <<fieldName(field) << " (" <<orderName(order) << ")"<<endl;
    printResidentHeader(width);

    Node* current = copy;
    while(current!=NULL){
        printResidentRow(current);
        current = current->next;
    }

    printSeparator(width, '=');
    cout<<endl;

    //print performance stats for this sort run
    printPerformance(elapsedMs, memUsed);

    freeList(copy); //free the copied list after displaying
}

//BINARY SEARCH (SORTED)
//binary search by age
void arraySortInt(Resident* arr, int size, bool byAge){
    for(int i=1; i<size; i++){
        Resident key = arr[i]; //pick this resudent to find their correct position
        int keyVal = byAge ? key.Age : key.DailyDistance; //extract the field being sorted by
        int j = i-1; //start looking leftward from the position just before the picked resident
        //shift resident one space to the right if it is larger than the picked resident
        while(j >= 0 && (byAge ? arr[j].Age : arr[j].DailyDistance) > keyVal){
            arr[j+1] = arr[j]; //slide this resident one space to the right to open a gap
            j--; //move one more step to the left
        }
        arr[j+1] = key; //place the picked resident into the correct gap
    }
}

//LINEAR SEARCH - sorts a copy of linked list by a given field
Node* sortListCopyByField(Node* head, const string& field){
    int size = 0; 
    Resident* arr = listToArray(head, size); //convert linked list to a temporary array
    if(!arr){
        return NULL; //for empty list, nothing to search
    }

    string f = toLower(field); //convert ffield name to lowercase for comparison

    if(f == "age"){
        arraySortInt(arr, size, true); //sort array by age in ascending 
    }else if(f == "distance"){
        arraySortInt(arr, size, false); //sort array by daily distance in ascending
    }else if(f == "transport"){
        //sort array alphabetically by mode of transport using insertion sort
        for(int i=1; i<size; i++){
            Resident key = arr[i]; //pick this resident to find their correct position
            string keyMode = toLower(key.ModeOfTransport); //get their transport mode in lowercase
            int j = i-1; //start looking leftware from the position before the picked resident
            //shift residents rightward whose transport mode comes after the piked mode alphabetically
            while(j >= 0 && toLower(arr[j].ModeOfTransport) > keyMode){
                arr[j+1] = arr[j]; //slide this resident one space to the right
                j--; //move one more step to the left
            }
            arr[j+1] = key; //place picked resident in their correct alphabetical position
        }
    }
    
    //rebuild a new linked list from the sorted array
    Node* sortedHead = NULL;
    for(int i=0; i<size; i++){
        insert(sortedHead, arr[i]); 
    }
    
    delete[] arr; //free temporary array from memory
    return sortedHead; //return head of the new sorted linked list
}
