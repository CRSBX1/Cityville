#include "header.hpp"

//LINEAR SEARCH (UNSORTED)
//combined linear search (age, transport mode, distance)
void linearSearchCombined_Unsorted(Node* head, int ageGroup, const string& targetMode, int distThreshold, char distOp, Node*& matchedHead, int& matchCount){
    matchedHead = NULL; //initialize results list as empty
    matchCount = 0; //initialize results list as empty

    //match the selected age group to its range
    int minAge, maxAge;
    switch(ageGroup){
        case 1:
            minAge = 6; maxAge = 17; break; //children & teenagers
        case 2:
            minAge = 18; maxAge = 25; break; //university / young adults
        case 3:
            minAge = 26; maxAge = 45; break; //working adults early career
        case 4:
            minAge = 46; maxAge = 60; break; //working adults late career
        case 5: 
            minAge = 61; maxAge = 100; break; //senior citizens / retirees
        default:
            minAge = -1; maxAge = -1; break; //invalid group, filter will be skipped
    }
    bool filterAge = (ageGroup >= 1 && ageGroup <= 5); //age filter for valid group
    bool filterTransport = !targetMode.empty(); //transport filter for valid group
    bool filterDistance = (distOp == '>' || distOp == '<' || distOp == '='); //distance filter for valid group

    string targetLower = toLower(targetMode); //convert transport target to lowercase for comparison

    Node* current = head; //start scanning from first resident
    while(current != NULL){
        //each filter starts as passed and is only false if the resident fails the filter
        bool passAge = true;
        bool passTransport = true;
        bool passDistance = true;

        //apply age filter if it is valid
        if(filterAge){
            passAge = (current->data.Age >= minAge && current->data.Age <= maxAge);
        }
        //apply transport filter if it is valid
        if(filterTransport){
            passTransport = (toLower(current->data.ModeOfTransport) == targetLower);
        }
        //apply distance filter if it is valid
        if(filterDistance){
            if(distOp == '>' && !(current->data.DailyDistance > distThreshold)){
                passDistance = false;
            }else if(distOp == '<' && !(current->data.DailyDistance < distThreshold)){
                passDistance = false;
            }else if(distOp == '=' && !(current->data.DailyDistance == distThreshold)){
                passDistance = false;
            }       
        }
        //only include residents that passed every filter
        if(passAge && passTransport && passDistance){
            insert(matchedHead, current->data); //copy matching resident into results list
            matchCount++; //increment match counter
        }
        current = current->next; //move to the next resident in the list
    }
}

//LINEAR SEARCH (S0RTED)
//the only difference between sorted and unsorted linear is that sorted linear implements early exits to stop scanning once no more matches can be found
//combined linear search
void linearSearchCombined_Sorted(Node* head, int ageGroup, const string& targetMode, int distThreshold, char distOp, Node*& matchedHead, int& matchCount){
    matchedHead = NULL; //initialize results list as empty
    matchCount = 0; //initialize match counter to 0

    //match the selected age group to its range
    int minAge, maxAge;
    switch(ageGroup){
        case 1:
            minAge = 6; maxAge = 17; break; //children & teenagers
        case 2:
            minAge = 18; maxAge = 25; break; //university / young adults
        case 3:
            minAge = 26; maxAge = 45; break; //working adults early career
        case 4:
            minAge = 46; maxAge = 60; break; //working adults late career
        case 5: 
            minAge = 61; maxAge = 100; break; //senior citizens / retirees
        default:
            minAge = -1; maxAge = -1; break; //invalid group, filter will be skipped
    }
    bool filterAge = (ageGroup >= 1 && ageGroup <= 5); //age filter for valid group
    bool filterTransport = !targetMode.empty(); //transport filter for valid group
    bool filterDistance = (distOp == '>' || distOp == '<' || distOp == '='); //distance filter for valid group

    string targetLower = toLower(targetMode); //convert transport target to lowercase for comparison

    Node* current = head; //start scanning from first resident
    while(current != NULL){
        //each filter starts as passed and is only false if the resident fails the filter
        bool passAge = true;
        bool passTransport = true;
        bool passDistance = true;

        //apply age filter if it is valid
        if(filterAge){
            if(current->data.Age > maxAge){ 
                break; //early exit, sorted by age
            }
            passAge = (current->data.Age >= minAge);
        }
        //apply transport filter if it is valid
        if(filterTransport){
            string curMode = toLower(current->data.ModeOfTransport);
            if(!filterAge && curMode > targetLower){
                break; //early exit, only if age is not active
            }
            passTransport = (curMode == targetLower);
        }
        //apply distance filter if it is valid
        if(filterDistance){
            int dist = current->data.DailyDistance;
            if(!filterAge && !filterTransport){ //early exit only if age and transport is not active
                if(distOp == '<' && dist >= distThreshold){
                    break;
                }if(distOp == '=' && dist > distThreshold){
                    break;
                }
            }
            if(distOp == '>' && dist > distThreshold){
                passDistance = true;
            }else if(distOp == '<' && dist < distThreshold){
                passDistance = true;
            }else if(distOp == '=' && dist == distThreshold){
                passDistance = true;
            }else{
                passDistance = false;
            }
        }
        //only include residents that passed every filter
        if(passAge && passTransport && passDistance){
            insert(matchedHead, current->data); //copy matching resident into results list
            matchCount++; //increment match counter
        }
        current = current->next; //move to the next resident in the list
    }
}

