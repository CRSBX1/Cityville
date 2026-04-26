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
            cout<<endl;
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


//searching experiments
//converts strings to lowercase before comparing
string toLower(const string& inputStr){
    string lowerStr = inputStr;
    //iterate through each character in the string
    for(int i=0; i<(int)lowerStr.size(); i++){
        lowerStr[i] = tolower(lowerStr[i]); //convert character to lowercase
    }
    return lowerStr; 
}

void printSearchResults(Node* matchedHead, int matchCount, const string& label, const string& dataset){
    int width = 106;
    printSeparator(width, '=');
    cout << "Search Results | Criteria: " << label << " | Dataset: " << dataset << endl;
    printSeparator(width, '=');
    
    if(matchCount == 0){
        cout << "No residents matched the search criteria." << endl;
        printSeparator(width, '=');
        cout << endl;
        return;
    }

    cout << left
         << setw(14) << "Resident ID"
         << setw(8)  << "Age"
         << setw(20) << "Mode of Transport"
         << setw(20) << "Daily Dist (km)"
         << setw(14) << "CO2 Factor"
         << setw(14) << "Days/Month"
         << setw(16) << "Monthly CO2 (kg)"
         << endl;
    printSeparator(width, '-');

    //go through the matched results list and print each resident
    Node* cur = matchedHead;
    while(cur != NULL){
        float co2 = calculateMonthlyCO2(cur); //calculate this resident monthly CO2
        cout << left
             << setw(14) << cur->data.ResidentId
             << setw(8)  << cur->data.Age
             << setw(20) << cur->data.ModeOfTransport
             << setw(20) << cur->data.DailyDistance
             << setw(14) << fixed << setprecision(2) << cur->data.CarbonEmissionFactor
             << setw(14) << cur->data.AverageDayPerMonth
             << setw(16) << fixed << setprecision(2) << co2
             << endl;
        cur = cur->next; //move to the next matched resident
    }
     //print total match count
    printSeparator(width, '-');
    cout << "Total matches: " << matchCount << endl;
    printSeparator(width, '=');
    cout << endl;
}


//linear search by age group
void linearSearchByAgeGroup(Node* head, int minAge, int maxAge, Node*& matchedHead, int& matchCount){
    matchedHead = NULL; //initialize results list as empty
    matchCount = 0; //initialize results list as empty
    Node* current = head; //start scanning from first resident
    while(current != NULL){
        //check whether this resident's age is within the specified range
        if(current->data.Age >= minAge && current->data.Age <= maxAge){
            insert(matchedHead, current->data); //copy matching resident into results list
            matchCount++; //increment match counter
        }
        current = current->next; //move to the next resident in the list
    }
}

//linear seach by mode of transport
void linearSearchByTransport(Node* head, const string& targetMode, Node*& matchedHead, int& matchCount){
    matchedHead = NULL; //initialize results list as empty
    matchCount = 0; //initialize results list as empty

    string targetLower = toLower(targetMode); //change the target to lowercase for comparison

    Node* current = head; //start scanning from first resident
    while(current != NULL){
        //compare resident transport mode (lowercase) against the target
        if(toLower(current->data.ModeOfTransport) == targetLower){
            insert(matchedHead, current->data); //copy matching resident into results list
            matchCount++; //increment match counter
        }
        current = current->next; //move to the next resident in the list
    }
}

//linear search by daily distance
void linearSearchByDistance(Node* head, int threshold, char op, Node*& matchedHead, int& matchCount){
    matchedHead = NULL; //initialize results list as empty
    matchCount = 0; //initialize results list as empty

    Node* current = head; //start scanning from first resident
    while(current != NULL){
        bool match = false; //assume no match 
         
        //check distance against threshold using the specified operator
        if(op == '>' && current->data.DailyDistance > threshold){
            match = true; //resident distance exceeds threshold
        }else if(op == '<' && current->data.DailyDistance < threshold){
            match = true; //resident distance is below threshold
        }else if(op == '=' && current->data.DailyDistance == threshold){
            match = true; //resident distance equals to threshold
        }

        if(match){
            insert(matchedHead, current->data); //copy matching resident into results list
            matchCount++; //increment match counter
        }
        current = current->next; //move to the next resident in the list
    }
}

