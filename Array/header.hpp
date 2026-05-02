#ifndef HEADER_HPP
#define HEADER_HPP
#include <string>
#include <fstream> //remove these later
#include <sstream> //remove these later
#include <iomanip> //remove these later
#include <iostream>
using namespace std;

struct MemoryMetrics
{
    static size_t allocatedMemory;
    static size_t freedMemory;
    size_t trackMemoryUsage();
    void reset();
};

enum SortKey
{
    BY_AGE,
    BY_DAILY_DISTANCE,
    BY_CARBON_EMISSION,
    BY_TRANSPORT_MODE,
    BY_AGE_TRANSPORT,         // age and transport mode
    BY_AGE_DISTANCE,          // age and daily distance
    BY_TRANSPORT_DISTANCE,    // transport mode and daily distance
    BY_AGE_TRANSPORT_DISTANCE // age, transport mode, and daily distance
};

class resident
{
private:
    string residentID;
    int age;
    string transportMode;
    int encodedTransport; /// Car=0,Bus=1,Bicycle=2,Walking=3,School Bus=4,Carpool=5*/
    float dailyDistance;
    float carbonEmissionFactor;
    float averageDayPerMonth;
    string ageGroup;

public:
    // Constructors
    resident();
    resident(string id, int ageInput, string transport, float distance, float carbonEmission, float averageDay);

    // Getter Methods
    string getID();
    int getAge();
    int getTransportEncode();
    string getTransport();
    float getDistance();
    float getCarbonEmission();
    float getAverageDay();
    string getAgeGroup();

    // Setter Methods
    void setID(int id);
    void setAge(int ageInput);
    void setTransport(string transport);
    void setDistance(float distance);
    void setCarbonEmission();
    void setAverageDay();
    void setAgeGroup(int &senior, int &late, int &early, int &university, int &child);
    void setAgeGroup(string ageGroup);
    void setTransportEncode(int encode);
};

class sortAlgorithm
{
private:
    MemoryMetrics memTracker;                     // memory tracker
    size_t before, after, usedMemory, peakMemory; // memory usage variables
    int maxDepth;                                 // tracks deepest recursion Quick Sort reaches
    int currentDepth;                             // tracks current recursion level
    size_t *insBefore = new size_t[3];
    size_t *insPeak = new size_t[3];
    size_t *insAfter = new size_t[3];
    size_t *bsBefore = new size_t[3];
    size_t *bsPeak = new size_t[3];
    size_t *bsAfter = new size_t[3];
    size_t *qsBefore = new size_t[3];
    size_t *qsPeak = new size_t[3];
    size_t *qsAfter = new size_t[3];

public:
    bool compareLess(resident a, resident b, enum SortKey key);
    void copyArray(const resident src[], resident dst[], int size);
    string getSortKeyLabel(enum SortKey key);
    void insertionSort(resident arr[], int size, SortKey key);
    void bubbleSort(resident arr[], int size, SortKey key);
    int partition(resident arr[], int low, int high, SortKey key);
    void quickSort(resident arr[], int low, int high, SortKey key);
    void printSortedTable(resident arr[], int size, SortKey key,
                          const string &algorithmName, const string &cityName);
    void printPerformanceTable(double insTime[], double bsTime[], double qsTime[],
                               const string cities[], int sizes[], SortKey key,
                               size_t insBefore[], size_t insPeak[], size_t insAfter[],
                               size_t bsBefore[], size_t bsPeak[], size_t bsAfter[],
                               size_t qsBefore[], size_t qsPeak[], size_t qsAfter[]);
    void runSortingSection(resident dataset1[], int size1,
                           resident dataset2[], int size2,
                           resident dataset3[], int size3);
};

