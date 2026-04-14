#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
using namespace std;

const int total_age_groups = 5; //total number of age groups
const int max_transport_types = 15; //max number of different transport mode per group

//each node holds one resident's data
struct Resident {
    string ResidentId;
    int Age;
    string ModeOfTransport;
    int DailyDistance;
    float CarbonEmissionFactor;
    int AverageDayPerMonth;
};

struct Node {
    Resident data;
    Node* next;
};

//how many residents use each transport mode within one age group
struct TransportCount {
    string modeName[max_transport_types];
    int userCount[max_transport_types];
    float modeCO2[max_transport_types]; //total CO2 produced by each transport mode
    int totalModes;
};

//a lightweight node that points to an existing resident node, used to build per-group lists
struct GroupMemberNode {
    Node* residentPtr; //points to the actual resident node in the city list
    GroupMemberNode* next;
};

//summary for one age group
struct AgeGroupSummary {
    string groupLabel;
    int totalResidents;
    float totalCO2;
    float averageCO2;
    float percentOfGrandTotal;
    string preferredTransportMode;
    TransportCount transportUsage;
    GroupMemberNode* membersHead; //head of this group's member list
};

//returns which age group index a given age belongs to
int getAgeGroupIndex(int age) {
    if(age >= 6  && age <= 17 ) { //children & teenagers
        return 0;
    } else if(age >= 18 && age <= 25) { //university & young adults
        return 1;
    } else if(age >= 26 && age <= 45) { //working adults early career
        return 2;
    } else if(age >= 46 && age <= 60) { //working adults late career
        return 3;
    } else if(age >= 61 && age <= 100) { //senior citizens / retirees
        return 4;
    } else {
        return -1; //age outside defined range
    }
}

//returns the display label for a given age group index
string getAgeGroupLabel(int groupIndex) {
    const string labels[total_age_groups] = {
        "Children & Teenagers        (6-17)",
        "University / Young Adults   (18-25)",
        "Working Adults Early Career (26-45)",
        "Working Adults Late Career  (46-60)",
        "Senior Citizens / Retirees  (61-100)"
    };
    if(groupIndex >= 0 && groupIndex < total_age_groups) {
        return labels[groupIndex]; //return a label if index is valid
    } else {
        return "Unknown Group"; //return for invalid index
    }
}

//calculates one resident's monthly CO2 emission
//formula = emission factor * distance per day * days per month
float calculateMonthlyCO2(const Node* resident) {
    return resident->data.CarbonEmissionFactor * (float)resident->data.DailyDistance * (float)resident->data.AverageDayPerMonth;
}

//adds mode to tally for one age group, also accumulates CO2 per mode
void addToTransportCount(TransportCount& tally, const string& mode, float co2) {
    for(int i = 0; i < tally.totalModes; i++) {
        if(tally.modeName[i] == mode) {
            tally.userCount[i]++; //increase count if mode found
            tally.modeCO2[i] += co2; //add CO2 to that mode's total
            return;
        }
    }
    //if mode not found, add mode as new entry (new transport type)
    if(tally.totalModes < max_transport_types) {
        tally.modeName[tally.totalModes] = mode; //store mode name
        tally.userCount[tally.totalModes] = 1; //first resident using the new mode
        tally.modeCO2[tally.totalModes] = co2; //set initial CO2 for this mode
        tally.totalModes++; //increase mode count
    }
}

//return name of mode with highest user count
string getMostUsedMode(const TransportCount& tally) {
    if(tally.totalModes == 0) {
        return "None";
    }
    int highestIndex = 0;
    for(int i = 1; i < tally.totalModes; i++) {
        if(tally.userCount[i] > tally.userCount[highestIndex]) {
            highestIndex = i;
        }
    }
    return tally.modeName[highestIndex];
}

