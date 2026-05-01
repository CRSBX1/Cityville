#include "header.hpp"

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

//LINEAR SEARCH SUB MENU
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
    MemoryMetrics::reset();
    auto timer = startTimer();
    linearSearchCombined_Unsorted(searchList, ageGroup, targetMode, distThreshold, distOp, matchedHead, matchCount);
    double elapsedMs = stopTimer(timer);
    size_t memUsed = MemoryMetrics::trackMemoryUsage();
    printSearchResults(matchedHead, matchCount, "[UNSORTED LINEAR] " + label, datasetName);
    printPerformance(elapsedMs, memUsed);
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
    MemoryMetrics::reset();
    timer = startTimer();
    linearSearchCombined_Sorted(sortedList, ageGroup, targetMode, distThreshold, distOp, matchedHead, matchCount);
    elapsedMs = stopTimer(timer);
    memUsed = MemoryMetrics::trackMemoryUsage();
    printSearchResults(matchedHead, matchCount, "[SORTED LINEAR] " + label, datasetName);
    printPerformance(elapsedMs, memUsed);
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

//BINARY SEARCH SUB MENU
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
            MemoryMetrics::reset();
            auto timer = startTimer();
            binarySearchByAgeGroup_Unsorted(searchList, minAge, maxAge, unsortedHead, unsortedCount);
            double elapsedMs = stopTimer(timer);
            size_t memUsed = MemoryMetrics::trackMemoryUsage();
            printSearchResults(unsortedHead, unsortedCount, "[UNSORTED BINARY] " + label, datasetName);
            printPerformance(elapsedMs, memUsed);
            freeList(unsortedHead);

            //converts list to array, sorts it, and then runs binary search
            MemoryMetrics::reset();
            timer = startTimer();
            binarySearchByAgeGroup_Sorted(searchList, minAge, maxAge, sortedHead, sortedCount);
            elapsedMs = stopTimer(timer);
            memUsed = MemoryMetrics::trackMemoryUsage();
            printSearchResults(sortedHead, sortedCount, "[SORTED BINARY] " + label, datasetName);
            printPerformance(elapsedMs, memUsed);
            freeList(sortedHead);

        } else if(choice == 2) {
            //binary search by transport mode
            cout << "\nEnter transport mode to search: ";
            cin.ignore();
            string mode;
            getline(cin, mode);
            label = "Mode = " + mode;

            //binary search on unsorted list
            MemoryMetrics::reset();
            auto timer = startTimer();
            binarySearchByTransport_Unsorted(searchList, mode, unsortedHead, unsortedCount);
            double elapsedMs = stopTimer(timer);
            size_t memUsed = MemoryMetrics::trackMemoryUsage();
            printSearchResults(unsortedHead, unsortedCount, "[UNSORTED BINARY] " + label, datasetName);
            printPerformance(elapsedMs, memUsed);
            freeList(unsortedHead);

            //converts list to array, sorts it, and then runs binary search
            MemoryMetrics::reset();
            timer = startTimer();
            binarySearchByTransport_Sorted(searchList, mode, sortedHead, sortedCount);
            elapsedMs = stopTimer(timer);
            memUsed = MemoryMetrics::trackMemoryUsage();
            printSearchResults(sortedHead, sortedCount, "[SORTED BINARY] " + label, datasetName);
            printPerformance(elapsedMs, memUsed);
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
            MemoryMetrics::reset();
            auto timer = startTimer();
            binarySearchByDistance_Unsorted(searchList, target, unsortedHead, unsortedCount);
            double elapsedMs = stopTimer(timer);
            size_t memUsed = MemoryMetrics::trackMemoryUsage();
            printSearchResults(unsortedHead, unsortedCount, "[UNSORTED BINARY] " + label, datasetName);
            printPerformance(elapsedMs, memUsed);
            freeList(unsortedHead);

            //converts list to array, sorts it, and then runs binary search
            MemoryMetrics::reset();
            timer = startTimer();
            binarySearchByDistance_Sorted(searchList, target, sortedHead, sortedCount);
            elapsedMs = stopTimer(timer);
            memUsed = MemoryMetrics::trackMemoryUsage();
            printSearchResults(sortedHead, sortedCount, "[SORTED BINARY] " + label, datasetName);
            printPerformance(elapsedMs, memUsed);
            freeList(sortedHead);
        }
        //free combined list after searching is done, if all cities is selected
        if(cityChoice == 4){
            freeList(searchList);
        }

    } while(choice != 4);
}