//combined linear search
void linearSearchCombined(Node* head, int ageGroup, const string& targetMode, int distThreshold, char distOp, Node*& matchedHead, int& matchCount){
    matchedHead = NULL; //initialize results list as empty
    matchCount = 0; //initialize results list as empty

    //match the selected age group to its range
    int minAge, maxAge;
    switch(ageGroup){
        case 1:
            minAge = 6; maxAge = 17; break; //children & teenagers
        case 2:
            minAge = 18; maxAge = 25; break; //university / young adults
        case 3:
            minAge = 26; maxAge = 45; break; //working adults early career
        case 4:
            minAge = 46; maxAge = 60; break; //working adults late career
        case 5: 
            minAge = 61; maxAge = 100; break; //senior citizens / retirees
        default:
            minAge = -1; maxAge = -1; break; //invalid group, filter will be skipped
    }
    bool filterAge = (ageGroup >= 1 && ageGroup <= 5); //age filter for valid group
    bool filterTransport = !targetMode.empty(); //transport filter for valid group
    bool filterDistance = (distOp == '>' || distOp == '<' || distOp == '='); //distance filter for valid group

    string targetLower = toLower(targetMode); //convert transport target to lowercase for comparison

    Node* current = head; //start scanning from first resident
    while(current != NULL){
        //each filter starts as passed and is only false if the resident fails the filter
        bool passAge = true;
        bool passTransport = true;
        bool passDistance = true;

        //apply age filter if it is valid
        if(filterAge){
            passAge = (current->data.Age >= minAge && current->data.Age <= maxAge);
        }
        //apply transport filter if it is valid
        if(filterTransport){
            passTransport = (toLower(current->data.ModeOfTransport) == targetLower);
        }
        //apply distance filter if it is valid
        if(filterDistance){
            if(distOp == '>' && !(current->data.DailyDistance > distThreshold)){
                passDistance = false;
            }else if(distOp == '<' && !(current->data.DailyDistance < distThreshold)){
                passDistance = false;
            }else if(distOp == '=' && !(current->data.DailyDistance == distThreshold)){
                passDistance = false;
            }       
        }
        //only include residents that passed every filter
        if(passAge && passTransport && passDistance){
            insert(matchedHead, current->data); //copy matching resident into results list
            matchCount++; //increment match counter
        }
        current = current->next; //move to the next resident in the list
    }
}

//binary search
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

//binary search by age
void binarySearchByAgeGroup(Node* head, int minAge, int maxAge, Node*& matchedHead, int& matchCount){
    matchedHead = NULL; //initialize results list as empty
    matchCount = 0; //initialize match counter to 0

    int size = 0;
    Resident* arr = listToArray(head, size); //convert linked list to array
    if(!arr){
        return; //for empty list, nothing to search
    }

    arraySortInt(arr, size, true); //sort array by age in ascending 

    //binary search to find the first index where age >= minAge
    int lo = 0, hi = size -1;
    int startIdx = size; //set default to array size, meaning no matching resident has been found yet
    while(lo <= hi){
        int mid = lo + (hi - lo) / 2; //calculate the middle index to avoid integer overflow
        if(arr[mid].Age >= minAge){
            //if resident qualifies, save this position for startIdx, 
            //but keep searching leftward in case earlier resident also qualifies
            startIdx = mid;
            hi = mid-1; //move the right pointer leftward to search the left half
        }else{
            //the resident is too young, all residents to the left are also too young
            //discard the entire left half and search the right half instead
            lo = mid+1; //move left pointer rightward
        }
    }
    //after the loop, startIdx will hold the first index where Age >= minAge

    //walk rightward from startIdx and collect every resident whose age is within range
    for(int i=startIdx; i<size && arr[i].Age <= maxAge; i++){
        insert(matchedHead, arr[i]); //add qualified resident to results list
        matchCount++; //increment match counter
    }
    delete[] arr; //free temporary array from memory
}

//binary search by mode of transport
void binarySearchByTransport(Node* head, const string& targetMode, Node*& matchedHead, int& matchCount){
    matchedHead = NULL; //initialize results list as empty
    matchCount = 0; //initialize match counter to 0

    int size = 0;
    Resident *arr = listToArray(head, size); //convert linked list to array
    if(!arr){
        return; //for empty list, nothing to search
    }

    //sort array alphabetically using insertion sort, toLower is also applied for fair comparison
    for(int i=1; i<size; i++){
        Resident key = arr[i];
        string keyMode = toLower(key.ModeOfTransport);
        int j = i-1;
        //shift residents rightward whose transport mode comes after the piked mode alphabetically
        while(j >= 0 && toLower(arr[j].ModeOfTransport) > keyMode){
            arr[j+1] = arr[j]; //slide this resident one space to the right
            j--;
        }
        arr[j+1] = key; //place picked resident in their correct alphabetical position
    } 

    string targetLower = toLower(targetMode); //convert search target to lowercase for consistent comparison

    //locate any one resident using the target transport mode
    int lo = 0, hi = size-1, foundIdx = -1;
    while(lo<=hi){
        int mid = lo + (hi - lo) / 2;
        string midMode = toLower(arr[mid].ModeOfTransport); //get this resident mode in lowercase
        if(midMode == targetLower){
            foundIdx = mid; //found one match, save the index
            break; //stop searching, expansion will find the rest
        }else if(midMode < targetLower){
            //middle mode comes before target alphabetically (search right half)
            lo = mid + 1;
        }else{
            //middle mode comes after target alphabetically (search left half)
            hi = mid - 1;
        }
    }
    
    //if foundIdx = -1, no resident uses the target transport mode
    if(foundIdx == -1){
        delete[] arr; //free array before exiting
        return; //results list remains empty
    }
    //move leftward from foundIdx to find the first resident using this mode
    //stops when the resident to the left uses a different transport mode
    int left = foundIdx;
    while(left > 0 && toLower(arr[left-1].ModeOfTransport) == targetLower){
        left--; //step one position to the left
    }
    //move rightward from foundIdx to find the last resident using this mode
    //stops when the resident to the right uses a different transport mode
    int right = foundIdx;
    while(right < size-1 && toLower(arr[right+1].ModeOfTransport) == targetLower){
        right++; //step one position to the right
    }
    //now left and right mark the exact boundaries of all residents using this transport mode

    //get all residents between left and right boundaries into the results list
    for(int i=left; i<=right; i++){
        insert(matchedHead, arr[i]); //add this resident to results list
        matchCount++; //increment match counter
    }

    delete[] arr; //free temporary array from memory
}