//categorizes all residents from linked list into their age groups and computes summaries
//also links each resident node into a per-group member list
void categorizeByAgeGroup(Node* head, AgeGroupSummary groupSummary[]) {
    //initialize every group to 0 before counting
    for(int i = 0; i < total_age_groups; i++) {
        groupSummary[i].groupLabel             = getAgeGroupLabel(i);
        groupSummary[i].totalResidents         = 0;
        groupSummary[i].totalCO2               = 0.0f;
        groupSummary[i].averageCO2             = 0.0f;
        groupSummary[i].percentOfGrandTotal    = 0.0f;
        groupSummary[i].preferredTransportMode = "None";
        groupSummary[i].transportUsage.totalModes = 0;
        groupSummary[i].membersHead            = NULL; //empty member list for each group
    }
    float grandTotalCO2 = 0.0f; //keeps running total of CO2 across all groups combined

    //walk through every resident in the linked list
    Node* current = head;
    while(current != NULL) {
        int groupIndex = getAgeGroupIndex(current->data.Age); //find which age group the resident belongs to
        if(groupIndex != -1) { //resident is skipped if age is outside defined range
            float residentCO2 = calculateMonthlyCO2(current); //calculate how much CO2 this resident produces per month
            groupSummary[groupIndex].totalResidents++; //add resident to their group count
            groupSummary[groupIndex].totalCO2 += residentCO2; //add CO2 to their group total
            grandTotalCO2 += residentCO2; //also add CO2 to overall grand total
            addToTransportCount(groupSummary[groupIndex].transportUsage, current->data.ModeOfTransport, residentCO2); //record which transport mode the resident uses
            
            //link this resident into the group's member list
            GroupMemberNode* gNode = new GroupMemberNode();
            gNode->residentPtr = current;
            gNode->next = groupSummary[groupIndex].membersHead; 
            groupSummary[groupIndex].membersHead = gNode;
        }
        current = current->next;
    }

    //calculate avg and percentages
    for(int i = 0; i < total_age_groups; i++) {
        if(groupSummary[i].totalResidents > 0) {
            //avg CO2 = total CO2 for this group / number of residents in this group
            groupSummary[i].averageCO2 = groupSummary[i].totalCO2 / (float)groupSummary[i].totalResidents;
            if(grandTotalCO2 > 0.0f) {
                groupSummary[i].percentOfGrandTotal = (groupSummary[i].totalCO2 / grandTotalCO2) * 100.0f;
            }
            groupSummary[i].preferredTransportMode = getMostUsedMode(groupSummary[i].transportUsage); //find which transport mode is most used by residents in this group
        }
    }
}

//frees all GroupMemberNode linked lists inside a groupSummary array
void freeMemberLists(AgeGroupSummary groupSummary[]) {
    for(int i = 0; i < total_age_groups; i++) {
        GroupMemberNode* current = groupSummary[i].membersHead;
        while(current != NULL) {
            GroupMemberNode* temp = current;
            current = current->next;
            delete temp; //release each wrapper node (does not touch the actual resident node)
        }
        groupSummary[i].membersHead = NULL; //reset head after freeing
    }
}

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

//prints a horizontal separator line
void printSeparator(int width, char c) {
    for(int i = 0; i < width; i++) {
        cout << c;
    }
    cout << endl;
}

//prints the standard resident table column header
void printResidentHeader(int width) {
    printSeparator(width, '-');
    cout << left
         << setw(14) << "Resident ID"
         << setw(8)  << "Age"
         << setw(20) << "Mode of Transport"
         << setw(16) << "Daily Dist (km)"
         << setw(14) << "CO2 Factor"
         << setw(10) << "Days/Month"
         << endl;
    printSeparator(width, '-');
}

//prints one resident's data row
void printResidentRow(Node* res) {
    cout << left
         << setw(14) << res->data.ResidentId
         << setw(8)  << res->data.Age
         << setw(20) << res->data.ModeOfTransport
         << setw(16) << res->data.DailyDistance
         << setw(14) << res->data.CarbonEmissionFactor
         << setw(10) << res->data.AverageDayPerMonth
         << endl;
}

//prints raw linked-list data for one city
void printList(Node* head) {
    Node* temp = head;
    while (temp != NULL) {
        printResidentRow(temp); //print one resident's data row
        temp = temp->next;
    }
}

//displays raw data for all three cities with a header
void displayRawData(Node* headA, Node* headB, Node* headC) {
    Node*  lists[3] = { headA,    headB,    headC    };
    string names[3] = { "City A", "City B", "City C" };

    int width = 82;
    for(int l = 0; l < 3; l++) {
        printSeparator(width, '=');
        cout << names[l] << endl;
        printResidentHeader(width);
        printSeparator(width, '-');
        printList(lists[l]); //print all residents for this city
        printSeparator(width, '=');
        cout << endl;
    }
}

