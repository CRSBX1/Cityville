#include "header.hpp"
#include <iostream>
#include <chrono>
#include <limits>
#include <cmath>

searchAlgorithm::searchAlgorithm(resident* dataset1, resident* dataset2, resident* dataset3, enum SortKey BY_AGE,
    enum SortKey BY_DAILY_DISTANCE, enum SortKey BY_TRANSPORT_MODE, enum SortKey BY_AGE_TRANSPORT,
    enum SortKey BY_AGE_DISTANCE, enum SortKey BY_TRANSPORT_DISTANCE, enum SortKey BY_AGE_TRANSPORT_DISTANCE,
    sortAlgorithm sortAlgo) {

    //Allocate array
    allDatasets[0] = dataset1;
    allDatasets[1] = dataset2;
    allDatasets[2] = dataset3;

    //copy existing datasets to the newly created arrays;
    for(int i = 0; i < 3; i++)
    {
        for(int j = 0; j < arraySizes[i]; j++)
        {
            allDataByAgeSorted[i][j] = allDatasets[i][j];
            allDataByTransportSorted[i][j] = allDatasets[i][j];
            allDataByDistanceSorted[i][j] = allDatasets[i][j];
            allDataByAgeTransportSorted[i][j] = allDatasets[i][j];
            allDataByAgeDistanceSorted[i][j] = allDatasets[i][j];
            allDataByTransportDistanceSorted[i][j] = allDatasets[i][j];
            allDataByAgeTransportDistanceSorted[i][j] = allDatasets[i][j];
        }
    }

    //sort the arrays based on age
    for(int i=0;i<3;i++){
        sortAlgo.insertionSort(allDataByAgeSorted[i],arraySizes[i],BY_AGE);
    }

    //sort the arrays based on daily distance
    for(int i=0;i<3;i++){
        sortAlgo.insertionSort(allDataByDistanceSorted[i],arraySizes[i],BY_DAILY_DISTANCE);
    }

    //sort the arrays based on transport mode
    for(int i=0;i<3;i++){
        sortAlgo.insertionSort(allDataByTransportSorted[i],arraySizes[i],BY_TRANSPORT_MODE);
    }

    //sort the arrays based on age and transport mode
    for(int i=0;i<3;i++){
        sortAlgo.insertionSort(allDataByAgeTransportSorted[i],arraySizes[i],BY_AGE_TRANSPORT);
    }

    //sort the arrays based on age and daily distance
    for(int i=0;i<3;i++){
        sortAlgo.insertionSort(allDataByAgeDistanceSorted[i],arraySizes[i],BY_AGE_DISTANCE);
    }

    //sort the arrays based on transport mode and daily distance
    for(int i=0;i<3;i++){
        sortAlgo.insertionSort(allDataByTransportDistanceSorted[i],arraySizes[i],BY_TRANSPORT_DISTANCE);
    }

    //sort the arrays based on age, transport mode, and daily distance
    for(int i=0;i<3;i++){
        sortAlgo.insertionSort(allDataByAgeTransportDistanceSorted[i],arraySizes[i],BY_AGE_TRANSPORT_DISTANCE);
    }
}

