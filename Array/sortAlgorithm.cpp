#include "header.hpp"
#include <iostream>
#include <chrono>
using namespace std;

//  Comparator – returns true if 'a' should come before 'b'
//  Used by both sorting algorithms

bool sortAlgorithm::compareLess(resident a, resident b, enum SortKey key)
{
    if (key == BY_AGE)
    {
        return a.getAge() < b.getAge();
    }
    else if (key == BY_DAILY_DISTANCE)
    {
        return a.getDistance() < b.getDistance();
    }
    else if (key == BY_TRANSPORT_MODE)
    {
        return a.getTransportEncode() < b.getTransportEncode();
    }
    else if (key == BY_AGE_TRANSPORT)
    {
        if (a.getAge() != b.getAge())
            return a.getAge() < b.getAge();
        return a.getTransportEncode() < b.getTransportEncode();
    }
    else if (key == BY_AGE_DISTANCE)
    {
        if (a.getAge() != b.getAge())
            return a.getAge() < b.getAge();
        return a.getDistance() < b.getDistance();
    }
    else if (key == BY_TRANSPORT_DISTANCE)
    {
        if (a.getTransportEncode() != b.getTransportEncode())
            return a.getTransportEncode() < b.getTransportEncode();
        return a.getDistance() < b.getDistance();
    }
    else if (key == BY_AGE_TRANSPORT_DISTANCE)
    {
        if (a.getAge() != b.getAge())
            return a.getAge() < b.getAge();
        if (a.getTransportEncode() != b.getTransportEncode())
            return a.getTransportEncode() < b.getTransportEncode();
        return a.getDistance() < b.getDistance();
    }
    else // BY_CARBON_EMISSION
        return (a.getDistance() * a.getCarbonEmission() * a.getAverageDay()) <
               (b.getDistance() * b.getCarbonEmission() * b.getAverageDay());
}

//  Copy array – preserves original before sorting
void sortAlgorithm::copyArray(const resident src[], resident dst[], int size)
{
    for (int i = 0; i < size; i++)
        dst[i] = src[i];
}

//  Helper: return readable label for sort key
string sortAlgorithm::getSortKeyLabel(enum SortKey key)
{
    if (key == BY_AGE)
        return "Age";
    if (key == BY_DAILY_DISTANCE)
        return "Daily Distance (km)";
    if (key == BY_CARBON_EMISSION)
        return "Total Carbon Emission (kg CO2)";
    if (key == BY_TRANSPORT_MODE)
        return "Transport Mode";
    if (key == BY_AGE_TRANSPORT)
        return "Age then Transport Mode";
    if (key == BY_AGE_DISTANCE)
        return "Age then Daily Distance";
    if (key == BY_TRANSPORT_DISTANCE)
        return "Transport Mode then Daily Distance";
    return "Age, Transport, and Daily Distance";
}
//  INSERTION SORT
//  - Picks each element and inserts it into its correct place
//  - Time: O(n^2) worst | O(n) best (nearly sorted data)
//  - Space: O(1) – sorts in-place, no extra memory needed
void sortAlgorithm::insertionSort(resident arr[], int size, SortKey key)
{
    size_t beforeMemory = memTracker.trackMemoryUsage();
    peakMemory = 0;

    for (int i = 1; i < size; i++)
    {
        resident key_elem = arr[i];
        int j = i - 1;
        while (j >= 0 && !compareLess(arr[j], key_elem, key))
        {
            arr[j + 1] = arr[j];
            j--;
        }
        arr[j + 1] = key_elem;
    }
    // insertion sort only ever uses one temp variable (key_elem)
    // so stack memory is always fixed at sizeof(resident)
    peakMemory = memTracker.trackMemoryUsage() - beforeMemory;
}

