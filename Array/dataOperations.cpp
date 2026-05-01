#include "header.hpp"
using namespace std;

void dataOperations::setSeniorCount(int count){ seniorCount = count; }
void dataOperations::setLateCount(int count){ lateCount = count; }
void dataOperations::setEarlyCount(int count){ earlyCount = count; }
void dataOperations::setUniversityCount(int count){ universityCount = count; }
void dataOperations::setChildCount(int count){ childCount = count; }

resident* dataOperations::importDataset(string filename, resident dataset[], int size){
    ifstream fileData(filename);
    if (!fileData.is_open()){
        cerr << "Failed to open " << filename << endl;
        return nullptr;
    }
    getline(fileData, line);
    for (int i = 0; i < size && getline(fileData, line); i++)
    {
        stringstream ss(line);
        getline(ss, residentID, ',');
        getline(ss, age, ',');
        getline(ss, transport, ',');
        getline(ss, dailyDistance, ',');
        getline(ss, carbonEmissionFactor, ',');
        getline(ss, averageDay, ',');
        dataset[i] = resident(residentID, stoi(age), transport, stof(dailyDistance), stof(carbonEmissionFactor), stof(averageDay));
    }
    fileData.close();
    return dataset;
}

void dataOperations::categorizeAgeGroup(resident array[], int size){
    for(int i=0; i<size;i++){
        array[i].setAgeGroup(seniorCount, lateCount, earlyCount, universityCount, childCount);
    }
}

void dataOperations::calculateTransportModeCount(resident array[], string ageGroup,
    int& carCount, int& busCount, int& bicycleCount, int& walkCount,
    int& schoolBusCount, int& poolCount, int size){
    carCount = 0;
    busCount = 0;
    bicycleCount = 0;
    walkCount = 0;
    schoolBusCount = 0;
    poolCount = 0;
    for(int i=0; i<size;i++){
        if (array[i].getAgeGroup() == ageGroup)
        {
            if (array[i].getTransport() == "Car")carCount++;
            else if (array[i].getTransport() == "Bus") busCount++;
            else if (array[i].getTransport() == "Bicycle") bicycleCount++;
            else if (array[i].getTransport() == "Walking") walkCount++;
            else if (array[i].getTransport() == "School Bus") schoolBusCount++;
            else if (array[i].getTransport() == "Carpool") poolCount++;
        }
    }
}

void dataOperations::favoriteTransportMode(resident array[], int size, string ageGroup, int carCount, int busCount, int bicycleCount, int walkCount,
    int schoolBusCount, int poolCount){
    calculateTransportModeCount(array, ageGroup, carCount, busCount, bicycleCount, walkCount, schoolBusCount, poolCount, size);
    int maxCount = carCount;
    if (busCount > maxCount) maxCount = busCount;
    if (bicycleCount > maxCount) maxCount = bicycleCount;
    if (walkCount > maxCount) maxCount = walkCount;
    if (schoolBusCount > maxCount) maxCount = schoolBusCount;
    if (poolCount > maxCount) maxCount = poolCount;
    if (maxCount == 0) {
        cout << "No transportation mode data available for " << ageGroup << "." << endl;
        return;
    }
    else if (maxCount == carCount) cout << "The most preferred transportation mode for " << ageGroup << " is: Car, with " << maxCount << " users" << endl;
    else if (maxCount == busCount) cout << "The most preferred transportation mode for " << ageGroup << " is: Bus, with " << maxCount << " users" << endl;
    else if (maxCount == bicycleCount) cout << "The most preferred transportation mode for " << ageGroup << " is: Bicycle, with " << maxCount << " users" << endl;
    else if (maxCount == walkCount) cout << "The most preferred transportation mode for " << ageGroup << " is: Walking, with " << maxCount << " users" << endl;
    else if (maxCount == schoolBusCount) cout << "The most preferred transportation mode for " << ageGroup << " is: School Bus, with " << maxCount << " users" << endl;
    else if (maxCount == poolCount) cout << "The most preferred transportation mode for " << ageGroup << " is: Carpool, with " << maxCount << " users" << endl;
}

void dataOperations::totalCarbonEmission(resident* array[], int size1, int size2, int size3){
    string* datasetName = new string[]{"Dataset 1 - City A", "Dataset 2 - City B", "Dataset 3 - City C"}; 
    int* size = new int[]{size1, size2, size3};
    float seniorEmission, lateEmission, earlyEmission, universityEmission, childEmission, totalEmission;
    for(int i=0; i<3;i++){
        seniorEmission = 0, lateEmission = 0, earlyEmission = 0, universityEmission = 0, childEmission = 0, totalEmission = 0;
        for(int j=0; j<size[i]; j++){
            totalEmission += array[i][j].getCarbonEmission();
            if (array[i][j].getAgeGroup() == "Senior Citizens/Retirees") seniorEmission += array[i][j].getCarbonEmission();
            else if (array[i][j].getAgeGroup() == "Working Adults (Late Career)") lateEmission += array[i][j].getCarbonEmission();
            else if (array[i][j].getAgeGroup() == "Working Adults (Early Career)") earlyEmission += array[i][j].getCarbonEmission();
            else if (array[i][j].getAgeGroup() == "University Students/Young Adults") universityEmission += array[i][j].getCarbonEmission();
            else if (array[i][j].getAgeGroup() == "Children & Teenagers") childEmission += array[i][j].getCarbonEmission();
        }
        displayTotalEmission(datasetName[i], totalEmission, seniorEmission, lateEmission, earlyEmission, universityEmission, childEmission);
        averageCarbonEmissionPerResident(array[i], seniorEmission, lateEmission, earlyEmission, universityEmission, childEmission, totalEmission, size[i]);
    }
}

