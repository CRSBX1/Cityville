#include "header.hpp"

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