//BFS SEARCH SUB MENU
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
        MemoryMetrics::reset();
        auto timer = startTimer();
        bfsSearch(combined, "All Cities", ageGroup, targetMode, distThreshold, distOp, matchedHead, matchCount);
        double elapsedMs = stopTimer(timer);
        size_t memUsed = MemoryMetrics::trackMemoryUsage();
        string label = buildSearchLabel("BFS", ageGroup, targetMode, distThreshold, distOp);
        printSearchResults(matchedHead, matchCount, label, "All Cities");
        printPerformance(elapsedMs, memUsed);
        freeList(combined); //free the combined list after searching
    } else {
        int idx = cityChoice - 1; //convert to 0-based index
        MemoryMetrics::reset();
        auto timer = startTimer();
        bfsSearch(lists[idx], names[idx], ageGroup, targetMode, distThreshold, distOp, matchedHead, matchCount);
        double elapsedMs = stopTimer(timer);
        size_t memUsed = MemoryMetrics::trackMemoryUsage();
        string label = buildSearchLabel("BFS", ageGroup, targetMode, distThreshold, distOp);
        printSearchResults(matchedHead, matchCount, label, names[idx]);
        printPerformance(elapsedMs, memUsed);
    }

    freeList(matchedHead); //free the results list after displaying
}

//DFS SEARCH SUB MENU
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
        MemoryMetrics::reset();
        auto timer = startTimer();
        dfsSearch(combined, "All Cities", ageGroup, targetMode, distThreshold, distOp, matchedHead, matchCount);
        double elapsedMs = stopTimer(timer);
        size_t memUsed = MemoryMetrics::trackMemoryUsage();
        string label = buildSearchLabel("DFS", ageGroup, targetMode, distThreshold, distOp);
        printSearchResults(matchedHead, matchCount, label, "All Cities");
        printPerformance(elapsedMs, memUsed);
        freeList(combined); //free the combined list after searching
    } else {
        int idx = cityChoice - 1; //convert to 0-based index
        MemoryMetrics::reset();
        auto timer = startTimer();
        dfsSearch(lists[idx], names[idx], ageGroup, targetMode, distThreshold, distOp, matchedHead, matchCount);
        double elapsedMs = stopTimer(timer);
        size_t memUsed = MemoryMetrics::trackMemoryUsage();
        string label = buildSearchLabel("DFS", ageGroup, targetMode, distThreshold, distOp);
        printSearchResults(matchedHead, matchCount, label, names[idx]);
        printPerformance(elapsedMs, memUsed);
    }

    freeList(matchedHead); //free the results list after displaying
}

//A*SEARCH SUB MENU
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
            MemoryMetrics::reset();
            auto timer = startTimer();
            aStarSearchUnsorted(lists[i], ageGroup, transport,
                                distThreshold, distOp,
                                matchedHead, matchCount);
            double elapsedMs = stopTimer(timer);
            size_t memUsed = MemoryMetrics::trackMemoryUsage();
            printSearchResults(matchedHead, matchCount,
                               "[UNSORTED A*] " + criteria, names[i]);
            printPerformance(elapsedMs, memUsed);
            freeList(matchedHead);

            //--- SORTED A* ---
            matchedHead = NULL;
            matchCount  = 0;

            MemoryMetrics::reset();
            timer = startTimer();
            aStarSearchSorted(lists[i], ageGroup, transport,
                              distThreshold, distOp,
                              matchedHead, matchCount);

            elapsedMs = stopTimer(timer);
            memUsed = MemoryMetrics::trackMemoryUsage();
            printSearchResults(matchedHead, matchCount,
                               "[SORTED A*] " + criteria, names[i]);
            printPerformance(elapsedMs, memUsed);
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
        cout << "3. Breadth-First Search" << endl;
        cout << "4. Depth-First Search" << endl;
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
