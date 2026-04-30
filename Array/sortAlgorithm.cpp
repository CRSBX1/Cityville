#include "header.hpp"
#include <iostream>
#include <chrono>
using namespace std;

//=============================================================
//  Comparator – returns true if 'a' should come before 'b'
//  Used by both sorting algorithms
// ============================================================

bool sortAlgorithm::compareLess(resident a, resident b, enum SortKey key)
{
    if (key == BY_AGE){
        return a.getAge() < b.getAge();
    }
    else if (key == BY_DAILY_DISTANCE){
        return a.getDistance() < b.getDistance();
    }
    else if (key == BY_TRANSPORT_MODE){
        return a.getTransportEncode() < b.getTransportEncode();
    }
    else if (key == BY_AGE_TRANSPORT){
        if (a.getAge() != b.getAge())
            return a.getAge() < b.getAge();
        return a.getTransportEncode() < b.getTransportEncode();
    }
    else if (key == BY_AGE_DISTANCE){
        if (a.getAge() != b.getAge())
            return a.getAge() < b.getAge();
        return a.getDistance() < b.getDistance();
    }
    else if (key == BY_TRANSPORT_DISTANCE){
        if (a.getTransportEncode() != b.getTransportEncode())
            return a.getTransportEncode() < b.getTransportEncode();
        return a.getDistance() < b.getDistance();
    }
    else if (key == BY_AGE_TRANSPORT_DISTANCE){
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

// ============================================================
//  Copy array – preserves original before sorting
// ============================================================
void sortAlgorithm::copyArray(const resident src[], resident dst[], int size)
{
    for (int i = 0; i < size; i++)
        dst[i] = src[i];
}

// ============================================================
//  Helper: return readable label for sort key
// ============================================================
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

// ============================================================
//  INSERTION SORT
//  - Picks each element and inserts it into its correct place
//  - Time: O(n^2) worst | O(n) best (nearly sorted data)
//  - Space: O(1) – sorts in-place, no extra memory needed
// ============================================================
void sortAlgorithm::insertionSort(resident arr[], int size, SortKey key)
{
    for (int i = 1; i < size; i++)
    {
        resident key_elem = arr[i]; // Element to be inserted
        int j = i - 1;

        // Shift larger elements one position to the right
        while (j >= 0 && !compareLess(arr[j], key_elem, key))
        {
            arr[j + 1] = arr[j];
            j--;
        }
        arr[j + 1] = key_elem; // Insert element at correct position
    }
}

// ============================================================
//  QUICK SORT – Partition Helper
//  - Picks last element as pivot
//  - Moves smaller elements left, larger elements right
//  - Returns the final index of the pivot
// ============================================================
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

// ============================================================
//  QUICK SORT – Main Recursive Function
//  - Divides array around a pivot and recursively sorts halves
//  - Time: O(n log n) average | O(n^2) worst case
//  - Space: O(log n) – recursive call stack
// ============================================================
void sortAlgorithm::quickSort(resident arr[], int low, int high, SortKey key)
{
    if (low < high)
    {
        int pivotIndex = partition(arr, low, high, key);
        quickSort(arr, low, pivotIndex - 1, key);  // Sort left side
        quickSort(arr, pivotIndex + 1, high, key); // Sort right side
    }
}

// ============================================================
//  Print sorted results as a formatted table (up to 20 rows)
// ============================================================
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
         << setw(16) << "Age Group"
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
             << setw(16) << arr[i].getAgeGroup().substr(0, 15)
             << setw(14) << arr[i].getTransport()
             << setw(16) << fixed << setprecision(2) << arr[i].getDistance()
             << setw(18) << arr[i].getCarbonEmission()
             << setw(18) << totalEmission << endl;
    }
    if (size > 20)
        cout << "  ... (" << size - 20 << " more rows not shown)" << endl;
    cout << "  " << string(95, '-') << endl;
}

// ============================================================
//  Print performance comparison table for both algorithms
// ============================================================
void sortAlgorithm::printPerformanceTable(double insTime[], double qsTime[],
                           const string cities[], int sizes[], SortKey key)
{
    cout << "\n  Performance Comparison - Sorted by: " << getSortKeyLabel(key) << endl;
    cout << "  " << string(86, '=') << endl;
    cout << "  " << left
         << setw(20) << "Algorithm"
         << setw(12) << "Dataset"
         << setw(10) << "Size"
         << setw(24) << "Time (microseconds)"
         << setw(16) << "Time Complexity"
         << setw(16) << "Space Complexity" << endl;
    cout << "  " << string(86, '-') << endl;

    for (int d = 0; d < 3; d++)
    {
        cout << "  " << left
             << setw(20) << (d == 0 ? "Insertion Sort" : "")
             << setw(12) << cities[d]
             << setw(10) << sizes[d]
             << setw(24) << fixed << setprecision(3) << insTime[d]
             << setw(16) << (d == 0 ? "O(n^2)/O(n)" : "")
             << setw(16) << (d == 0 ? "O(1)" : "") << endl;
    }
    cout << "  " << string(86, '-') << endl;

    for (int d = 0; d < 3; d++)
    {
        cout << "  " << left
             << setw(20) << (d == 0 ? "Quick Sort" : "")
             << setw(12) << cities[d]
             << setw(10) << sizes[d]
             << setw(24) << fixed << setprecision(3) << qsTime[d]
             << setw(16) << (d == 0 ? "O(n log n)" : "")
             << setw(16) << (d == 0 ? "O(log n)" : "") << endl;
    }
    cout << "  " << string(86, '=') << endl;
}