//  BUBBLE SORT
//  - Repeatedly compares and swaps adjacent elements
//  - Early exit flag stops if no swaps occur in a full pass
//  - Time: O(n^2) worst/average | O(n) best (already sorted)
//  - Space: O(1) – only needs one temp variable for swapping
void sortAlgorithm::bubbleSort(resident arr[], int size, SortKey key)
{
    size_t beforeMemory = memTracker.trackMemoryUsage();
    peakMemory = 0;

    bool swapped;
    for (int i = 0; i < size - 1; i++)
    {
        swapped = false;
        // each pass bubbles the largest remaining element to the end
        // so we reduce the inner loop by i each time
        for (int j = 0; j < size - 1 - i; j++)
        {
            if (!compareLess(arr[j], arr[j + 1], key) && arr[j].getID() != arr[j + 1].getID())
            {
                // swap adjacent elements
                resident temp = arr[j];
                arr[j] = arr[j + 1];
                arr[j + 1] = temp;
                swapped = true;
            }
        }
        // if no swaps happened this pass, array is already sorted
        // exit early — this is what makes best case O(n)
        if (!swapped)
            break;
    }
    // bubble sort only ever uses one temp variable for swapping
    // so stack memory is always fixed at sizeof(resident)
    peakMemory = memTracker.trackMemoryUsage() - beforeMemory;
}

//  QUICK SORT – Partition Helper
//  - Picks last element as pivot
//  - Moves smaller elements left, larger elements right
//  - Returns the final index of the pivot
int sortAlgorithm::partition(resident arr[], int low, int high, SortKey key)
{
    resident pivot = arr[high]; // Last element is pivot
    int i = low - 1;            // Boundary of smaller region

    for (int j = low; j < high; j++)
    {
        if (compareLess(arr[j], pivot, key))
        {
            i++;
            resident temp = arr[i];
            arr[i] = arr[j];
            arr[j] = temp;
        }
    }
    // Place pivot in its correct sorted position
    resident temp = arr[i + 1];
    arr[i + 1] = arr[high];
    arr[high] = temp;
    return i + 1;
}

//  QUICK SORT – Main Recursive Function
//  - Divides array around a pivot and recursively sorts halves
//  - Time: O(n log n) average | O(n^2) worst case
//  - Space: O(log n) – recursive call stack
void sortAlgorithm::quickSort(resident arr[], int low, int high, SortKey key)
{
    size_t beforeMemory = memTracker.trackMemoryUsage();
    peakMemory = 0;

    // track how deep the recursion goes
    currentDepth++;
    if (currentDepth > maxDepth)
        maxDepth = currentDepth;

    if (low < high)
    {
        int pivotIndex = partition(arr, low, high, key);
        quickSort(arr, low, pivotIndex - 1, key);
        quickSort(arr, pivotIndex + 1, high, key);
    }

    peakMemory = memTracker.trackMemoryUsage() - beforeMemory;
    // going back up one recursion level
    currentDepth--;
}

//  Print sorted results as a formatted table (up to 20 rows)
void sortAlgorithm::printSortedTable(resident arr[], int size, SortKey key,
                                     const string &algorithmName, const string &cityName)
{
    cout << "\n  Algorithm : " << algorithmName << endl;
    cout << "  Dataset   : " << cityName << endl;
    cout << "  Sorted by : " << getSortKeyLabel(key) << " (Ascending)" << endl;
    cout << "  " << string(95, '-') << endl;
    cout << "  " << left
         << setw(10) << "ID"
         << setw(6) << "Age"
         << setw(35) << "Age Group"
         << setw(14) << "Transport"
         << setw(16) << "Distance (km)"
         << setw(18) << "Emission Factor"
         << setw(18) << "Total Emission" << endl;
    cout << "  " << string(95, '-') << endl;

    int displayCount = (size < 20) ? size : 20;
    for (int i = 0; i < displayCount; i++)
    {
        double totalEmission = arr[i].getDistance() * arr[i].getCarbonEmission() * arr[i].getAverageDay();
        cout << "  " << left
             << setw(10) << arr[i].getID()
             << setw(6) << arr[i].getAge()
             << setw(35) << arr[i].getAgeGroup()
             << setw(14) << arr[i].getTransport()
             << setw(16) << fixed << setprecision(2) << arr[i].getDistance()
             << setw(18) << arr[i].getCarbonEmission()
             << setw(18) << totalEmission << endl;
    }
    if (size > 20)
        cout << "  ... (" << size - 20 << " more rows not shown)" << endl;
    cout << "  " << string(95, '-') << endl;
}