//displays categorized data per city
void displayCategorizedData(Node* headA, Node* headB, Node* headC) {
    Node*  lists[3] = { headA,    headB,    headC    };
    string names[3] = { "City A", "City B", "City C" };

    int width = 82;

    for(int l = 0; l < 3; l++) {
        AgeGroupSummary groups[total_age_groups];
        categorizeByAgeGroup(lists[l], groups); //single pass: fills summaries and builds member lists

        printSeparator(width, '=');
        cout << names[l] << endl;
        printSeparator(width, '=');

        //print residents group by group by traversing each group's member linked list
        for(int g = 0; g < total_age_groups; g++) {
            if(groups[g].membersHead == NULL) {
                continue; //skip empty groups
            }

            cout << "\nCategory: " << groups[g].groupLabel << endl;
            printResidentHeader(width);

            //traverse the group's member linked list and print each resident
            GroupMemberNode* gCurrent = groups[g].membersHead;
            while(gCurrent != NULL) {
                printResidentRow(gCurrent->residentPtr); 
                gCurrent = gCurrent->next; //move to next member in this group
            }
        }

        printSeparator(width, '=');
        cout << endl;

        freeMemberLists(groups); //release all GroupMemberNode wrapper nodes for this city
    }
}

//displays categorized data (all cities combined)
void displayCategorizedDataAllCities(Node* headA, Node* headB, Node* headC) {
    Node*  lists[3] = {headA, headB, headC};
    int width = 82;

    //copy all residents from all three cities into one combined linked list
    Node* combined = NULL;
    for(int l = 0; l < 3; l++) {
        Node* current = lists[l];
        while(current != NULL) {
            insert(combined, current->data); //insert a copy of each resident into combined list
            current = current->next;
        }
    }

    AgeGroupSummary groups[total_age_groups];
    categorizeByAgeGroup(combined, groups); //single pass over combined list

    //print residents group by group by traversing each group's member linked list
    for(int g = 0; g < total_age_groups; g++) {
        if(groups[g].membersHead == NULL) {
            continue; //skip empty groups
        }

        cout << "\nCategory: " << groups[g].groupLabel << endl;
        printResidentHeader(width);

        //traverse the group's member linked list and print each resident
        GroupMemberNode* gCurrent = groups[g].membersHead;
        while(gCurrent != NULL) {
            printResidentRow(gCurrent->residentPtr);
            gCurrent = gCurrent->next;
        }
    }

    printSeparator(width, '=');
    cout << endl;

    freeMemberLists(groups); //release all GroupMemberNode wrapper nodes

    //free the combined list 
    Node* curr = combined;
    while(curr != NULL) {
        Node* temp = curr;
        curr = curr->next;
        delete temp; //release each copied node
    }
}

//displays carbon emissions broken down by transport mode for one city
void displayEmissionsByTransportMode(Node* head, const string& cityName) {
    //build a transport tally by walking through every resident in the list
    TransportCount cityTally;
    cityTally.totalModes = 0;

    Node* current = head;
    while(current != NULL) {
        float co2 = calculateMonthlyCO2(current); //get this resident's monthly CO2
        addToTransportCount(cityTally, current->data.ModeOfTransport, co2); //add to tally
        current = current->next;
    }

    //sum up grand total CO2 across all modes for this city
    float grandTotal = 0.0f;
    for(int i = 0; i < cityTally.totalModes; i++) {
        grandTotal += cityTally.modeCO2[i];
    }

    //print table header
    int width = 76;
    printSeparator(width, '=');
    cout << "City: " << cityName << " | Carbon Emissions by Mode of Transport" << endl;
    printSeparator(width, '=');
    cout << left
         << setw(22) << "Mode of Transport"
         << setw(10) << "Count"
         << setw(24) << "Total Emission (kg CO2)"
         << setw(16) << "Average per Resident"
         << endl;
    printSeparator(width, '-');

    //print one row per transport mode
    for(int i = 0; i < cityTally.totalModes; i++) {
        float avg = 0.0f;
        if(cityTally.userCount[i] > 0) {
            avg = cityTally.modeCO2[i] / (float)cityTally.userCount[i]; //avg = total CO2 / number of users of this mode
        }
        cout << left
             << setw(22) << cityTally.modeName[i]
             << setw(10) << cityTally.userCount[i]
             << setw(24) << fixed << setprecision(2) << cityTally.modeCO2[i]
             << setw(16) << avg
             << endl;
    }

    printSeparator(width, '-');
    cout << "Total Emission for " << cityName << ": "
         << fixed << setprecision(2) << grandTotal << " kg CO2" << endl;
    printSeparator(width, '=');
    cout << endl;
}