//BINARY SEARCH (SORTED)
//binary search by age
void binarySearchByAgeGroup_Sorted(Node* head, int minAge, int maxAge, Node*& matchedHead, int& matchCount){
    matchedHead = NULL; //initialize results list as empty
    matchCount = 0; //initialize match counter to 0

    int size = 0;
    Resident* arr = listToArray(head, size); //convert linked list to array
    if(!arr){
        return; //for empty list, nothing to search
    }

    arraySortInt(arr, size, true); //sort array by age in ascending 

    //binary search to find the first index where age >= minAge
    int lo = 0, hi = size -1;
    int startIdx = size; //set default to array size, meaning no matching resident has been found yet
    while(lo <= hi){
        int mid = lo + (hi - lo) / 2; //calculate the middle index to avoid integer overflow
        if(arr[mid].Age >= minAge){
            //if resident qualifies, save this position for startIdx, 
            //but keep searching leftward in case earlier resident also qualifies
            startIdx = mid;
            hi = mid-1; //move the right pointer leftward to search the left half
        }else{
            //the resident is too young, all residents to the left are also too young
            //discard the entire left half and search the right half instead
            lo = mid+1; //move left pointer rightward
        }
    }
    //after the loop, startIdx will hold the first index where Age >= minAge

    //walk rightward from startIdx and collect every resident whose age is within range
    for(int i=startIdx; i<size && arr[i].Age <= maxAge; i++){
        insert(matchedHead, arr[i]); //add qualified resident to results list
        matchCount++; //increment match counter
    }
    delete[] arr; //free temporary array from memory
}

//binary search by mode of transport
void binarySearchByTransport_Sorted(Node* head, const string& targetMode, Node*& matchedHead, int& matchCount){
    matchedHead = NULL; //initialize results list as empty
    matchCount = 0; //initialize match counter to 0

    int size = 0;
    Resident *arr = listToArray(head, size); //convert linked list to array
    if(!arr){
        return; //for empty list, nothing to search
    }

    //sort array alphabetically using insertion sort, toLower is also applied for fair comparison
    for(int i=1; i<size; i++){
        Resident key = arr[i];
        string keyMode = toLower(key.ModeOfTransport);
        int j = i-1;
        //shift residents rightward whose transport mode comes after the piked mode alphabetically
        while(j >= 0 && toLower(arr[j].ModeOfTransport) > keyMode){
            arr[j+1] = arr[j]; //slide this resident one space to the right
            j--;
        }
        arr[j+1] = key; //place picked resident in their correct alphabetical position
    } 

    string targetLower = toLower(targetMode); //convert search target to lowercase for consistent comparison

    //locate any one resident using the target transport mode
    int lo = 0, hi = size-1, foundIdx = -1;
    while(lo<=hi){
        int mid = lo + (hi - lo) / 2;
        string midMode = toLower(arr[mid].ModeOfTransport); //get this resident mode in lowercase
        if(midMode == targetLower){
            foundIdx = mid; //found one match, save the index
            break; //stop searching, expansion will find the rest
        }else if(midMode < targetLower){
            //middle mode comes before target alphabetically (search right half)
            lo = mid + 1;
        }else{
            //middle mode comes after target alphabetically (search left half)
            hi = mid - 1;
        }
    }
    
    //if foundIdx = -1, no resident uses the target transport mode
    if(foundIdx == -1){
        delete[] arr; //free array before exiting
        return; //results list remains empty
    }
    //move leftward from foundIdx to find the first resident using this mode
    //stops when the resident to the left uses a different transport mode
    int left = foundIdx;
    while(left > 0 && toLower(arr[left-1].ModeOfTransport) == targetLower){
        left--; //step one position to the left
    }
    //move rightward from foundIdx to find the last resident using this mode
    //stops when the resident to the right uses a different transport mode
    int right = foundIdx;
    while(right < size-1 && toLower(arr[right+1].ModeOfTransport) == targetLower){
        right++; //step one position to the right
    }
    //now left and right mark the exact boundaries of all residents using this transport mode

    //get all residents between left and right boundaries into the results list
    for(int i=left; i<=right; i++){
        insert(matchedHead, arr[i]); //add this resident to results list
        matchCount++; //increment match counter
    }

    delete[] arr; //free temporary array from memory
}