//  Print performance comparison table for both algorithms
void sortAlgorithm::printPerformanceTable(double insTime[], double bsTime[], double qsTime[],
                                          const string cities[], int sizes[], SortKey key,
                                          size_t insBefore[], size_t insPeak[], size_t insAfter[],
                                          size_t bsBefore[], size_t bsPeak[], size_t bsAfter[],
                                          size_t qsBefore[], size_t qsPeak[], size_t qsAfter[])
{
    cout << "\n  Performance Comparison - Sorted by: " << getSortKeyLabel(key) << endl;
    cout << "  " << string(125, '=') << endl;
    cout << "  " << left
         << setw(20) << "Algorithm"
         << setw(12) << "Dataset"
         << setw(10) << "Size"
         << setw(24) << "Time (microseconds)"
         << setw(18) << "Memory (Before)"
         << setw(20) << "Allocated Memory"
         << setw(16) << "Memory (After)" << endl;
    cout << "  " << string(125, '-') << endl;

    for (int d = 0; d < 3; d++)
    {
        cout << "  " << left
             << setw(20) << (d == 0 ? "Insertion Sort" : "")
             << setw(12) << cities[d]
             << setw(10) << sizes[d]
             << setw(24) << fixed << setprecision(3) << insTime[d]
             << setw(18) << insBefore[d]
             << setw(20) << insPeak[d]
             << setw(16) << insAfter[d] << endl;
    }
    cout << "  " << string(125, '-') << endl;

    for (int d = 0; d < 3; d++)
    {
        cout << "  " << left
             << setw(20) << (d == 0 ? "Bubble Sort" : "")
             << setw(12) << cities[d]
             << setw(10) << sizes[d]
             << setw(24) << fixed << setprecision(3) << bsTime[d]
             << setw(18) << bsBefore[d]
             << setw(20) << bsPeak[d]
             << setw(16) << bsAfter[d] << endl;
    }
    cout << "  " << string(125, '-') << endl;

    for (int d = 0; d < 3; d++)
    {
        cout << "  " << left
             << setw(20) << (d == 0 ? "Quick Sort" : "")
             << setw(12) << cities[d]
             << setw(10) << sizes[d]
             << setw(24) << fixed << setprecision(3) << qsTime[d]
             << setw(18) << qsBefore[d]
             << setw(20) << qsPeak[d]
             << setw(16) << qsAfter[d] << endl;
    }
    cout << "  " << string(125, '=') << endl
         << endl;
}

