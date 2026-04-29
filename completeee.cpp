#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <chrono> //for high-resolution execution time measurement
using namespace std;


//CONSTANT
const int total_age_groups = 5; //total number of age groups
const int max_transport_types = 15; //max number of different transport mode per group


//DATA STRUCTURE
//stores one person's data
struct Resident {
    string ResidentId;
    int Age;
    string ModeOfTransport;
    int DailyDistance;
    float CarbonEmissionFactor;
    int AverageDayPerMonth;
};

//wraps a resident into a linked list node 
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

//a lightweight node that links a resident into a group list without copying or moving original data
//to build per age group lists without wasting memory
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


//FUNCTION
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
        if(groupSummary[i].totalResidents > 0) { //loop through the groups and calculate
            //avg CO2 = total CO2 for this group / number of residents in this group
            groupSummary[i].averageCO2 = groupSummary[i].totalCO2 / (float)groupSummary[i].totalResidents;
            if(grandTotalCO2 > 0.0f) {
                //percentage of total emissions = this group's CO2 : all groups' CO2 combined x 100
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

//time complexity record
//used by all sorting and searching experiments to measure execution time
//startTimer() captures the current time, stopTimer() returns elapsed milliseconds
chrono::high_resolution_clock::time_point startTimer() {
    return chrono::high_resolution_clock::now();
}
//calculates elapsed time in milliseconds since the given start time
//call this right after the operation finishes
double stopTimer(chrono::high_resolution_clock::time_point start) {
    auto end = chrono::high_resolution_clock::now();
    return chrono::duration<double, milli>(end - start).count();
}


//DISPLAY FUNCTION
//prints a horizontal separator line
void printSeparator(int width, char c) {
    for(int i = 0; i < width; i++) {
        cout << c;
    }
    cout << endl;
}

//prints execution time and memory usage after an operation completes
//elapsedMs – wall-clock milliseconds the operation took
//memKB     – estimated memory used, calculated as nodeCount * sizeof(Node) / 1024
void printPerformance(double elapsedMs) {
    printSeparator(50, '-');
    cout << fixed << setprecision(6);
    cout << "  Execution Time : " << elapsedMs << " ms" << endl;
    printSeparator(50, '-');
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


//SORTING
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

void displaySortResult(Node* originalHead, const string& cityName, const string& algoName, SortField field, SortOrder order, bool useInsertion){
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
    printPerformance(elapsedMs);

    freeList(copy); //free the copied list after displaying
}


//SEARCHING
//converts strings to lowercase before comparing
string toLower(const string& inputStr){
    string lowerStr = inputStr;
    //iterate through each character in the string
    for(int i=0; i<(int)lowerStr.size(); i++){
        lowerStr[i] = tolower(lowerStr[i]); //convert character to lowercase
    }
    return lowerStr; 
}

//for age, transport, distance
void printSearchResults(Node* matchedHead, int matchCount, const string& label, const string& dataset){
    int width = 150;
    printSeparator(width, '=');
    cout<<"Search Results | Criteria: " <<label<< " | Dataset: " <<dataset<<endl;
    printSeparator(width, '=');
    
    if(matchCount == 0){
        cout<<"No residents matched the search criteria."<<endl;
        printSeparator(width, '=');
        cout<<endl;
        return;
    }

    cout<<left<<setw(14)<<"Resident ID"
        <<setw(8)<<"Age"
        <<setw(22)<<"Mode of Transport"
        <<setw(24)<<"Daily Distance (km)"
        <<setw(14)<<"CO2 Factor"
        <<setw(12)<<"Days/Month"
        <<setw(26)<<"Monthly CO2 (kg)"
        <<endl;
    printSeparator(width, '=');

    //go through the matched results list and print each resident
    Node* cur = matchedHead;
    while(cur != NULL){
        float co2 = calculateMonthlyCO2(cur); //calculate this resident monthly CO2
        cout<<left
            <<setw(14)<<cur->data.ResidentId
            <<setw(8)<<cur->data.Age
            <<setw(22)<<cur->data.ModeOfTransport
            <<setw(24)<<cur->data.DailyDistance
            <<setw(14)<<fixed<<setprecision(2)<<cur->data.CarbonEmissionFactor
            <<setw(12)<<cur->data.AverageDayPerMonth
            <<setw(18)<<fixed<<setprecision(2)<<co2
            <<endl;
        cur = cur->next; //move to the next matched resident
    }
     //print total match count
    printSeparator(width, '-');
    cout<<"Total matches: "<<matchCount<<endl;
    printSeparator(width, '=');
    cout<<endl;
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
void binarySearchByAgeGroup_Sorted(Node* head, int minAge, int maxAge, Node*& matchedHead, int& matchCount){
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
void binarySearchByTransport_Sorted(Node* head, const string& targetMode, Node*& matchedHead, int& matchCount){
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
void binarySearchByDistance_Sorted(Node* head, int target, Node*& matchedHead, int& matchCount){
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

//BINARY SEARCH (UNSORTED)
//the only difference between sorted and unsorted binary search is by removing the sorting step
//binary search by age
void binarySearchByAgeGroup_Unsorted(Node* head, int minAge, int maxAge, Node*& matchedHead, int& matchCount){
    matchedHead = NULL; 
    matchCount = 0; 

    int size = 0;
    Resident* arr = listToArray(head, size); 
    if(!arr){
        return; 
    }

    int lo = 0, hi = size -1;
    int startIdx = size; 
    while(lo <= hi){
        int mid = lo + (hi - lo) / 2; 
        if(arr[mid].Age >= minAge){
            startIdx = mid;
            hi = mid-1; 
        }else{
            lo = mid+1; 
        }
    }

    for(int i=startIdx; i<size && arr[i].Age <= maxAge; i++){
        insert(matchedHead, arr[i]); 
        matchCount++;
    }
    delete[] arr; 
}

//binary search by mode of transport
void binarySearchByTransport_Unsorted(Node* head, const string& targetMode, Node*& matchedHead, int& matchCount){
    matchedHead = NULL; 
    matchCount = 0; 

    int size = 0;
    Resident *arr = listToArray(head, size); 
    if(!arr){
        return; 
    }

    string targetLower = toLower(targetMode); 

    int lo = 0, hi = size-1, foundIdx = -1;
    while(lo<=hi){
        int mid = lo + (hi - lo) / 2;
        string midMode = toLower(arr[mid].ModeOfTransport); 
        if(midMode == targetLower){
            foundIdx = mid; 
            break; 
        }else if(midMode < targetLower){
            lo = mid + 1;
        }else{
            hi = mid - 1;
        }
    }
    
    if(foundIdx == -1){
        delete[] arr; 
        return; 
    }

    int left = foundIdx;
    while(left > 0 && toLower(arr[left-1].ModeOfTransport) == targetLower){
        left--; 
    }
    int right = foundIdx;
    while(right < size-1 && toLower(arr[right+1].ModeOfTransport) == targetLower){
        right++; //step one position to the right
    }
   
    for(int i=left; i<=right; i++){
        insert(matchedHead, arr[i]); 
        matchCount++; 
    }

    delete[] arr; 
}

//binary search by daily distance
void binarySearchByDistance_Unsorted(Node* head, int target, Node*& matchedHead, int& matchCount){
    matchedHead = NULL; 
    matchCount = 0; 

    int size = 0;
    Resident *arr = listToArray(head, size); 
    if(!arr){
        return; 
    }

    int lo = 0, hi = size-1, foundIdx = -1;
    while(lo<=hi){
        int mid = lo + (hi - lo) / 2; 
        if(arr[mid].DailyDistance == target){
            foundIdx = mid; 
            break; 
        }else if(arr[mid].DailyDistance < target){
            lo = mid + 1;
        }else{
            hi = mid - 1;
        }
    }

    if(foundIdx == -1){
        delete[] arr;
        return;
    }

    int left = foundIdx;
    while(left > 0 && arr[left-1].DailyDistance == target){
        left--;
    }
    int right = foundIdx;
    while(right < size-1 && arr[right+1].DailyDistance == target){
        right++;
    }
   
    for(int i=left; i<=right; i++){
        insert(matchedHead, arr[i]); 
        matchCount++; 
    }

    delete[] arr; 
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

//LINEAR SEARCH (UNSORTED)
//combined linear search (age, transport mode, distance)
void linearSearchCombined_Unsorted(Node* head, int ageGroup, const string& targetMode, int distThreshold, char distOp, Node*& matchedHead, int& matchCount){
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

//LINEAR SEARCH (S0RTED)
//the only difference between sorted and unsorted linear is that sorted linear implements early exits to stop scanning once no more matches can be found
//combined linear search
void linearSearchCombined_Sorted(Node* head, int ageGroup, const string& targetMode, int distThreshold, char distOp, Node*& matchedHead, int& matchCount){
    matchedHead = NULL; //initialize results list as empty
    matchCount = 0; //initialize match counter to 0

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
            if(current->data.Age > maxAge){ 
                break; //early exit, sorted by age
            }
            passAge = (current->data.Age >= minAge);
        }
        //apply transport filter if it is valid
        if(filterTransport){
            string curMode = toLower(current->data.ModeOfTransport);
            if(!filterAge && curMode > targetLower){
                break; //early exit, only if age is not active
            }
            passTransport = (curMode == targetLower);
        }
        //apply distance filter if it is valid
        if(filterDistance){
            int dist = current->data.DailyDistance;
            if(!filterAge && !filterTransport){ //early exit only if age and transport is not active
                if(distOp == '<' && dist >= distThreshold){
                    break;
                }if(distOp == '=' && dist > distThreshold){
                    break;
                }
            }
            if(distOp == '>' && dist > distThreshold){
                passDistance = true;
            }else if(distOp == '<' && dist < distThreshold){
                passDistance = true;
            }else if(distOp == '=' && dist == distThreshold){
                passDistance = true;
            }else{
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

//BFS SEARCH AND DFS SEARCH
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

//A* SEARCH
//one entry in the A* open list (min-heap)
struct AStarNode {
    float f;//total priority score: f = g + h (heap ordered by this)
    float g;//number of search criteria this resident fails
    float h;//heuristic nudge: 0 if perfect match, 0.1 otherwise
    Resident data;//full copy of the resident's data
};

//min-heap: data structure where the smallest value is always at the top,
//gives priority for the best resident first
struct MinHeap {
    AStarNode* arr;//dynamic array storing heap nodes
    int size;//current number of nodes in the heap
    int capacity;//maximum nodes the heap can hold

    //creates a dynamic array with size cap
    //constructor
    MinHeap(int cap) : size(0), capacity(cap) {
        arr = new AStarNode[cap];
    }
    //destructor
    ~MinHeap() { delete[] arr; }//frees the heap memory when the heap is destroyed

    void swapNodes(int i, int j) { //swaps two heap elements
        AStarNode tmp = arr[i]; 
        arr[i] = arr[j]; 
        arr[j] = tmp;
    }

    //bubble new node upward until heap property (parent.f <= child.f) is restored
    void heapifyUp(int i) {
        while(i > 0) {
            int parent = (i - 1) / 2; //the parent of index i is at (i - 1) / 2
            if(arr[parent].f > arr[i].f) { //If the parent has a higher f than the child, swap them
                 swapNodes(parent, i); 
                 i = parent;  //Then move up to the parent's position and check again.
                }
            else break;
        }
    }

    //push replacement node downward until heap property is restored
    void heapifyDown(int i) {
        while(true) {
            int smallest = i;
            int l = 2 * i + 1; //left child
            int r = 2 * i + 2; //right child
            //compares the current node with its children
            if(l < size && arr[l].f < arr[smallest].f) smallest = l; //If the left child exists and has a smaller f, it becomes the new smallest
            if(r < size && arr[r].f < arr[smallest].f) smallest = r; //If the right child exists and is even smaller, it becomes the new smallest
            if(smallest == i) break; //if neither child is smaller, the node in the right place stop
            swapNodes(i, smallest); 
            i = smallest; //otherwise, swap with the smaller child and continue checkng from that child's position
        }
    }

    //add a new node (resident) to heap
    void push(const AStarNode& node) {
        if(size < capacity) { 
            arr[size++] = node;
            heapifyUp(size - 1); 
        } 
    }

    //remove and return the node with the lowest f-score (the root)
    AStarNode pop() { 
        AStarNode top = arr[0]; //saves the root node (the lowest f score) 
        arr[0] = arr[--size]; //moves the last element to the root position and decrements size
        heapifyDown(0); //pushes the new root downward to restore heap order
        return top; //return the lowest
    }

    bool empty() const {
         return size == 0;  //returns true if the heap has no nodes
        }
};

//count nodes in a linked list
int countNodes(Node* head) {
    int n = 0;
    for(Node* c = head; c != NULL; c = c->next) n++;
    return n;
}

//compute g-cost: number of a resident fails
float computeAStarG(const Resident& r,
                    bool filterAge,   int minAge,   int maxAge,
                    bool filterTrans, const string& targetLower,
                    bool filterDist,  int distThreshold, char distOp)
{
    float g = 0.0f; //start with 0 failures
    //add 1 for each criterion the resident fails
    if(filterAge && !(r.Age >= minAge && r.Age <= maxAge))
        g += 1.0f; //age is outside the target range
    if(filterTrans && toLower(r.ModeOfTransport) != targetLower)
        g += 1.0f; //transport mode does not match
    if(filterDist) {
        bool ok = false;
        if(distOp == '>' && r.DailyDistance >  distThreshold) ok = true;
        if(distOp == '<' && r.DailyDistance <  distThreshold) ok = true;
        if(distOp == '=' && r.DailyDistance == distThreshold) ok = true;
        if(!ok) g += 1.0f; //distance condition not satisfied
    }
    return g; //return number of failures
}

//compute h-cost: 0 for a perfect match, 0.1 otherwise (admissible heuristic)
float computeAStarH(float g) {
    return (g == 0.0f) ? 0.0f : 0.1f;
}

//A* on the raw unsorted linked list
void aStarSearchUnsorted(Node* head,
                         int ageGroup,
                         const string& targetMode,
                         int distThreshold, char distOp,
                         Node*& matchedHead, int& matchCount)
{
    matchedHead = NULL; //results linked list starts empty
    matchCount  = 0;

    int minAge = -1, maxAge = -1;
    switch(ageGroup) { //converts age group number into min/max range
        case 1: minAge =  6; maxAge =  17; break; 
        case 2: minAge = 18; maxAge =  25; break; 
        case 3: minAge = 26; maxAge =  45; break; 
        case 4: minAge = 46; maxAge =  60; break; 
        case 5: minAge = 61; maxAge = 100; break; 
    }
    bool filterAge   = (ageGroup >= 1 && ageGroup <= 5); //true if the user select a valid age group, false if choose 0 (skip)
    bool filterTrans = !targetMode.empty(); //true if the user typed a transport mode, false if they pressed enter
    bool filterDist  = (distOp == '>' || distOp == '<' || distOp == '='); //true if the user entered a valid operator, false if they skipped
    string targetLower = toLower(targetMode); //convert the transport target to lowercase

    int n = countNodes(head); //count how many residents are in the list
    if(n == 0) return; //nothing to search then return immediately

    MinHeap heap(n); //open list sized for all node

    Node* cur = head; //start walking the linkedlist from the first node
    while(cur != NULL) {
        float g = computeAStarG(cur->data,
                                filterAge,   minAge,   maxAge,
                                filterTrans, targetLower,
                                filterDist,  distThreshold, distOp); //score this resident, how many criteria do they fail
        float h = computeAStarH(g); //get the heuristic value, 0 if perfect match, 0.1 otherwise
        AStarNode an; //create a star node
        an.g = g; an.h = h; an.f = g + h; an.data = cur->data; //fill in the scores and copy the resident data
        heap.push(an);       //insert into min-heap ordered by f
        cur = cur->next;     //advance to next node in the linked list
    }

    // pop in best-first order; collect only perfect matches (g == 0)
    while(!heap.empty()) {
        AStarNode best = heap.pop(); //lowest f node
        if(best.g == 0.0f) {
            insert(matchedHead, best.data); //perfect match: add to results
            matchCount++;
        } else {
            //g > 0 means this resident fails at least one criterion
            //all remaining heap nodes also have g > 0 — stop here
            break;
        }
    }
}

//A* on an age-sorted linked list
void aStarSearchSorted(Node* head,
                       int ageGroup,
                       const string& targetMode,
                       int distThreshold, char distOp,
                       Node*& matchedHead, int& matchCount)
{
    matchedHead = NULL; //results linked list starts empty
    matchCount  = 0;

    int minAge = -1, maxAge = -1;
    switch(ageGroup) {
        case 1: minAge =  6; maxAge =  17; break; 
        case 2: minAge = 18; maxAge =  25; break; 
        case 3: minAge = 26; maxAge =  45; break; 
        case 4: minAge = 46; maxAge =  60; break; 
        case 5: minAge = 61; maxAge = 100; break; 
    }
    bool filterAge   = (ageGroup >= 1 && ageGroup <= 5);
    bool filterTrans = !targetMode.empty();
    bool filterDist  = (distOp == '>' || distOp == '<' || distOp == '=');
    string targetLower = toLower(targetMode);

    Node* sortedList = copyList(head);
    if(sortedList == NULL) return;

    //sort the copy by age ascending using bubble sort
    bubbleSort(sortedList, sort_age, sort_asc);

    int n = countNodes(sortedList);
    MinHeap heap(n); //open list sized for all nodes

    Node* cur = sortedList;
    while(cur != NULL) {
        if(filterAge) {
            if(cur->data.Age < minAge) { cur = cur->next; continue; } 
            if(cur->data.Age > maxAge) break;
        }
        float g = computeAStarG(cur->data,
                                filterAge,   minAge,   maxAge,
                                filterTrans, targetLower,
                                filterDist,  distThreshold, distOp);
        float h = computeAStarH(g);
        AStarNode an;
        an.g = g; an.h = h; an.f = g + h; an.data = cur->data;
        heap.push(an);   //insert into min-heap ordered by f
        cur = cur->next; //advance to next node in the sorted list
    }

    //pop in best-first order; collect only perfect matches (g == 0)
    while(!heap.empty()) {
        AStarNode best = heap.pop();
        if(best.g == 0.0f) {
            insert(matchedHead, best.data); //perfect match: add to results
            matchCount++;
        } else {
            break; //all remaining nodes also have g > 0
        }
    }

    //free the temporary sorted copy
    freeList(sortedList);
}


//MENU
//SORTING MENU
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

//SEARCHING SUB MENU
//sub-menu for linear search
void linearSearchMenu(Node* cityA, Node* cityB, Node* cityC){
    Node* lists[3] = {cityA, cityB, cityC};
    string names[3] = {"City A", "City B", "City C"};

    printSeparator(57, '-');
    cout << "Linear Search" << endl;
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
    string label = buildSearchLabel("Linear", ageGroup, targetMode, distThreshold, distOp);

    Node* searchList = NULL; //points to the list that will be searched
    string datasetName; //stores the name of selected dataset for display

    if(cityChoice == 4) {
        //merge all 3 cities into 1 combined list
        datasetName = "All Cities";
        for(int l = 0; l < 3; l++) {
            Node* cur = lists[l];
            while(cur != NULL) {
                insert(searchList, cur->data); //copy each resident into combined list
                cur = cur->next;
            }
        }
    
    }else{
        //point directly to the selected city's list (no need to copy)
        searchList = lists[cityChoice-1];
        datasetName = names[cityChoice-1];
    }

    //run unsorted linear, scan every resident
    Node* matchedHead = NULL;
    int matchCount = 0;
    auto timer = startTimer();
    linearSearchCombined_Unsorted(searchList, ageGroup, targetMode, distThreshold, distOp, matchedHead, matchCount);
    double elapsedMs = stopTimer(timer);
    printSearchResults(matchedHead, matchCount, "[UNSORTED LINEAR] " + label, datasetName);
    printPerformance(elapsedMs);
    freeList(matchedHead);
    matchedHead = NULL;
    matchCount = 0;

    //prepare a sorted copy list for sorted linear search
    //the list is sorted by the first active filter so the early exit in sorted search works correctly
    Node* sortedList = NULL;
    bool linearSortedList = false;

    if(ageGroup >= 1 && ageGroup <= 5){
        //age filter is active, sort by age, so early exit runs if age exceeds max
        sortedList = sortListCopyByField(searchList, "age");
        linearSortedList = true;
    }else if(!targetMode.empty()){
        //transport filter is active (age skipped), sort by transport, so early exit runs if current mode passes the target alphabetically
        sortedList = sortListCopyByField(searchList, "transport");
        linearSortedList = true;
    }else if(distOp == '>' || distOp == '<' || distOp == '='){
        //distance filter is active (age & transport skipped), sort by distance, so early exit runs if distance exceeds threshold
        sortedList = sortListCopyByField(searchList, "distance");
        linearSortedList = true;
        }

    //if no filter was active, original list is printed
    if(!linearSortedList){
        sortedList = searchList;
    }

    //run sorted linear searh on sorted copy
    //all active filters are checked regardless of which field was used for sorting
    auto timer = startTimer();
    linearSearchCombined_Sorted(sortedList, ageGroup, targetMode, distThreshold, distOp, matchedHead, matchCount);
    double elapsedMs = stopTimer(timer);
    printSearchResults(matchedHead, matchCount, "[SORTED LINEAR] " + label, datasetName);
    printPerformance(elapsedMs);
    freeList(matchedHead);
    
    //free sorted copy if it was created
    if(linearSortedList){
        freeList(sortedList);
    }
    //free combined list if it was created during merging all cities
    if(cityChoice == 4){
        freeList(searchList);
    }
}

//sub-menu for binary search
void binarySearchMenu(Node* cityA, Node* cityB, Node* cityC) {
    Node* lists[3] = {cityA, cityB, cityC};
    string names[3] = {"City A", "City B", "City C"};

    int choice;
    do {
        //city selection
        printSeparator(57, '-');
        cout << "Binary Search" << endl;
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

        Node* searchList = NULL;
        string datasetName; //stores the name of selected dataset for display

        if(cityChoice == 4) {
            //merge all 3 cities into 1 combined list
            datasetName = "All Cities";
            for(int l = 0; l < 3; l++) {
                Node* cur = lists[l];
                while(cur != NULL) {
                    insert(searchList, cur->data); //copy each resident into combined list
                    cur = cur->next;
                }
            }
        
        }else{
            //point directly to the selected city's list (no need to copy)
            searchList = lists[cityChoice-1];
            datasetName = names[cityChoice-1];
        }

        //category selection
        printSeparator(57, '-');
        cout << "Binary Search" << endl;
        cout << "\n1. Search by age group" << endl;
        cout << "2. Search by mode of transport" << endl;
        cout << "3. Search by daily distance" << endl;
        cout << "4. Back" << endl;
        printSeparator(57, '-');
        cout << "Select search type: ";
        cin >> choice;

        if(choice == 4) {
            if(cityChoice == 4){
                freeList(searchList); //free combined list before going back to dataset selection
            }
            cout << endl;
            break;
        }
        if(choice < 1 || choice > 3) {
            if(cityChoice == 4){
                freeList(searchList); //free combined list before breaking out of loop
            }
            cout << "Invalid choice. Please try again" << endl;
            continue;
        }

        //unsorted and sorted head holds the search results for unsorted and sorted search respectively
        Node* unsortedHead = NULL, *sortedHead = NULL;
        int unsortedCount = 0, sortedCount = 0;
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
            label = "Age Group = " + getAgeGroupLabel(grp - 1);

            //binary search on unsorted list
            auto timer = startTimer();
            binarySearchByAgeGroup_Unsorted(searchList, minAge, maxAge, unsortedHead, unsortedCount);
            double elapsedMs = stopTimer(timer);
            printSearchResults(unsortedHead, unsortedCount, "[UNSORTED BINARY] " + label, datasetName);
            printPerformance(elapsedMs);
            freeList(unsortedHead);

            //converts list to array, sorts it, and then runs binary search
            auto timer = startTimer();
            binarySearchByAgeGroup_Sorted(searchList, minAge, maxAge, sortedHead, sortedCount);
            double elapsedMs = stopTimer(timer);
            printSearchResults(sortedHead, sortedCount, "[SORTED BINARY] " + label, datasetName);
            printPerformance(elapsedMs);
            freeList(sortedHead);

        } else if(choice == 2) {
            //binary search by transport mode
            cout << "\nEnter transport mode to search: ";
            cin.ignore();
            string mode;
            getline(cin, mode);
            label = "Mode = " + mode;

            //binary search on unsorted list
            binarySearchByTransport_Unsorted(searchList, mode, unsortedHead, unsortedCount);
            printSearchResults(unsortedHead, unsortedCount, "[UNSORTED BINARY] " + label, datasetName);
            freeList(unsortedHead);

            //converts list to array, sorts it, and then runs binary search
            binarySearchByTransport_Sorted(searchList, mode, sortedHead, sortedCount);
            printSearchResults(sortedHead, sortedCount, "[SORTED BINARY] " + label, datasetName);
            freeList(sortedHead);

        } else if(choice == 3) {
            //binary search by exact daily distance
            cout << "\nEnter exact daily distance to search: ";
            int target;
            cin >> target;
            stringstream ss;
            ss << target;
            label = "Distance = " + ss.str();

            //binary search on unsorted list
            binarySearchByDistance_Unsorted(searchList, target, unsortedHead, unsortedCount);
            printSearchResults(unsortedHead, unsortedCount, "[UNSORTED BINARY] " + label, datasetName);
            freeList(unsortedHead);

            //converts list to array, sorts it, and then runs binary search
            binarySearchByDistance_Sorted(searchList, target, sortedHead, sortedCount);
            printSearchResults(sortedHead, sortedCount, "[SORTED BINARY] " + label, datasetName);
            freeList(sortedHead);
        }
        //free combined list after searching is done, if all cities is selected
        if(cityChoice == 4){
            freeList(searchList);
        }

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
        auto timer = startTimer();
        bfsSearch(combined, "All Cities", ageGroup, targetMode, distThreshold, distOp, matchedHead, matchCount);
        double elapsedMs = stopTimer(timer);
        string label = buildSearchLabel("BFS", ageGroup, targetMode, distThreshold, distOp);
        printSearchResults(matchedHead, matchCount, label, "All Cities");
        printPerformance(elapsedMs);
        freeList(combined); //free the combined list after searching
    } else {
        int idx = cityChoice - 1; //convert to 0-based index
        auto timer = startTimer();
        bfsSearch(lists[idx], names[idx], ageGroup, targetMode, distThreshold, distOp, matchedHead, matchCount);
        double elapsedMs = stopTimer(timer);
        string label = buildSearchLabel("BFS", ageGroup, targetMode, distThreshold, distOp);
        printSearchResults(matchedHead, matchCount, label, names[idx]);
        printPerformance(elapsedMs);
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
        auto timer = startTimer();
        dfsSearch(combined, "All Cities", ageGroup, targetMode, distThreshold, distOp, matchedHead, matchCount);
        double elapsedMs = stopTimer(timer);
        string label = buildSearchLabel("DFS", ageGroup, targetMode, distThreshold, distOp);
        printSearchResults(matchedHead, matchCount, label, "All Cities");
        printPerformance(elapsedMs);
        freeList(combined); //free the combined list after searching
    } else {
        int idx = cityChoice - 1; //convert to 0-based index
        auto timer = startTimer();
        dfsSearch(lists[idx], names[idx], ageGroup, targetMode, distThreshold, distOp, matchedHead, matchCount);
        double elapsedMs = stopTimer(timer);
        string label = buildSearchLabel("DFS", ageGroup, targetMode, distThreshold, distOp);
        printSearchResults(matchedHead, matchCount, label, names[idx]);
        printPerformance(elapsedMs);
    }

    freeList(matchedHead); //free the results list after displaying
}

//menu for A* search experiments
void aStarSearchMenu(Node* cityA, Node* cityB, Node* cityC) {
    Node*  lists[3] = { cityA,    cityB,    cityC    };
    string names[3] = { "City A", "City B", "City C" };

    int choice;
    do {
        printSeparator(60, '-');
        cout << "A* Search" << endl;
        cout << "\nDataset:" << endl;
        cout << "1. City A" << endl;
        cout << "2. City B" << endl;
        cout << "3. City C" << endl;
        cout << "4. All Cities" << endl;
        cout << "5. Back" << endl;
        printSeparator(60, '-');
        cout << "Select dataset: ";
        cin >> choice;

        if(choice == 5) { cout << endl; break; }
        if(choice < 1 || choice > 4) {
            cout << "Invalid choice. Please try again." << endl;
            continue;
        }

        //collect all three search filters from user using the shared helper
        int ageGroup;
        string transport;
        int distThreshold;
        char distOp;
        selectSearchCriteria(ageGroup, transport, distThreshold, distOp);

        //build the criteria label using the shared helper (shared by both variants)
        string criteria = buildSearchLabel("", ageGroup, transport, distThreshold, distOp);
        if(criteria.empty()) criteria = "No filter";

        cout << endl;

        int start = (choice == 4) ? 0 : choice - 1;
        int end   = (choice == 4) ? 2 : choice - 1;

        for(int i = start; i <= end; i++) {
            //--- UNSORTED A* ---
            Node* matchedHead = NULL;
            int   matchCount  = 0;
            auto timer = startTimer();
            aStarSearchUnsorted(lists[i], ageGroup, transport,
                                distThreshold, distOp,
                                matchedHead, matchCount);
            double elapsedMs = stopTimer(timer);
            printSearchResults(matchedHead, matchCount,
                               "[UNSORTED A*] " + criteria, names[i]);
            printPerformance(elapsedMs);
            freeList(matchedHead);

            //--- SORTED A* ---
            matchedHead = NULL;
            matchCount  = 0;

            timer = startTimer();
            aStarSearchSorted(lists[i], ageGroup, transport,
                              distThreshold, distOp,
                              matchedHead, matchCount);

            elapsedMs = stopTimer(timer);
            printSearchResults(matchedHead, matchCount,
                               "[SORTED A*] " + criteria, names[i]);
            printPerformance(elapsedMs);
            freeList(matchedHead);
        }

    } while(choice != 5);
}

//SEARCHING MENU
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
        cout << "5. A* Search" << endl;
        cout << "6. Back to main menu" << endl;
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
                aStarSearchMenu(cityA, cityB, cityC);
                break;
            case 6:
                cout << endl;
                break;
            default:
                cout << "Invalid choice. Please try again" << endl;
                break;
        }
    } while(choice != 6);
}

//MAIN MENU
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