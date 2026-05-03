#include "header.hpp"

//shared city selection menu
//showBack: if true, shows a "Back" option (returns 5)
//defaultChoice: which city to default to on invalid input
int selectCityExtended(bool showBack, int defaultChoice) {
    cout << "\nDataset:" << endl;
    cout << "1. City A" << endl;
    cout << "2. City B" << endl;
    cout << "3. City C" << endl;
    cout << "4. All Cities (Combined)" << endl;
    if(showBack) cout << "5. Back" << endl;
    cout << "Select dataset: ";
    int cityChoice;
    cin >> cityChoice;

    if(showBack && cityChoice == 5) return 5; //back
    if(cityChoice < 1 || cityChoice > 4) {
        string defaultName;
        switch(defaultChoice) {
            case 1: defaultName = "City A"; break;
            case 2: defaultName = "City B"; break;
            case 3: defaultName = "City C"; break;
            case 4: defaultName = "All Cities"; break;
            default: defaultName = "City A"; defaultChoice = 1; break;
        }
        cout << "Invalid choice. Default to " << defaultName << endl;
        return defaultChoice;
    }
    return cityChoice;
}

//resolves a city choice into the actual search list and dataset name
Node* resolveSearchList(Node* lists[], string names[], int cityChoice, string& datasetName, bool& needsFree) {
    needsFree = false;
    if(cityChoice == 4) {
        datasetName = "All Cities";
        needsFree = true;
        return combineLists(lists, 3);
    } else {
        int idx = cityChoice - 1;
        datasetName = names[idx];
        return lists[idx];
    }
}

//prompts user to select search criteria and stores the selections
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
        ageGroup = 0;
    }

    //filter 2: transport mode (empty = skip filter)
    cout << "\nEnter transport mode to filter (press Enter to skip): ";
    cin.ignore();
    getline(cin, targetMode);

    //filter 3: daily distance threshold
    cout << "\nFilter by daily distance:" << endl;
    cout << "Operator (>, <, = or 0 to skip): ";
    cin >> distOp;

    distThreshold = 0;
    if(isValidDistOp(distOp)) {
        cout << "Enter distance threshold: ";
        cin >> distThreshold;
    } else {
        distOp = '0';
    }
}

//prompts user to select an age group and returns the group number (1-5)
//also sets minAge and maxAge via getAgeRange. Defaults to group 1 on invalid input.
int selectAgeGroup(int& minAge, int& maxAge) {
    cout << "\nSelect age group:" << endl;
    cout << "1. Children & Teenagers        (6-17)" << endl;
    cout << "2. University / Young Adults   (18-25)" << endl;
    cout << "3. Working Adults Early Career (26-45)" << endl;
    cout << "4. Working Adults Late Career  (46-60)" << endl;
    cout << "5. Senior Citizens / Retirees  (61-100)" << endl;
    cout << "Select group: ";
    int grp;
    cin >> grp;

    getAgeRange(grp, minAge, maxAge);
    if(minAge == -1) {
        cout << "Invalid group. Default to group 1" << endl;
        grp = 1;
        getAgeRange(grp, minAge, maxAge);
    }
    return grp;
}