//binary search by daily distance
void binarySearchByDistance(Node* head, int target, Node*& matchedHead, int& matchCount){
    matchedHead = NULL; //initialize results list as empty
    matchCount = 0; //initialize match counter to 0

    int size = 0;
    Resident *arr = listToArray(head, size); //convert linked list to array
    if(!arr){
        return; //for empty list, nothing to search
    }

    arraySortInt(arr, size, false); //sort array by daily distance in ascending

    int lo = 0, hi = size-1, foundIdx = -1;
    while(lo<=hi){
        int mid = lo + (hi - lo) / 2; //calculate middle index
        if(arr[mid].DailyDistance == target){
            foundIdx = mid; //found one match, save the index
            break; //stop searching, expansion will find the rest
        }else if(arr[mid].DailyDistance < target){
            //middle resident distance is smaller than target, discard all resident to the left
            lo = mid + 1;
        }else{
            //middle resident distance is larger than target, discard all resident to the right
            hi = mid - 1;
        }
    }

    if(foundIdx == -1){
        delete[] arr;
        return;
    }

    //move leftward from foundIdx to find the first resident using this distance
    //stops when the resident to the left has a different distance value
    int left = foundIdx;
    while(left > 0 && arr[left-1].DailyDistance == target){
        left--;
    }
    //move rightward from foundIdx to find the last resident using this distance
    //stops when the resident to the right has a different distance value
    int right = foundIdx;
    while(right < size-1 && arr[right+1].DailyDistance == target){
        right++;
    }
    //get all residents between left and right boundaries into the results list
    for(int i=left; i<=right; i++){
        insert(matchedHead, arr[i]); //add this resident to results list
        matchCount++; //increment match counter
    }

    delete[] arr; //free temporary array from memory
}


//tree node for the transport-based tree used by BFS and DFS
//tree structure: Root -> Transport Modes -> Age Groups -> Distance Ranges
//each node can hold resident data at any level for filtering
const int max_tree_children = 20; //max children per tree node

struct TreeNode {
    string label; //display label for this node (e.g. "Car", "26-45", "12-18 km")
    int level; //0=root, 1=transport mode, 2=age group, 3=distance range
    Node* residentsHead; //linked list of residents that belong to this node (leaf level)
    int residentCount; //number of residents at this node
    TreeNode* children[max_tree_children]; //child nodes
    int childCount; //number of children
};

//creates a new tree node with the given label and level
TreeNode* createTreeNode(const string& label, int level) {
    TreeNode* node = new TreeNode();
    node->label = label;
    node->level = level;
    node->residentsHead = NULL;
    node->residentCount = 0;
    node->childCount = 0;
    for(int i = 0; i < max_tree_children; i++) {
        node->children[i] = NULL;
    }
    return node;
}

//finds a child node by label, returns NULL if not found
TreeNode* findChild(TreeNode* parent, const string& label) {
    for(int i = 0; i < parent->childCount; i++) {
        if(parent->children[i]->label == label) {
            return parent->children[i]; //found matching child
        }
    }
    return NULL; //no child with this label
}

//adds a child node to a parent, returns the new child
TreeNode* addChild(TreeNode* parent, const string& label, int level) {
    TreeNode* existing = findChild(parent, label);
    if(existing != NULL) {
        return existing; //child already exists, reuse it
    }
    if(parent->childCount >= max_tree_children) {
        return NULL; //cannot add more children
    }
    TreeNode* child = createTreeNode(label, level);
    parent->children[parent->childCount] = child;
    parent->childCount++;
    return child;
}

//returns the age group label string for a given age (used as level 2 node label)
string getAgeRangeLabel(int age) {
    if(age >= 6  && age <= 17)  return "6-17";
    if(age >= 18 && age <= 25)  return "18-25";
    if(age >= 26 && age <= 35)  return "26-35";
    if(age >= 36 && age <= 49)  return "36-49";
    if(age >= 50 && age <= 58)  return "50-58";
    if(age >= 59 && age <= 100) return "59-100";
    return "Unknown";
}

//returns a distance range label for a given daily distance
string getDistanceRangeLabel(int dist) {
    if(dist <= 5)  return "1-5 km";
    if(dist <= 10) return "6-10 km";
    if(dist <= 15) return "11-15 km";
    if(dist <= 20) return "16-20 km";
    if(dist <= 25) return "21-25 km";
    return "26+ km";
}