void searchAlgorithm::chooseAlgorithm(){
    int searchMainInput;
    string datasetChoice;
    string searchCriteriaCondition;
    while(true)
    {
        peakMemory = 0;
        cout << sep << endl;
        cout << "Welcome to search menu:" << endl;
        cout << "1. Proceed" << endl;
        cout << "2. Exit" << endl;
        cout << "Enter your choice: ";
        cin >> searchMainInput; 

        if(searchMainInput > 2 || searchMainInput<1){
            cout << "Invalid input, try again";
            continue;
        }
        if(searchMainInput==2){
            break;
        }
        cout << sep << endl;
        cout << "Enter target dataset:" << endl;
        cout << "1. Unsorted data" << endl;
        cout << "2. Sorted data based on age" << endl;
        cout << "3. Sorted data based on daily distance" << endl;
        cout << "4. Sorted data based on transport mode" << endl;
        cout << "5. Sorted data based on age and transport mode" << endl;
        cout << "6. Sorted data based on age and daily distance" << endl;
        cout << "7. Sorted data based on transport mode and daily distance" << endl;
        cout << "8. Sorted data based on age, transport mode, and daily distance" << endl;
        cout << "Enter dataset: ";
        cin >> dataChoice;

        if(dataChoice > 8 || dataChoice<1){
            cout << "Invalid input, try again";
            continue;
        }
        cout << sep << endl;
        cout << "Choose your search algorithm: " << endl;
        cout << "1. Linear Search" << endl;
        cout << "2. Binary Search" << endl;
        cout << "3. Breadth-First Search (BFS)" << endl;
        cout << "4. Depth-First Search (DFS)" << endl;
        cout << "5. A* search" << endl;
        cout << "Enter your choice: ";
        cin >> userChoice;

        if(userChoice>5 || userChoice<1){
            cout << "Invalid input, try again";
            continue;
        }

        cout << sep << endl;
        cout << "Enter search criteria:" << endl;
        cout << "1. Age Group" << endl;
        cout << "2. Transport" << endl;
        cout << "3. Daily Distance" << endl;
        cout << "4. Daily Distance Threshold" << endl;
        cout << "5. Age Group & Transport" << endl;
        cout << "6. Age Group & Daily Distance" << endl;
        cout << "7. Daily Distance & Transport" << endl;
        cout << "8. Daily Distance & Age Group & Transport" << endl;
        cout << "Enter your choice: ";
        cin >> criteriaInputChoice;

        if(criteriaInputChoice<1 || criteriaInputChoice>8){
            cout << "Invalid input, try again";
            continue;
        }

        cout << sep << endl;
        cout << "Enter desired search result:" << endl;
        cout << "1. Specific key search (one data)" << endl;
        cout << "2. All matched data" << endl;
        cout << "Enter your choice: ";
        cin >> searchKey;

        if(searchKey<1 || searchKey>2){
            cout << "Invalid input, try again";
            continue;
        }

        switch(dataChoice){
            case 1:
                dataset = allDatasets;
                datasetChoice = "Unsorted data";
                break;
            case 2:
                dataset = allDataByAgeSorted;
                datasetChoice = "Sorted data (age)";
                break;
            case 3:
                dataset = allDataByDistanceSorted;
                datasetChoice = "Sorted data (daily distance)";
                break;
            case 4:
                dataset = allDataByTransportSorted;
                datasetChoice = "Sorted data (transport)";
                break;
            case 5:
                dataset = allDataByAgeTransportSorted;
                datasetChoice = "Sorted data (age and transport)";
                break;
            case 6:
                dataset = allDataByAgeDistanceSorted;
                datasetChoice = "Sorted data (age and daily distance)";
                break;
            case 7:
                dataset = allDataByTransportDistanceSorted;
                datasetChoice = "Sorted data (transport and daily distance)";
                break;
            case 8:
                dataset = allDataByAgeTransportDistanceSorted;
                datasetChoice = "Sorted data (age, transport, and daily distance)";
                break;
        }

        enum criteria criteriaEnum = criteriaChoice(criteriaInputChoice, ageGroup, transport, dailyDistance, searchCriteriaCondition);
        if(userChoice == 1){
            before =  memTracker.trackMemoryUsage();
            auto start = chrono::high_resolution_clock::now();
            linearSearch(dataset, arraySizes, criteriaEnum);
            auto end = chrono::high_resolution_clock::now();
            exTimeAlgo = chrono::duration<double, micro>(end - start).count();
            after = memTracker.trackMemoryUsage();
            usedMemory = after - before;
            printFinalResult("Linear search", datasetChoice, searchCriteriaCondition, before, after, usedMemory);

        }
        else if(userChoice == 2){
            before = memTracker.trackMemoryUsage();
            auto start = chrono::high_resolution_clock::now();
            binarySearch(dataset, criteriaEnum, arraySizes);
            auto end = chrono::high_resolution_clock::now();
            exTimeAlgo = chrono::duration<double, micro>(end - start).count();
            after = memTracker.trackMemoryUsage();
            usedMemory = after - before;
            printFinalResult("Binary search", datasetChoice, searchCriteriaCondition, before, after, usedMemory);
        }
        else if(userChoice == 3){
            before = memTracker.trackMemoryUsage();
            auto start = chrono::high_resolution_clock::now();
            breadthFirstSearch(dataset, arraySizes, criteriaEnum);
            auto end = chrono::high_resolution_clock::now();
            exTimeAlgo = chrono::duration<double, micro>(end - start).count();
            after = memTracker.trackMemoryUsage();
            usedMemory = after - before;
            printFinalResult("Breadth-first search (BFS)", datasetChoice, searchCriteriaCondition, before, after, usedMemory);
        }
        else if(userChoice == 4){
            before = memTracker.trackMemoryUsage();
            auto start = chrono::high_resolution_clock::now();
            depthFirstSearch(dataset, arraySizes, criteriaEnum);
            auto end = chrono::high_resolution_clock::now();
            exTimeAlgo = chrono::duration<double, micro>(end - start).count();
            after = memTracker.trackMemoryUsage();
            usedMemory = after - before;
            printFinalResult("Depth-first search (DFS)", datasetChoice, searchCriteriaCondition, before, after, usedMemory);
        }
        else if(userChoice == 5){
            before = memTracker.trackMemoryUsage();
            auto start = chrono::high_resolution_clock::now();
            aStarSearch(dataset, arraySizes, criteriaEnum);
            auto end = chrono::high_resolution_clock::now();
            exTimeAlgo = chrono::duration<double, micro>(end - start).count();
            after = memTracker.trackMemoryUsage();
            usedMemory = after - before;
            printFinalResult("A* search", datasetChoice, searchCriteriaCondition, before, after, usedMemory);
        }
        
    }

}