void dataOperations::carbonEmissionPerTransportMode(resident array[], string datasetName, int size){
    float carEmission = 0, busEmission = 0, bicycleEmission = 0, walkEmission = 0, schoolBusEmission = 0, poolEmission = 0;
    int carCount = 0, busCount = 0, bicycleCount = 0, walkCount = 0, schoolBusCount = 0, carpoolCount = 0;
    for(int i=0; i<size;i++){
        if (array[i].getTransport() == "Car"){
            carEmission += array[i].getCarbonEmission();
            carCount++;}
        else if (array[i].getTransport() == "Bus") {
            busEmission += array[i].getCarbonEmission();
            busCount++;}
        else if (array[i].getTransport() == "Bicycle") {
            bicycleEmission += array[i].getCarbonEmission();
            bicycleCount++;}
        else if (array[i].getTransport() == "Walking") {
            walkEmission += array[i].getCarbonEmission();
            walkCount++;}
        else if (array[i].getTransport() == "School Bus") {
            schoolBusEmission += array[i].getCarbonEmission();
            schoolBusCount++;}
        else if (array[i].getTransport() == "Carpool") {
            poolEmission += array[i].getCarbonEmission();
            carpoolCount++;}
    }
    cout << "\n" << datasetName << endl;
    cout << "--------------------------------------------------------------------------" << endl;
    cout << left << setw(15) << "Mode"
         << setw(10) << "Count"
         << setw(25) << "Total Emission (kg CO2)" << endl;
    cout << "--------------------------------------------------------------------------" << endl;
    cout << left << setw(15) << "Car"
         << setw(10) << carCount
         << setw(25) << carEmission << endl;
    cout << left << setw(15) << "Bus"
         << setw(10) << busCount
         << setw(25) << busEmission << endl;
    cout << left << setw(15) << "Bicycle"
         << setw(10) << bicycleCount
         << setw(25) << bicycleEmission << endl;
    cout << left << setw(15) << "Walking"
         << setw(10) << walkCount
         << setw(25) << walkEmission << endl;
    cout << left << setw(15) << "School Bus"
         << setw(10) << schoolBusCount
         << setw(25) << schoolBusEmission << endl;
    cout << left << setw(15) << "Carpool"
         << setw(10) << carpoolCount
         << setw(25) << poolEmission << endl;
    cout << "--------------------------------------------------------------------------" << endl;
}

void dataOperations::displayTotalEmission(string datasetName, float totalEmission, float seniorEmission,
    float lateEmission, float earlyEmission, float universityEmission, float childEmission){
    cout << "====================================================================================================" << endl;
    cout << "Carbon Emission: " << datasetName << endl << endl;
    cout << "Total Carbon Emission: " << totalEmission << endl;
    cout << "Senior Citizens/Retirees: " << seniorEmission << endl;
    cout << "Working Adults (Late Career): " << lateEmission << endl;
    cout << "Working Adults (Early Career): " << earlyEmission << endl;
    cout << "University Students/Young Adults: " << universityEmission << endl;
    cout << "Children & Teenagers: " << childEmission << endl;
    cout << endl;
}

void dataOperations::averageCarbonEmissionPerResident(resident array[], float seniorEmission, float lateEmission, float earlyEmission,
    float universityEmission, float childEmission, float totalEmission, int size){
    float seniorAverageEmission = seniorEmission / seniorCount;
    float lateAverageEmission = lateEmission / lateCount;
    float earlyAverageEmission = earlyEmission / earlyCount;
    float universityAverageEmission = universityEmission / universityCount;
    float childAverageEmission = childEmission / childCount;
    cout << "Average Carbon Emission per Resident: " << totalEmission/size << endl;
    cout << "Senior Citizens/Retirees: " << seniorAverageEmission << endl;
    cout << "Working Adults (Late Career): " << lateAverageEmission << endl;
    cout << "Working Adults (Early Career): " << earlyAverageEmission << endl;
    cout << "University Students/Young Adults: " << universityAverageEmission << endl;
    cout << "Children & Teenagers: " << childAverageEmission << endl;
}