//builds the transport tree from a linked list of residents
//tree: Root -> Transport Mode -> Age Range -> Distance Range (with residents at leaves)
TreeNode* buildTransportTree(Node* head, const string& rootLabel) {
    TreeNode* root = createTreeNode(rootLabel, 0); //level 0 = root

    Node* current = head;
    while(current != NULL) {
        //level 1: transport mode
        TreeNode* modeNode = addChild(root, current->data.ModeOfTransport, 1);

        //level 2: age range within this transport mode
        string ageLabel = getAgeRangeLabel(current->data.Age);
        TreeNode* ageNode = addChild(modeNode, ageLabel, 2);

        //level 3: distance range within this age group (leaf node)
        string distLabel = getDistanceRangeLabel(current->data.DailyDistance);
        TreeNode* distNode = addChild(ageNode, distLabel, 3);

        //attach resident to the leaf node
        Node* resCopy = new Node();
        resCopy->data = current->data;
        resCopy->next = distNode->residentsHead;
        distNode->residentsHead = resCopy;
        distNode->residentCount++;

        current = current->next; //move to next resident
    }
    return root;
}

//frees all nodes in the transport tree recursively
void freeTree(TreeNode* node) {
    if(node == NULL) return;
    //free all children first
    for(int i = 0; i < node->childCount; i++) {
        freeTree(node->children[i]);
    }
    //free the resident list at this node
    Node* cur = node->residentsHead;
    while(cur != NULL) {
        Node* temp = cur;
        cur = cur->next;
        delete temp;
    }
    delete node; //free the tree node itself
}

//checks whether a single resident passes all active filters
//filters are: age group, transport mode, and daily distance threshold
bool passesFilters(const Resident& res, int ageGroup, const string& targetLower, int distThreshold, char distOp) {
    bool filterAge = (ageGroup >= 1 && ageGroup <= 5); //age filter active for valid group
    bool filterTransport = !targetLower.empty(); //transport filter active if user entered a mode
    bool filterDistance = (distOp == '>' || distOp == '<' || distOp == '='); //distance filter active for valid operator

    bool passAge = true;
    bool passTransport = true;
    bool passDistance = true;

    //apply age filter if active
    if(filterAge) {
        int minAge, maxAge;
        switch(ageGroup) {
            case 1: minAge = 6;  maxAge = 17;  break; //children & teenagers
            case 2: minAge = 18; maxAge = 25;  break; //university / young adults
            case 3: minAge = 26; maxAge = 45;  break; //working adults early career
            case 4: minAge = 46; maxAge = 60;  break; //working adults late career
            case 5: minAge = 61; maxAge = 100; break; //senior citizens / retirees
            default: minAge = -1; maxAge = -1;  break;
        }
        passAge = (res.Age >= minAge && res.Age <= maxAge);
    }
    //apply transport filter if active
    if(filterTransport) {
        passTransport = (toLower(res.ModeOfTransport) == targetLower);
    }
    //apply distance filter if active
    if(filterDistance) {
        if(distOp == '>' && !(res.DailyDistance > distThreshold)) {
            passDistance = false;
        } else if(distOp == '<' && !(res.DailyDistance < distThreshold)) {
            passDistance = false;
        } else if(distOp == '=' && !(res.DailyDistance == distThreshold)) {
            passDistance = false;
        }
    }

    return passAge && passTransport && passDistance; //resident must pass every active filter
}

//simple pointer-based queue for BFS traversal over tree nodes
struct TreeQueueNode {
    TreeNode* treePtr; //pointer to the tree node
    TreeQueueNode* next;
};

struct TreeQueue {
    TreeQueueNode* front;
    TreeQueueNode* rear;
};

//initializes tree queue to empty state
void initTreeQueue(TreeQueue& q) {
    q.front = NULL;
    q.rear = NULL;
}

//returns true if tree queue has no elements
bool isTreeQueueEmpty(const TreeQueue& q) {
    return q.front == NULL;
}

//adds a tree node to the back of the queue
void enqueueTree(TreeQueue& q, TreeNode* node) {
    TreeQueueNode* newNode = new TreeQueueNode();
    newNode->treePtr = node;
    newNode->next = NULL;
    if(q.rear == NULL) {
        q.front = newNode;
        q.rear = newNode;
    } else {
        q.rear->next = newNode;
        q.rear = newNode;
    }
}

//removes and returns the tree node at the front of the queue
TreeNode* dequeueTree(TreeQueue& q) {
    if(q.front == NULL) return NULL;
    TreeQueueNode* temp = q.front;
    TreeNode* result = temp->treePtr;
    q.front = q.front->next;
    if(q.front == NULL) q.rear = NULL;
    delete temp;
    return result;
}

//returns indentation string based on tree level
string indent(int level) {
    string s = "";
    for(int i = 0; i < level; i++) {
        s += "  ";
    }
    return s;
}