//binary search by daily distance
void binarySearchByDistance_Sorted(Node* head, int target, Node*& matchedHead, int& matchCount){
    matchedHead = NULL; //initialize results list as empty
    matchCount = 0; //initialize match counter to 0

    int size = 0;
    Resident *arr = listToArray(head, size); //convert linked list to array
    if(!arr){
        return; //for empty list, nothing to search
    }

    arraySortInt(arr, size, false); //sort array by daily distance in ascending

    int lo = 0, hi = size-1, foundIdx = -1;
    while(lo<=hi){
        int mid = lo + (hi - lo) / 2; //calculate middle index
        if(arr[mid].DailyDistance == target){
            foundIdx = mid; //found one match, save the index
            break; //stop searching, expansion will find the rest
        }else if(arr[mid].DailyDistance < target){
            //middle resident distance is smaller than target, discard all resident to the left
            lo = mid + 1;
        }else{
            //middle resident distance is larger than target, discard all resident to the right
            hi = mid - 1;
        }
    }

    if(foundIdx == -1){
        delete[] arr;
        return;
    }

    //move leftward from foundIdx to find the first resident using this distance
    //stops when the resident to the left has a different distance value
    int left = foundIdx;
    while(left > 0 && arr[left-1].DailyDistance == target){
        left--;
    }
    //move rightward from foundIdx to find the last resident using this distance
    //stops when the resident to the right has a different distance value
    int right = foundIdx;
    while(right < size-1 && arr[right+1].DailyDistance == target){
        right++;
    }
    //get all residents between left and right boundaries into the results list
    for(int i=left; i<=right; i++){
        insert(matchedHead, arr[i]); //add this resident to results list
        matchCount++; //increment match counter
    }

    delete[] arr; //free temporary array from memory
}

//BINARY SEARCH (UNSORTED)
//the only difference between sorted and unsorted binary search is by removing the sorting step
//binary search by age
void binarySearchByAgeGroup_Unsorted(Node* head, int minAge, int maxAge, Node*& matchedHead, int& matchCount){
    matchedHead = NULL; 
    matchCount = 0; 

    int size = 0;
    Resident* arr = listToArray(head, size); 
    if(!arr){
        return; 
    }

    int lo = 0, hi = size -1;
    int startIdx = size; 
    while(lo <= hi){
        int mid = lo + (hi - lo) / 2; 
        if(arr[mid].Age >= minAge){
            startIdx = mid;
            hi = mid-1; 
        }else{
            lo = mid+1; 
        }
    }

    for(int i=startIdx; i<size && arr[i].Age <= maxAge; i++){
        insert(matchedHead, arr[i]); 
        matchCount++;
    }
    delete[] arr; 
}