enum searchAlgorithm::criteria searchAlgorithm::criteriaChoice(int choice, string& ageGroup,
    string& transport, float& dailyDistance, string& searchCriteria){
    enum criteria userChoiceEnum;
    if(choice == 1){
        userChoiceEnum = AGE_GROUP;
        searchCriteria = "Age group";
    }
    else if(choice == 2){
        userChoiceEnum = TRANSPORT;
        searchCriteria = "Transport";
    }
    else if(choice == 3){
        userChoiceEnum = DAILY_DISTANCE;
        searchCriteria = "Daily distance";
    }
    else if(choice == 4){
        int thresholdChoice;
        cout << sep << endl;
        cout << "Choose threshold direction:" << endl;
        cout << "1. Greater than" << endl;
        cout << "2. Less than" << endl;
        cout << "Enter your choice: ";
        cin >> thresholdChoice;
        if(thresholdChoice == 1){
            userChoiceEnum = DAILY_DISTANCE_GREATER;
            searchCriteria = "Daily distance threshold (greater than)";
        }
        else{
            userChoiceEnum = DAILY_DISTANCE_LESS;
            searchCriteria = "Daily distance threshold (less than)";
        }
    }
    else if(choice == 5){
        userChoiceEnum = AGE_TRANSPORT;
        searchCriteria = "Age group and transport";
    }
    else if(choice == 6){
        userChoiceEnum = AGE_DISTANCE;
        searchCriteria = "Age group and daily distance";
    }
    else if(choice == 7){
        userChoiceEnum = TRANSPORT_DISTANCE;
        searchCriteria = "Transport and daily distance";
    }
    else{
        userChoiceEnum = AGE_TRANSPORT_DISTANCE;
        searchCriteria = "Age group, transport, and daily distance";
    }
    
    auto selectAgeGroup = [&]() {
        while(true){
        int ageMenuChoice;
        cout << "Select age group:" << endl;
        cout << "1. Children & Teenagers" << endl;
        cout << "2. University Students/Young Adults" << endl;
        cout << "3. Working Adults (Early Career)" << endl;
        cout << "4. Working Adults (Late Career)" << endl;
        cout << "5. Senior Citizens/Retirees" << endl;
        cout << "Enter choice: ";
        cin >> ageMenuChoice;
        switch (ageMenuChoice) {
            case 1: return string("Children & Teenagers");
            case 2: return string("University Students/Young Adults");
            case 3: return string("Working Adults (Early Career)");
            case 4: return string("Working Adults (Late Career)");
            case 5: return string("Senior Citizens/Retirees");
            default:
                cout << "Invalid age group selection, try again." << endl;
                continue;
            }
        }
    };

    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    switch(userChoiceEnum){
        case AGE_GROUP:
            ageGroup = selectAgeGroup();
            break;
        case TRANSPORT:
            cout << "Enter transport mode: ";
            getline(cin, transport);
            break;
        case DAILY_DISTANCE:
            cout << "Enter daily distance: ";
            cin >> dailyDistance;
            break;
        case DAILY_DISTANCE_GREATER:
            cout << "Enter daily distance threshold: ";
            cin >> dailyDistance;
            break;
        case DAILY_DISTANCE_LESS:
            cout << "Enter daily distance threshold: ";
            cin >> dailyDistance;
            break;
        case AGE_TRANSPORT:
            ageGroup = selectAgeGroup();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cout << "Enter transport mode: ";
            getline(cin, transport);
            break;
        case AGE_DISTANCE:
            ageGroup = selectAgeGroup();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cout << "Enter daily distance: ";
            cin >> dailyDistance;
            break;
        case TRANSPORT_DISTANCE:
            cout << "Enter transport mode: ";
            getline(cin, transport);
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cout << "Enter daily distance: ";
            cin >> dailyDistance;
            break;
        case AGE_TRANSPORT_DISTANCE:
            ageGroup = selectAgeGroup();
            cout << "Enter transport mode: ";
            getline(cin, transport);
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cout << "Enter daily distance: ";
            cin >> dailyDistance;
            break;
    }
    return userChoiceEnum;
}