//BFS traversal over the transport tree
//visits nodes level by level: Root -> all transport modes -> all age groups -> all distance ranges
//collects residents at leaf nodes that pass the search filters
void bfsSearch(Node* head, const string& rootLabel, int ageGroup, const string& targetMode, int distThreshold, char distOp, Node*& matchedHead, int& matchCount) {
    matchedHead = NULL; //initialize results list as empty
    matchCount = 0; //initialize match counter to 0

    //build the transport tree from the resident linked list
    TreeNode* root = buildTransportTree(head, rootLabel);

    string targetLower = toLower(targetMode); //convert target to lowercase for comparison

    //initialize BFS queue and enqueue the root
    TreeQueue q;
    initTreeQueue(q);
    enqueueTree(q, root);

    cout << "\nBFS traversal order (level by level):" << endl;
    int step = 1;
    bool firstFound = false; //tracks whether the first match has been found

    //process nodes level by level
    while(!isTreeQueueEmpty(q)) {
        TreeNode* current = dequeueTree(q); //get the next node to visit

        //only print traversal steps until the first match is found
        if(!firstFound) {
            cout << indent(current->level) << step << ". [L" << current->level << "] " << current->label;
            if(current->residentCount > 0) {
                cout << " (" << current->residentCount << " residents)";
            }
            if(current->childCount > 0) {
                cout << " [" << current->childCount << " children]";
            }
            cout << endl;
        }
        step++;

        //collect matching residents at leaf nodes (level 3 = distance range)
        Node* resCur = current->residentsHead;
        while(resCur != NULL) {
            if(passesFilters(resCur->data, ageGroup, targetLower, distThreshold, distOp)) {
                insert(matchedHead, resCur->data); //copy matching resident into results list
                matchCount++; //increment match counter
                if(!firstFound) {
                    cout << indent(current->level) << "   >> First match found at step " << (step - 1) << endl;
                    firstFound = true; //stop printing traversal from here
                }
            }
            resCur = resCur->next;
        }

        //enqueue all children of the current node
        for(int i = 0; i < current->childCount; i++) {
            enqueueTree(q, current->children[i]);
        }
    }
    cout << "Total nodes visited: " << (step - 1) << endl;

    freeTree(root); //free the entire tree after traversal
}

//DFS recursive helper for the transport tree
//visits current node, collects matching residents, then recurses into children
//only prints traversal steps until the first match is found
void dfsTreeHelper(TreeNode* current, int ageGroup, const string& targetLower, int distThreshold, char distOp, Node*& matchedHead, int& matchCount, int& step, bool& firstFound) {
    //only print traversal steps until the first match is found
    if(!firstFound) {
        cout << indent(current->level) << step << ". [L" << current->level << "] " << current->label;
        if(current->residentCount > 0) {
            cout << " (" << current->residentCount << " residents)";
        }
        if(current->childCount > 0) {
            cout << " [" << current->childCount << " children]";
        }
        cout << endl;
    }
    step++;

    //collect matching residents at this node
    Node* resCur = current->residentsHead;
    while(resCur != NULL) {
        if(passesFilters(resCur->data, ageGroup, targetLower, distThreshold, distOp)) {
            insert(matchedHead, resCur->data); //copy matching resident into results list
            matchCount++; //increment match counter
            if(!firstFound) {
                cout << indent(current->level) << "   >> First match found at step " << (step - 1) << endl;
                firstFound = true; //stop printing traversal from here
            }
        }
        resCur = resCur->next;
    }

    //recurse into all children (depth-first)
    for(int i = 0; i < current->childCount; i++) {
        dfsTreeHelper(current->children[i], ageGroup, targetLower, distThreshold, distOp, matchedHead, matchCount, step, firstFound);
    }
}

//DFS traversal over the transport tree
//visits nodes depth-first: Root -> Car -> Car/26-35 -> 12-18km -> Car/36-49 -> ...
//collects residents at leaf nodes that pass the search filters
void dfsSearch(Node* head, const string& rootLabel, int ageGroup, const string& targetMode, int distThreshold, char distOp, Node*& matchedHead, int& matchCount) {
    matchedHead = NULL; //initialize results list as empty
    matchCount = 0; //initialize match counter to 0

    //build the transport tree from the resident linked list
    TreeNode* root = buildTransportTree(head, rootLabel);

    string targetLower = toLower(targetMode); //convert target to lowercase for comparison

    cout << "\nDFS traversal order (depth-first):" << endl;
    int step = 1;
    bool firstFound = false; //tracks whether the first match has been found
    dfsTreeHelper(root, ageGroup, targetLower, distThreshold, distOp, matchedHead, matchCount, step, firstFound);
    cout << "Total nodes visited: " << (step - 1) << endl;

    freeTree(root); //free the entire tree after traversal
}

