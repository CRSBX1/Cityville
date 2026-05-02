#include "header.hpp"

//prints a horizontal separator line
void printSeparator(int width, char c) {
    for(int i = 0; i < width; i++) {
        cout << c;
    }
    cout << endl;
}

//prints execution time and memory usage after an operation completes
void printPerformance(double elapsedMs, size_t memoryBytes) {
    printSeparator(50, '-');
    cout << fixed << setprecision(6);
    cout << "  Execution Time : " << elapsedMs << " ms" << endl;
    cout << "  Memory Usage   : " << memoryBytes << " bytes" << endl;
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