void searchAlgorithm::linearSearch(resident* arrayData[], int subarraySize[], enum criteria searchCriteria){
    int arraySize = 3;
    int dataSize;
    for(int j = 0; j<arraySize; j++){
        searchResult = 0;
        bool flag = true;
        int counter = 0;
        dataSize = subarraySize[j];
        auto start = chrono::high_resolution_clock::now();
        for(int i=0; i<dataSize && flag==true; i++){
            switch(searchCriteria){
                case AGE_GROUP:
                    if(matchesCriteria(arrayData[j][i], searchCriteria, dailyDistance)){
                        setSearchResults(i, structArr[j], flag, counter);
                    }
                    break;
                case TRANSPORT:
                    if(matchesCriteria(arrayData[j][i], searchCriteria, dailyDistance)){
                        setSearchResults(i, structArr[j], flag, counter);
                    }
                    break;
                case DAILY_DISTANCE:
                    if(matchesCriteria(arrayData[j][i], searchCriteria, dailyDistance)){
                        setSearchResults(i, structArr[j], flag, counter);
                    }
                    break;
                case AGE_TRANSPORT:
                    if(matchesCriteria(arrayData[j][i], searchCriteria, dailyDistance)){
                        setSearchResults(i, structArr[j], flag, counter);
                    }
                    break;
                case AGE_DISTANCE:
                    if(matchesCriteria(arrayData[j][i], searchCriteria, dailyDistance)){
                        setSearchResults(i, structArr[j], flag, counter);
                    }
                    break;
                case DAILY_DISTANCE_GREATER:
                    if(matchesCriteria(arrayData[j][i], searchCriteria, dailyDistance)){
                        setSearchResults(i, structArr[j], flag, counter);
                    }
                    break;
                case DAILY_DISTANCE_LESS:
                    if(matchesCriteria(arrayData[j][i], searchCriteria, dailyDistance)){
                        setSearchResults(i, structArr[j], flag, counter);
                    }
                    break;
                case TRANSPORT_DISTANCE:
                    if(matchesCriteria(arrayData[j][i], searchCriteria, dailyDistance)){
                        setSearchResults(i, structArr[j], flag, counter);
                    }
                    break;
                case AGE_TRANSPORT_DISTANCE:
                    if(matchesCriteria(arrayData[j][i], searchCriteria, dailyDistance)){
                        setSearchResults(i, structArr[j], flag, counter);
                    }
                    break;
            }
        }
        auto end = chrono::high_resolution_clock::now();
        structArr[j].time = chrono::duration<double, micro>(end-start).count(); 
        structArr[j].amount = searchResult;
        peakMemory += memTracker.trackMemoryUsage() - before;
    }
}