//binary search by mode of transport
void binarySearchByTransport_Unsorted(Node* head, const string& targetMode, Node*& matchedHead, int& matchCount){
    matchedHead = NULL; 
    matchCount = 0; 

    int size = 0;
    Resident *arr = listToArray(head, size); 
    if(!arr){
        return; 
    }

    string targetLower = toLower(targetMode); 

    int lo = 0, hi = size-1, foundIdx = -1;
    while(lo<=hi){
        int mid = lo + (hi - lo) / 2;
        string midMode = toLower(arr[mid].ModeOfTransport); 
        if(midMode == targetLower){
            foundIdx = mid; 
            break; 
        }else if(midMode < targetLower){
            lo = mid + 1;
        }else{
            hi = mid - 1;
        }
    }
    
    if(foundIdx == -1){
        delete[] arr; 
        return; 
    }

    int left = foundIdx;
    while(left > 0 && toLower(arr[left-1].ModeOfTransport) == targetLower){
        left--; 
    }
    int right = foundIdx;
    while(right < size-1 && toLower(arr[right+1].ModeOfTransport) == targetLower){
        right++; //step one position to the right
    }
   
    for(int i=left; i<=right; i++){
        insert(matchedHead, arr[i]); 
        matchCount++; 
    }

    delete[] arr; 
}

//binary search by daily distance
void binarySearchByDistance_Unsorted(Node* head, int target, Node*& matchedHead, int& matchCount){
    matchedHead = NULL; 
    matchCount = 0; 

    int size = 0;
    Resident *arr = listToArray(head, size); 
    if(!arr){
        return; 
    }

    int lo = 0, hi = size-1, foundIdx = -1;
    while(lo<=hi){
        int mid = lo + (hi - lo) / 2; 
        if(arr[mid].DailyDistance == target){
            foundIdx = mid; 
            break; 
        }else if(arr[mid].DailyDistance < target){
            lo = mid + 1;
        }else{
            hi = mid - 1;
        }
    }

    if(foundIdx == -1){
        delete[] arr;
        return;
    }

    int left = foundIdx;
    while(left > 0 && arr[left-1].DailyDistance == target){
        left--;
    }
    int right = foundIdx;
    while(right < size-1 && arr[right+1].DailyDistance == target){
        right++;
    }
   
    for(int i=left; i<=right; i++){
        insert(matchedHead, arr[i]); 
        matchCount++; 
    }

    delete[] arr; 
}

//checks whether a single resident passes all active filters
//filters are: age group, transport mode, and daily distance threshold
bool passesFilters(const Resident& res, int ageGroup, const string& targetLower, int distThreshold, char distOp) {
    bool filterAge = (ageGroup >= 1 && ageGroup <= 5); //age filter active for valid group
    bool filterTransport = !targetLower.empty(); //transport filter active if user entered a mode
    bool filterDistance = (distOp == '>' || distOp == '<' || distOp == '='); //distance filter active for valid operator

    bool passAge = true;
    bool passTransport = true;
    bool passDistance = true;

    //apply age filter if active
    if(filterAge) {
        int minAge, maxAge;
        switch(ageGroup) {
            case 1: minAge = 6;  maxAge = 17;  break; //children & teenagers
            case 2: minAge = 18; maxAge = 25;  break; //university / young adults
            case 3: minAge = 26; maxAge = 45;  break; //working adults early career
            case 4: minAge = 46; maxAge = 60;  break; //working adults late career
            case 5: minAge = 61; maxAge = 100; break; //senior citizens / retirees
            default: minAge = -1; maxAge = -1;  break;
        }
        passAge = (res.Age >= minAge && res.Age <= maxAge);
    }
    //apply transport filter if active
    if(filterTransport) {
        passTransport = (toLower(res.ModeOfTransport) == targetLower);
    }
    //apply distance filter if active
    if(filterDistance) {
        if(distOp == '>' && !(res.DailyDistance > distThreshold)) {
            passDistance = false;
        } else if(distOp == '<' && !(res.DailyDistance < distThreshold)) {
            passDistance = false;
        } else if(distOp == '=' && !(res.DailyDistance == distThreshold)) {
            passDistance = false;
        }
    }

    return passAge && passTransport && passDistance; //resident must pass every active filter
}

