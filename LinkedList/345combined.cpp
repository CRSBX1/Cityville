#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
using namespace std;

const int total_age_groups    = 5; //total number of age groups
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

//summary for one age group
struct AgeGroupSummary {
    string groupLabel;
    int totalResidents;
    float totalCO2;
    float averageCO2;
    float percentOfGrandTotal;
    string preferredTransportMode;
    TransportCount transportUsage;
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
        tally.modeName[tally.totalModes]  = mode; //store mode name
        tally.userCount[tally.totalModes] = 1; //first resident using the new mode
        tally.modeCO2[tally.totalModes]   = co2; //set initial CO2 for this mode
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
        }
        current = current->next; //move to next resident
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

void mainMenu(Node* cityA, Node* cityB, Node* cityC) {
    int choice;

    do {
        printSeparator(53, '=');
        cout << "Carbon Emission Analysis System\n";
        cout << "1. View emissions per mode of transport\n";
        cout << "2. View emissions per age group comparison\n";
        cout << "3. View emissions by transport mode across all cities\n";
        cout << "4. View cross-city comparison\n";
        cout << "0. Exit\n";
        printSeparator(53, '=');
        cout << "Enter choice: ";
        cin >> choice;

        switch (choice) {
            case 1: //carbon emissions per mode of transport, displayed per city
                cout << "\n================== Carbon Emissions Per Mode of Transport ==================" << endl << endl;
                displayEmissionsByTransportMode(cityA, "City A");
                displayEmissionsByTransportMode(cityB, "City B");
                displayEmissionsByTransportMode(cityC, "City C");
                break;
            case 2:
                //comparison tables across datasets and age groups
                cout << "\n========================================= Comparison Tables =========================================" << endl << endl;
                //per city breakdown by age group
                displayTable1_ByAgeGroup(cityA, "City A");
                displayTable1_ByAgeGroup(cityB, "City B");
                displayTable1_ByAgeGroup(cityC, "City C");
                break;
            case 3:
                //all cities merged, broken down by transport mode
                displayTable2_CombinedTransport(cityA, cityB, cityC);
                break;
            case 4:
                //one row per city showing totals and extreme modes
                displayTable3_CrossCity(cityA, cityB, cityC);
                break;
        }
    } while (choice != 0);
}

int main() {
    Node* cityA=NULL;
    Node* cityB=NULL;
    Node* cityC=NULL;

    loadData(cityA,"CityA.txt");
    loadData(cityB,"CityB.txt");
    loadData(cityC,"CityC.txt");

    mainMenu(cityA,cityB,cityC);

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