//menu for BFS and DFS search experiments
//prompts user to select a city dataset and returns the 0-based index
int selectCity() {
    int cityChoice;
    cout << "\nDataset:" << endl;
    cout << "1. City A" << endl;
    cout << "2. City B" << endl;
    cout << "3. City C" << endl;
    cout << "Select dataset: ";
    cin >> cityChoice;
    if(cityChoice < 1 || cityChoice > 3) {
        cout << "Invalid choice. Default to City A" << endl;
        cityChoice = 1;
    }
    return cityChoice - 1; //convert to 0-based index
}

//prompts user to select search criteria and stores the selections
//all three filters are optional; user can enter 0 or press Enter to skip
void selectSearchCriteria(int& ageGroup, string& targetMode, int& distThreshold, char& distOp) {
    //filter 1: age group (0 = skip filter)
    cout << "\nFilter by age group (0 to skip):" << endl;
    cout << "1. Children & Teenagers        (6-17)" << endl;
    cout << "2. University / Young Adults   (18-25)" << endl;
    cout << "3. Working Adults Early Career (26-45)" << endl;
    cout << "4. Working Adults Late Career  (46-60)" << endl;
    cout << "5. Senior Citizens / Retirees  (61-100)" << endl;
    cout << "Select age group filter (0 to skip): ";
    cin >> ageGroup;
    if(ageGroup < 0 || ageGroup > 5) {
        cout << "Invalid choice. Skipping age group filter" << endl;
        ageGroup = 0; //disable age filter
    }

    //filter 2: transport mode (empty = skip filter)
    cout << "\nEnter transport mode to filter (press Enter to skip): ";
    cin.ignore(); //clear leftover newline from previous cin
    getline(cin, targetMode);

    //filter 3: daily distance threshold (0 with no operator = skip filter)
    cout << "\nFilter by daily distance:" << endl;
    cout << "Operator (>, <, = or 0 to skip): ";
    cin >> distOp;

    distThreshold = 0;
    if(distOp == '>' || distOp == '<' || distOp == '=') {
        cout << "Enter distance threshold: ";
        cin >> distThreshold;
    } else {
        distOp = '0'; //mark as inactive so filters skip distance check
    }
}

//builds a descriptive label string showing which filters were active
string buildSearchLabel(const string& algoName, int ageGroup, const string& targetMode, int distThreshold, char distOp) {
    string label = algoName;
    if(ageGroup >= 1 && ageGroup <= 5) {
        label += " | Age Group = " + getAgeGroupLabel(ageGroup - 1);
    }
    if(!targetMode.empty()) {
        label += " | Mode = " + targetMode;
    }
    if(distOp == '>' || distOp == '<' || distOp == '=') {
        label += " | Distance ";
        label += distOp;
        label += " ";
        stringstream ss;
        ss << distThreshold;
        label += ss.str();
    }
    return label;
}

//sub-menu for linear search
void linearSearchMenu(Node* cityA, Node* cityB, Node* cityC) {
    Node* lists[3] = {cityA, cityB, cityC};
    string names[3] = {"City A", "City B", "City C"};

    int choice;
    do {
        printSeparator(57, '-');
        cout << "Linear Search" << endl;
        cout << "\n1. Search by age group" << endl;
        cout << "2. Search by mode of transport" << endl;
        cout << "3. Search by daily distance" << endl;
        cout << "4. Combined search (all criteria)" << endl;
        cout << "5. Back" << endl;
        printSeparator(57, '-');
        cout << "Select search type: ";
        cin >> choice;

        if(choice == 5) {
            cout << endl;
            break;
        }
        if(choice < 1 || choice > 4) {
            cout << "Invalid choice. Please try again" << endl;
            continue;
        }

        int idx = selectCity(); //ask user which city to search

        Node* matchedHead = NULL;
        int matchCount = 0;
        string label;

        if(choice == 1) {
            //search by age group
            cout << "\nSelect age group:" << endl;
            cout << "1. Children & Teenagers        (6-17)" << endl;
            cout << "2. University / Young Adults   (18-25)" << endl;
            cout << "3. Working Adults Early Career (26-45)" << endl;
            cout << "4. Working Adults Late Career  (46-60)" << endl;
            cout << "5. Senior Citizens / Retirees  (61-100)" << endl;
            cout << "Select group: ";
            int grp;
            cin >> grp;

            int minAge, maxAge;
            switch(grp) {
                case 1: minAge = 6;  maxAge = 17;  break;
                case 2: minAge = 18; maxAge = 25;  break;
                case 3: minAge = 26; maxAge = 45;  break;
                case 4: minAge = 46; maxAge = 60;  break;
                case 5: minAge = 61; maxAge = 100; break;
                default:
                    cout << "Invalid group. Default to group 1" << endl;
                    minAge = 6; maxAge = 17; grp = 1; break;
            }
            linearSearchByAgeGroup(lists[idx], minAge, maxAge, matchedHead, matchCount);
            label = "Linear | Age Group = " + getAgeGroupLabel(grp - 1);

        } else if(choice == 2) {
            //search by transport mode
            cout << "\nEnter transport mode to search: ";
            cin.ignore();
            string mode;
            getline(cin, mode);
            linearSearchByTransport(lists[idx], mode, matchedHead, matchCount);
            label = "Linear | Mode = " + mode;

        } else if(choice == 3) {
            //search by daily distance
            cout << "\nOperator (>, <, =): ";
            char op;
            cin >> op;
            if(op != '>' && op != '<' && op != '=') {
                cout << "Invalid operator. Default to >" << endl;
                op = '>';
            }
            cout << "Enter distance threshold: ";
            int threshold;
            cin >> threshold;
            linearSearchByDistance(lists[idx], threshold, op, matchedHead, matchCount);
            stringstream ss;
            ss << threshold;
            label = "Linear | Distance ";
            label += op;
            label += " " + ss.str();

        } else if(choice == 4) {
            //combined search using all three criteria
            int ageGroup;
            string targetMode;
            int distThreshold;
            char distOp;
            selectSearchCriteria(ageGroup, targetMode, distThreshold, distOp);
            linearSearchCombined(lists[idx], ageGroup, targetMode, distThreshold, distOp, matchedHead, matchCount);
            label = buildSearchLabel("Linear Combined", ageGroup, targetMode, distThreshold, distOp);
        }

        printSearchResults(matchedHead, matchCount, label, names[idx]);
        freeList(matchedHead); //free the results list after displaying

    } while(choice != 5);
}

