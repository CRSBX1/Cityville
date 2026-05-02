#include "header.hpp"

//converts strings to lowercase before comparing
string toLower(const string& inputStr){
    string lowerStr = inputStr;
    //iterate through each character in the string
    for(int i=0; i<(int)lowerStr.size(); i++){
        lowerStr[i] = tolower(lowerStr[i]); //convert character to lowercase
    }
    return lowerStr; 
}

//time complexity record
//used by all sorting and searching experiments to measure execution time
//startTimer() captures the current time, stopTimer() returns elapsed milliseconds
chrono::high_resolution_clock::time_point startTimer() {
    return chrono::high_resolution_clock::now();
}
//calculates elapsed time in milliseconds since the given start time
//call this right after the operation finishes
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
    if(age >= 6  && age <= 17 ) { //children & teenagers
        return 0;
    } else if(age >= 18 && age <= 25) { //university & young adults
        return 1;
    } else if(age >= 26 && age <= 45) { //working adults early career
        return 2;
    } else if(age >= 46 && age <= 60) { //working adults late career
        return 3;
    } else if(age >= 61 && age <= 100) { //senior citizens / retirees
        return 4;
    } else {
        return -1; //age outside defined range
    }
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
        return labels[groupIndex]; //return a label if index is valid
    } else {
        return "Unknown Group"; //return for invalid index
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