//compute g-cost: number of a resident fails
float computeAStarG(const Resident& r,
                    bool filterAge,   int minAge,   int maxAge,
                    bool filterTrans, const string& targetLower,
                    bool filterDist,  int distThreshold, char distOp)
{
    float g = 0.0f; //start with 0 failures
    //add 1 for each criterion the resident fails
    if(filterAge && !(r.Age >= minAge && r.Age <= maxAge))
        g += 1.0f; //age is outside the target range
    if(filterTrans && toLower(r.ModeOfTransport) != targetLower)
        g += 1.0f; //transport mode does not match
    if(filterDist) {
        bool ok = false;
        if(distOp == '>' && r.DailyDistance >  distThreshold) ok = true;
        if(distOp == '<' && r.DailyDistance <  distThreshold) ok = true;
        if(distOp == '=' && r.DailyDistance == distThreshold) ok = true;
        if(!ok) g += 1.0f; //distance condition not satisfied
    }
    return g; //return number of failures
}


//A*SEARCH
//compute h-cost: 0 for a perfect match, 0.1 otherwise (admissible heuristic)
float computeAStarH(float g) {
    return (g == 0.0f) ? 0.0f : 0.1f;
}

//A* on the raw unsorted linked list
void aStarSearchUnsorted(Node* head,
                         int ageGroup,
                         const string& targetMode,
                         int distThreshold, char distOp,
                         Node*& matchedHead, int& matchCount)
{
    matchedHead = NULL; //results linked list starts empty
    matchCount  = 0;

    int minAge = -1, maxAge = -1;
    switch(ageGroup) { //converts age group number into min/max range
        case 1: minAge =  6; maxAge =  17; break; 
        case 2: minAge = 18; maxAge =  25; break; 
        case 3: minAge = 26; maxAge =  45; break; 
        case 4: minAge = 46; maxAge =  60; break; 
        case 5: minAge = 61; maxAge = 100; break; 
    }
    bool filterAge   = (ageGroup >= 1 && ageGroup <= 5); //true if the user select a valid age group, false if choose 0 (skip)
    bool filterTrans = !targetMode.empty(); //true if the user typed a transport mode, false if they pressed enter
    bool filterDist  = (distOp == '>' || distOp == '<' || distOp == '='); //true if the user entered a valid operator, false if they skipped
    string targetLower = toLower(targetMode); //convert the transport target to lowercase

    int n = countNodes(head); //count how many residents are in the list
    if(n == 0) return; //nothing to search then return immediately

    MinHeap heap(n); //open list sized for all node

    Node* cur = head; //start walking the linkedlist from the first node
    while(cur != NULL) {
        float g = computeAStarG(cur->data,
                                filterAge,   minAge,   maxAge,
                                filterTrans, targetLower,
                                filterDist,  distThreshold, distOp); //score this resident, how many criteria do they fail
        float h = computeAStarH(g); //get the heuristic value, 0 if perfect match, 0.1 otherwise
        AStarNode an; //create a star node
        an.g = g; an.h = h; an.f = g + h; an.data = cur->data; //fill in the scores and copy the resident data
        heap.push(an);       //insert into min-heap ordered by f
        cur = cur->next;     //advance to next node in the linked list
    }

    // pop in best-first order; collect only perfect matches (g == 0)
    while(!heap.empty()) {
        AStarNode best = heap.pop(); //lowest f node
        if(best.g == 0.0f) {
            insert(matchedHead, best.data); //perfect match: add to results
            matchCount++;
        } else {
            //g > 0 means this resident fails at least one criterion
            //all remaining heap nodes also have g > 0 — stop here
            break;
        }
    }
}