//displays emissions summary per age group for one city
void displayTable1_ByAgeGroup(Node* head, const string& cityName) {
    AgeGroupSummary groups[total_age_groups];
    categorizeByAgeGroup(head, groups); //fill in all group summaries

    //print table header
    int width = 101;
    printSeparator(width, '=');
    cout << "Emissions by Age Group | City: " << cityName << endl;
    printSeparator(width, '=');
    cout << left
         << setw(38) << "Age Group"
         << setw(22) << "Preferred Mode"
         << setw(20) << "Total Emissions"
         << setw(20) << "Avg Emission/Resident"
         << endl;
    printSeparator(width, '-');

    //print one row per age group, skip groups with no residents
    for(int i = 0; i < total_age_groups; i++) {
        if(groups[i].totalResidents == 0) {
            continue; //skip empty age groups
        }
        cout << left
             << setw(38) << groups[i].groupLabel
             << setw(22) << groups[i].preferredTransportMode
             << setw(20) << fixed << setprecision(2) << groups[i].totalCO2
             << setw(20) << groups[i].averageCO2
             << endl;
    }

    printSeparator(width, '=');
    cout << endl;

    freeMemberLists(groups); //release all GroupMemberNode wrapper nodes
}

//displays total emissions per transport mode across all cities
void displayTable2_CombinedTransport(Node* headA, Node* headB, Node* headC) {
    //build one combined tally by processing all three city lists
    TransportCount combined;
    combined.totalModes = 0;

    //loop through all three city lists using an array of pointers
    Node* lists[3] = { headA, headB, headC };
    for(int l = 0; l < 3; l++) {
        Node* current = lists[l];
        while(current != NULL) {
            float co2 = calculateMonthlyCO2(current); //get this resident's monthly CO2
            addToTransportCount(combined, current->data.ModeOfTransport, co2); //merge into combined tally
            current = current->next;
        }
    }

    //print table header
    int width = 72;
    printSeparator(width, '=');
    cout << "Emissions by Transport Mode Across All Cities" << endl;
    printSeparator(width, '=');
    cout << left
         << setw(22) << "Mode"
         << setw(16) << "Resident Count"
         << setw(22) << "Total Emissions"
         << setw(15) << "Avg Emission"
         << endl;
    printSeparator(width, '-');

    //print one row per transport mode found across all cities
    for(int i = 0; i < combined.totalModes; i++) {
        float avg = 0.0f;
        if(combined.userCount[i] > 0) {
            avg = combined.modeCO2[i] / (float)combined.userCount[i]; //avg = total CO2 / total users of this mode
        }
        cout << left
             << setw(22) << combined.modeName[i]
             << setw(16) << combined.userCount[i]
             << setw(22) << fixed << setprecision(2) << combined.modeCO2[i]
             << setw(15) << avg
             << endl;
    }

    printSeparator(width, '=');
    cout << endl;
}

