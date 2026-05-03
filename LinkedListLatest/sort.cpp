#include "header.hpp"

bool shouldSwap(Node* a, Node* b, SortField field, SortOrder order){
    float value1, value2;
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
            return false;
    }
    if(order == sort_asc){
        return value1 > value2; //ascending: swap if left > right
    } else {
        return value1 < value2; //descending: swap if left < right
    }
}

void bubbleSort(Node* head, SortField field, SortOrder order){
    if(head == NULL || head->next == NULL) return;
    bool swapped;
    do{
        swapped = false;
        Node* current = head;
        while(current->next != NULL){
            if(shouldSwap(current, current->next, field, order)){
                Resident temp = current->data;
                current->data = current->next->data;
                current->next->data = temp;
                swapped = true;
            }
            current = current->next;
        } 
    } while (swapped);
}

void insertionSort(Node* head, SortField field, SortOrder order){
    if(head == NULL || head->next == NULL) return;
    Node* sortedEnd = head;

    while(sortedEnd->next != NULL){
        Node* key_node = sortedEnd->next;
        Resident key = key_node->data;

        Node tempNode;
        tempNode.data = key; 
        tempNode.next = NULL; 

        Node* insertBefore = NULL;
        Node* cur = head;

        while(cur != sortedEnd->next){
            if(shouldSwap(cur, &tempNode, field, order)){
                insertBefore = cur;
                break; 
            }
            cur = cur->next;
        }

        if(insertBefore != NULL){
            Node* shift = sortedEnd;
            while(shift != insertBefore){
                shift->next->data = shift->data;
                Node* prev = head;
                while(prev->next != shift){
                    prev = prev->next;
                }
                shift = prev;
            }
            insertBefore->data = key;
        }
        sortedEnd = sortedEnd->next;
    }
}

//converts SortField enum to a readable string for display
string fieldName(SortField field){
    switch(field){
        case sort_age:      return "Age";
        case sort_distance: return "Daily Distance";
        case sort_carbem:   return "Carbon Emission Factor";
        default:            return "Unknown";
    }
}

//convert SortOrder enum to a readable string for display
string orderName(SortOrder order){
    return (order == sort_asc) ? "Ascending" : "Descending";
}

void displaySortResult(Node* originalHead, const string& cityName, const string& algoName, SortField field, SortOrder order, bool useInsertion){
    MemoryMetrics::reset();
    Node* copy = copyList(originalHead);

    auto timer = startTimer();
    if(useInsertion){
        insertionSort(copy, field, order);
    }else{
        bubbleSort(copy, field, order);
    }
    double elapsedMs = stopTimer(timer);    
    size_t memUsed = MemoryMetrics::trackMemoryUsage();

    int width = 82;
    printSeparator(width, '=');
    cout<<algoName << " | " << cityName << " | Sorted by: " <<fieldName(field) << " (" <<orderName(order) << ")"<<endl;
    printResidentHeader(width);

    Node* current = copy;
    while(current != NULL){
        printResidentRow(current);
        current = current->next;
    }

    printSeparator(width, '=');
    cout<<endl;
    printPerformance(elapsedMs, memUsed);

    freeList(copy);
}

//insertion sort for an array by integer field (age or distance)
void arraySortInt(Resident* arr, int size, bool byAge){
    for(int i=1; i<size; i++){
        Resident key = arr[i];
        int keyVal = byAge ? key.Age : key.DailyDistance;
        int j = i-1;
        while(j >= 0 && (byAge ? arr[j].Age : arr[j].DailyDistance) > keyVal){
            arr[j+1] = arr[j];
            j--;
        }
        arr[j+1] = key;
    }
}

//insertion sort for an array alphabetically by transport mode
//extracted from duplicated inline sorts in binarySearchByTransport and sortListCopyByField
void arraySortByTransport(Resident* arr, int size){
    for(int i=1; i<size; i++){
        Resident key = arr[i];
        string keyMode = toLower(key.ModeOfTransport);
        int j = i-1;
        while(j >= 0 && toLower(arr[j].ModeOfTransport) > keyMode){
            arr[j+1] = arr[j];
            j--;
        }
        arr[j+1] = key;
    }
}

//sorts a copy of linked list by a given field, returns new sorted list
Node* sortListCopyByField(Node* head, const string& field){
    int size = 0; 
    Resident* arr = listToArray(head, size);
    if(!arr) return NULL;

    string f = toLower(field);

    if(f == "age"){
        arraySortInt(arr, size, true);
    }else if(f == "distance"){
        arraySortInt(arr, size, false);
    }else if(f == "transport"){
        arraySortByTransport(arr, size); //use extracted helper
    }
    
    //rebuild a new linked list from the sorted array
    Node* sortedHead = NULL;
    for(int i=0; i<size; i++){
        insert(sortedHead, arr[i]); 
    }
    
    delete[] arr;
    return sortedHead;
}