void dataOperations::emissionComparison(resident array[], string ageGroup, int size, string datasetName){
    int carCount = 0, busCount = 0, bicycleCount = 0, walkCount = 0, schoolBusCount = 0, poolCount = 0;
    float totalEmission = 0, carEmission = 0, busEmission = 0, bicycleEmission = 0, walkEmission = 0, schoolEmission = 0, poolEmission = 0;
    float avgCarEmission = 0, avgBusEmission = 0, avgBicycleEmission = 0, avgWalkEmission = 0, avgSchoolEmission = 0, avgPoolEmission = 0;
    for(int i=0; i<size;i++){
        if(array[i].getAgeGroup()==ageGroup){
            if(array[i].getTransport() == "Car") {
                carEmission += array[i].getCarbonEmission();
                carCount++;}
            else if(array[i].getTransport() == "Bus") {
                busEmission += array[i].getCarbonEmission();
                busCount++;}
            else if(array[i].getTransport() == "Bicycle") {
                bicycleEmission += array[i].getCarbonEmission();
                bicycleCount++;}
            else if(array[i].getTransport() == "Walking") {
                walkEmission += array[i].getCarbonEmission();
                walkCount++;}
            else if(array[i].getTransport() == "School Bus") {
                schoolEmission += array[i].getCarbonEmission();
                schoolBusCount++;}
            else if(array[i].getTransport() == "Carpool") {
                poolEmission += array[i].getCarbonEmission();
                poolCount++;}
            totalEmission += array[i].getCarbonEmission();
        }
    }
    avgCarEmission = carEmission / carCount;
    avgBusEmission = busEmission / busCount;
    avgBicycleEmission = bicycleEmission / bicycleCount;
    avgWalkEmission = walkEmission / walkCount;
    avgSchoolEmission = schoolEmission / schoolBusCount;
    avgPoolEmission = poolEmission / poolCount;
    displayEmissionComparison(carCount, busCount, bicycleCount, walkCount, schoolBusCount, poolCount, totalEmission, carEmission,
        busEmission, bicycleEmission, walkEmission, schoolEmission, poolEmission, avgCarEmission, avgBusEmission,
        avgBicycleEmission, avgWalkEmission, avgSchoolEmission, avgPoolEmission, ageGroup, datasetName);
}

void dataOperations::displayEmissionComparison(int carCount, int busCount, int bicycleCount,
    int walkCount, int schoolBusCount, int poolCount, float totalEmission, float carEmission,
    float busEmission, float bicycleEmission, float walkEmission, float schoolEmission, float poolEmission,
    float avgCarEmission, float avgBusEmission, float avgBicycleEmission, float avgWalkEmission, float avgSchoolEmission,
    float avgPoolEmission, string ageGroup, string datasetName){
    cout << endl;
    cout << datasetName << endl;
    cout << "Emission Comparison: Transport Mode per Age Group (" << ageGroup << ")" << endl;
    cout << "---------------------------------------------------------------------------------------------" << endl;
    cout << left << setw(15) << "Mode"
         << setw(10) << "Count"
         << setw(25) << "Total Emission (kg CO2)" 
         << setw(30) << "Average Emission per Resident (kg CO2)" << endl;
    cout << "---------------------------------------------------------------------------------------------" << endl;
    cout << left << setw(15) << "Car"
         << setw(10) << carCount
         << setw(25) << carEmission
         << setw(30) << avgCarEmission << endl;
    cout << left << setw(15) << "Bus"
         << setw(10) << busCount
         << setw(25) << busEmission
         << setw(30) << avgBusEmission << endl;
    cout << left << setw(15) << "Bicycle"
         << setw(10) << bicycleCount
         << setw(25) << bicycleEmission
         << setw(30) << avgBicycleEmission << endl;
    cout << left << setw(15) << "Walking"
         << setw(10) << walkCount
         << setw(25) << walkEmission
         << setw(30) << avgWalkEmission << endl;
    cout << left << setw(15) << "School Bus"
         << setw(10) << schoolBusCount
         << setw(25) << schoolEmission
         << setw(30) << avgSchoolEmission << endl;
    cout << left << setw(15) << "Carpool"
         << setw(10) << poolCount
         << setw(25) << poolEmission
         << setw(30) << avgPoolEmission << endl;
    cout << "---------------------------------------------------------------------------------------------" << endl;
    cout << "Total Emission for age group: " << totalEmission << endl;
}

void dataOperations::displayAgeGroupData(){
    cout << "Age Group Details" << endl;
    cout << "Number of senior citizens/retirees: " << seniorCount << endl;
    cout << "Number of working adults in their late careers: " << lateCount << endl;
    cout << "Number of working adults in their early careers: " << earlyCount << endl;
    cout << "Number of university students/young adults: " << universityCount << endl;
    cout << "Number of children and teenagers: " << childCount << endl;
}

void dataOperations::displayData(resident array[], int size){
    cout << size << " residents found in the dataset." << endl;
    cout << " | " <<"Resident ID" << " | " << "Age" << " | " << "Transport Mode"
        << " | " << "Daily Distance" << " | " << "Carbon Emission Factor" << " | "
        << "Average Day per Month" << " | " << "Age Group" << " | " << endl;
    for(int i=0; i<size;i++){
        cout << " | " <<array[i].getID() << " | " << array[i].getAge() << " | " << array[i].getTransport()
        << " | " << array[i].getDistance() << " | " << array[i].getCarbonEmission() << " | "
        << array[i].getAverageDay() << " | " << array[i].getAgeGroup() << " | " << endl;
    }
}