//sub-menu for binary search
void binarySearchMenu(Node* cityA, Node* cityB, Node* cityC) {
    Node* lists[3] = {cityA, cityB, cityC};
    string names[3] = {"City A", "City B", "City C"};

    int choice;
    do {
        printSeparator(57, '-');
        cout << "Binary Search" << endl;
        cout << "\n1. Search by age group" << endl;
        cout << "2. Search by mode of transport" << endl;
        cout << "3. Search by daily distance (exact match)" << endl;
        cout << "4. Back" << endl;
        printSeparator(57, '-');
        cout << "Select search type: ";
        cin >> choice;

        if(choice == 4) {
            cout << endl;
            break;
        }
        if(choice < 1 || choice > 3) {
            cout << "Invalid choice. Please try again" << endl;
            continue;
        }

        int idx = selectCity(); //ask user which city to search

        Node* matchedHead = NULL;
        int matchCount = 0;
        string label;

        if(choice == 1) {
            //binary search by age group
            cout << "\nSelect age group:" << endl;
            cout << "1. Children & Teenagers        (6-17)" << endl;
            cout << "2. University / Young Adults   (18-25)" << endl;
            cout << "3. Working Adults Early Career (26-45)" << endl;
            cout << "4. Working Adults Late Career  (46-60)" << endl;
            cout << "5. Senior Citizens / Retirees  (61-100)" << endl;
            cout << "Select group: ";
            int grp;
            cin >> grp;

            int minAge, maxAge;
            switch(grp) {
                case 1: minAge = 6;  maxAge = 17;  break;
                case 2: minAge = 18; maxAge = 25;  break;
                case 3: minAge = 26; maxAge = 45;  break;
                case 4: minAge = 46; maxAge = 60;  break;
                case 5: minAge = 61; maxAge = 100; break;
                default:
                    cout << "Invalid group. Default to group 1" << endl;
                    minAge = 6; maxAge = 17; grp = 1; break;
            }
            binarySearchByAgeGroup(lists[idx], minAge, maxAge, matchedHead, matchCount);
            label = "Binary | Age Group = " + getAgeGroupLabel(grp - 1);

        } else if(choice == 2) {
            //binary search by transport mode
            cout << "\nEnter transport mode to search: ";
            cin.ignore();
            string mode;
            getline(cin, mode);
            binarySearchByTransport(lists[idx], mode, matchedHead, matchCount);
            label = "Binary | Mode = " + mode;

        } else if(choice == 3) {
            //binary search by exact daily distance
            cout << "\nEnter exact daily distance to search: ";
            int target;
            cin >> target;
            binarySearchByDistance(lists[idx], target, matchedHead, matchCount);
            stringstream ss;
            ss << target;
            label = "Binary | Distance = " + ss.str();
        }

        printSearchResults(matchedHead, matchCount, label, names[idx]);
        freeList(matchedHead); //free the results list after displaying

    } while(choice != 4);
}

