#include "header.hpp"

//LINEAR SEARCH (UNSORTED)
//combined linear search (age, transport mode, distance)
void linearSearchCombined_Unsorted(Node* head, int ageGroup, const string& targetMode, int distThreshold, char distOp, Node*& matchedHead, int& matchCount){
    matchedHead = NULL;
    matchCount = 0;

    int minAge, maxAge;
    getAgeRange(ageGroup, minAge, maxAge);

    bool filterAge = isValidAgeGroup(ageGroup);
    bool filterTransport = !targetMode.empty();
    bool filterDistance = isValidDistOp(distOp);

    string targetLower = toLower(targetMode);

    Node* current = head;
    while(current != NULL){
        bool passAge = true;
        bool passTransport = true;
        bool passDistance = true;

        if(filterAge){
            passAge = (current->data.Age >= minAge && current->data.Age <= maxAge);
        }
        if(filterTransport){
            passTransport = (toLower(current->data.ModeOfTransport) == targetLower);
        }
        if(filterDistance){
            passDistance = passesDistanceFilter(current->data.DailyDistance, distThreshold, distOp);
        }

        if(passAge && passTransport && passDistance){
            insert(matchedHead, current->data);
            matchCount++;
        }
        current = current->next;
    }
}

//LINEAR SEARCH (SORTED)
//sorted linear implements early exits to stop scanning once no more matches can be found
void linearSearchCombined_Sorted(Node* head, int ageGroup, const string& targetMode, int distThreshold, char distOp, Node*& matchedHead, int& matchCount){
    matchedHead = NULL;
    matchCount = 0;

    int minAge, maxAge;
    getAgeRange(ageGroup, minAge, maxAge);

    bool filterAge = isValidAgeGroup(ageGroup);
    bool filterTransport = !targetMode.empty();
    bool filterDistance = isValidDistOp(distOp);

    string targetLower = toLower(targetMode);

    Node* current = head;
    while(current != NULL){
        bool passAge = true;
        bool passTransport = true;
        bool passDistance = true;

        if(filterAge){
            if(current->data.Age > maxAge){ 
                break; //early exit, sorted by age
            }
            passAge = (current->data.Age >= minAge);
        }
        if(filterTransport){
            string curMode = toLower(current->data.ModeOfTransport);
            if(!filterAge && curMode > targetLower){
                break; //early exit, only if age is not active
            }
            passTransport = (curMode == targetLower);
        }
        if(filterDistance){
            int dist = current->data.DailyDistance;
            if(!filterAge && !filterTransport){
                if(distOp == '<' && dist >= distThreshold) break;
                if(distOp == '=' && dist > distThreshold) break;
            }
            passDistance = passesDistanceFilter(dist, distThreshold, distOp);
        }

        if(passAge && passTransport && passDistance){
            insert(matchedHead, current->data);
            matchCount++;
        }
        current = current->next;
    }
}

//binary search by age group (sorted)
//converts list to array, sorts by age, then uses binary search
void binarySearchByAgeGroup_Sorted(Node* head, int minAge, int maxAge, Node*& matchedHead, int& matchCount){
    matchedHead = NULL;
    matchCount = 0;

    int size = 0;
    Resident* arr = listToArray(head, size);
    if(!arr) return;

    arraySortInt(arr, size, true); //sort array by age ascending

    //binary search to find the first index where age >= minAge
    int lo = 0, hi = size - 1;
    int startIdx = size;
    while(lo <= hi){
        int mid = lo + (hi - lo) / 2;
        if(arr[mid].Age >= minAge){
            startIdx = mid;
            hi = mid - 1;
        }else{
            lo = mid + 1;
        }
    }

    //walk rightward from startIdx and collect every resident whose age is within range
    for(int i = startIdx; i < size && arr[i].Age <= maxAge; i++){
        insert(matchedHead, arr[i]);
        matchCount++;
    }
    delete[] arr;
}