//  SECTION 6 – Sorting Experiments Runner
void sortAlgorithm::runSortingSection(resident dataset1[], int size1,
                                      resident dataset2[], int size2,
                                      resident dataset3[], int size3)
{
    cout << "\n";
    cout << "  SECTION 6 - SORTING EXPERIMENTS  " << endl;

    cout << "\n  Sort residents by:" << endl;
    cout << "1. Age" << endl;
    cout << "2. Daily Distance" << endl;
    cout << "3. Total Carbon Emission" << endl;
    cout << "Enter choice (1-3): ";

    int choice;
    cin >> choice;
    SortKey key;
    if (choice == 1)
        key = BY_AGE;
    else if (choice == 2)
        key = BY_DAILY_DISTANCE;
    else
        key = BY_CARBON_EMISSION;

    // Working copies so originals are never modified
    resident *copy1 = new resident[200], *copy2 = new resident[178], *copy3 = new resident[122];

    string *cities = new string[3]{"City A", "City B", "City C"};
    int *sizes = new int[3]{size1, size2, size3};

    double *insTime = new double[3];
    double *bsTime = new double[3];
    double *qsTime = new double[3];

    //  INSERTION SORT on all 3 datasets
    cout << "\n"
         << endl;
    cout << "  INSERTION SORT RESULTS  " << endl;

    copyArray(dataset1, copy1, size1);
    insBefore[0] = memTracker.trackMemoryUsage();
    auto start = chrono::high_resolution_clock::now();
    insertionSort(copy1, size1, key);
    auto end = chrono::high_resolution_clock::now();
    insTime[0] = chrono::duration<double, micro>(end - start).count();
    insAfter[0] = memTracker.trackMemoryUsage();
    insPeak[0] = peakMemory;
    printSortedTable(copy1, size1, key, "Insertion Sort", "City A");
    cout << "  Execution Time: " << fixed << setprecision(3) << insTime[0] << " microseconds\n"
         << endl;
    cout << "  Memory before sorting: " << insBefore[0] << " bytes" << endl;
    cout << "  Memory allocated during sorting: " << insPeak[0] << " bytes" << endl;
    cout << "  Memory after sorting: " << insAfter[0] << " bytes" << endl;
    cout << "  Algorithm impact towards overall system memory usage: " << insAfter[0] - insBefore[0] << " bytes" << endl;

    copyArray(dataset2, copy2, size2);
    insBefore[1] = memTracker.trackMemoryUsage();
    start = chrono::high_resolution_clock::now();
    insertionSort(copy2, size2, key);
    end = chrono::high_resolution_clock::now();
    insTime[1] = chrono::duration<double, micro>(end - start).count();
    insAfter[1] = memTracker.trackMemoryUsage();
    insPeak[1] = peakMemory;
    printSortedTable(copy2, size2, key, "Insertion Sort", "City B");
    cout << "  Execution Time: " << fixed << setprecision(3) << insTime[1] << " microseconds\n"
         << endl;
    cout << "  Memory before sorting: " << insBefore[1] << " bytes" << endl;
    cout << "  Memory allocated during sorting: " << insPeak[1] << " bytes" << endl;
    cout << "  Memory after sorting: " << insAfter[1] << " bytes" << endl;
    cout << "  Algorithm impact towards overall system memory usage: " << insAfter[1] - insBefore[1] << " bytes" << endl;

    copyArray(dataset3, copy3, size3);
    insBefore[2] = memTracker.trackMemoryUsage();
    start = chrono::high_resolution_clock::now();
    insertionSort(copy3, size3, key);
    end = chrono::high_resolution_clock::now();
    insTime[2] = chrono::duration<double, micro>(end - start).count();
    insAfter[2] = memTracker.trackMemoryUsage();
    insPeak[2] = peakMemory;
    printSortedTable(copy3, size3, key, "Insertion Sort", "City C");
    cout << "  Execution Time: " << fixed << setprecision(3) << insTime[2] << " microseconds\n"
         << endl;
    cout << "  Memory before sorting: " << insBefore[2] << " bytes" << endl;
    cout << "  Memory allocated during sorting: " << insPeak[2] << " bytes" << endl;
    cout << "  Memory after sorting: " << insAfter[2] << " bytes" << endl;
    cout << "  Algorithm impact towards overall system memory usage: " << insAfter[2] - insBefore[2] << " bytes" << endl;

    //  BUBBLE SORT on all 3 datasets
    cout << "\n"
         << endl;
    cout << "  BUBBLE SORT RESULTS  " << endl;

    copyArray(dataset1, copy1, size1);
    bsBefore[0] = memTracker.trackMemoryUsage();
    start = chrono::high_resolution_clock::now();
    bubbleSort(copy1, size1, key);
    end = chrono::high_resolution_clock::now();
    bsAfter[0] = memTracker.trackMemoryUsage();
    bsTime[0] = chrono::duration<double, micro>(end - start).count();
    bsPeak[0] = peakMemory;
    printSortedTable(copy1, size1, key, "Bubble Sort", "City A");
    cout << "  Execution Time: " << fixed << setprecision(3) << bsTime[0] << " microseconds\n"
         << endl;
    cout << "  Memory before sorting: " << bsBefore[0] << " bytes" << endl;
    cout << "  Memory allocated during sorting: " << bsPeak[0] << " bytes" << endl;
    cout << "  Memory after sorting: " << bsAfter[0] << " bytes" << endl;
    cout << "  Algorithm impact towards overall system memory usage: " << bsAfter[0] - bsBefore[0] << " bytes" << endl;

    copyArray(dataset2, copy2, size2);
    bsBefore[1] = memTracker.trackMemoryUsage();
    start = chrono::high_resolution_clock::now();
    bubbleSort(copy2, size2, key);
    end = chrono::high_resolution_clock::now();
    bsAfter[1] = memTracker.trackMemoryUsage();
    bsPeak[1] = peakMemory;
    bsTime[1] = chrono::duration<double, micro>(end - start).count();
    printSortedTable(copy2, size2, key, "Bubble Sort", "City B");
    cout << "  Execution Time: " << fixed << setprecision(3) << bsTime[1] << " microseconds\n"
         << endl;
    cout << "  Memory before sorting: " << bsBefore[1] << " bytes" << endl;
    cout << "  Memory allocated during sorting: " << bsPeak[1] << " bytes" << endl;
    cout << "  Memory after sorting: " << bsAfter[1] << " bytes" << endl;
    cout << "  Algorithm impact towards overall system memory usage: " << bsAfter[1] - bsBefore[1] << " bytes" << endl;

    copyArray(dataset3, copy3, size3);
    bsBefore[2] = memTracker.trackMemoryUsage();
    start = chrono::high_resolution_clock::now();
    bubbleSort(copy3, size3, key);
    end = chrono::high_resolution_clock::now();
    bsAfter[2] = memTracker.trackMemoryUsage();
    bsTime[2] = chrono::duration<double, micro>(end - start).count();
    bsPeak[2] = peakMemory;
    printSortedTable(copy3, size3, key, "Bubble Sort", "City C");
    cout << "  Execution Time: " << fixed << setprecision(3) << bsTime[2] << " microseconds\n"
         << endl;
    cout << "  Memory before sorting: " << bsBefore[2] << " bytes" << endl;
    cout << "  Memory allocated during sorting: " << bsPeak[2] << " bytes" << endl;
    cout << "  Memory after sorting: " << bsAfter[2] << " bytes" << endl;
    cout << "  Algorithm impact towards overall system memory usage: " << bsAfter[2] - bsBefore[2] << " bytes" << endl;

    //  QUICK SORT on all 3 datasets
    cout << "\n"
         << endl;
    cout << "  QUICK SORT RESULTS  " << endl;

    copyArray(dataset1, copy1, size1);
    qsBefore[0] = memTracker.trackMemoryUsage();
    maxDepth = 0;
    currentDepth = 0; // reset before each run
    start = chrono::high_resolution_clock::now();
    quickSort(copy1, 0, size1 - 1, key);
    end = chrono::high_resolution_clock::now();
    qsAfter[0] = memTracker.trackMemoryUsage();
    qsPeak[0] = peakMemory;
    qsTime[0] = chrono::duration<double, micro>(end - start).count();
    printSortedTable(copy1, size1, key, "Quick Sort", "City A");
    cout << "  Memory before sorting: " << qsBefore[0] << " bytes" << endl;
    cout << "  Memory allocated during sorting: " << qsPeak[0] << " bytes" << endl;
    cout << "  Memory after sorting: " << qsAfter[0] << " bytes" << endl;
    cout << "  Algorithm impact towards overall system memory usage: " << qsAfter[0] - qsBefore[0] << " bytes" << endl;

    copyArray(dataset2, copy2, size2);
    qsBefore[1] = memTracker.trackMemoryUsage();
    maxDepth = 0;
    currentDepth = 0; // reset
    start = chrono::high_resolution_clock::now();
    quickSort(copy2, 0, size2 - 1, key);
    end = chrono::high_resolution_clock::now();
    qsAfter[1] = memTracker.trackMemoryUsage();
    qsPeak[1] = peakMemory;
    qsTime[1] = chrono::duration<double, micro>(end - start).count();
    printSortedTable(copy2, size2, key, "Quick Sort", "City B");
    cout << "  Memory before sorting: " << qsBefore[1] << " bytes" << endl;
    cout << "  Memory allocated during sorting: " << qsPeak[1] << " bytes" << endl;
    cout << "  Memory after sorting: " << qsAfter[1] << " bytes" << endl;
    cout << "  Algorithm impact towards overall system memory usage: " << qsAfter[1] - qsBefore[1] << " bytes" << endl;

    copyArray(dataset3, copy3, size3);
    qsBefore[2] = memTracker.trackMemoryUsage();
    maxDepth = 0;
    currentDepth = 0; // reset
    start = chrono::high_resolution_clock::now();
    quickSort(copy3, 0, size3 - 1, key);
    end = chrono::high_resolution_clock::now();
    qsAfter[2] = memTracker.trackMemoryUsage();
    qsTime[2] = chrono::duration<double, micro>(end - start).count();
    qsPeak[2] = peakMemory;
    printSortedTable(copy3, size3, key, "Quick Sort", "City C");
    cout << "  Memory before sorting: " << qsBefore[2] << " bytes" << endl;
    cout << "  Memory allocated during sorting: " << qsPeak[2] << " bytes" << endl;
    cout << "  Memory after sorting: " << qsAfter[2] << " bytes" << endl;
    cout << "  Algorithm impact towards overall system memory usage: " << qsAfter[2] - qsBefore[2] << " bytes" << endl;

    //  Performance Comparison Table
    printPerformanceTable(insTime, bsTime, qsTime, cities, sizes, key,
                          insBefore, insPeak, insAfter,
                          bsBefore, bsPeak, bsAfter,
                          qsBefore, qsPeak, qsAfter);
}