void searchAlgorithm::binarySearch(resident* arrayData[], enum criteria searchCriteria, int subarraySize[]) {
    int left, right, mid, age, resultCount, t, minAge, maxAge, encode;
    float d;

    encode = getEncode(transport);
    minAge = getMinAge(ageGroup);
    maxAge = getMaxAge(ageGroup);

    for (int j = 0; j < 3; j++) {
        resultCount = 0;
        structArr[j].amount = 0;
        resident* arr = arrayData[j];
        left = 0;
        right = subarraySize[j] - 1;
        auto start = chrono::high_resolution_clock::now();
        switch (searchCriteria) {
            case AGE_GROUP:
            while (left <= right) {
                mid = left + (right - left) / 2;
                age = arr[mid].getAge();
                if (matchesCriteria(arr[mid], searchCriteria, dailyDistance)){
                    setBinarySearchResults(structArr[j], mid, arr, subarraySize[j],
                                searchCriteria, dailyDistance,
                                resultCount);
                    break;
                }
                else if (age < minAge){
                    left = mid + 1;
                }
                else{
                    right = mid - 1;
                }
            }
            break;
        case TRANSPORT:
            while (left <= right) {
                mid = left + (right - left) / 2;
                t = arr[mid].getTransportEncode();

                if (matchesCriteria(arr[mid], searchCriteria, dailyDistance)){
                    setBinarySearchResults(structArr[j], mid, arr, subarraySize[j],
                                searchCriteria, dailyDistance,
                                resultCount);
                    break;
                }
                else if (t < encode){
                    left = mid + 1;
                }
                else{
                    right = mid - 1;
                }
            }
            break;
        case DAILY_DISTANCE:
            while (left <= right) {
                mid = left + (right - left) / 2;
                d = arr[mid].getDistance();

                if (matchesCriteria(arr[mid], searchCriteria, dailyDistance)){
                    setBinarySearchResults(structArr[j], mid, arr, subarraySize[j],
                                searchCriteria, dailyDistance,
                                resultCount);
                    break;
                }
                else if (d < dailyDistance){
                    left = mid + 1;
                }
                else{
                    right = mid - 1;
                }
            }
            break;
        case DAILY_DISTANCE_GREATER:
            while (left <= right) {
                mid = left + (right - left) / 2;
                d = arr[mid].getDistance();

                if (matchesCriteria(arr[mid], searchCriteria, dailyDistance)){
                    setBinarySearchResults(structArr[j], mid, arr, subarraySize[j],
                                searchCriteria, dailyDistance,
                                resultCount);
                    break;
                }
                else {
                    left = mid + 1;
                }
            }
            break;
        case DAILY_DISTANCE_LESS:
            while (left <= right) {
                mid = left + (right - left) / 2;
                d = arr[mid].getDistance();

                if (matchesCriteria(arr[mid], searchCriteria, dailyDistance)){
                    setBinarySearchResults(structArr[j], mid, arr, subarraySize[j],
                                searchCriteria, dailyDistance,
                                resultCount);
                    break;
                }
                else {
                    right = mid - 1;
                }
            }
            break;
        case AGE_TRANSPORT:
            while (left <= right) {
                mid = left + (right - left) / 2;

                age = arr[mid].getAge();
                t = arr[mid].getTransportEncode();

                if (/*age == minAge && t == encode*/ matchesCriteria(arr[mid], searchCriteria, dailyDistance)){
                    setBinarySearchResults(structArr[j], mid, arr, subarraySize[j],
                                searchCriteria, dailyDistance,
                                resultCount);
                    break;
                }
                else if (age < minAge || (age == minAge && t < encode)){
                    left = mid + 1;
                }
                else{
                    right = mid - 1;
                }
            }
            break;
        case AGE_DISTANCE:
            while (left <= right) {
                mid = left + (right - left) / 2;

                age = arr[mid].getAge();
                d = arr[mid].getDistance();

                if (/*age == minAge && d == dailyDistance*/ matchesCriteria(arr[mid], searchCriteria, dailyDistance)){
                    setBinarySearchResults(structArr[j], mid, arr, subarraySize[j],
                                searchCriteria, dailyDistance,
                                resultCount);
                    break;
                }
                else if (age < minAge || (age == minAge && d < dailyDistance)){
                    left = mid + 1;
                }
                else{
                    right = mid - 1;
                }
            }
            break;
        case TRANSPORT_DISTANCE:
            while (left <= right) {
                mid = left + (right - left) / 2;

                t = arr[mid].getTransportEncode();
                d = arr[mid].getDistance();

                if (/*t == encode && d == dailyDistance*/ matchesCriteria(arr[mid], searchCriteria, dailyDistance)){
                    setBinarySearchResults(structArr[j], mid, arr, subarraySize[j],
                                searchCriteria, dailyDistance,
                                resultCount);
                   break;
                }
                else if (t < encode || (t == encode && d < dailyDistance)){
                    left = mid + 1;
                }
                else{
                    right = mid - 1;
                }
            }
            break;
        case AGE_TRANSPORT_DISTANCE:
            while (left <= right) {
                mid = left + (right - left) / 2;

                age = arr[mid].getAge();
                t = arr[mid].getTransportEncode();
                d = arr[mid].getDistance();

                if (matchesCriteria(arr[mid], searchCriteria, dailyDistance)){
                    setBinarySearchResults(structArr[j], mid, arr, subarraySize[j],
                                searchCriteria, dailyDistance,
                                resultCount);
                    break;
                }
                else if (age < minAge ||
                        (age == minAge && t < encode) ||
                        (age == minAge && t == encode && d < dailyDistance)){
                    left = mid + 1;
                }
                else{
                    right = mid - 1;
                }
            }
            break;
        }
        auto end = chrono::high_resolution_clock::now();
        structArr[j].time = chrono::duration<double, micro>(end-start).count(); 
        peakMemory += memTracker.trackMemoryUsage() - before;
    }
}

