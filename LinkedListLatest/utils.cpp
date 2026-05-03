#include "header.hpp"

//converts strings to lowercase before comparing
string toLower(const string& inputStr){
    string lowerStr = inputStr;
    for(int i=0; i<(int)lowerStr.size(); i++){
        lowerStr[i] = tolower(lowerStr[i]);
    }
    return lowerStr; 
}

//time complexity record
chrono::high_resolution_clock::time_point startTimer() {
    return chrono::high_resolution_clock::now();
}

//calculates elapsed time in milliseconds since the given start time
double stopTimer(chrono::high_resolution_clock::time_point start) {
    auto end = chrono::high_resolution_clock::now();
    return chrono::duration<double, milli>(end - start).count();
}

//calculates elapsed time in microseconds since the given start time
double stopTimerMicro(chrono::high_resolution_clock::time_point start) {
    auto end = chrono::high_resolution_clock::now();
    return chrono::duration<double, micro>(end - start).count();
}

//returns which age group index a given age belongs to
int getAgeGroupIndex(int age) {
    if(age >= 6  && age <= 17 ) return 0;      //children & teenagers
    else if(age >= 18 && age <= 25) return 1;   //university & young adults
    else if(age >= 26 && age <= 45) return 2;   //working adults early career
    else if(age >= 46 && age <= 60) return 3;   //working adults late career
    else if(age >= 61 && age <= 100) return 4;  //senior citizens / retirees
    else return -1;                              //age outside defined range
}

//returns the display label for a given age group index
string getAgeGroupLabel(int groupIndex) {
    const string labels[total_age_groups] = {
        "Children & Teenagers        (6-17)",
        "University / Young Adults   (18-25)",
        "Working Adults Early Career (26-45)",
        "Working Adults Late Career  (46-60)",
        "Senior Citizens / Retirees  (61-100)"
    };
    if(groupIndex >= 0 && groupIndex < total_age_groups) {
        return labels[groupIndex];
    } else {
        return "Unknown Group";
    }
}

//returns the age group label string for a given age (used as level 2 node label)
string getAgeRangeLabel(int age) {
    if(age >= 6  && age <= 17)  return "6-17";
    if(age >= 18 && age <= 25)  return "18-25";
    if(age >= 26 && age <= 35)  return "26-35";
    if(age >= 36 && age <= 49)  return "36-49";
    if(age >= 50 && age <= 58)  return "50-58";
    if(age >= 59 && age <= 100) return "59-100";
    return "Unknown";
}

//returns a distance range label for a given daily distance
string getDistanceRangeLabel(int dist) {
    if(dist <= 5)  return "1-5 km";
    if(dist <= 10) return "6-10 km";
    if(dist <= 15) return "11-15 km";
    if(dist <= 20) return "16-20 km";
    if(dist <= 25) return "21-25 km";
    return "26+ km";
}

//converts an age group number (1-5) to its min/max age range
//sets minAge=-1, maxAge=-1 for invalid groups
void getAgeRange(int ageGroup, int& minAge, int& maxAge) {
    switch(ageGroup) {
        case 1: minAge = 6;   maxAge = 17;  break; //children & teenagers
        case 2: minAge = 18;  maxAge = 25;  break; //university / young adults
        case 3: minAge = 26;  maxAge = 45;  break; //working adults early career
        case 4: minAge = 46;  maxAge = 60;  break; //working adults late career
        case 5: minAge = 61;  maxAge = 100; break; //senior citizens / retirees
        default: minAge = -1; maxAge = -1;  break; //invalid group
    }
}

//returns true if ageGroup is a valid selection (1-5)
bool isValidAgeGroup(int ageGroup) {
    return (ageGroup >= 1 && ageGroup <= 5);
}

//returns true if distOp is a valid distance operator
bool isValidDistOp(char distOp) {
    return (distOp == '>' || distOp == '<' || distOp == '=');
}

//checks if a distance value passes the given operator and threshold
bool passesDistanceFilter(int distance, int threshold, char op) {
    if(op == '>' ) return distance > threshold;
    if(op == '<' ) return distance < threshold;
    if(op == '=' ) return distance == threshold;
    return true; //invalid operator means no filtering
}
