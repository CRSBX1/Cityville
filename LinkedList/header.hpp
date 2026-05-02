#ifndef HEADER_HPP
#define HEADER_HPP

#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <chrono>
#include <cstdlib>
using namespace std;


//CONSTANTS
const int total_age_groups = 5;
const int max_transport_types = 15;
const int max_tree_children = 20;

//DATA STRUCTURES
struct Resident {
    string ResidentId;
    int Age;
    string ModeOfTransport;
    int DailyDistance;
    float CarbonEmissionFactor;
    int AverageDayPerMonth;
};

struct Node {
    Resident data;
    Node* next;
};

struct TransportCount {
    string modeName[max_transport_types];
    int userCount[max_transport_types];
    float modeCO2[max_transport_types];
    int totalModes;
};

struct GroupMemberNode {
    Node* residentPtr;
    GroupMemberNode* next;
};

struct AgeGroupSummary {
    string groupLabel;
    int totalResidents;
    float totalCO2;
    float averageCO2;
    float percentOfGrandTotal;
    string preferredTransportMode;
    TransportCount transportUsage;
    GroupMemberNode* membersHead;
};

enum SortField { sort_age, sort_distance, sort_carbem };
enum SortOrder { sort_asc, sort_desc };

struct TreeNode {
    string label;
    int level;
    Node* residentsHead;
    int residentCount;
    TreeNode* children[max_tree_children];
    int childCount;
};

struct TreeQueueNode {
    TreeNode* treePtr;
    TreeQueueNode* next;
};

struct TreeQueue {
    TreeQueueNode* front;
    TreeQueueNode* rear;
};

struct AStarNode {
    float f;
    float g;
    float h;
    Resident data;
};

struct MinHeap {
    AStarNode* arr;
    int size;
    int capacity;

    MinHeap(int cap) : size(0), capacity(cap) {
        arr = new AStarNode[cap];
    }
    ~MinHeap() { delete[] arr; }

    void swapNodes(int i, int j) {
        AStarNode tmp = arr[i]; arr[i] = arr[j]; arr[j] = tmp;
    }
    void heapifyUp(int i) {
        while(i > 0) {
            int parent = (i - 1) / 2;
            if(arr[parent].f > arr[i].f) { swapNodes(parent, i); i = parent; }
            else break;
        }
    }
    void heapifyDown(int i) {
        while(true) {
            int smallest = i;
            int l = 2 * i + 1;
            int r = 2 * i + 2;
            if(l < size && arr[l].f < arr[smallest].f) smallest = l;
            if(r < size && arr[r].f < arr[smallest].f) smallest = r;
            if(smallest == i) break;
            swapNodes(i, smallest); i = smallest;
        }
    }
    void push(const AStarNode& node) {
        if(size < capacity) { arr[size++] = node; heapifyUp(size - 1); }
    }
    AStarNode pop() {
        AStarNode top = arr[0];
        arr[0] = arr[--size];
        heapifyDown(0);
        return top;
    }
    bool empty() const { return size == 0; }
};

//MEMORY TRACKING
struct MemoryMetrics {
    static size_t allocatedMemory;
    static size_t freedMemory;
    static size_t trackMemoryUsage();
    static void reset();
};

//FUNCTION DECLARATIONS
//utils.cpp
string toLower(const string& inputStr);
chrono::high_resolution_clock::time_point startTimer();
double stopTimer(chrono::high_resolution_clock::time_point start);
double stopTimerMicro(chrono::high_resolution_clock::time_point start);
int getAgeGroupIndex(int age);
string getAgeGroupLabel(int groupIndex);
string getAgeRangeLabel(int age);
string getDistanceRangeLabel(int dist);

//data.cpp
void insert(Node*& head, Resident r);
void loadData(Node*& head, string filename);
Node* copyList(Node* head);
void freeList(Node* head);
Resident* listToArray(Node* head, int& size);
int countNodes(Node* head);

// analysis.cpp
float calculateMonthlyCO2(const Node* resident);
void addToTransportCount(TransportCount& tally, const string& mode, float co2);
string getMostUsedMode(const TransportCount& tally);
void categorizeByAgeGroup(Node* head, AgeGroupSummary groupSummary[]);
void freeMemberLists(AgeGroupSummary groupSummary[]);

// display.cpp
void printSeparator(int width, char c);
void printPerformance(double elapsedMs, size_t memoryBytes);
void printResidentHeader(int width);
void printResidentRow(Node* res);
void printList(Node* head);
void displayRawData(Node* headA, Node* headB, Node* headC);
void displayCategorizedData(Node* headA, Node* headB, Node* headC);
void displayCategorizedDataAllCities(Node* headA, Node* headB, Node* headC);
void displayEmissionsByTransportMode(Node* head, const string& cityName);
void displayTable1_ByAgeGroup(Node* head, const string& cityName);
void displayTable2_CombinedTransport(Node* headA, Node* headB, Node* headC);
void displayTable3_CrossCity(Node* headA, Node* headB, Node* headC);
void printSearchResults(Node* matchedHead, int matchCount, const string& label, const string& dataset);

