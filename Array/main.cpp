#include <iostream>
#include "header.hpp"
#include <fstream>
#include <sstream>
#include <iomanip>
#include <chrono>
#include <string>

double calculateResidentTotalEmission(resident r)
{
    return r.getDistance() * r.getCarbonEmission() * r.getAverageDay();
}
// ============================================================
//  Sort Key Options – determines which field to sort by
// ============================================================
enum SortKey
{
    BY_AGE,
    BY_DAILY_DISTANCE,
    BY_CARBON_EMISSION
};

// ============================================================
//  Comparator – returns true if 'a' should come before 'b'
//  Used by both sorting algorithms
// ============================================================
bool compareLess(resident a, resident b, SortKey key)
{
    if (key == BY_AGE)
        return a.getAge() < b.getAge();
    else if (key == BY_DAILY_DISTANCE)
        return a.getDistance() < b.getDistance();
    else // BY_CARBON_EMISSION
        return (a.getDistance() * a.getCarbonEmission() * a.getAverageDay()) <
               (b.getDistance() * b.getCarbonEmission() * b.getAverageDay());
}

// ============================================================
//  Copy array – preserves original before sorting
// ============================================================
void copyArray(const resident src[], resident dst[], int size)
{
    for (int i = 0; i < size; i++)
        dst[i] = src[i];
}

// ============================================================
//  Helper: return readable label for sort key
// ============================================================
string getSortKeyLabel(SortKey key)
{
    if (key == BY_AGE)
        return "Age";
    if (key == BY_DAILY_DISTANCE)
        return "Daily Distance (km)";
    return "Total Carbon Emission (kg CO2)";
}

