#include <iostream>

using namespace std;

const int total_age_groups = 5; //total number of age groups
const int max_transport_types = 15; //max number of different transport mode per group

//each node holds one resident's data
struct Resident{
    string residentID;
    int age;
    string modeOfTransport;
    int dailyDistance; 
    float carbonEmissionFactor;
    int averageDayPerMonth;
};

struct Node{
    Resident data;
    Node* next; 
};

//how many residents use each transport mode within one age group
struct TransportCount{
    string modeName[max_transport_types];
    int userCount[max_transport_types];
    int totalModes;
};

//summary for one age group
struct AgeGroupSummary{
    string groupLabel;
    int totalResidents;
    float totalCO2;
    float averageCO2;
    float percentOfGrandTotal;
    string preferredTransportMode;
    TransportCount transportUsage; 
};


//returns which age group index a given age belongs to
int getAgeGroupIndex(int age){
    if(age>=6 && age<=17){ //children & teenagers
        return 0;
    }else if(age>=18 && age<=25){ //university & young adults
        return 1;
    }else if(age>=26 && age<=45){ //working adults early career
        return 2;
    }else if(age>=46 && age<=60){ //working adults late career
        return 3;
    }else if(age>=61 && age<=100){ //senior citizens / retirees
        return 4;
    }else{
        return -1; //age outside defined range
    }
}

//returns the display label for a given age group index
string getAgeGroupLabel(int groupIndex){
    const string labels[total_age_groups] = {
        "Children & Teenagers        (6-17)",
        "University / Young Adults   (18-25)",
        "Working Adults Early Career (26-45)",
        "Working Adults Late Career  (46-60)",
        "Senior Citizens / Retirees  (61-100)"
    };
    if(groupIndex>=0 && groupIndex<total_age_groups){
        return labels[groupIndex]; //returns a label if index is valid
    }else{
        return "Unknown Group"; //return for invalid index
    }
}

//calculates one resident monthly CO2 emission
//formula = emission factors * distance per day * days per month
float calculateMonthlyCO2(const Node* resident){
    return resident->data.carbonEmissionFactor * (float)resident->data.dailyDistance * (float)resident->data.averageDayPerMonth;
}

//adds mode to tally for one age group
void addToTransportCount(TransportCount& tally, const string& mode){
    for(int i=0; i<tally.totalModes; i++){
        if(tally.modeName[i] == mode){
            tally.userCount[i]++; //increase count if mode found
            return;
        }
    }
    //if mode not found, add mode as new entry (new transport type)
    if(tally.totalModes < max_transport_types){
        tally.modeName[tally.totalModes] = mode; //store mode name
        tally.userCount[tally.totalModes] = 1; //first resident using the new mode
        tally.totalModes++; //increase mode count
    }
}

//return name of mode with highest user count
string getMostUsedMode(const TransportCount& tally){
    if(tally.totalModes == 0){
        return "None";
    }
    int highestIndex = 0;
    for(int i=1; i<tally.totalModes; i++){
        if(tally.userCount[i] > tally.userCount[highestIndex]){
            highestIndex = i;
        }
    }
    return tally.modeName[highestIndex]; 
}


void categorizeByAgeGroup(Node* head, AgeGroupSummary groupSummary[]){
    //initialize every group to 0 before counting
    for(int i=0; i<total_age_groups; i++){
        groupSummary[i].groupLabel = getAgeGroupLabel(i);
        groupSummary[i].totalResidents = 0;
        groupSummary[i].totalCO2 = 0.0f;
        groupSummary[i].averageCO2 = 0.0f;
        groupSummary[i].percentOfGrandTotal = 0.0f;
        groupSummary[i].preferredTransportMode = "None";
        groupSummary[i].transportUsage.totalModes = 0;
    }
    float grandTotalCO2 = 0.0f; //keeps running total of CO2 across all groups combined

    //walk through every resident in the linked list
    Node* current = head;
    while(current != nullptr){
        int groupIndex = getAgeGroupIndex(current->data.age); //find which age group the resident belongs to
        if(groupIndex != -1){ //resident is skipped if age is outside defined range
            float residentCO2 = calculateMonthlyCO2(current); //calculate how much CO2 this resident produce per month
            groupSummary[groupIndex].totalResidents++; //add resident to their group count
            groupSummary[groupIndex].totalCO2 += residentCO2; //add CO2 to their group total
            grandTotalCO2 += residentCO2; //also add CO2 to overall grand total
            addToTransportCount(groupSummary[groupIndex].transportUsage, current->data.modeOfTransport); //record which transport mode the resident use
        }
        current = current->next; //move to next resident
    }

    //calculate avg and percentages
    for(int i=0; i<total_age_groups; i++){
        if(groupSummary[i].totalResidents > 0){ 
            //avg CO2 = total CO2 for this group /  number of residents in this group
            groupSummary[i].averageCO2 = groupSummary[i].totalCO2 / (float)groupSummary[i].totalResidents; 
            if(grandTotalCO2 > 0.0f){
                groupSummary[i].percentOfGrandTotal = (groupSummary[i].totalCO2 / grandTotalCO2) * 100.0f;
            }
            groupSummary[i].preferredTransportMode = getMostUsedMode(groupSummary[i].transportUsage); //find which transport mode is most used by residents in this group
        }
    }
}