//A* on an age-sorted linked list
void aStarSearchSorted(Node* head,
                       int ageGroup,
                       const string& targetMode,
                       int distThreshold, char distOp,
                       Node*& matchedHead, int& matchCount)
{
    matchedHead = NULL; //results linked list starts empty
    matchCount  = 0;

    int minAge = -1, maxAge = -1;
    switch(ageGroup) {
        case 1: minAge =  6; maxAge =  17; break; 
        case 2: minAge = 18; maxAge =  25; break; 
        case 3: minAge = 26; maxAge =  45; break; 
        case 4: minAge = 46; maxAge =  60; break; 
        case 5: minAge = 61; maxAge = 100; break; 
    }
    bool filterAge   = (ageGroup >= 1 && ageGroup <= 5);
    bool filterTrans = !targetMode.empty();
    bool filterDist  = (distOp == '>' || distOp == '<' || distOp == '=');
    string targetLower = toLower(targetMode);

    Node* sortedList = copyList(head);
    if(sortedList == NULL) return;

    //sort the copy by age ascending using bubble sort
    bubbleSort(sortedList, sort_age, sort_asc);

    int n = countNodes(sortedList);
    MinHeap heap(n); //open list sized for all nodes

    Node* cur = sortedList;
    while(cur != NULL) {
        if(filterAge) {
            if(cur->data.Age < minAge) { cur = cur->next; continue; } 
            if(cur->data.Age > maxAge) break;
        }
        float g = computeAStarG(cur->data,
                                filterAge,   minAge,   maxAge,
                                filterTrans, targetLower,
                                filterDist,  distThreshold, distOp);
        float h = computeAStarH(g);
        AStarNode an;
        an.g = g; an.h = h; an.f = g + h; an.data = cur->data;
        heap.push(an);   //insert into min-heap ordered by f
        cur = cur->next; //advance to next node in the sorted list
    }

    //pop in best-first order; collect only perfect matches (g == 0)
    while(!heap.empty()) {
        AStarNode best = heap.pop();
        if(best.g == 0.0f) {
            insert(matchedHead, best.data); //perfect match: add to results
            matchCount++;
        } else {
            break; //all remaining nodes also have g > 0
        }
    }

    //free the temporary sorted copy
    freeList(sortedList);
}


// TREE / BFS / DFS SEARCH
//creates a new tree node with the given label and level
TreeNode* createTreeNode(const string& label, int level) {
    TreeNode* node = new TreeNode();
    node->label = label;
    node->level = level;
    node->residentsHead = NULL;
    node->residentCount = 0;
    node->childCount = 0;
    for(int i = 0; i < max_tree_children; i++) {
        node->children[i] = NULL;
    }
    return node;
}

//finds a child node by label, returns NULL if not found
TreeNode* findChild(TreeNode* parent, const string& label) {
    for(int i = 0; i < parent->childCount; i++) {
        if(parent->children[i]->label == label) {
            return parent->children[i];
        }
    }
    return NULL;
}

//adds a child node to a parent, returns the new child
TreeNode* addChild(TreeNode* parent, const string& label, int level) {
    TreeNode* existing = findChild(parent, label);
    if(existing != NULL) {
        return existing;
    }
    if(parent->childCount >= max_tree_children) {
        return NULL;
    }
    TreeNode* child = createTreeNode(label, level);
    parent->children[parent->childCount] = child;
    parent->childCount++;
    return child;
}

//builds the transport tree from a linked list of residents
TreeNode* buildTransportTree(Node* head, const string& rootLabel) {
    TreeNode* root = createTreeNode(rootLabel, 0);

    Node* current = head;
    while(current != NULL) {
        TreeNode* modeNode = addChild(root, current->data.ModeOfTransport, 1);
        string ageLabel = getAgeRangeLabel(current->data.Age);
        TreeNode* ageNode = addChild(modeNode, ageLabel, 2);
        string distLabel = getDistanceRangeLabel(current->data.DailyDistance);
        TreeNode* distNode = addChild(ageNode, distLabel, 3);

        Node* resCopy = new Node();
        resCopy->data = current->data;
        resCopy->next = distNode->residentsHead;
        distNode->residentsHead = resCopy;
        distNode->residentCount++;

        current = current->next;
    }
    return root;
}

//frees all nodes in the transport tree recursively
void freeTree(TreeNode* node) {
    if(node == NULL) return;
    for(int i = 0; i < node->childCount; i++) {
        freeTree(node->children[i]);
    }
    Node* cur = node->residentsHead;
    while(cur != NULL) {
        Node* temp = cur;
        cur = cur->next;
        delete temp;
    }
    delete node;
}

//initializes tree queue to empty state
void initTreeQueue(TreeQueue& q) {
    q.front = NULL;
    q.rear = NULL;
}

//returns true if tree queue has no elements
bool isTreeQueueEmpty(const TreeQueue& q) {
    return q.front == NULL;
}

//adds a tree node to the back of the queue
void enqueueTree(TreeQueue& q, TreeNode* node) {
    TreeQueueNode* newNode = new TreeQueueNode();
    newNode->treePtr = node;
    newNode->next = NULL;
    if(q.rear == NULL) {
        q.front = newNode;
        q.rear = newNode;
    } else {
        q.rear->next = newNode;
        q.rear = newNode;
    }
}

