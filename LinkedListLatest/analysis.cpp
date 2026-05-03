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
            tally.userCount[i]++;
            tally.modeCO2[i] += co2;
            return;
        }
    }
    //if mode not found, add mode as new entry
    if(tally.totalModes < max_transport_types) {
        tally.modeName[tally.totalModes] = mode;
        tally.userCount[tally.totalModes] = 1;
        tally.modeCO2[tally.totalModes] = co2;
        tally.totalModes++;
    }
}

//return name of mode with highest user count
string getMostUsedMode(const TransportCount& tally) {
    if(tally.totalModes == 0) return "None";
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
        groupSummary[i].membersHead            = NULL;
    }
    float grandTotalCO2 = 0.0f;

    Node* current = head;
    while(current != NULL) {
        int groupIndex = getAgeGroupIndex(current->data.Age);
        if(groupIndex != -1) {
            float residentCO2 = calculateMonthlyCO2(current);
            groupSummary[groupIndex].totalResidents++;
            groupSummary[groupIndex].totalCO2 += residentCO2;
            grandTotalCO2 += residentCO2;
            addToTransportCount(groupSummary[groupIndex].transportUsage, current->data.ModeOfTransport, residentCO2);
            
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
            groupSummary[i].averageCO2 = groupSummary[i].totalCO2 / (float)groupSummary[i].totalResidents;
            if(grandTotalCO2 > 0.0f) {
                groupSummary[i].percentOfGrandTotal = (groupSummary[i].totalCO2 / grandTotalCO2) * 100.0f;
            }
            groupSummary[i].preferredTransportMode = getMostUsedMode(groupSummary[i].transportUsage);
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
            delete temp;
        }
        groupSummary[i].membersHead = NULL;
    }
}