//builds a descriptive label string showing which filters were active
string buildSearchLabel(const string& algoName, int ageGroup, const string& targetMode, int distThreshold, char distOp) {
    string label = algoName;
    if(isValidAgeGroup(ageGroup)) {
        label += " | Age Group = " + getAgeGroupLabel(ageGroup - 1);
    }
    if(!targetMode.empty()) {
        label += " | Mode = " + targetMode;
    }
    if(isValidDistOp(distOp)) {
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
        cout << "5. View emissions & preferred transport per age group\n";
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
            case 4:
                cout << "\n================== Carbon Emissions Per Mode of Transport ==================" << endl << endl;
                displayEmissionsByTransportMode(cityA, "City A");
                displayEmissionsByTransportMode(cityB, "City B");
                displayEmissionsByTransportMode(cityC, "City C");
                break;
            case 5:
                cout << "\n========================================= Comparison Tables =========================================" << endl << endl;
                displayTable1_ByAgeGroup(cityA, "City A");
                displayTable1_ByAgeGroup(cityB, "City B");
                displayTable1_ByAgeGroup(cityC, "City C");
                break;
            case 6:
                displayTable2_CombinedTransport(cityA, cityB, cityC);
                break;
            case 7:
                displayTable3_CrossCity(cityA, cityB, cityC);
                break;
            case 8:
                sortingExperimentsMenu(cityA, cityB, cityC);
                break;
            case 9:
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

        if(choice == 4){ cout<<endl; break; }

        SortField field;
        switch(choice){
            case 1: field = sort_age; break;
            case 2: field = sort_distance; break;
            case 3: field = sort_carbem; break;
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
        SortOrder order = (orderChoice == 1) ? sort_asc : sort_desc;

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

        int cityChoice = selectCityExtended(false, 4); //no back option, default to All Cities
        cout<<endl;

        int start = (cityChoice == 4) ? 0 : cityChoice - 1;
        int end = (cityChoice == 4) ? 2 : cityChoice - 1;

        for(int i=start; i<=end; i++){
            bool useInsertion = (algoChoice == 2);
            string algoName = useInsertion ? "Insertion Sort" : "Bubble Sort";
            displaySortResult(lists[i], names[i], algoName, field, order, useInsertion);
        }
    }while(choice!=4);
}

//SEARCHING MENU

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
        cout << "6. Search Comparison" << endl;
        cout << "7. Back to main menu" << endl;
        printSeparator(57, '-');
        cout << "Select search algorithm: ";
        cin >> choice;

        switch(choice) {
            case 1: linearSearchMenu(cityA, cityB, cityC); break;
            case 2: binarySearchMenu(cityA, cityB, cityC); break;
            case 3: bfsSearchMenu(cityA, cityB, cityC); break;
            case 4: dfsSearchMenu(cityA, cityB, cityC); break;
            case 5: aStarSearchMenu(cityA, cityB, cityC); break;
            case 6: searchComparisonMenu(cityA, cityB, cityC); break;
            case 7: cout << endl; break;
            default: cout << "Invalid choice. Please try again" << endl; break;
        }
    } while(choice != 7);
}

//LINEAR SEARCH MENU

void linearSearchMenu(Node* cityA, Node* cityB, Node* cityC){
    Node* lists[3] = {cityA, cityB, cityC};
    string names[3] = {"City A", "City B", "City C"};

    printSeparator(57, '-');
    cout << "Linear Search" << endl;
    int cityChoice = selectCityExtended();
    if(cityChoice == 5) { cout << endl; return; }

    //collect search filters
    int ageGroup; string targetMode; int distThreshold; char distOp;
    selectSearchCriteria(ageGroup, targetMode, distThreshold, distOp);
    string label = buildSearchLabel("Linear", ageGroup, targetMode, distThreshold, distOp);

    //resolve search list
    string datasetName;
    bool needsFree;
    Node* searchList = resolveSearchList(lists, names, cityChoice, datasetName, needsFree);

    //run unsorted linear search
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

    //prepare a sorted copy for sorted linear search
    Node* sortedList = NULL;
    bool freeSortedList = false;

    if(isValidAgeGroup(ageGroup)){
        sortedList = sortListCopyByField(searchList, "age");
        freeSortedList = true;
    }else if(!targetMode.empty()){
        sortedList = sortListCopyByField(searchList, "transport");
        freeSortedList = true;
    }else if(isValidDistOp(distOp)){
        sortedList = sortListCopyByField(searchList, "distance");
        freeSortedList = true;
    }

    if(!freeSortedList){
        sortedList = searchList; //no filter active, use original
    }

    //run sorted linear search
    MemoryMetrics::reset();
    timer = startTimer();
    linearSearchCombined_Sorted(sortedList, ageGroup, targetMode, distThreshold, distOp, matchedHead, matchCount);
    elapsedMs = stopTimer(timer);
    memUsed = MemoryMetrics::trackMemoryUsage();
    printSearchResults(matchedHead, matchCount, "[SORTED LINEAR] " + label, datasetName);
    printPerformance(elapsedMs, memUsed);
    freeList(matchedHead);
    
    if(freeSortedList) freeList(sortedList);
    if(needsFree) freeList(searchList);
}

//BINARY SEARCH MENU