bool searchAlgorithm::matchesCriteria(resident& r, enum criteria searchCriteria, float dailyDistance){
    int encode, minAge, maxAge;

    // transport encoding
    if (transport == "Car") encode = 0;
    else if (transport == "Bus") encode = 1;
    else if (transport == "Bicycle") encode = 2;
    else if (transport == "Walking") encode = 3;
    else if (transport == "School Bus") encode = 4;
    else if (transport == "Carpool") encode = 5;

    // age group range
    if (ageGroup == "Children & Teenagers") { minAge = 0; maxAge = 17; }
    else if (ageGroup == "University Students/Young Adults") { minAge = 18; maxAge = 25; }
    else if (ageGroup == "Working Adults (Early Career)") { minAge = 26; maxAge = 45; }
    else if (ageGroup == "Working Adults (Late Career)") { minAge = 46; maxAge = 60; }
    else if (ageGroup == "Senior Citizens/Retirees") { minAge = 61; maxAge = 100; }
    
    switch(searchCriteria){
        case AGE_GROUP:
            return r.getAge() >= minAge && r.getAge() <= maxAge;
        case TRANSPORT:
            return r.getTransportEncode() == encode;
        case DAILY_DISTANCE:
            return r.getDistance() == dailyDistance;
        case DAILY_DISTANCE_GREATER:
            return r.getDistance() > dailyDistance;
        case DAILY_DISTANCE_LESS:
            return r.getDistance() < dailyDistance;
        case AGE_TRANSPORT:
            return r.getAge() >= minAge && r.getAge() <= maxAge && r.getTransportEncode() == encode;
        case AGE_DISTANCE:
            return r.getAge() >= minAge && r.getAge() <= maxAge && r.getDistance() == dailyDistance;
        case TRANSPORT_DISTANCE:
            return r.getTransportEncode() == encode && r.getDistance() == dailyDistance;
        case AGE_TRANSPORT_DISTANCE:
            return r.getAge() >= minAge && r.getAge() <= maxAge && r.getTransportEncode() == encode && r.getDistance() == dailyDistance;
        default:
            return false;
    }
}

float searchAlgorithm::estimateHeuristic(resident& r, enum criteria searchCriteria, float dailyDistance){
    int minAge, maxAge, encode;
    encode = getEncode(transport);
    minAge = getMinAge(ageGroup);
    maxAge = getMaxAge(ageGroup);
    float score = 0.0f;
    switch(searchCriteria){
        case AGE_GROUP:
            if (r.getAge() < minAge) score = minAge - r.getAge();
            else if (r.getAge() > maxAge) score = r.getAge() - maxAge;
            else score = 0.0f;
            break;
        case TRANSPORT:
            score = (r.getTransportEncode() == encode) ? 0.0f : 1.0f;
            break;
        case DAILY_DISTANCE:
            score = fabs(r.getDistance() - dailyDistance);
            break;
        case DAILY_DISTANCE_GREATER:
            score = (r.getDistance() > dailyDistance) ? 0.0f : dailyDistance - r.getDistance();
            break;
        case DAILY_DISTANCE_LESS:
            score = (r.getDistance() < dailyDistance) ? 0.0f : r.getDistance() - dailyDistance;
            break;
        case AGE_TRANSPORT:
            score = 0.0f;
            if (r.getAge() < minAge) score += minAge - r.getAge();
            else if (r.getAge() > maxAge) score += r.getAge() - maxAge;
            score += (r.getTransportEncode() == encode) ? 0.0f : 1.0f;
            break;
        case AGE_DISTANCE:
            if (r.getAge() < minAge) score += minAge - r.getAge();
            else if (r.getAge() > maxAge) score += r.getAge() - maxAge;
            score += fabs(r.getDistance() - dailyDistance);
            break;
        case TRANSPORT_DISTANCE:
            score = (r.getTransportEncode() == encode) ? 0.0f : 1.0f;
            score += fabs(r.getDistance() - dailyDistance);
            break;
        case AGE_TRANSPORT_DISTANCE:
            if (r.getAge() < minAge) score += minAge - r.getAge();
            else if (r.getAge() > maxAge) score += r.getAge() - maxAge;
            score += (r.getTransportEncode() == encode) ? 0.0f : 1.0f;
            score += fabs(r.getDistance() - dailyDistance);
            break;
        default:
            score = 0.0f;
    }
    return score;
}