class searchAlgorithm
{
private:
    MemoryMetrics memTracker;                     // memory tracker
    size_t before, after, usedMemory, peakMemory; // memory usage variables
    int searchResult = 0;
    int dataChoice;
    int userChoice;
    int criteriaInputChoice;
    int searchKey;
    string ageGroup;
    string transport;
    string sep = string(100, '=');
    float dailyDistance;
    float exTimeAlgo;
    resident **allDatasets = new resident *[3];
    int *arraySizes = new int[3]{200, 178, 122};
    // Create arrays of sorted arrays based on search criterias
    resident *dataByAge1 = new resident[200], *dataByAge2 = new resident[178], *dataByAge3 = new resident[122];                                                    // Age (and in turn, age group)
    resident *dataByTransport1 = new resident[200], *dataByTransport2 = new resident[178], *dataByTransport3 = new resident[122];                                  // Transport mode
    resident *dataByDistance1 = new resident[200], *dataByDistance2 = new resident[178], *dataByDistance3 = new resident[122];                                     // Daily distance
    resident *dataByAgeTransport1 = new resident[200], *dataByAgeTransport2 = new resident[178], *dataByAgeTransport3 = new resident[122];                         // Age and transport mode
    resident *dataByAgeDistance1 = new resident[200], *dataByAgeDistance2 = new resident[178], *dataByAgeDistance3 = new resident[122];                            // Age and daily distance
    resident *dataByTransportDistance1 = new resident[200], *dataByTransportDistance2 = new resident[178], *dataByTransportDistance3 = new resident[122];          // Transport mode and daily distance
    resident *dataByAgeTransportDistance1 = new resident[200], *dataByAgeTransportDistance2 = new resident[178], *dataByAgeTransportDistance3 = new resident[122]; // Age, transport mode and daily distance
    resident **allDataByAgeSorted = new resident *[3]{dataByAge1, dataByAge2, dataByAge3};
    resident **allDataByTransportSorted = new resident *[3]{dataByTransport1, dataByTransport2, dataByTransport3};
    resident **allDataByDistanceSorted = new resident *[3]{dataByDistance1, dataByDistance2, dataByDistance3};
    resident **allDataByAgeTransportSorted = new resident *[3]{dataByAgeTransport1, dataByAgeTransport2, dataByAgeTransport3};
    resident **allDataByAgeDistanceSorted = new resident *[3]{dataByAgeDistance1, dataByAgeDistance2, dataByAgeDistance3};
    resident **allDataByTransportDistanceSorted = new resident *[3]{dataByTransportDistance1, dataByTransportDistance2, dataByTransportDistance3};
    resident **allDataByAgeTransportDistanceSorted = new resident *[3]{dataByAgeTransportDistance1, dataByAgeTransportDistance2, dataByAgeTransportDistance3};
    enum criteria
    {
        AGE_GROUP,
        TRANSPORT,
        DAILY_DISTANCE,
        DAILY_DISTANCE_GREATER,
        DAILY_DISTANCE_LESS,
        AGE_TRANSPORT,
        AGE_DISTANCE,
        TRANSPORT_DISTANCE,
        AGE_TRANSPORT_DISTANCE
    };
    // Create struct to store search results
    struct searchStruct
    {
        int amount;
        double time;
        int results[200];
    } data1Result, data2Result, data3Result;
    struct searchStruct structArr[3] = {data1Result, data2Result, data3Result};
    resident **dataset;
    void setSearchResults(int search, struct searchStruct &result, bool &flag, int &counter);
    void setBinarySearchResults(struct searchStruct &result, int mid,
                                resident arr[], int size, enum criteria searchCriteria,
                                float dailyDistance,
                                int &resultCount);
    bool matchesCriteria(resident &r, enum criteria searchCriteria, float dailyDistance);
    float estimateHeuristic(resident &r, enum criteria searchCriteria, float dailyDistance);
    void breadthFirstSearch(resident *arrayData[], int subarraySize[], enum criteria searchCriteria);
    void depthFirstSearch(resident *arrayData[], int subarraySize[], enum criteria searchCriteria);
    void aStarSearch(resident *arrayData[], int subarraySize[], enum criteria searchCriteria);
    enum criteria criteriaChoice(int choice, string &ageGroup, string &transport, float &dailyDistance, string &searchCriteria);
    void printFinalResult(string algoName, string datasetName, string targetCriteria, size_t initialMemory, size_t peakMemory, size_t finalMemory, int iterationCount);
    int getEncode(string transport);
    int getMinAge(string ageGroup);
    int getMaxAge(string ageGroup);

public:
    searchAlgorithm(resident *dataset1, resident *dataset2, resident *dataset3, enum SortKey BY_AGE,
                    enum SortKey BY_DAILY_DISTANCE, enum SortKey BY_TRANSPORT_MODE, enum SortKey BY_AGE_TRANSPORT,
                    enum SortKey BY_AGE_DISTANCE, enum SortKey BY_TRANSPORT_DISTANCE, enum SortKey BY_AGE_TRANSPORT_DISTANCE,
                    sortAlgorithm sortAlgo);
    void chooseAlgorithm();
    void linearSearch(resident *arrayData[], int subarraySize[], enum criteria searchCriteria);
    void binarySearch(resident *arrayData[], enum criteria searchCriteria, int subarraySize[]);
    void averageSearchInfo(string searchName, size_t axMemory[], float exTime[]);
};

void *operator new(size_t size);
void operator delete(void *memory) noexcept;
void operator delete(void *memory, size_t size) noexcept;
void *operator new[](size_t size);
void operator delete[](void *memory) noexcept;
void operator delete[](void *memory, size_t size) noexcept;

class dataOperations{
    private:
        string residentID, age, transport, dailyDistance, carbonEmissionFactor, averageDay, line;
        int seniorCount = 0, lateCount = 0, earlyCount = 0, universityCount = 0, childCount = 0;
        int carCount, busCount, bicycleCount, walkCount, schoolBusCount, poolCount;
        void calculateTransportModeCount(resident array[], string ageGroup, int& carCount, int& busCount,
            int& bicycleCount, int& walkCount, int& schoolBusCount, int& poolCount, int size);
    public:
        resident* importDataset(string filename, resident dataset[], int size);
        void categorizeAgeGroup(resident array[], int size);
        void favoriteTransportMode(resident array[], int size, string ageGroup, int carCount, int busCount,
            int bicycleCount, int walkCount, int schoolBusCount, int poolCount);
        void totalCarbonEmission(resident* array[], int size1, int size2, int size3);
        void displayTotalEmission(string datasetName, float totalEmission, float
            seniorEmission, float lateEmission, float earlyEmission, float universityEmission, float childEmission);
        void averageCarbonEmissionPerResident(resident array[], float seniorEmission, float lateEmission,
            float earlyEmission, float universityEmission, float childEmission, float totalEmission, int size);
        void displayFavoriteTransportMode(string ageGroup, string favoriteMode);
        void displayAgeGroupData();
        void displayData(resident array[], int size);
        void carbonEmissionPerTransportMode(resident array[], string datasetName, int size);
        void emissionComparison(resident array[], string ageGroup, int size, string datasetName);
        void displayEmissionComparison(int carCount, int busCount, int bicycleCount,
            int walkCount, int schoolBusCount, int poolCount, float totalEmission, float carEmission,
            float busEmission, float bicycleEmission, float walkEmission, float schoolEmission, float poolEmission,
            float avgCarEmission, float avgBusEmission, float avgBicycleEmission, float avgWalkEmission, float avgSchoolEmission,
            float avgPoolEmission, string ageGroup, string datasetName);
        //setters for age group counts
        void setSeniorCount(int count);
        void setLateCount(int count);
        void setEarlyCount(int count);
        void setUniversityCount(int count);
        void setChildCount(int count);
};

#endif