void binarySearchMenu(Node* cityA, Node* cityB, Node* cityC) {
    Node* lists[3] = {cityA, cityB, cityC};
    string names[3] = {"City A", "City B", "City C"};

    int choice;
    do {
        printSeparator(57, '-');
        cout << "Binary Search" << endl;
        int cityChoice = selectCityExtended();
        if(cityChoice == 5) { cout << endl; return; }

        string datasetName;
        bool needsFree;
        Node* searchList = resolveSearchList(lists, names, cityChoice, datasetName, needsFree);

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
            if(needsFree) freeList(searchList);
            cout << endl;
            break;
        }
        if(choice < 1 || choice > 3) {
            if(needsFree) freeList(searchList);
            cout << "Invalid choice. Please try again" << endl;
            continue;
        }

        Node* unsortedHead = NULL, *sortedHead = NULL;
        int unsortedCount = 0, sortedCount = 0;
        string label;

        if(choice == 1) {
            //binary search by age group
            int minAge, maxAge;
            int grp = selectAgeGroup(minAge, maxAge);
            label = "Age Group = " + getAgeGroupLabel(grp - 1);

            //unsorted binary search
            MemoryMetrics::reset();
            auto timer = startTimer();
            binarySearchByAgeGroup_Unsorted(searchList, minAge, maxAge, unsortedHead, unsortedCount);
            double elapsedMs = stopTimer(timer);
            size_t memUsed = MemoryMetrics::trackMemoryUsage();
            printSearchResults(unsortedHead, unsortedCount, "[UNSORTED BINARY] " + label, datasetName);
            printPerformance(elapsedMs, memUsed);
            freeList(unsortedHead);

            //sorted binary search
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

            MemoryMetrics::reset();
            auto timer = startTimer();
            binarySearchByTransport_Unsorted(searchList, mode, unsortedHead, unsortedCount);
            double elapsedMs = stopTimer(timer);
            size_t memUsed = MemoryMetrics::trackMemoryUsage();
            printSearchResults(unsortedHead, unsortedCount, "[UNSORTED BINARY] " + label, datasetName);
            printPerformance(elapsedMs, memUsed);
            freeList(unsortedHead);

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

            MemoryMetrics::reset();
            auto timer = startTimer();
            binarySearchByDistance_Unsorted(searchList, target, unsortedHead, unsortedCount);
            double elapsedMs = stopTimer(timer);
            size_t memUsed = MemoryMetrics::trackMemoryUsage();
            printSearchResults(unsortedHead, unsortedCount, "[UNSORTED BINARY] " + label, datasetName);
            printPerformance(elapsedMs, memUsed);
            freeList(unsortedHead);

            MemoryMetrics::reset();
            timer = startTimer();
            binarySearchByDistance_Sorted(searchList, target, sortedHead, sortedCount);
            elapsedMs = stopTimer(timer);
            memUsed = MemoryMetrics::trackMemoryUsage();
            printSearchResults(sortedHead, sortedCount, "[SORTED BINARY] " + label, datasetName);
            printPerformance(elapsedMs, memUsed);
            freeList(sortedHead);
        }

        if(needsFree) freeList(searchList);

    } while(choice != 4);
}

//BFS SEARCH MENU