//removes and returns the tree node at the front of the queue
TreeNode* dequeueTree(TreeQueue& q) {
    if(q.front == NULL) return NULL;
    TreeQueueNode* temp = q.front;
    TreeNode* result = temp->treePtr;
    q.front = q.front->next;
    if(q.front == NULL) q.rear = NULL;
    delete temp;
    return result;
}

//returns indentation string based on tree level
string indent(int level) {
    string s = "";
    for(int i = 0; i < level; i++) {
        s += "  ";
    }
    return s;
}

//BFS traversal over the transport tree
void bfsSearch(Node* head, const string& rootLabel, int ageGroup, const string& targetMode, int distThreshold, char distOp, Node*& matchedHead, int& matchCount) {
    matchedHead = NULL;
    matchCount = 0;

    TreeNode* root = buildTransportTree(head, rootLabel);
    string targetLower = toLower(targetMode);

    TreeQueue q;
    initTreeQueue(q);
    enqueueTree(q, root);

    cout << "\nBFS traversal order (level by level):" << endl;
    int step = 1;
    bool firstFound = false;

    while(!isTreeQueueEmpty(q)) {
        TreeNode* current = dequeueTree(q);

        if(!firstFound) {
            cout << indent(current->level) << step << ". [L" << current->level << "] " << current->label;
            if(current->residentCount > 0) cout << " (" << current->residentCount << " residents)";
            if(current->childCount > 0) cout << " [" << current->childCount << " children]";
            cout << endl;
        }
        step++;

        Node* resCur = current->residentsHead;
        while(resCur != NULL) {
            if(passesFilters(resCur->data, ageGroup, targetLower, distThreshold, distOp)) {
                insert(matchedHead, resCur->data);
                matchCount++;
                if(!firstFound) {
                    cout << indent(current->level) << "   >> First match found at step " << (step - 1) << endl;
                    firstFound = true;
                }
            }
            resCur = resCur->next;
        }

        for(int i = 0; i < current->childCount; i++) {
            enqueueTree(q, current->children[i]);
        }
    }
    cout << "Total nodes visited: " << (step - 1) << endl;
    freeTree(root);
}

//DFS SEARCH
//DFS recursive helper
void dfsTreeHelper(TreeNode* current, int ageGroup, const string& targetLower, int distThreshold, char distOp, Node*& matchedHead, int& matchCount, int& step, bool& firstFound) {
    if(!firstFound) {
        cout << indent(current->level) << step << ". [L" << current->level << "] " << current->label;
        if(current->residentCount > 0) cout << " (" << current->residentCount << " residents)";
        if(current->childCount > 0) cout << " [" << current->childCount << " children]";
        cout << endl;
    }
    step++;

    Node* resCur = current->residentsHead;
    while(resCur != NULL) {
        if(passesFilters(resCur->data, ageGroup, targetLower, distThreshold, distOp)) {
            insert(matchedHead, resCur->data);
            matchCount++;
            if(!firstFound) {
                cout << indent(current->level) << "   >> First match found at step " << (step - 1) << endl;
                firstFound = true;
            }
        }
        resCur = resCur->next;
    }

    for(int i = 0; i < current->childCount; i++) {
        dfsTreeHelper(current->children[i], ageGroup, targetLower, distThreshold, distOp, matchedHead, matchCount, step, firstFound);
    }
}

//DFS traversal over the transport tree
void dfsSearch(Node* head, const string& rootLabel, int ageGroup, const string& targetMode, int distThreshold, char distOp, Node*& matchedHead, int& matchCount) {
    matchedHead = NULL;
    matchCount = 0;

    TreeNode* root = buildTransportTree(head, rootLabel);
    string targetLower = toLower(targetMode);

    cout << "\nDFS traversal order (depth-first):" << endl;
    int step = 1;
    bool firstFound = false;
    dfsTreeHelper(root, ageGroup, targetLower, distThreshold, distOp, matchedHead, matchCount, step, firstFound);
    cout << "Total nodes visited: " << (step - 1) << endl;
    freeTree(root);
}