void searchAlgorithm::breadthFirstSearch(resident* arrayData[], int subarraySize[], enum criteria searchCriteria){

    for (int j = 0; j < 3; j++) {
        int dataSize = subarraySize[j];
        int counter = 0;
        bool flag = true;
        int* queue =  new int[200];
        bool* visited = new bool[200]{false};
        int head = 0, tail = 0;
        searchResult = 0;
        if (dataSize > 0) {
            queue[tail++] = 0;
        }

        auto start = chrono::high_resolution_clock::now();
        while (head < tail && (searchKey == 2 || flag)) {
            int idx = queue[head++];
            if (idx < 0 || idx >= dataSize || visited[idx]) continue;
            visited[idx] = true;

            if (matchesCriteria(arrayData[j][idx], searchCriteria, dailyDistance)) {
                setSearchResults(idx, structArr[j], flag, counter);
            }

            if (searchKey == 2 || flag) {
                if (idx - 1 >= 0 && !visited[idx - 1]) queue[tail++] = idx - 1;
                if (idx + 1 < dataSize && !visited[idx + 1]) queue[tail++] = idx + 1;
            }
        }

        auto end = chrono::high_resolution_clock::now();
        structArr[j].time = chrono::duration<double, micro>(end - start).count();
        structArr[j].amount = searchResult;
        peakMemory += memTracker.trackMemoryUsage() - before;
        delete[] queue;
        delete[] visited;
    }
    
}

void searchAlgorithm::depthFirstSearch(resident* arrayData[], int subarraySize[], enum criteria searchCriteria){

    for (int j = 0; j < 3; j++) {
        int dataSize = subarraySize[j];
        int counter = 0;
        bool flag = true;
        int* stack = new int[200];
        bool* visited = new bool[200]{false};
        int top = -1;
        searchResult = 0;
        if (dataSize > 0) {
            stack[++top] = 0;
        }

        auto start = chrono::high_resolution_clock::now();
        while (top >= 0 && (searchKey == 2 || flag)) {
            int idx = stack[top--];
            if (idx < 0 || idx >= dataSize || visited[idx]) continue;
            visited[idx] = true;

            if (matchesCriteria(arrayData[j][idx], searchCriteria, dailyDistance)) {
                setSearchResults(idx, structArr[j], flag, counter);
            }

            if (searchKey == 2 || flag) {
                if (idx + 1 < dataSize && !visited[idx + 1]) stack[++top] = idx + 1;
                if (idx - 1 >= 0 && !visited[idx - 1]) stack[++top] = idx - 1;
            }
        }

        auto end = chrono::high_resolution_clock::now();
        structArr[j].time = chrono::duration<double, micro>(end - start).count();
        structArr[j].amount = searchResult;
        peakMemory += memTracker.trackMemoryUsage() - before;
        delete[] stack;
        delete[] visited;
    }
}

void searchAlgorithm::aStarSearch(resident* arrayData[], int subarraySize[], enum criteria searchCriteria){

    for (int j = 0; j < 3; j++) {
        int dataSize = subarraySize[j];
        int counter = 0;
        bool flag = true;
        int* openSet = new int[200];
        float* openScore = new float[200];
        bool* inOpen = new bool[200]{false};
        bool* closed = new bool[200]{false};
        int openCount = 0;
        searchResult = 0;

        if (dataSize > 0) {
            openSet[openCount] = 0;
            openScore[openCount] = estimateHeuristic(arrayData[j][0], searchCriteria, dailyDistance);
            inOpen[0] = true;
            openCount++;
        }

        auto start = chrono::high_resolution_clock::now();
        while (openCount > 0 && (searchKey == 2 || flag)) {
            int bestIndex = 0;
            for (int k = 1; k < openCount; k++) {
                if (openScore[k] < openScore[bestIndex]) {
                    bestIndex = k;
                }
            }

            int idx = openSet[bestIndex];
            openCount--;
            if (bestIndex < openCount) {
                openSet[bestIndex] = openSet[openCount];
                openScore[bestIndex] = openScore[openCount];
            }
            inOpen[idx] = false;
            closed[idx] = true;

            if (matchesCriteria(arrayData[j][idx], searchCriteria, dailyDistance)) {
                setSearchResults(idx, structArr[j], flag, counter);
            }

            if (!(searchKey == 2 || flag)) {
                break;
            }

            int neighbors[2] = {idx - 1, idx + 1};
            for (int n = 0; n < 2; n++) {
                int neighbor = neighbors[n];
                if (neighbor < 0 || neighbor >= dataSize || closed[neighbor]) continue;
                float newScore = estimateHeuristic(arrayData[j][neighbor], searchCriteria, dailyDistance);
                if (!inOpen[neighbor]) {
                    openSet[openCount] = neighbor;
                    openScore[openCount] = newScore;
                    inOpen[neighbor] = true;
                    openCount++;
                } else {
                    for (int q = 0; q < openCount; q++) {
                        if (openSet[q] == neighbor && newScore < openScore[q]) {
                            openScore[q] = newScore;
                            break;
                        }
                    }
                }
            }
        }

        auto end = chrono::high_resolution_clock::now();
        structArr[j].time = chrono::duration<double, micro>(end - start).count();
        structArr[j].amount = searchResult;
        peakMemory += memTracker.trackMemoryUsage() - before;
        delete[] openSet;
        delete[] openScore;
        delete[] inOpen;
        delete[] closed;
    }
}