void bfsSearchMenu(Node* cityA, Node* cityB, Node* cityC) {
    Node* lists[3] = {cityA, cityB, cityC};
    string names[3] = {"City A", "City B", "City C"};

    printSeparator(57, '-');
    cout << "BFS Tree Search" << endl;
    int cityChoice = selectCityExtended();
    if(cityChoice == 5) { cout << endl; return; }

    int ageGroup; string targetMode; int distThreshold; char distOp;
    selectSearchCriteria(ageGroup, targetMode, distThreshold, distOp);

    string datasetName;
    bool needsFree;
    Node* searchList = resolveSearchList(lists, names, cityChoice, datasetName, needsFree);

    Node* matchedHead = NULL;
    int matchCount = 0;

    //unsorted BFS
    MemoryMetrics::reset();
    auto timer = startTimer();
    bfsSearchUnsorted(searchList, datasetName, ageGroup, targetMode, distThreshold, distOp, matchedHead, matchCount);
    double elapsedMs = stopTimer(timer);
    size_t memUsed = MemoryMetrics::trackMemoryUsage();
    string label = buildSearchLabel("BFS", ageGroup, targetMode, distThreshold, distOp);
    printSearchResults(matchedHead, matchCount, "[UNSORTED BFS] " + label, datasetName);
    printPerformance(elapsedMs, memUsed);
    freeList(matchedHead);
    matchedHead = NULL;
    matchCount = 0;

    //sorted BFS
    MemoryMetrics::reset();
    timer = startTimer();
    bfsSearchSorted(searchList, datasetName, ageGroup, targetMode, distThreshold, distOp, matchedHead, matchCount);
    elapsedMs = stopTimer(timer);
    memUsed = MemoryMetrics::trackMemoryUsage();
    printSearchResults(matchedHead, matchCount, "[SORTED BFS] " + label, datasetName);
    printPerformance(elapsedMs, memUsed);
    freeList(matchedHead);

    if(needsFree) freeList(searchList);
}

//DFS SEARCH MENU

void dfsSearchMenu(Node* cityA, Node* cityB, Node* cityC) {
    Node* lists[3] = {cityA, cityB, cityC};
    string names[3] = {"City A", "City B", "City C"};

    printSeparator(57, '-');
    cout << "DFS Tree Search" << endl;
    int cityChoice = selectCityExtended();
    if(cityChoice == 5) { cout << endl; return; }

    int ageGroup; string targetMode; int distThreshold; char distOp;
    selectSearchCriteria(ageGroup, targetMode, distThreshold, distOp);

    string datasetName;
    bool needsFree;
    Node* searchList = resolveSearchList(lists, names, cityChoice, datasetName, needsFree);

    Node* matchedHead = NULL;
    int matchCount = 0;

    //unsorted DFS
    MemoryMetrics::reset();
    auto timer = startTimer();
    dfsSearchUnsorted(searchList, datasetName, ageGroup, targetMode, distThreshold, distOp, matchedHead, matchCount);
    double elapsedMs = stopTimer(timer);
    size_t memUsed = MemoryMetrics::trackMemoryUsage();
    string label = buildSearchLabel("DFS", ageGroup, targetMode, distThreshold, distOp);
    printSearchResults(matchedHead, matchCount, "[UNSORTED DFS] " + label, datasetName);
    printPerformance(elapsedMs, memUsed);
    freeList(matchedHead);
    matchedHead = NULL;
    matchCount = 0;

    //sorted DFS
    MemoryMetrics::reset();
    timer = startTimer();
    dfsSearchSorted(searchList, datasetName, ageGroup, targetMode, distThreshold, distOp, matchedHead, matchCount);
    elapsedMs = stopTimer(timer);
    memUsed = MemoryMetrics::trackMemoryUsage();
    printSearchResults(matchedHead, matchCount, "[SORTED DFS] " + label, datasetName);
    printPerformance(elapsedMs, memUsed);
    freeList(matchedHead);

    if(needsFree) freeList(searchList);
}

//A* SEARCH MENU
void aStarSearchMenu(Node* cityA, Node* cityB, Node* cityC) {
    Node*  lists[3] = { cityA,    cityB,    cityC    };
    string names[3] = { "City A", "City B", "City C" };

    do {
        printSeparator(60, '-');
        cout << "A* Search" << endl;
        int cityChoice = selectCityExtended(); //with back, default City A

        if(cityChoice == 5) { cout << endl; break; }

        int ageGroup; string transport; int distThreshold; char distOp;
        selectSearchCriteria(ageGroup, transport, distThreshold, distOp);

        string criteria = buildSearchLabel("", ageGroup, transport, distThreshold, distOp);
        if(criteria.empty()) criteria = "No filter";

        cout << endl;

        int start = (cityChoice == 4) ? 0 : cityChoice - 1;
        int end   = (cityChoice == 4) ? 2 : cityChoice - 1;

        for(int i = start; i <= end; i++) {
            Node* matchedHead = NULL;
            int matchCount = 0;

            //unsorted A*
            MemoryMetrics::reset();
            auto timer = startTimer();
            aStarSearchUnsorted(lists[i], ageGroup, transport, distThreshold, distOp, matchedHead, matchCount);
            double elapsedMs = stopTimer(timer);
            size_t memUsed = MemoryMetrics::trackMemoryUsage();
            printSearchResults(matchedHead, matchCount, "[UNSORTED A*] " + criteria, names[i]);
            printPerformance(elapsedMs, memUsed);
            freeList(matchedHead);
            matchedHead = NULL;
            matchCount = 0;

            //sorted A*
            MemoryMetrics::reset();
            timer = startTimer();
            aStarSearchSorted(lists[i], ageGroup, transport, distThreshold, distOp, matchedHead, matchCount);
            elapsedMs = stopTimer(timer);
            memUsed = MemoryMetrics::trackMemoryUsage();
            printSearchResults(matchedHead, matchCount, "[SORTED A*] " + criteria, names[i]);
            printPerformance(elapsedMs, memUsed);
            freeList(matchedHead);
        }

    } while(true);
}