//sort.cpp
bool shouldSwap(Node* a, Node* b, SortField field, SortOrder order);
void bubbleSort(Node* head, SortField field, SortOrder order);
void insertionSort(Node* head, SortField field, SortOrder order);
string fieldName(SortField field);
string orderName(SortOrder order);
void displaySortResult(Node* originalHead, const string& cityName, const string& algoName, SortField field, SortOrder order, bool useInsertion);
void arraySortInt(Resident* arr, int size, bool byAge);
Node* sortListCopyByField(Node* head, const string& field);

//search.cpp
void linearSearchCombined_Unsorted(Node* head, int ageGroup, const string& targetMode, int distThreshold, char distOp, Node*& matchedHead, int& matchCount);
void linearSearchCombined_Sorted(Node* head, int ageGroup, const string& targetMode, int distThreshold, char distOp, Node*& matchedHead, int& matchCount);
void binarySearchByAgeGroup_Sorted(Node* head, int minAge, int maxAge, Node*& matchedHead, int& matchCount);
void binarySearchByTransport_Sorted(Node* head, const string& targetMode, Node*& matchedHead, int& matchCount);
void binarySearchByDistance_Sorted(Node* head, int target, Node*& matchedHead, int& matchCount);
void binarySearchByAgeGroup_Unsorted(Node* head, int minAge, int maxAge, Node*& matchedHead, int& matchCount);
void binarySearchByTransport_Unsorted(Node* head, const string& targetMode, Node*& matchedHead, int& matchCount);
void binarySearchByDistance_Unsorted(Node* head, int target, Node*& matchedHead, int& matchCount);
bool passesFilters(const Resident& res, int ageGroup, const string& targetLower, int distThreshold, char distOp);
float computeAStarG(const Resident& r, bool filterAge, int minAge, int maxAge, bool filterTrans, const string& targetLower, bool filterDist, int distThreshold, char distOp);
float computeAStarH(float g);
void aStarSearchUnsorted(Node* head, int ageGroup, const string& targetMode, int distThreshold, char distOp, Node*& matchedHead, int& matchCount);
void aStarSearchSorted(Node* head, int ageGroup, const string& targetMode, int distThreshold, char distOp, Node*& matchedHead, int& matchCount);

//tree.cpp
TreeNode* createTreeNode(const string& label, int level);
TreeNode* findChild(TreeNode* parent, const string& label);
TreeNode* addChild(TreeNode* parent, const string& label, int level);
TreeNode* buildTransportTree(Node* head, const string& rootLabel);
void freeTree(TreeNode* node);
void initTreeQueue(TreeQueue& q);
bool isTreeQueueEmpty(const TreeQueue& q);
void enqueueTree(TreeQueue& q, TreeNode* node);
TreeNode* dequeueTree(TreeQueue& q);
string indent(int level);
void bfsSearchUnsorted(Node* head, const string& rootLabel, int ageGroup, const string& targetMode, int distThreshold, char distOp, Node*& matchedHead, int& matchCount);
void bfsSearchSorted(Node* head, const string& rootLabel, int ageGroup, const string& targetMode, int distThreshold, char distOp, Node*& matchedHead, int& matchCount);
void dfsTreeHelper(TreeNode* current, int ageGroup, const string& targetLower, int distThreshold, char distOp, Node*& matchedHead, int& matchCount);
void dfsSearchUnsorted(Node* head, const string& rootLabel, int ageGroup, const string& targetMode, int distThreshold, char distOp, Node*& matchedHead, int& matchCount);
void dfsSearchSorted(Node* head, const string& rootLabel, int ageGroup, const string& targetMode, int distThreshold, char distOp, Node*& matchedHead, int& matchCount);

//menu.cpp
void mainMenu(Node* cityA, Node* cityB, Node* cityC);
void sortingExperimentsMenu(Node* cityA, Node* cityB, Node* cityC);
void searchingMenu(Node* cityA, Node* cityB, Node* cityC);
void searchComparisonMenu(Node* cityA, Node* cityB, Node* cityC);
void linearSearchMenu(Node* cityA, Node* cityB, Node* cityC);
void binarySearchMenu(Node* cityA, Node* cityB, Node* cityC);
void bfsSearchMenu(Node* cityA, Node* cityB, Node* cityC);
void dfsSearchMenu(Node* cityA, Node* cityB, Node* cityC);
void aStarSearchMenu(Node* cityA, Node* cityB, Node* cityC);
int selectCity();
void selectSearchCriteria(int& ageGroup, string& targetMode, int& distThreshold, char& distOp);
string buildSearchLabel(const string& algoName, int ageGroup, const string& targetMode, int distThreshold, char distOp);

#endif