// ============================================================
//  INSERTION SORT
//  - Picks each element and inserts it into its correct place
//  - Time: O(n^2) worst | O(n) best (nearly sorted data)
//  - Space: O(1) – sorts in-place, no extra memory needed
// ============================================================
void insertionSort(resident arr[], int size, SortKey key)
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
int partition(resident arr[], int low, int high, SortKey key)
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
void quickSort(resident arr[], int low, int high, SortKey key)
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
void printSortedTable(resident arr[], int size, SortKey key,
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
void printPerformanceTable(double insTime[], double qsTime[],
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
void runSortingSection(resident dataset1[], int size1,
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

using namespace std;

int main()
{
    // Declare arrays and variables to store dataset information
    string residentID, age, transport, dailyDistance, carbonEmissionFactor, averageDay, line;
    resident dataset1[200];
    resident dataset2[178];
    resident dataset3[122];

    // Section 3. Dataset Implementation
    // Import dataset 1
    ifstream fileData1("dataset1-cityA.csv");
    getline(fileData1, line);
    for (int i = 0; i < 200 && getline(fileData1, line); i++)
    {
        stringstream ss(line);
        getline(ss, residentID, ',');
        getline(ss, age, ',');
        getline(ss, transport, ',');
        getline(ss, dailyDistance, ',');
        getline(ss, carbonEmissionFactor, ',');
        getline(ss, averageDay, ',');
        dataset1[i] = resident(residentID, stoi(age), transport, stof(dailyDistance), stof(carbonEmissionFactor), stof(averageDay));
    }
    fileData1.close();

    // Import dataset 2
    ifstream fileData2("dataset2-cityB.csv");
    getline(fileData2, line);
    for (int i = 0; i < 178 && getline(fileData2, line); i++)
    {
        stringstream ss(line);
        getline(ss, residentID, ',');
        getline(ss, age, ',');
        getline(ss, transport, ',');
        getline(ss, dailyDistance, ',');
        getline(ss, carbonEmissionFactor, ',');
        getline(ss, averageDay, ',');
        dataset2[i] = resident(residentID, stoi(age), transport, stof(dailyDistance), stof(carbonEmissionFactor), stof(averageDay));
    }
    fileData2.close();

    // Import dataset 3
    ifstream fileData3("dataset3-cityC.csv");
    getline(fileData3, line);
    for (int i = 0; i < 122 && getline(fileData3, line); i++)
    {
        stringstream ss(line);
        getline(ss, residentID, ',');
        getline(ss, age, ',');
        getline(ss, transport, ',');
        getline(ss, dailyDistance, ',');
        getline(ss, carbonEmissionFactor, ',');
        getline(ss, averageDay, ',');
        dataset3[i] = resident(residentID, stoi(age), transport, stof(dailyDistance), stof(carbonEmissionFactor), stof(averageDay));
    }
    fileData3.close();

    // Calculate array sizes
    int dataset1Size = sizeof(dataset1) / sizeof(dataset1[0]);
    int dataset2Size = sizeof(dataset2) / sizeof(dataset2[0]);
    int dataset3Size = sizeof(dataset3) / sizeof(dataset3[0]);

    // Print each array to verify dataset implementation
    for (int i = 0; i < dataset1Size; i++)
    {
        cout << dataset1[i].getID() << " " << dataset1[i].getAge() << " " << dataset1[i].getTransport() << " " << dataset1[i].getDistance() << " " << dataset1[i].getCarbonEmission() << " " << dataset1[i].getAverageDay() << endl;
    }
    for (int i = 0; i < dataset2Size; i++)
    {
        cout << dataset2[i].getID() << " " << dataset2[i].getAge() << " " << dataset2[i].getTransport() << " " << dataset2[i].getDistance() << " " << dataset2[i].getCarbonEmission() << " " << dataset2[i].getAverageDay() << endl;
    }
    for (int i = 0; i < dataset3Size; i++)
    {
        cout << dataset3[i].getID() << " " << dataset3[i].getAge() << " " << dataset3[i].getTransport() << " " << dataset3[i].getDistance() << " " << dataset3[i].getCarbonEmission() << " " << dataset3[i].getAverageDay() << endl;
    }

    // Section 4. Age Group Categorization
    // Section 4a
    // Declare int variables to count the number of residents in each age group
    int seniorCount = 0, lateCount = 0, earlyCount = 0, universityCount = 0, childCount = 0;
    // Recategorize age groups for dataset 1
    for (int i = 0; i < dataset1Size; i++)
    {
        dataset1[i].setAgeGroup(seniorCount, lateCount, earlyCount, universityCount, childCount);
    }

    // Recategorize age groups for dataset 2
    for (int i = 0; i < dataset2Size; i++)
    {
        dataset2[i].setAgeGroup(seniorCount, lateCount, earlyCount, universityCount, childCount);
    }

    // Recategorize age groups for dataset 3
    for (int i = 0; i < dataset3Size; i++)
    {
        dataset3[i].setAgeGroup(seniorCount, lateCount, earlyCount, universityCount, childCount);
    }

    // Verify Recategorization
    for (int i = 0; i < dataset1Size; i++)
    {
        cout << dataset1[i].getID() << " " << dataset1[i].getAge() << " " << dataset1[i].getAgeGroup() << endl;
    }

    for (int i = 0; i < dataset2Size; i++)
    {
        cout << dataset2[i].getID() << " " << dataset2[i].getAge() << " " << dataset2[i].getAgeGroup() << endl;
    }

    for (int i = 0; i < dataset3Size; i++)
    {
        cout << dataset3[i].getID() << " " << dataset3[i].getAge() << " " << dataset3[i].getAgeGroup() << endl;
    }

    // Determine the number of resident per age group
    cout << "Number of senior citizens/retirees: " << seniorCount << endl;
    cout << "Number of working adults in their late careers: " << lateCount << endl;
    cout << "Number of working adults in their early careers: " << earlyCount << endl;
    cout << "Number of university students/young adults: " << universityCount << endl;
    cout << "Number of children and teenagers: " << childCount << endl;

    // Section 4b
    // i. Preferred Transport Modes
    // Create int variables to count the number of transportation modes per age group
    int seniorCar = 0, seniorBus = 0, seniorBicycle = 0, seniorWalk = 0, seniorSchoolBus = 0, seniorPool = 0;                         // Senior
    int lateCar = 0, lateBus = 0, lateBicycle = 0, lateWalk = 0, lateSchoolBus = 0, latePool = 0;                                     // Late career adults
    int earlyCar = 0, earlyBus = 0, earlyBicycle = 0, earlyWalk = 0, earlySchoolBus = 0, earlyPool = 0;                               // Early career adults
    int universityCar = 0, universityBus = 0, universityBicycle = 0, universityWalk = 0, universitySchoolBus = 0, universityPool = 0; // University students/young adults
    int childCar = 0, childBus = 0, childBicycle = 0, childWalk = 0, childSchoolBus = 0, childPool = 0;                               // Children and teenagers

    for (int i = 0; i < dataset1Size; i++)
    {
        if (dataset1[i].getAgeGroup() == "Senior Citizens/Retirees")
        {
            if (dataset1[i].getTransport() == "Car")
            {
                seniorCar++;
            }
            else if (dataset1[i].getTransport() == "Bus")
            {
                seniorBus++;
            }
            else if (dataset1[i].getTransport() == "Bicycle")
            {
                seniorBicycle++;
            }
            else if (dataset1[i].getTransport() == "Walking")
            {
                seniorWalk++;
            }
            else if (dataset1[i].getTransport() == "School Bus")
            {
                seniorSchoolBus++;
            }
            else if (dataset1[i].getTransport() == "Carpool")
            {
                seniorPool++;
            }
        }
        else if (dataset1[i].getAgeGroup() == "Working Adults (Late Career)")
        {
            if (dataset1[i].getTransport() == "Car")
            {
                lateCar++;
            }
            else if (dataset1[i].getTransport() == "Bus")
            {
                lateBus++;
            }
            else if (dataset1[i].getTransport() == "Bicycle")
            {
                lateBicycle++;
            }
            else if (dataset1[i].getTransport() == "Walking")
            {
                lateWalk++;
            }
            else if (dataset1[i].getTransport() == "School Bus")
            {
                lateSchoolBus++;
            }
            else if (dataset1[i].getTransport() == "Carpool")
            {
                latePool++;
            }
        }
        else if (dataset1[i].getAgeGroup() == "Working Adults (Early Career)")
        {
            if (dataset1[i].getTransport() == "Car")
            {
                earlyCar++;
            }
            else if (dataset1[i].getTransport() == "Bus")
            {
                earlyBus++;
            }
            else if (dataset1[i].getTransport() == "Bicycle")
            {
                earlyBicycle++;
            }
            else if (dataset1[i].getTransport() == "Walking")
            {
                earlyWalk++;
            }
            else if (dataset1[i].getTransport() == "School Bus")
            {
                earlySchoolBus++;
            }
            else if (dataset1[i].getTransport() == "Carpool")
            {
                earlyPool++;
            }
        }
        else if (dataset1[i].getAgeGroup() == "University Students/Young Adults")
        {
            if (dataset1[i].getTransport() == "Car")
            {
                universityCar++;
            }
            else if (dataset1[i].getTransport() == "Bus")
            {
                universityBus++;
            }
            else if (dataset1[i].getTransport() == "Bicycle")
            {
                universityBicycle++;
            }
            else if (dataset1[i].getTransport() == "Walking")
            {
                universityWalk++;
            }
            else if (dataset1[i].getTransport() == "School Bus")
            {
                universitySchoolBus++;
            }
            else if (dataset1[i].getTransport() == "Carpool")
            {
                universityPool++;
            }
        }
        else if (dataset1[i].getAgeGroup() == "Children & Teenagers")
        {
            if (dataset1[i].getTransport() == "Car")
            {
                childCar++;
            }
            else if (dataset1[i].getTransport() == "Bus")
            {
                childBus++;
            }
            else if (dataset1[i].getTransport() == "Bicycle")
            {
                childBicycle++;
            }
            else if (dataset1[i].getTransport() == "Walking")
            {
                childWalk++;
            }
            else if (dataset1[i].getTransport() == "School Bus")
            {
                childSchoolBus++;
            }
            else if (dataset1[i].getTransport() == "Carpool")
            {
                childPool++;
            }
        }
    }

    for (int i = 0; i < dataset2Size; i++)
    {
        if (dataset2[i].getAgeGroup() == "Senior Citizens/Retirees")
        {
            if (dataset2[i].getTransport() == "Car")
            {
                seniorCar++;
            }
            else if (dataset2[i].getTransport() == "Bus")
            {
                seniorBus++;
            }
            else if (dataset2[i].getTransport() == "Bicycle")
            {
                seniorBicycle++;
            }
            else if (dataset2[i].getTransport() == "Walking")
            {
                seniorWalk++;
            }
            else if (dataset2[i].getTransport() == "School Bus")
            {
                seniorSchoolBus++;
            }
            else if (dataset2[i].getTransport() == "Carpool")
            {
                seniorPool++;
            }
        }
        else if (dataset2[i].getAgeGroup() == "Working Adults (Late Career)")
        {
            if (dataset2[i].getTransport() == "Car")
            {
                lateCar++;
            }
            else if (dataset2[i].getTransport() == "Bus")
            {
                lateBus++;
            }
            else if (dataset2[i].getTransport() == "Bicycle")
            {
                lateBicycle++;
            }
            else if (dataset2[i].getTransport() == "Walking")
            {
                lateWalk++;
            }
            else if (dataset2[i].getTransport() == "School Bus")
            {
                lateSchoolBus++;
            }
            else if (dataset2[i].getTransport() == "Carpool")
            {
                latePool++;
            }
        }
        else if (dataset2[i].getAgeGroup() == "Working Adults (Early Career)")
        {
            if (dataset2[i].getTransport() == "Car")
            {
                earlyCar++;
            }
            else if (dataset2[i].getTransport() == "Bus")
            {
                earlyBus++;
            }
            else if (dataset2[i].getTransport() == "Bicycle")
            {
                earlyBicycle++;
            }
            else if (dataset2[i].getTransport() == "Walking")
            {
                earlyWalk++;
            }
            else if (dataset2[i].getTransport() == "School Bus")
            {
                earlySchoolBus++;
            }
            else if (dataset2[i].getTransport() == "Carpool")
            {
                earlyPool++;
            }
        }
        else if (dataset2[i].getAgeGroup() == "University Students/Young Adults")
        {
            if (dataset2[i].getTransport() == "Car")
            {
                universityCar++;
            }
            else if (dataset2[i].getTransport() == "Bus")
            {
                universityBus++;
            }
            else if (dataset2[i].getTransport() == "Bicycle")
            {
                universityBicycle++;
            }
            else if (dataset2[i].getTransport() == "Walking")
            {
                universityWalk++;
            }
            else if (dataset2[i].getTransport() == "School Bus")
            {
                universitySchoolBus++;
            }
            else if (dataset2[i].getTransport() == "Carpool")
            {
                universityPool++;
            }
        }
        else if (dataset2[i].getAgeGroup() == "Children & Teenagers")
        {
            if (dataset2[i].getTransport() == "Car")
            {
                childCar++;
            }
            else if (dataset2[i].getTransport() == "Bus")
            {
                childBus++;
            }
            else if (dataset2[i].getTransport() == "Bicycle")
            {
                childBicycle++;
            }
            else if (dataset2[i].getTransport() == "Walking")
            {
                childWalk++;
            }
            else if (dataset2[i].getTransport() == "School Bus")
            {
                childSchoolBus++;
            }
            else if (dataset2[i].getTransport() == "Carpool")
            {
                childPool++;
            }
        }
    }

    for (int i = 0; i < dataset3Size; i++)
    {
        if (dataset3[i].getAgeGroup() == "Senior Citizens/Retirees")
        {
            if (dataset3[i].getTransport() == "Car")
            {
                seniorCar++;
            }
            else if (dataset3[i].getTransport() == "Bus")
            {
                seniorBus++;
            }
            else if (dataset3[i].getTransport() == "Bicycle")
            {
                seniorBicycle++;
            }
            else if (dataset3[i].getTransport() == "Walking")
            {
                seniorWalk++;
            }
            else if (dataset3[i].getTransport() == "School Bus")
            {
                seniorSchoolBus++;
            }
            else if (dataset3[i].getTransport() == "Carpool")
            {
                seniorPool++;
            }
        }
        else if (dataset3[i].getAgeGroup() == "Working Adults (Late Career)")
        {
            if (dataset3[i].getTransport() == "Car")
            {
                lateCar++;
            }
            else if (dataset3[i].getTransport() == "Bus")
            {
                lateBus++;
            }
            else if (dataset3[i].getTransport() == "Bicycle")
            {
                lateBicycle++;
            }
            else if (dataset3[i].getTransport() == "Walking")
            {
                lateWalk++;
            }
            else if (dataset3[i].getTransport() == "School Bus")
            {
                lateSchoolBus++;
            }
            else if (dataset3[i].getTransport() == "Carpool")
            {
                latePool++;
            }
        }
        else if (dataset3[i].getAgeGroup() == "Working Adults (Early Career)")
        {
            if (dataset3[i].getTransport() == "Car")
            {
                earlyCar++;
            }
            else if (dataset3[i].getTransport() == "Bus")
            {
                earlyBus++;
            }
            else if (dataset3[i].getTransport() == "Bicycle")
            {
                earlyBicycle++;
            }
            else if (dataset3[i].getTransport() == "Walking")
            {
                earlyWalk++;
            }
            else if (dataset3[i].getTransport() == "School Bus")
            {
                earlySchoolBus++;
            }
            else if (dataset3[i].getTransport() == "Carpool")
            {
                earlyPool++;
            }
        }
        else if (dataset3[i].getAgeGroup() == "University Students/Young Adults")
        {
            if (dataset3[i].getTransport() == "Car")
            {
                universityCar++;
            }
            else if (dataset3[i].getTransport() == "Bus")
            {
                universityBus++;
            }
            else if (dataset3[i].getTransport() == "Bicycle")
            {
                universityBicycle++;
            }
            else if (dataset3[i].getTransport() == "Walking")
            {
                universityWalk++;
            }
            else if (dataset3[i].getTransport() == "School Bus")
            {
                universitySchoolBus++;
            }
            else if (dataset3[i].getTransport() == "Carpool")
            {
                universityPool++;
            }
        }
        else if (dataset3[i].getAgeGroup() == "Children & Teenagers")
        {
            if (dataset3[i].getTransport() == "Car")
            {
                childCar++;
            }
            else if (dataset3[i].getTransport() == "Bus")
            {
                childBus++;
            }
            else if (dataset3[i].getTransport() == "Bicycle")
            {
                childBicycle++;
            }
            else if (dataset3[i].getTransport() == "Walking")
            {
                childWalk++;
            }
            else if (dataset3[i].getTransport() == "School Bus")
            {
                childSchoolBus++;
            }
            else if (dataset3[i].getTransport() == "Carpool")
            {
                childPool++;
            }
        }
    }

    int seniorTransportCount[6] = {seniorCar, seniorBus, seniorBicycle, seniorWalk, seniorSchoolBus, seniorPool};
    int lateTransportCount[6] = {lateCar, lateBus, lateBicycle, lateWalk, lateSchoolBus, latePool};
    int earlyTransportCount[6] = {earlyCar, earlyBus, earlyBicycle, earlyWalk, earlySchoolBus, earlyPool};
    int universityTransportCount[6] = {universityCar, universityBus, universityBicycle, universityWalk, universitySchoolBus, universityPool};
    int childTransportCount[6] = {childCar, childBus, childBicycle, childWalk, childSchoolBus, childPool};

    // Determine each age group's most preferred transportation mode
    string seniorFavorite, lateFavorite, earlyFavorite, universityFavorite, childFavorite;
    string transportMode[6] = {"Car", "Bus", "Bicycle", "Walking", "School Bus", "Carpool"};
    int seniorMax, lateMax, earlyMax, universityMax, childMax;

    // Senior
    seniorMax = seniorTransportCount[0];
    seniorFavorite = transportMode[0];
    for (int i = 1; i < 6; i++)
    {
        if (seniorMax < seniorTransportCount[i])
        {
            seniorMax = seniorTransportCount[i];
            seniorFavorite = transportMode[i];
        }
        else if (seniorMax == seniorTransportCount[i])
        {
            seniorFavorite = seniorFavorite + ", " + transportMode[i];
        }
    }
    if (seniorMax == 0)
    {
        seniorFavorite = "None";
    }

    // Late career adults
    lateMax = lateTransportCount[0];
    lateFavorite = transportMode[0];
    for (int i = 1; i < 6; i++)
    {
        if (lateMax < lateTransportCount[i])
        {
            lateMax = lateTransportCount[i];
            lateFavorite = transportMode[i];
        }
        else if (lateMax == lateTransportCount[i])
        {
            lateFavorite = lateFavorite + ", " + transportMode[i];
        }
    }
    if (lateMax == 0)
    {
        lateFavorite = "None";
    }

    // Early career adults
    earlyMax = earlyTransportCount[0];
    earlyFavorite = transportMode[0];
    for (int i = 1; i < 6; i++)
    {
        if (earlyMax < earlyTransportCount[i])
        {
            earlyMax = earlyTransportCount[i];
            earlyFavorite = transportMode[i];
        }
        else if (earlyMax == earlyTransportCount[i])
        {
            earlyFavorite = earlyFavorite + ", " + transportMode[i];
        }
    }
    if (earlyMax == 0)
    {
        earlyFavorite = "None";
    }

    // University students/young adults
    universityMax = universityTransportCount[0];
    universityFavorite = transportMode[0];
    for (int i = 1; i < 6; i++)
    {
        if (universityMax < universityTransportCount[i])
        {
            universityMax = universityTransportCount[i];
            universityFavorite = transportMode[i];
        }
        else if (universityMax == universityTransportCount[i])
        {
            universityFavorite = universityFavorite + ", " + transportMode[i];
        }
    }
    if (universityMax == 0)
    {
        universityFavorite = "None";
    }

    // Children and teenagers
    childMax = childTransportCount[0];
    childFavorite = transportMode[0];
    for (int i = 1; i < 6; i++)
    {
        if (childMax < childTransportCount[i])
        {
            childMax = childTransportCount[i];
            childFavorite = transportMode[i];
        }
        else if (childMax == childTransportCount[i])
        {
            childFavorite = childFavorite + ", " + transportMode[i];
        }
    }
    if (childMax == 0)
    {
        childFavorite = "None";
    }

    cout << "Senior Citizens/Retirees' most preferred transportation mode(s): " << seniorFavorite << " with a count of " << seniorMax << " users." << endl;
    cout << "Working Adults (Late Career)'s most preferred transportation mode(s): " << lateFavorite << " with a count of " << lateMax << " users." << endl;
    cout << "Working Adults (Early Career)'s most preferred transportation mode(s): " << earlyFavorite << " with a count of " << earlyMax << " users." << endl;
    cout << "University Students / Young Adults' most preferred transportation mode(s): " << universityFavorite << " with a count of " << universityMax << " users." << endl;
    cout << "Children & Teenagers' most preferred transportation mode(s): " << childFavorite << " with a count of " << childMax << " users." << endl;

    // ii. Total carbon emissions produced
    float seniorEmission = 0, lateEmission = 0, earlyEmission = 0, universityEmission = 0, childEmission = 0;
    for (int i = 0; i < dataset1Size; i++)
    {
        if (dataset1[i].getAgeGroup() == "Senior Citizens/Retirees")
        {
            seniorEmission += dataset1[i].getCarbonEmission();
        }
        else if (dataset1[i].getAgeGroup() == "Working Adults (Late Career)")
        {
            lateEmission += dataset1[i].getCarbonEmission();
        }
        else if (dataset1[i].getAgeGroup() == "Working Adults (Early Career)")
        {
            earlyEmission += dataset1[i].getCarbonEmission();
        }
        else if (dataset1[i].getAgeGroup() == "University Students/Young Adults")
        {
            universityEmission += dataset1[i].getCarbonEmission();
        }
        else if (dataset1[i].getAgeGroup() == "Children & Teenagers")
        {
            childEmission += dataset1[i].getCarbonEmission();
        }
    }

    for (int i = 0; i < dataset2Size; i++)
    {
        if (dataset2[i].getAgeGroup() == "Senior Citizens/Retirees")
        {
            seniorEmission += dataset2[i].getCarbonEmission();
        }
        else if (dataset2[i].getAgeGroup() == "Working Adults (Late Career)")
        {
            lateEmission += dataset2[i].getCarbonEmission();
        }
        else if (dataset2[i].getAgeGroup() == "Working Adults (Early Career)")
        {
            earlyEmission += dataset2[i].getCarbonEmission();
        }
        else if (dataset2[i].getAgeGroup() == "University Students/Young Adults")
        {
            universityEmission += dataset2[i].getCarbonEmission();
        }
        else if (dataset2[i].getAgeGroup() == "Children & Teenagers")
        {
            childEmission += dataset2[i].getCarbonEmission();
        }
    }

    for (int i = 0; i < dataset3Size; i++)
    {
        if (dataset3[i].getAgeGroup() == "Senior Citizens/Retirees")
        {
            seniorEmission += dataset3[i].getCarbonEmission();
        }
        else if (dataset3[i].getAgeGroup() == "Working Adults (Late Career)")
        {
            lateEmission += dataset3[i].getCarbonEmission();
        }
        else if (dataset3[i].getAgeGroup() == "Working Adults (Early Career)")
        {
            earlyEmission += dataset3[i].getCarbonEmission();
        }
        else if (dataset3[i].getAgeGroup() == "University Students/Young Adults")
        {
            universityEmission += dataset3[i].getCarbonEmission();
        }
        else if (dataset3[i].getAgeGroup() == "Children & Teenagers")
        {
            childEmission += dataset3[i].getCarbonEmission();
        }
    }

    // Output every age group's produced carbon emission
    cout << "Senior citizens or retirees produced carbon emission: " << seniorEmission << " kg CO2/km." << endl;
    cout << "Late career working adults produced carbon emission: " << lateEmission << " kg CO2/km." << endl;
    cout << "Early career working adults produced carbon emission: " << earlyEmission << " kg CO2/km." << endl;
    cout << "University students' or young adults produced carbon emission: " << universityEmission << " kg CO2/km." << endl;
    cout << "Children and Teenagers produced carbon emission: " << childEmission << " kg CO2/km." << endl;

    // iii. Average carbon emission per resident
    float seniorAverageEmission = 0, lateAverageEmission = 0, earlyAverageEmission = 0, universityAverageEmission = 0, childAverageEmission = 0;
    seniorAverageEmission = seniorEmission / seniorCount;
    lateAverageEmission = lateEmission / lateCount;
    earlyAverageEmission = earlyEmission / earlyCount;
    universityAverageEmission = universityEmission / universityCount;
    childAverageEmission = childEmission / childCount;

    cout << "Senior citizens or retirees average produced carbon emission per citizen: " << seniorAverageEmission << " kg CO2/km." << endl;
    cout << "Late career working adults average produced carbon emission per citizen: " << lateAverageEmission << " kg CO2/km." << endl;
    cout << "Early career working adults average produced carbon emission per citizen: " << earlyAverageEmission << " kg CO2/km." << endl;
    cout << "University students' or young adults average produced carbon emission per citizen: " << universityAverageEmission << " kg CO2/km." << endl;
    cout << "Children and teenagers average produced carbon emission per citizen: " << childAverageEmission << " kg CO2/km." << endl;

    // Section 5a Total Carbon Emissions per Dataset
    double totalEmissionDataset1 = 0;
    double totalEmissionDataset2 = 0;
    double totalEmissionDataset3 = 0;

    for (int i = 0; i < dataset1Size; i++)
    {
        totalEmissionDataset1 += calculateResidentTotalEmission(dataset1[i]);
    }

    for (int i = 0; i < dataset2Size; i++)
    {
        totalEmissionDataset2 += calculateResidentTotalEmission(dataset2[i]);
    }

    for (int i = 0; i < dataset3Size; i++)
    {
        totalEmissionDataset3 += calculateResidentTotalEmission(dataset3[i]);
    }

    cout << "\nSection 5(a): Total Carbon Emissions per Dataset" << endl;
    cout << "------------------------------------------------------" << endl;
    cout << "Dataset\t\tTotal Emission (kg CO2)" << endl;
    cout << "------------------------------------------------------" << endl;
    cout << "City A\t\t" << totalEmissionDataset1 << endl;
    cout << "City B\t\t" << totalEmissionDataset2 << endl;
    cout << "City C\t\t" << totalEmissionDataset3 << endl;
    cout << "------------------------------------------------------" << endl;

    // Section 5b Carbon Emissions per Mode of Transport (City A, City B, City C)
    double cityACarEmission = 0, cityABusEmission = 0, cityABicycleEmission = 0, cityAWalkingEmission = 0, cityASchoolBusEmission = 0, cityACarpoolEmission = 0;
    double cityBCarEmission = 0, cityBBusEmission = 0, cityBBicycleEmission = 0, cityBWalkingEmission = 0, cityBSchoolBusEmission = 0, cityBCarpoolEmission = 0;
    double cityCCarEmission = 0, cityCBusEmission = 0, cityCBicycleEmission = 0, cityCWalkingEmission = 0, cityCSchoolBusEmission = 0, cityCCarpoolEmission = 0;

    int cityACarCount = 0, cityABusCount = 0, cityABicycleCount = 0, cityAWalkingCount = 0, cityASchoolBusCount = 0, cityACarpoolCount = 0;
    int cityBCarCount = 0, cityBBusCount = 0, cityBBicycleCount = 0, cityBWalkingCount = 0, cityBSchoolBusCount = 0, cityBCarpoolCount = 0;
    int cityCCarCount = 0, cityCBusCount = 0, cityCBicycleCount = 0, cityCWalkingCount = 0, cityCSchoolBusCount = 0, cityCCarpoolCount = 0;

    for (int i = 0; i < dataset1Size; i++)
    {
        double residentEmission = calculateResidentTotalEmission(dataset1[i]);

        if (dataset1[i].getTransport() == "Car")
        {
            cityACarEmission += residentEmission;
            cityACarCount++;
        }
        else if (dataset1[i].getTransport() == "Bus")
        {
            cityABusEmission += residentEmission;
            cityABusCount++;
        }
        else if (dataset1[i].getTransport() == "Bicycle")
        {
            cityABicycleEmission += residentEmission;
            cityABicycleCount++;
        }
        else if (dataset1[i].getTransport() == "Walking")
        {
            cityAWalkingEmission += residentEmission;
            cityAWalkingCount++;
        }
        else if (dataset1[i].getTransport() == "School Bus")
        {
            cityASchoolBusEmission += residentEmission;
            cityASchoolBusCount++;
        }
        else if (dataset1[i].getTransport() == "Carpool")
        {
            cityACarpoolEmission += residentEmission;
            cityACarpoolCount++;
        }
    }

    for (int i = 0; i < dataset2Size; i++)
    {
        double residentEmission = calculateResidentTotalEmission(dataset2[i]);

        if (dataset2[i].getTransport() == "Car")
        {
            cityBCarEmission += residentEmission;
            cityBCarCount++;
        }
        else if (dataset2[i].getTransport() == "Bus")
        {
            cityBBusEmission += residentEmission;
            cityBBusCount++;
        }
        else if (dataset2[i].getTransport() == "Bicycle")
        {
            cityBBicycleEmission += residentEmission;
            cityBBicycleCount++;
        }
        else if (dataset2[i].getTransport() == "Walking")
        {
            cityBWalkingEmission += residentEmission;
            cityBWalkingCount++;
        }
        else if (dataset2[i].getTransport() == "School Bus")
        {
            cityBSchoolBusEmission += residentEmission;
            cityBSchoolBusCount++;
        }
        else if (dataset2[i].getTransport() == "Carpool")
        {
            cityBCarpoolEmission += residentEmission;
            cityBCarpoolCount++;
        }
    }

    for (int i = 0; i < dataset3Size; i++)
    {
        double residentEmission = calculateResidentTotalEmission(dataset3[i]);

        if (dataset3[i].getTransport() == "Car")
        {
            cityCCarEmission += residentEmission;
            cityCCarCount++;
        }
        else if (dataset3[i].getTransport() == "Bus")
        {
            cityCBusEmission += residentEmission;
            cityCBusCount++;
        }
        else if (dataset3[i].getTransport() == "Bicycle")
        {
            cityCBicycleEmission += residentEmission;
            cityCBicycleCount++;
        }
        else if (dataset3[i].getTransport() == "Walking")
        {
            cityCWalkingEmission += residentEmission;
            cityCWalkingCount++;
        }
        else if (dataset3[i].getTransport() == "School Bus")
        {
            cityCSchoolBusEmission += residentEmission;
            cityCSchoolBusCount++;
        }
        else if (dataset3[i].getTransport() == "Carpool")
        {
            cityCCarpoolEmission += residentEmission;
            cityCCarpoolCount++;
        }
    }

    cout << "\nSection 5(b): Carbon Emissions per Mode of Transport" << endl;
    cout << fixed << setprecision(2);

    cout << "\nCity A" << endl;
    cout << "--------------------------------------------------------------------------" << endl;
    cout << left << setw(15) << "Mode"
         << setw(10) << "Count"
         << setw(25) << "Total Emission (kg CO2)" << endl;
    cout << "--------------------------------------------------------------------------" << endl;
    cout << left << setw(15) << "Car"
         << setw(10) << cityACarCount
         << setw(25) << cityACarEmission << endl;
    cout << left << setw(15) << "Bus"
         << setw(10) << cityABusCount
         << setw(25) << cityABusEmission << endl;
    cout << left << setw(15) << "Bicycle"
         << setw(10) << cityABicycleCount
         << setw(25) << cityABicycleEmission << endl;
    cout << left << setw(15) << "Walking"
         << setw(10) << cityAWalkingCount
         << setw(25) << cityAWalkingEmission << endl;
    cout << left << setw(15) << "School Bus"
         << setw(10) << cityASchoolBusCount
         << setw(25) << cityASchoolBusEmission << endl;
    cout << left << setw(15) << "Carpool"
         << setw(10) << cityACarpoolCount
         << setw(25) << cityACarpoolEmission << endl;
    cout << "--------------------------------------------------------------------------" << endl;

    cout << "\nCity B" << endl;
    cout << "--------------------------------------------------------------------------" << endl;
    cout << left << setw(15) << "Mode"
         << setw(10) << "Count"
         << setw(25) << "Total Emission (kg CO2)" << endl;
    cout << "--------------------------------------------------------------------------" << endl;
    cout << left << setw(15) << "Car"
         << setw(10) << cityBCarCount
         << setw(25) << cityBCarEmission << endl;
    cout << left << setw(15) << "Bus"
         << setw(10) << cityBBusCount
         << setw(25) << cityBBusEmission << endl;
    cout << left << setw(15) << "Bicycle"
         << setw(10) << cityBBicycleCount
         << setw(25) << cityBBicycleEmission << endl;
    cout << left << setw(15) << "Walking"
         << setw(10) << cityBWalkingCount
         << setw(25) << cityBWalkingEmission << endl;
    cout << left << setw(15) << "School Bus"
         << setw(10) << cityBSchoolBusCount
         << setw(25) << cityBSchoolBusEmission << endl;
    cout << left << setw(15) << "Carpool"
         << setw(10) << cityBCarpoolCount
         << setw(25) << cityBCarpoolEmission << endl;
    cout << "--------------------------------------------------------------------------" << endl;

    cout << "\nCity C" << endl;
    cout << "--------------------------------------------------------------------------" << endl;
    cout << left << setw(15) << "Mode"
         << setw(10) << "Count"
         << setw(25) << "Total Emission (kg CO2)" << endl;
    cout << "--------------------------------------------------------------------------" << endl;
    cout << left << setw(15) << "Car"
         << setw(10) << cityCCarCount
         << setw(25) << cityCCarEmission << endl;
    cout << left << setw(15) << "Bus"
         << setw(10) << cityCBusCount
         << setw(25) << cityCBusEmission << endl;
    cout << left << setw(15) << "Bicycle"
         << setw(10) << cityCBicycleCount
         << setw(25) << cityCBicycleEmission << endl;
    cout << left << setw(15) << "Walking"
         << setw(10) << cityCWalkingCount
         << setw(25) << cityCWalkingEmission << endl;
    cout << left << setw(15) << "School Bus"
         << setw(10) << cityCSchoolBusCount
         << setw(25) << cityCSchoolBusEmission << endl;
    cout << left << setw(15) << "Carpool"
         << setw(10) << cityCCarpoolCount
         << setw(25) << cityCCarpoolEmission << endl;
    cout << "--------------------------------------------------------------------------" << endl;

    // Section 5(c): Compare emissions across datasets and age groups
    const int AGE_GROUP_COUNT = 5;
    string ageGroupLabels[AGE_GROUP_COUNT] = {
        "Senior Citizens/Retirees",
        "Working Adults (Late Career)",
        "Working Adults (Early Career)",
        "University Students/Young Adults",
        "Children & Teenagers"};

    int cityAgeCounts[3][AGE_GROUP_COUNT] = {0};
    double cityAgeEmissions[3][AGE_GROUP_COUNT] = {0};

    auto getAgeGroupIndex = [&](string ageGroup) -> int
    {
        if (ageGroup == "Senior Citizens/Retirees")
        {
            return 0;
        }
        else if (ageGroup == "Working Adults (Late Career)")
        {
            return 1;
        }
        else if (ageGroup == "Working Adults (Early Career)")
        {
            return 2;
        }
        else if (ageGroup == "University Students/Young Adults")
        {
            return 3;
        }
        else if (ageGroup == "Children & Teenagers")
        {
            return 4;
        }
        return -1;
    };

    for (int i = 0; i < dataset1Size; i++)
    {
        int ageIndex = getAgeGroupIndex(dataset1[i].getAgeGroup());
        if (ageIndex != -1)
        {
            cityAgeCounts[0][ageIndex]++;
            cityAgeEmissions[0][ageIndex] += calculateResidentTotalEmission(dataset1[i]);
        }
    }

    for (int i = 0; i < dataset2Size; i++)
    {
        int ageIndex = getAgeGroupIndex(dataset2[i].getAgeGroup());
        if (ageIndex != -1)
        {
            cityAgeCounts[1][ageIndex]++;
            cityAgeEmissions[1][ageIndex] += calculateResidentTotalEmission(dataset2[i]);
        }
    }

    for (int i = 0; i < dataset3Size; i++)
    {
        int ageIndex = getAgeGroupIndex(dataset3[i].getAgeGroup());
        if (ageIndex != -1)
        {
            cityAgeCounts[2][ageIndex]++;
            cityAgeEmissions[2][ageIndex] += calculateResidentTotalEmission(dataset3[i]);
        }
    }

    cout << "\nSection 5(c): Compare Emissions Across Datasets and Age Groups" << endl;

    cout << "\nTotal Emission by Age Group (kg CO2)" << endl;
    cout << "---------------------------------------------------------------------------------------------------------------" << endl;
    cout << left << setw(35) << "Age Group"
         << setw(15) << "City A"
         << setw(15) << "City B"
         << setw(15) << "City C"
         << setw(15) << "Overall" << endl;
    cout << "---------------------------------------------------------------------------------------------------------------" << endl;

    for (int i = 0; i < AGE_GROUP_COUNT; i++)
    {
        double overallEmission = cityAgeEmissions[0][i] + cityAgeEmissions[1][i] + cityAgeEmissions[2][i];
        cout << left << setw(35) << ageGroupLabels[i]
             << setw(15) << cityAgeEmissions[0][i]
             << setw(15) << cityAgeEmissions[1][i]
             << setw(15) << cityAgeEmissions[2][i]
             << setw(15) << overallEmission << endl;
    }
    cout << "---------------------------------------------------------------------------------------------------------------" << endl;

    cout << "\nAverage Emission per Resident by Age Group (kg CO2)" << endl;
    cout << "---------------------------------------------------------------------------------------------------------------" << endl;
    cout << left << setw(35) << "Age Group"
         << setw(15) << "City A"
         << setw(15) << "City B"
         << setw(15) << "City C"
         << setw(15) << "Overall" << endl;
    cout << "---------------------------------------------------------------------------------------------------------------" << endl;

    for (int i = 0; i < AGE_GROUP_COUNT; i++)
    {
        int overallCount = cityAgeCounts[0][i] + cityAgeCounts[1][i] + cityAgeCounts[2][i];
        double avgA = (cityAgeCounts[0][i] != 0) ? cityAgeEmissions[0][i] / cityAgeCounts[0][i] : 0;
        double avgB = (cityAgeCounts[1][i] != 0) ? cityAgeEmissions[1][i] / cityAgeCounts[1][i] : 0;
        double avgC = (cityAgeCounts[2][i] != 0) ? cityAgeEmissions[2][i] / cityAgeCounts[2][i] : 0;
        double avgOverall = (overallCount != 0) ? (cityAgeEmissions[0][i] + cityAgeEmissions[1][i] + cityAgeEmissions[2][i]) / overallCount : 0;

        cout << left << setw(35) << ageGroupLabels[i]
             << setw(15) << avgA
             << setw(15) << avgB
             << setw(15) << avgC
             << setw(15) << avgOverall << endl;
    }
    cout << "---------------------------------------------------------------------------------------------------------------" << endl;

    cout << "\nResident Count by Age Group" << endl;
    cout << "---------------------------------------------------------------------------------------------------------------" << endl;
    cout << left << setw(35) << "Age Group"
         << setw(15) << "City A"
         << setw(15) << "City B"
         << setw(15) << "City C"
         << setw(15) << "Overall" << endl;
    cout << "---------------------------------------------------------------------------------------------------------------" << endl;

    for (int i = 0; i < AGE_GROUP_COUNT; i++)
    {
        int overallCount = cityAgeCounts[0][i] + cityAgeCounts[1][i] + cityAgeCounts[2][i];
        cout << left << setw(35) << ageGroupLabels[i]
             << setw(15) << cityAgeCounts[0][i]
             << setw(15) << cityAgeCounts[1][i]
             << setw(15) << cityAgeCounts[2][i]
             << setw(15) << overallCount << endl;
    }
    cout << "---------------------------------------------------------------------------------------------------------------" << endl;

    // Section 6 – Sorting Experiments
    runSortingSection(dataset1, dataset1Size, dataset2, dataset2Size, dataset3, dataset3Size);

    return 0;
}