//EARCH COMPARISON MENU
//runs a selected search algorithm 3 times each for unsorted and sorted,
//records time (microseconds) and memory per iteration, then computes the average

void searchComparisonMenu(Node* cityA, Node* cityB, Node* cityC) {
    Node* lists[3] = {cityA, cityB, cityC};
    string names[3] = {"City A", "City B", "City C"};

    int choice;
    do {
        printSeparator(57, '-');
        cout << "Search Comparison" << endl;
        cout << "\nSelect algorithm:" << endl;
        cout << "1. Linear Search" << endl;
        cout << "2. Binary Search (by Age Group)" << endl;
        cout << "3. Binary Search (by Transport Mode)" << endl;
        cout << "4. Binary Search (by Daily Distance)" << endl;
        cout << "5. BFS Search" << endl;
        cout << "6. DFS Search" << endl;
        cout << "7. A* Search" << endl;
        cout << "8. Back" << endl;
        printSeparator(57, '-');
        cout << "Select algorithm: ";
        cin >> choice;

        if(choice == 8) { cout << endl; break; }
        if(choice < 1 || choice > 7) {
            cout << "Invalid choice. Please try again." << endl;
            continue;
        }

        //select city
        int cityChoice = selectCityExtended(false, 1); //no back option, default to City A

        //build the search list using helper
        string datasetName;
        bool needsFree;
        Node* searchList = resolveSearchList(lists, names, cityChoice, datasetName, needsFree);

        //collect search criteria based on algorithm type
        int ageGroup = 0;
        string targetMode = "";
        int distThreshold = 0;
        char distOp = '0';
        int minAge = -1, maxAge = -1;
        int exactDist = 0;

        if(choice == 1 || choice == 5 || choice == 6 || choice == 7) {
            //linear, BFS, DFS, A* use combined filters
            selectSearchCriteria(ageGroup, targetMode, distThreshold, distOp);
        } else if(choice == 2) {
            //binary search by age group
            selectAgeGroup(minAge, maxAge);
        } else if(choice == 3) {
            //binary search by transport mode
            cout << "\nEnter transport mode to search: ";
            cin.ignore();
            getline(cin, targetMode);
        } else if(choice == 4) {
            //binary search by daily distance
            cout << "\nEnter exact daily distance to search: ";
            cin >> exactDist;
        }

        //build algorithm name for display
        string algoNames[] = {"", "Linear search", "Binary search (Age)", "Binary search (Transport)", "Binary search (Distance)", "BFS search", "DFS search", "A* search"};
        string algoName = algoNames[choice];

        string iterNames[3] = {"First Iteration", "Second Iteration", "Third Iteration"};
        int width = 75;

        //run both unsorted and sorted
        for(int s = 0; s < 2; s++) {
            bool useSorted = (s == 1);
            string sortLabel = useSorted ? "Sorted" : "Unsorted";

            //prepare sorted list for linear search sorted variant
            Node* sortedList = NULL;
            bool freeSortedList = false;

            if(useSorted && choice == 1) {
                if(isValidAgeGroup(ageGroup)) {
                    sortedList = sortListCopyByField(searchList, "age");
                } else if(!targetMode.empty()) {
                    sortedList = sortListCopyByField(searchList, "transport");
                } else if(isValidDistOp(distOp)) {
                    sortedList = sortListCopyByField(searchList, "distance");
                } else {
                    sortedList = searchList;
                }
                if(sortedList != searchList) freeSortedList = true;
            }

            //run 3 iterations
            double times[3];
            size_t mems[3];

            for(int run = 0; run < 3; run++) {
                Node* matchedHead = NULL;
                int matchCount = 0;

                MemoryMetrics::reset();
                auto timer = startTimer();

                //call the appropriate search function
                if(choice == 1) {
                    if(useSorted) {
                        linearSearchCombined_Sorted(sortedList, ageGroup, targetMode, distThreshold, distOp, matchedHead, matchCount);
                    } else {
                        linearSearchCombined_Unsorted(searchList, ageGroup, targetMode, distThreshold, distOp, matchedHead, matchCount);
                    }
                } else if(choice == 2) {
                    if(useSorted) {
                        binarySearchByAgeGroup_Sorted(searchList, minAge, maxAge, matchedHead, matchCount);
                    } else {
                        binarySearchByAgeGroup_Unsorted(searchList, minAge, maxAge, matchedHead, matchCount);
                    }
                } else if(choice == 3) {
                    if(useSorted) {
                        binarySearchByTransport_Sorted(searchList, targetMode, matchedHead, matchCount);
                    } else {
                        binarySearchByTransport_Unsorted(searchList, targetMode, matchedHead, matchCount);
                    }
                } else if(choice == 4) {
                    if(useSorted) {
                        binarySearchByDistance_Sorted(searchList, exactDist, matchedHead, matchCount);
                    } else {
                        binarySearchByDistance_Unsorted(searchList, exactDist, matchedHead, matchCount);
                    }
                } else if(choice == 5) {
                    if(useSorted) {
                        bfsSearchSorted(searchList, datasetName, ageGroup, targetMode, distThreshold, distOp, matchedHead, matchCount);
                    } else {
                        bfsSearchUnsorted(searchList, datasetName, ageGroup, targetMode, distThreshold, distOp, matchedHead, matchCount);
                    }
                } else if(choice == 6) {
                    if(useSorted) {
                        dfsSearchSorted(searchList, datasetName, ageGroup, targetMode, distThreshold, distOp, matchedHead, matchCount);
                    } else {
                        dfsSearchUnsorted(searchList, datasetName, ageGroup, targetMode, distThreshold, distOp, matchedHead, matchCount);
                    }
                } else if(choice == 7) {
                    if(useSorted) {
                        aStarSearchSorted(searchList, ageGroup, targetMode, distThreshold, distOp, matchedHead, matchCount);
                    } else {
                        aStarSearchUnsorted(searchList, ageGroup, targetMode, distThreshold, distOp, matchedHead, matchCount);
                    }
                }

                times[run] = stopTimerMicro(timer);
                mems[run] = MemoryMetrics::trackMemoryUsage();

                freeList(matchedHead);
            }

            //compute averages
            double avgTime = (times[0] + times[1] + times[2]) / 3.0;
            double avgMem = (mems[0] + mems[1] + mems[2]) / 3.0;

            //print comparison table
            cout << endl;
            printSeparator(width, '=');
            cout << "Average Execution Time & Auxiliary Memory Usage: " << algoName << " (" << sortLabel << ")" << endl;
            printSeparator(width, '=');
            cout << left << setw(25) << "Iteration"
                 << setw(25) << "Time (microseconds)"
                 << setw(25) << "Auxiliary Memory"
                 << endl;
            printSeparator(width, '-');

            for(int run = 0; run < 3; run++) {
                cout << left << setw(25) << iterNames[run]
                     << setw(25) << fixed << setprecision(3) << times[run]
                     << setw(25) << mems[run]
                     << endl;
                printSeparator(width, '-');
            }

            cout << left << setw(25) << "Average:"
                 << setw(25) << fixed << setprecision(3) << avgTime
                 << setw(25) << fixed << setprecision(0) << avgMem
                 << endl;
            printSeparator(width, '=');

            if(freeSortedList && sortedList != NULL) {
                freeList(sortedList);
            }
        }

        cout << endl;

        if(needsFree) freeList(searchList);

    } while(choice != 8);
}