//compares total emissions and highest/lowest emission modes across cities
void displayTable3_CrossCity(Node* headA, Node* headB, Node* headC) {
    //store each city's list and name in arrays for easy looping
    Node*  lists[3] = { headA,    headB,    headC    };
    string names[3] = { "City A", "City B", "City C" };

    //print table header
    int width = 80;
    printSeparator(width, '=');
    cout << "Cross-City Comparison" << endl;
    printSeparator(width, '=');
    cout << left
         << setw(10) << "City"
         << setw(20) << "Total Emissions"
         << setw(30) << "Highest Emission Mode"
         << setw(30) << "Lowest Emission Mode"
         << endl;
    printSeparator(width, '-');

    //process each city one by one
    for(int l = 0; l < 3; l++) {
        //build transport tally for this city
        TransportCount tally;
        tally.totalModes = 0;

        float cityTotal = 0.0f;
        Node* current = lists[l];
        while(current != NULL) {
            float co2 = calculateMonthlyCO2(current); //get this resident's monthly CO2
            addToTransportCount(tally, current->data.ModeOfTransport, co2); //add to this city's tally
            cityTotal += co2; //accumulate city grand total
            current = current->next;
        }

        //find which mode produced the most and least CO2 in this city
        string highMode = "None";
        string lowMode  = "None";
        if(tally.totalModes > 0) {
            int hiIdx = 0;
            int loIdx = 0;
            for(int i = 1; i < tally.totalModes; i++) {
                if(tally.modeCO2[i] > tally.modeCO2[hiIdx]) {
                    hiIdx = i; //update highest
                }
                if(tally.modeCO2[i] < tally.modeCO2[loIdx]) {
                    loIdx = i; //update lowest
                }
            }
            highMode = tally.modeName[hiIdx];
            lowMode  = tally.modeName[loIdx];
        }

        //print this city's row
        cout << left
             << setw(10) << names[l]
             << setw(20) << fixed << setprecision(2) << cityTotal
             << setw(30) << highMode
             << setw(30) << lowMode
             << endl;
    }

    printSeparator(width, '=');
    cout << endl;
}

// enumerations, meaningful names to integer
// which column the user wants to sort by
enum SortField{sort_age, sort_distance, sort_carbem};

// which direction the user wants
enum SortOrder{sort_asc,sort_desc};

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

//sort and displays a city's resident list without changing the original list
void displaySortResult(Node* originalHead, const string& cityName, const string& algoName, SortField field, SortOrder order, bool useInsertion){
    Node* copy = copyList(originalHead); //make a copy so the original list stays unchanged

    if(useInsertion){
        insertionSort(copy, field, order); //sort the copy using insertion sort
    }else{
        bubbleSort(copy, field, order); //sort the copy using bubble sort
    }

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

    freeList(copy); //free the copied list after displaying
}

void sortingExperimentsMenu(Node* cityA, Node* cityB, Node* cityC){
    Node* lists[3] = {cityA, cityB, cityC};
    string names[3] = {"City A", "City B", "City C"};

    int choice;
    do{
        printSeparator(57, '-');
        cout<<"Sorting Experiments"<<endl;
        cout<<"\nSort field:"<<endl;
        cout<<"1. Age"<<endl;
        cout<<"2. Daily Distance"<<endl;
        cout<<"3. Carbon Emission Factor"<<endl;
        cout<<"4. Back to main menu"<<endl;
        printSeparator(57, '-');
        cout<<"Select field to sort by: ";
        cin>>choice;

        if(choice == 4){
            break;
        }

        SortField field;
        switch(choice){
            case 1:
                field = sort_age; break;
            case 2:
                field = sort_distance; break;
            case 3:
                field = sort_carbem; break;
            default:
                cout<<"Invalid Input. Please try again"<<endl;
                continue;
        }

        int orderChoice;
        cout<<"\nSort order:"<<endl;
        cout<<"1. Ascending"<<endl;
        cout<<"2. Descending"<<endl;
        cout<<"Select order: ";
        cin>>orderChoice;

        if(orderChoice < 1 || orderChoice > 2){
            cout<<"Invalid choice. Default to ascending"<<endl;
            orderChoice = 1;
        }
        SortOrder order = (orderChoice == 1) ? sort_asc : sort_desc; //convert choice to SortOrder enum

        int algoChoice;
        cout<<"\nAlgorithm:"<<endl;
        cout<<"1. Bubble Sort"<<endl;
        cout<<"2. Insertion Sort"<<endl;
        cout<<"Select algorithm: ";
        cin>>algoChoice;

        if(algoChoice < 1 || algoChoice > 2){
            cout<<"Invalid choice. Default to Bubble Sort"<<endl;
            algoChoice = 1;
        }

        int cityChoice;
        cout<<"\nDataset:"<<endl;
        cout<<"1. City A"<<endl;
        cout<<"2. City B"<<endl;
        cout<<"3. City C"<<endl;
        cout<<"4. All Cities"<<endl;
        cout<<"Select dataset: ";
        cin>>cityChoice;

        if(cityChoice < 1 || cityChoice > 4){
            cout<<"Invalid choice. Default to All Cities"<<endl;
            cityChoice = 4;
        }
        cout<<endl;

        //if all cities, loop index 0 to 2, otherwise convert cityChoice to the correct index (cityChoice-1)
        int start = (cityChoice == 4) ? 0 : cityChoice - 1;
        int end = (cityChoice == 4) ? 2 : cityChoice - 1;

        for(int i=start; i<=end; i++){ //loop through selected city or all cities 
            if(algoChoice == 1){
                displaySortResult(lists[i], names[i], "Bubble Sort", field, order, false);
            }
            if(algoChoice == 2){
                displaySortResult(lists[i], names[i], "Insertion Sort", field, order, true);
            }
        }
    }while(choice!=4);
    
}