//set result for search algorithms except binary
void searchAlgorithm::setSearchResults(int search, struct searchStruct &result, bool& flag, int& counter){
    searchResult++;
    if(searchKey==1){
        result.results[0]=search;
        flag = false;
        return;
    }
    result.results[counter]=search;
    counter++;
}

//set search results for binary
void searchAlgorithm::setBinarySearchResults(struct searchStruct &result, int mid,
                                        resident arr[], int size, enum criteria searchCriteria,
                                        float dailyDistance,
                                        int& resultCount){
    if(searchKey == 1){
        result.amount = 1;
        result.results[0] = mid;
        return;
    }
    resultCount = 0;

    // expand left
    int i = mid;
    while (i >= 0 && matchesCriteria(arr[i], searchCriteria, dailyDistance)) {
        i--;
    }
    i++;

    // collect all valid matches
    while (i < size && matchesCriteria(arr[i], searchCriteria, dailyDistance)) {
        result.results[resultCount++] = i;
        i++;
    }
    result.amount = resultCount;
}

//print search algorithm result
void searchAlgorithm::printFinalResult(string algoName, string datasetName, string targetCriteria, size_t initialMemory, size_t afterMemory, size_t finalMemory){
    auto printIndex = [&](int amount, struct searchStruct target, string name) -> void{
        for(int i=0; i<amount;i++){
            cout << name << "'s " << target.results[i] << " index." << endl;
            if(i==10){
                cout << "and " << amount-10 << " others." << endl;
                break;
            }
        }
    };

    string sep2(50,'-');
    cout << sep << endl;
    cout << "Search Algorithm Results:" << endl;
    cout << "Algorithm name: " << algoName << endl;
    cout << "Used dataset: " << datasetName << endl;
    cout << "Search criteria: " << targetCriteria << endl;
    cout << sep2 << endl;
    cout << "Dataset 1" << endl;
    cout << "Matched data amount: " << structArr[0].amount << endl;
    cout << "Matched data indexes: " << endl;
    printIndex(structArr[0].amount, structArr[0], "Dataset 1");
    cout << "Execution time: " << structArr[0].time << " Microseconds" << endl;
    cout << sep2 << endl;
    cout << "Dataset 2" << endl;
    cout << "Matched data amount: " << structArr[1].amount << endl;
    cout << "Matched data indexes: " << endl;
    printIndex(structArr[1].amount, structArr[1], "Dataset 2");
    cout << "Execution time: " << structArr[1].time << " Microseconds" << endl;
    cout << sep2 << endl;
    cout << "Dataset 3" << endl;
    cout << "Matched data amount: " << structArr[2].amount << endl;
    cout << "Matched data indexes: " << endl;
    printIndex(structArr[2].amount, structArr[2], "Dataset 3");
    cout << "Execution time: " << structArr[2].time << " Microseconds" << endl;
    cout << sep2 << endl;
    cout << "Algorithm execution time: " << exTimeAlgo << " Microseconds" << endl << endl;
    cout << "Memory usage before algorithm execution: " << initialMemory << " bytes" << endl;
    cout << "Peak memory usage during algorithm execution: " << peakMemory << " bytes" << endl;
    cout << "Memory usage after algorithm execution: " << afterMemory << " bytes" << endl;
    cout << "Algorithm impact towards overall system memory usage: " << finalMemory << " bytes" << endl;
}

//helper functions
int searchAlgorithm::getEncode(string transport) {
    if (transport == "Car") return 0;
    if (transport == "Bus") return 1;
    if (transport == "Bicycle") return 2;
    if (transport == "Walking") return 3;
    if (transport == "School Bus") return 4;
    if (transport == "Carpool") return 5;
    return -1;
}

int searchAlgorithm::getMinAge(string ageGroup) {
    if (ageGroup == "Child") return 0;
    if (ageGroup == "Early Teenager") return 13;
    if (ageGroup == "Late Teenager") return 18;
    if (ageGroup == "University Student") return 18;
    if (ageGroup == "Senior") return 65;
    return -1;
}

int searchAlgorithm::getMaxAge(string ageGroup) {
    if (ageGroup == "Child") return 12;
    if (ageGroup == "Early Teenager") return 17;
    if (ageGroup == "Late Teenager") return 24;
    if (ageGroup == "University Student") return 24;
    if (ageGroup == "Senior") return 100;
    return -1;
}