// ============================================================
//  SECTION 6 – Sorting Experiments Runner
// ============================================================
void sortAlgorithm::runSortingSection(resident dataset1[], int size1,
                       resident dataset2[], int size2,
                       resident dataset3[], int size3)
{
    cout << "\n";
    cout << "  ╔══════════════════════════════════════════════════════════╗" << endl;
    cout << "  ║           SECTION 6 - SORTING EXPERIMENTS               ║" << endl;
    cout << "  ╚══════════════════════════════════════════════════════════╝" << endl;

    cout << "\n  Sort residents by:" << endl;
    cout << "  [1] Age" << endl;
    cout << "  [2] Daily Distance" << endl;
    cout << "  [3] Total Carbon Emission" << endl;
    cout << "  Enter choice (1-3): ";

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
    resident copy1[200], copy2[178], copy3[122];

    string cities[3] = {"City A", "City B", "City C"};
    int sizes[3] = {size1, size2, size3};

    double insTime[3];
    double qsTime[3];

    // ----------------------------------------------------------
    //  INSERTION SORT on all 3 datasets
    // ----------------------------------------------------------
    cout << "\n  ┌──────────────────────────────────────────────────────┐" << endl;
    cout << "  │               INSERTION SORT RESULTS                 │" << endl;
    cout << "  └──────────────────────────────────────────────────────┘" << endl;

    copyArray(dataset1, copy1, size1);
    auto start = chrono::high_resolution_clock::now();
    insertionSort(copy1, size1, key);
    auto end = chrono::high_resolution_clock::now();
    insTime[0] = chrono::duration<double, micro>(end - start).count();
    printSortedTable(copy1, size1, key, "Insertion Sort", "City A");
    cout << "  Execution Time: " << fixed << setprecision(3) << insTime[0] << " microseconds\n"
         << endl;

    copyArray(dataset2, copy2, size2);
    start = chrono::high_resolution_clock::now();
    insertionSort(copy2, size2, key);
    end = chrono::high_resolution_clock::now();
    insTime[1] = chrono::duration<double, micro>(end - start).count();
    printSortedTable(copy2, size2, key, "Insertion Sort", "City B");
    cout << "  Execution Time: " << fixed << setprecision(3) << insTime[1] << " microseconds\n"
         << endl;

    copyArray(dataset3, copy3, size3);
    start = chrono::high_resolution_clock::now();
    insertionSort(copy3, size3, key);
    end = chrono::high_resolution_clock::now();
    insTime[2] = chrono::duration<double, micro>(end - start).count();
    printSortedTable(copy3, size3, key, "Insertion Sort", "City C");
    cout << "  Execution Time: " << fixed << setprecision(3) << insTime[2] << " microseconds\n"
         << endl;

    // ----------------------------------------------------------
    //  QUICK SORT on all 3 datasets
    // ----------------------------------------------------------
    cout << "\n  ┌──────────────────────────────────────────────────────┐" << endl;
    cout << "  │                QUICK SORT RESULTS                    │" << endl;
    cout << "  └──────────────────────────────────────────────────────┘" << endl;

    copyArray(dataset1, copy1, size1);
    start = chrono::high_resolution_clock::now();
    quickSort(copy1, 0, size1 - 1, key);
    end = chrono::high_resolution_clock::now();
    qsTime[0] = chrono::duration<double, micro>(end - start).count();
    printSortedTable(copy1, size1, key, "Quick Sort", "City A");
    cout << "  Execution Time: " << fixed << setprecision(3) << qsTime[0] << " microseconds\n"
         << endl;

    copyArray(dataset2, copy2, size2);
    start = chrono::high_resolution_clock::now();
    quickSort(copy2, 0, size2 - 1, key);
    end = chrono::high_resolution_clock::now();
    qsTime[1] = chrono::duration<double, micro>(end - start).count();
    printSortedTable(copy2, size2, key, "Quick Sort", "City B");
    cout << "  Execution Time: " << fixed << setprecision(3) << qsTime[1] << " microseconds\n"
         << endl;

    copyArray(dataset3, copy3, size3);
    start = chrono::high_resolution_clock::now();
    quickSort(copy3, 0, size3 - 1, key);
    end = chrono::high_resolution_clock::now();
    qsTime[2] = chrono::duration<double, micro>(end - start).count();
    printSortedTable(copy3, size3, key, "Quick Sort", "City C");
    cout << "  Execution Time: " << fixed << setprecision(3) << qsTime[2] << " microseconds\n"
         << endl;

    // ----------------------------------------------------------
    //  Performance Comparison Table
    // ----------------------------------------------------------
    printPerformanceTable(insTime, qsTime, cities, sizes, key);

    cout << "\n  ANALYSIS:" << endl;
    cout << "  - Insertion Sort: O(n^2) worst but O(n) best on nearly-sorted data." << endl;
    cout << "    Uses O(1) extra memory - ideal for memory-constrained environments." << endl;
    cout << "  - Quick Sort: O(n log n) average - faster on large random datasets." << endl;
    cout << "    Uses O(log n) stack space due to recursion." << endl;
    cout << "  - Arrays allow direct index access, making both algorithms efficient." << endl;
    cout << "  - Quick Sort is the recommended choice for this dataset size." << endl;
    cout << endl;
}