void mainMenu(Node* cityA, Node* cityB, Node* cityC) {
    int choice;

    do {
        printSeparator(57, '=');
        cout << "Carbon Emission Analysis System\n";
        cout << "1. View raw data\n";
        cout << "2. View categorized data per city\n";
        cout << "3. View categorized data across all cities\n";
        cout << "4. View emissions per mode of transport\n";
        cout << "5. View emissions per age group comparison\n";
        cout << "6. View emissions by transport mode across all cities\n";
        cout << "7. View cross-city comparison\n";
        cout << "8. Sorting experiments\n";
        cout << "0. Exit\n";
        printSeparator(57, '=');
        cout << "Enter choice: ";
        cin >> choice;

        switch (choice) {
            case 1:
                cout << "\n==================================== Raw Data ====================================" << endl << endl;
                displayRawData(cityA, cityB, cityC);
                break;
            case 2:
                cout << "\n============================= Data by Age Categorization =========================" << endl << endl;
                displayCategorizedData(cityA, cityB, cityC);
                break;
            case 3:
                cout << "\n================ Data by Age Categorization (All Cities Combined) ================" << endl << endl;
                displayCategorizedDataAllCities(cityA, cityB, cityC);
                break;
            case 4: //carbon emissions per mode of transport, displayed per city
                cout << "\n================== Carbon Emissions Per Mode of Transport ==================" << endl << endl;
                displayEmissionsByTransportMode(cityA, "City A");
                displayEmissionsByTransportMode(cityB, "City B");
                displayEmissionsByTransportMode(cityC, "City C");
                break;
            case 5:
                //comparison tables across datasets and age groups
                cout << "\n========================================= Comparison Tables =========================================" << endl << endl;
                //per city breakdown by age group
                displayTable1_ByAgeGroup(cityA, "City A");
                displayTable1_ByAgeGroup(cityB, "City B");
                displayTable1_ByAgeGroup(cityC, "City C");
                break;
            case 6:
                //all cities merged, broken down by transport mode
                displayTable2_CombinedTransport(cityA, cityB, cityC);
                break;
            case 7:
                //one row per city showing totals and extreme modes
                displayTable3_CrossCity(cityA, cityB, cityC);
                break;
            case 8:
                //sorting algorithms
                sortingExperimentsMenu(cityA, cityB, cityC);
                break;
            case 0:
                cout<<"Exiting..."<<endl;
                break;
            default:
                cout<<"Invalid choice. Please try again"<<endl;
                break;
        }
    } while (choice != 0);
}

int main() {
    Node* cityA = NULL;
    Node* cityB = NULL;
    Node* cityC = NULL;

    loadData(cityA, "CityA.txt");
    loadData(cityB, "CityB.txt");
    loadData(cityC, "CityC.txt");

    mainMenu(cityA, cityB, cityC);

    //free all dynamically allocated nodes before exiting
    Node* lists[3] = { cityA, cityB, cityC };
    for(int l = 0; l < 3; l++) {
        Node* current = lists[l];
        while(current != NULL) {
            Node* temp = current;
            current = current->next;
            delete temp; //release memory for each node
        }
    }

    return 0;
}