//binary search by mode of transport (sorted)
//converts list to array, sorts alphabetically, then uses binary search
void binarySearchByTransport_Sorted(Node* head, const string& targetMode, Node*& matchedHead, int& matchCount){
    matchedHead = NULL;
    matchCount = 0;

    int size = 0;
    Resident* arr = listToArray(head, size);
    if(!arr) return;

    arraySortByTransport(arr, size); //sort alphabetically by transport mode

    string targetLower = toLower(targetMode);

    //locate any one resident using the target transport mode
    int lo = 0, hi = size - 1, foundIdx = -1;
    while(lo <= hi){
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

    //expand left and right to find all residents using this mode
    int left = foundIdx;
    while(left > 0 && toLower(arr[left-1].ModeOfTransport) == targetLower){
        left--;
    }
    int right = foundIdx;
    while(right < size-1 && toLower(arr[right+1].ModeOfTransport) == targetLower){
        right++;
    }

    for(int i = left; i <= right; i++){
        insert(matchedHead, arr[i]);
        matchCount++;
    }
    delete[] arr;
}

//binary search by daily distance (sorted)
//converts list to array, sorts by distance, then uses binary search
void binarySearchByDistance_Sorted(Node* head, int target, Node*& matchedHead, int& matchCount){
    matchedHead = NULL;
    matchCount = 0;

    int size = 0;
    Resident* arr = listToArray(head, size);
    if(!arr) return;

    arraySortInt(arr, size, false); //sort by daily distance ascending

    int lo = 0, hi = size - 1, foundIdx = -1;
    while(lo <= hi){
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

    //expand left and right to find all residents with this distance
    int left = foundIdx;
    while(left > 0 && arr[left-1].DailyDistance == target){
        left--;
    }
    int right = foundIdx;
    while(right < size-1 && arr[right+1].DailyDistance == target){
        right++;
    }

    for(int i = left; i <= right; i++){
        insert(matchedHead, arr[i]);
        matchCount++;
    }
    delete[] arr;
}

//binary search (unsorted)
//the only difference between sorted and unsorted binary search is by removing the sorting step
//binary search by age group (unsorted)
void binarySearchByAgeGroup_Unsorted(Node* head, int minAge, int maxAge, Node*& matchedHead, int& matchCount){
    matchedHead = NULL;
    matchCount = 0;

    int size = 0;
    Resident* arr = listToArray(head, size);
    if(!arr) return;

    int lo = 0, hi = size - 1;
    int startIdx = size;
    while(lo <= hi){
        int mid = lo + (hi - lo) / 2;
        if(arr[mid].Age >= minAge){
            startIdx = mid;
            hi = mid - 1;
        }else{
            lo = mid + 1;
        }
    }

    for(int i = startIdx; i < size && arr[i].Age <= maxAge; i++){
        insert(matchedHead, arr[i]);
        matchCount++;
    }
    delete[] arr;
}

//binary search by mode of transport (unsorted)
void binarySearchByTransport_Unsorted(Node* head, const string& targetMode, Node*& matchedHead, int& matchCount){
    matchedHead = NULL;
    matchCount = 0;

    int size = 0;
    Resident* arr = listToArray(head, size);
    if(!arr) return;

    string targetLower = toLower(targetMode);

    int lo = 0, hi = size - 1, foundIdx = -1;
    while(lo <= hi){
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
        right++;
    }

    for(int i = left; i <= right; i++){
        insert(matchedHead, arr[i]);
        matchCount++;
    }
    delete[] arr;
}

//binary search by daily distance (unsorted)
void binarySearchByDistance_Unsorted(Node* head, int target, Node*& matchedHead, int& matchCount){
    matchedHead = NULL;
    matchCount = 0;

    int size = 0;
    Resident* arr = listToArray(head, size);
    if(!arr) return;

    int lo = 0, hi = size - 1, foundIdx = -1;
    while(lo <= hi){
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

    for(int i = left; i <= right; i++){
        insert(matchedHead, arr[i]);
        matchCount++;
    }
    delete[] arr;
}

//FILTER HELPERS

//checks whether a single resident passes all active filters
bool passesFilters(const Resident& res, int ageGroup, const string& targetLower, int distThreshold, char distOp) {
    bool filterAge = isValidAgeGroup(ageGroup);
    bool filterTransport = !targetLower.empty();
    bool filterDistance = isValidDistOp(distOp);

    bool passAge = true;
    bool passTransport = true;
    bool passDistance = true;

    if(filterAge) {
        int minAge, maxAge;
        getAgeRange(ageGroup, minAge, maxAge);
        passAge = (res.Age >= minAge && res.Age <= maxAge);
    }
    if(filterTransport) {
        passTransport = (toLower(res.ModeOfTransport) == targetLower);
    }
    if(filterDistance) {
        passDistance = passesDistanceFilter(res.DailyDistance, distThreshold, distOp);
    }

    return passAge && passTransport && passDistance;
}

//compute g-cost: number of criteria a resident fails
float computeAStarG(const Resident& r,
                    bool filterAge,   int minAge,   int maxAge,
                    bool filterTrans, const string& targetLower,
                    bool filterDist,  int distThreshold, char distOp)
{
    float g = 0.0f;
    if(filterAge && !(r.Age >= minAge && r.Age <= maxAge))
        g += 1.0f;
    if(filterTrans && toLower(r.ModeOfTransport) != targetLower)
        g += 1.0f;
    if(filterDist) {
        if(!passesDistanceFilter(r.DailyDistance, distThreshold, distOp))
            g += 1.0f;
    }
    return g;
}

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
    matchedHead = NULL;
    matchCount  = 0;

    int minAge, maxAge;
    getAgeRange(ageGroup, minAge, maxAge);

    bool filterAge   = isValidAgeGroup(ageGroup);
    bool filterTrans = !targetMode.empty();
    bool filterDist  = isValidDistOp(distOp);
    string targetLower = toLower(targetMode);

    int n = countNodes(head);
    if(n == 0) return;

    MinHeap heap(n);

    Node* cur = head;
    while(cur != NULL) {
        float g = computeAStarG(cur->data,
                                filterAge,   minAge,   maxAge,
                                filterTrans, targetLower,
                                filterDist,  distThreshold, distOp);
        float h = computeAStarH(g);
        AStarNode an;
        an.g = g; an.h = h; an.f = g + h; an.data = cur->data;
        heap.push(an);
        cur = cur->next;
    }

    //pop in best-first order; collect only perfect matches (g == 0)
    while(!heap.empty()) {
        AStarNode best = heap.pop();
        if(best.g == 0.0f) {
            insert(matchedHead, best.data);
            matchCount++;
        } else {
            break; //all remaining nodes also have g > 0
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
    matchedHead = NULL;
    matchCount  = 0;

    int minAge, maxAge;
    getAgeRange(ageGroup, minAge, maxAge);

    bool filterAge   = isValidAgeGroup(ageGroup);
    bool filterTrans = !targetMode.empty();
    bool filterDist  = isValidDistOp(distOp);
    string targetLower = toLower(targetMode);

    Node* sortedList = copyList(head);
    if(sortedList == NULL) return;

    //sort the copy by age ascending using bubble sort
    bubbleSort(sortedList, sort_age, sort_asc);

    int n = countNodes(sortedList);
    MinHeap heap(n);

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
        heap.push(an);
        cur = cur->next;
    }

    //pop in best-first order; collect only perfect matches (g == 0)
    while(!heap.empty()) {
        AStarNode best = heap.pop();
        if(best.g == 0.0f) {
            insert(matchedHead, best.data);
            matchCount++;
        } else {
            break;
        }
    }

    freeList(sortedList);
}

//TREE / BFS / DFS

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

TreeNode* findChild(TreeNode* parent, const string& label) {
    for(int i = 0; i < parent->childCount; i++) {
        if(parent->children[i]->label == label) {
            return parent->children[i];
        }
    }
    return NULL;
}

TreeNode* addChild(TreeNode* parent, const string& label, int level) {
    TreeNode* existing = findChild(parent, label);
    if(existing != NULL) return existing;
    if(parent->childCount >= max_tree_children) return NULL;

    TreeNode* child = createTreeNode(label, level);
    parent->children[parent->childCount] = child;
    parent->childCount++;
    return child;
}

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

void initTreeQueue(TreeQueue& q) {
    q.front = NULL;
    q.rear = NULL;
}

bool isTreeQueueEmpty(const TreeQueue& q) {
    return q.front == NULL;
}

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

TreeNode* dequeueTree(TreeQueue& q) {
    if(q.front == NULL) return NULL;
    TreeQueueNode* temp = q.front;
    TreeNode* result = temp->treePtr;
    q.front = q.front->next;
    if(q.front == NULL) q.rear = NULL;
    delete temp;
    return result;
}

string indent(int level) {
    string s = "";
    for(int i = 0; i < level; i++) {
        s += "  ";
    }
    return s;
}

//BFS SEARCH (UNSORTED)
//BFS traversal over the transport tree built from the raw unsorted list
void bfsSearchUnsorted(Node* head, const string& rootLabel, int ageGroup, const string& targetMode, 
    int distThreshold, char distOp, Node*& matchedHead, int& matchCount) {
    matchedHead = NULL;
    matchCount = 0;

    TreeNode* root = buildTransportTree(head, rootLabel);
    string targetLower = toLower(targetMode);

    TreeQueue q;
    initTreeQueue(q);
    enqueueTree(q, root);

    while(!isTreeQueueEmpty(q)) {
        TreeNode* current = dequeueTree(q);

        Node* resCur = current->residentsHead;
        while(resCur != NULL) {
            if(passesFilters(resCur->data, ageGroup, targetLower, distThreshold, distOp)) {
                insert(matchedHead, resCur->data);
                matchCount++;
            }
            resCur = resCur->next;
        }

        for(int i = 0; i < current->childCount; i++) {
            enqueueTree(q, current->children[i]);
        }
    }
    freeTree(root);
}

//BFS SEARCH (SORTED)
//copies the list, sorts the copy by age ascending, then builds the tree and does BFS
void bfsSearchSorted(Node* head, const string& rootLabel, int ageGroup, const string& targetMode, 
    int distThreshold, char distOp, Node*& matchedHead, int& matchCount) {
    matchedHead = NULL;
    matchCount = 0;

    //copy and sort the list by age ascending
    Node* sortedList = copyList(head);
    if(sortedList == NULL) return;
    bubbleSort(sortedList, sort_age, sort_asc);

    TreeNode* root = buildTransportTree(sortedList, rootLabel);
    string targetLower = toLower(targetMode);

    TreeQueue q;
    initTreeQueue(q);
    enqueueTree(q, root);

    while(!isTreeQueueEmpty(q)) {
        TreeNode* current = dequeueTree(q);

        Node* resCur = current->residentsHead;
        while(resCur != NULL) {
            if(passesFilters(resCur->data, ageGroup, targetLower, distThreshold, distOp)) {
                insert(matchedHead, resCur->data);
                matchCount++;
            }
            resCur = resCur->next;
        }

        for(int i = 0; i < current->childCount; i++) {
            enqueueTree(q, current->children[i]);
        }
    }
    freeTree(root);

    //free the temporary sorted copy
    freeList(sortedList);
}

//DFS recursive helper
void dfsTreeHelper(TreeNode* current, int ageGroup, const string& targetLower, 
    int distThreshold, char distOp, Node*& matchedHead, int& matchCount) {
    Node* resCur = current->residentsHead;
    while(resCur != NULL) {
        if(passesFilters(resCur->data, ageGroup, targetLower, distThreshold, distOp)) {
            insert(matchedHead, resCur->data);
            matchCount++;
        }
        resCur = resCur->next;
    }

    for(int i = 0; i < current->childCount; i++) {
        dfsTreeHelper(current->children[i], ageGroup, targetLower, distThreshold, distOp, matchedHead, matchCount);
    }
}

//DFS SEARCH (UNSORTED)
//DFS traversal over the transport tree built from the raw unsorted list
void dfsSearchUnsorted(Node* head, const string& rootLabel, int ageGroup, const string& targetMode, 
    int distThreshold, char distOp, Node*& matchedHead, int& matchCount) {
    matchedHead = NULL;
    matchCount = 0;

    TreeNode* root = buildTransportTree(head, rootLabel);
    string targetLower = toLower(targetMode);

    dfsTreeHelper(root, ageGroup, targetLower, distThreshold, distOp, matchedHead, matchCount);
    freeTree(root);
}

//DFS SEARCH (SORTED)
//copies the list, sorts the copy by age ascending, then builds the tree and does DFS
void dfsSearchSorted(Node* head, const string& rootLabel, int ageGroup, const string& targetMode, 
    int distThreshold, char distOp, Node*& matchedHead, int& matchCount) {
    matchedHead = NULL;
    matchCount = 0;

    Node* sortedList = copyList(head);
    if(sortedList == NULL) return;
    bubbleSort(sortedList, sort_age, sort_asc);

    TreeNode* root = buildTransportTree(sortedList, rootLabel);
    string targetLower = toLower(targetMode);

    dfsTreeHelper(root, ageGroup, targetLower, distThreshold, distOp, matchedHead, matchCount);
    freeTree(root);

    //free the temporary sorted copy
    freeList(sortedList);
}