//sub-menu for BFS search
void bfsSearchMenu(Node* cityA, Node* cityB, Node* cityC) {
    Node* lists[3] = {cityA, cityB, cityC};
    string names[3] = {"City A", "City B", "City C"};

    printSeparator(57, '-');
    cout << "BFS Tree Search" << endl;
    cout << "\nDataset:" << endl;
    cout << "1. City A" << endl;
    cout << "2. City B" << endl;
    cout << "3. City C" << endl;
    cout << "4. All Cities (Combined)" << endl;
    cout << "5. Back" << endl;
    printSeparator(57, '-');
    cout << "Select dataset: ";
    int cityChoice;
    cin >> cityChoice;

    if(cityChoice == 5) {
        cout << endl;
        return; //go back to searching menu
    }
    if(cityChoice < 1 || cityChoice > 4) {
        cout << "Invalid choice. Default to City A" << endl;
        cityChoice = 1;
    }

    //collect all three search filters from user
    int ageGroup;
    string targetMode;
    int distThreshold;
    char distOp;
    selectSearchCriteria(ageGroup, targetMode, distThreshold, distOp);

    Node* matchedHead = NULL;
    int matchCount = 0;

    if(cityChoice == 4) {
        //build a combined linked list from all three cities
        Node* combined = NULL;
        for(int l = 0; l < 3; l++) {
            Node* cur = lists[l];
            while(cur != NULL) {
                insert(combined, cur->data); //copy each resident into combined list
                cur = cur->next;
            }
        }
        bfsSearch(combined, "All Cities", ageGroup, targetMode, distThreshold, distOp, matchedHead, matchCount);
        string label = buildSearchLabel("BFS", ageGroup, targetMode, distThreshold, distOp);
        printSearchResults(matchedHead, matchCount, label, "All Cities");
        freeList(combined); //free the combined list after searching
    } else {
        int idx = cityChoice - 1; //convert to 0-based index
        bfsSearch(lists[idx], names[idx], ageGroup, targetMode, distThreshold, distOp, matchedHead, matchCount);
        string label = buildSearchLabel("BFS", ageGroup, targetMode, distThreshold, distOp);
        printSearchResults(matchedHead, matchCount, label, names[idx]);
    }

    freeList(matchedHead); //free the results list after displaying
}

//sub-menu for DFS search
void dfsSearchMenu(Node* cityA, Node* cityB, Node* cityC) {
    Node* lists[3] = {cityA, cityB, cityC};
    string names[3] = {"City A", "City B", "City C"};

    printSeparator(57, '-');
    cout << "DFS Tree Search" << endl;
    cout << "\nDataset:" << endl;
    cout << "1. City A" << endl;
    cout << "2. City B" << endl;
    cout << "3. City C" << endl;
    cout << "4. All Cities (Combined)" << endl;
    cout << "5. Back" << endl;
    printSeparator(57, '-');
    cout << "Select dataset: ";
    int cityChoice;
    cin >> cityChoice;

    if(cityChoice == 5) {
        cout << endl;
        return; //go back to searching menu
    }
    if(cityChoice < 1 || cityChoice > 4) {
        cout << "Invalid choice. Default to City A" << endl;
        cityChoice = 1;
    }

    //collect all three search filters from user
    int ageGroup;
    string targetMode;
    int distThreshold;
    char distOp;
    selectSearchCriteria(ageGroup, targetMode, distThreshold, distOp);

    Node* matchedHead = NULL;
    int matchCount = 0;

    if(cityChoice == 4) {
        //build a combined linked list from all three cities
        Node* combined = NULL;
        for(int l = 0; l < 3; l++) {
            Node* cur = lists[l];
            while(cur != NULL) {
                insert(combined, cur->data); //copy each resident into combined list
                cur = cur->next;
            }
        }
        dfsSearch(combined, "All Cities", ageGroup, targetMode, distThreshold, distOp, matchedHead, matchCount);
        string label = buildSearchLabel("DFS", ageGroup, targetMode, distThreshold, distOp);
        printSearchResults(matchedHead, matchCount, label, "All Cities");
        freeList(combined); //free the combined list after searching
    } else {
        int idx = cityChoice - 1; //convert to 0-based index
        dfsSearch(lists[idx], names[idx], ageGroup, targetMode, distThreshold, distOp, matchedHead, matchCount);
        string label = buildSearchLabel("DFS", ageGroup, targetMode, distThreshold, distOp);
        printSearchResults(matchedHead, matchCount, label, names[idx]);
    }

    freeList(matchedHead); //free the results list after displaying
}

//top-level searching experiments menu with sub-menus for each algorithm
void searchingMenu(Node* cityA, Node* cityB, Node* cityC) {
    int choice;
    do {
        printSeparator(57, '-');
        cout << "Searching Experiments" << endl;
        cout << "\n1. Linear Search" << endl;
        cout << "2. Binary Search" << endl;
        cout << "3. BFS Tree Search" << endl;
        cout << "4. DFS Tree Search" << endl;
        cout << "5. Back to main menu" << endl;
        printSeparator(57, '-');
        cout << "Select search algorithm: ";
        cin >> choice;

        switch(choice) {
            case 1:
                linearSearchMenu(cityA, cityB, cityC);
                break;
            case 2:
                binarySearchMenu(cityA, cityB, cityC);
                break;
            case 3:
                bfsSearchMenu(cityA, cityB, cityC);
                break;
            case 4:
                dfsSearchMenu(cityA, cityB, cityC);
                break;
            case 5:
                cout << endl;
                break;
            default:
                cout << "Invalid choice. Please try again" << endl;
                break;
        }
    } while(choice != 5);
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
        cout << "9. Searching experiments\n";
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
            case 9:
                //searching algorithms (linear, binary, BFS, DFS)
                searchingMenu(cityA, cityB, cityC);
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