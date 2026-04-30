#include <iostream>
#include "header.hpp"
#include <fstream>
#include <sstream>
#include <iomanip>
#include <chrono>
#include <string>

//Memory tracker

double calculateResidentTotalEmission(resident r)
{
    return r.getDistance() * r.getCarbonEmission() * r.getAverageDay();
}
// ============================================================
//  Sort Key Options – determines which field to sort by
// ============================================================

using namespace std;

class mainMenu{
    private:
        resident* dataset1 = new resident[200];
        resident* dataset2 = new resident[178];
        resident* dataset3 = new resident[122];
        string menuChoice;
        string separator = string(100, '=') + "\n";
        dataOperations dataOps;
        sortAlgorithm sortAlgo;
        int datasetSize1, datasetSize2, datasetSize3;
    public:
        mainMenu(){
            dataOps.importDataset("dataset1-cityA.csv", dataset1, 200);
            dataOps.importDataset("dataset2-cityB.csv", dataset2, 178);
            dataOps.importDataset("dataset3-cityC.csv", dataset3, 122);
            datasetSize1 = 200;
            datasetSize2 = 178;
            datasetSize3 = 122;
        }

        void displayMenu(){
            cout << separator;
            cout << "Welcome to Cityville Transportation Analysis!" << endl;
            cout << "Please select an option:" << endl;
            cout << "1. View Dataset" << endl;
            cout << "2. Age Group Categorization" << endl;
            cout << "3. Preferred Transportation Mode per Age Group" << endl;
            cout << "4. Total Carbon Emissions per Dataset, per Age Group, and per Average Citizen Emission & Age Group" << endl;
            cout << "5. Carbon Emission per Transportation Mode" << endl;
            cout << "6. Carbon Emission Comparison" << endl;
            cout << "7. Sort Algorithms" << endl;
            cout << "8. Search Algorithms" << endl;
            cout << "9. Exit" << endl;
            cout << separator;
        }

        void handleMenuChoice(){
            int userChoice = 0;
            cout << "Enter your choice: ";
            cin >> userChoice;
            if(userChoice == 1){
                cout << separator;
                cout << "Dataset 1 - City A:" << endl;
                dataOps.displayData(dataset1, datasetSize1);
                cout << separator;
                cout << "Dataset 2 - City B:" << endl;
                dataOps.displayData(dataset2, datasetSize2);
                cout << separator;
                cout << "Dataset 3 - City C:" << endl;
                dataOps.displayData(dataset3, datasetSize3);
            }
            else if(userChoice == 2){                
                dataOps.categorizeAgeGroup(dataset1, datasetSize1);
                dataOps.categorizeAgeGroup(dataset2, datasetSize2);
                dataOps.categorizeAgeGroup(dataset3, datasetSize3);
                cout << "Age group categorization completed for all datasets." << endl;
                dataOps.displayAgeGroupData();
            }
            else if(userChoice == 3){
                int carCount, busCount, bicycleCount, walkCount, schoolBusCount, poolCount;
                dataOps.calculateTransportModeCount(dataset1, "Senior Citizens/Retirees",
                    carCount, busCount, bicycleCount, walkCount, schoolBusCount, poolCount, datasetSize1);
                dataOps.favoriteTransportMode("Senior Citizens/Retirees", carCount, busCount,
                    bicycleCount, walkCount, schoolBusCount, poolCount);
                dataOps.calculateTransportModeCount(dataset1, "Working Adults (Late Career)",
                    carCount, busCount, bicycleCount, walkCount, schoolBusCount, poolCount, datasetSize1);
                dataOps.favoriteTransportMode("Working Adults (Late Career)", carCount,
                    busCount, bicycleCount, walkCount, schoolBusCount, poolCount);
                dataOps.calculateTransportModeCount(dataset1, "Working Adults (Early Career)",
                    carCount, busCount, bicycleCount, walkCount, schoolBusCount, poolCount, datasetSize1);
                dataOps.favoriteTransportMode("Working Adults (Early Career)", carCount,
                    busCount, bicycleCount, walkCount, schoolBusCount, poolCount);
                dataOps.calculateTransportModeCount(dataset1, "University Students/Young Adults",
                    carCount, busCount, bicycleCount, walkCount, schoolBusCount, poolCount, datasetSize1);
                dataOps.favoriteTransportMode("University Students/Young Adults", carCount,
                    busCount, bicycleCount, walkCount, schoolBusCount, poolCount);
                dataOps.calculateTransportModeCount(dataset1, "Children & Teenagers", carCount,
                    busCount, bicycleCount, walkCount, schoolBusCount, poolCount, datasetSize1);
                dataOps.favoriteTransportMode("Children & Teenagers", carCount, busCount,
                    bicycleCount, walkCount, schoolBusCount, poolCount);
            }
            else if(userChoice == 4){
                float seniorEmission = 0, lateEmission = 0, earlyEmission = 0, universityEmission = 0, childEmission = 0, totalEmission = 0;
                dataOps.totalCarbonEmission(dataset1, "Dataset 1 - City A", datasetSize1);
                dataOps.totalCarbonEmission(dataset2, "Dataset 2 - City B", datasetSize2);
                dataOps.totalCarbonEmission(dataset3, "Dataset 3 - City C", datasetSize3);
            }
            else if(userChoice == 5){
                float carEmission = 0, busEmission = 0, bicycleEmission = 0, walkEmission = 0, schoolBusEmission = 0, poolEmission = 0;
                dataOps.carbonEmissionPerTransportMode(dataset1, "Dataset 1 - City A", datasetSize1);
                dataOps.carbonEmissionPerTransportMode(dataset2, "Dataset 2 - City B", datasetSize2);
                dataOps.carbonEmissionPerTransportMode(dataset3, "Dataset 3 - City C", datasetSize3);
            }
            else if(userChoice == 6){
                dataOps.emissionComparison(dataset1, "Senior Citizens/Retirees", datasetSize1);
                dataOps.emissionComparison(dataset1, "Working Adults (Late Career)", datasetSize1);
                dataOps.emissionComparison(dataset1, "Working Adults (Early Career)", datasetSize1);
                dataOps.emissionComparison(dataset1, "University Students/Young Adults", datasetSize1);
                dataOps.emissionComparison(dataset1, "Children & Teenagers", datasetSize1);
            }
            else if(userChoice == 7){
                sortAlgo.runSortingSection(dataset1, datasetSize1, dataset2, datasetSize2, dataset3, datasetSize3);
            }
            else if(userChoice == 8){
                searchAlgorithm seAlgo(dataset1, dataset2, dataset3, SortKey::BY_AGE,
                    SortKey::BY_DAILY_DISTANCE, SortKey::BY_TRANSPORT_MODE, SortKey::BY_AGE_TRANSPORT,
                    SortKey::BY_AGE_DISTANCE, SortKey::BY_TRANSPORT_DISTANCE, SortKey::BY_AGE_TRANSPORT_DISTANCE,
                    sortAlgo);
                seAlgo.chooseAlgorithm();
            }
            else if(userChoice == 9){
                cout << "Exiting program. Goodbye!" << endl;
                exit(0);
            }
            else{
                cout << "Invalid choice" << endl;
            }
        }
};

int main()
{
    mainMenu menu;
    while(true){
        menu.displayMenu();
        menu.handleMenuChoice();
    }
}
