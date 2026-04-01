#ifndef HEADER_HPP
#define HEADER_HPP
#include <string>
using namespace std;

class resident{
    private:
        string residentID;
        int age;
        string transportMode;
        float dailyDistance;
        float carbonEmissionFactor;
        float averageDayPerMonth;
        string ageGroup;
    public:
        //Constructors
        resident();
        resident(string id, int ageInput, string transport, float distance, float carbonEmission, float averageDay);

        //Getter Methods
        string getID();
        int getAge();
        string getTransport();
        float getDistance();
        float getCarbonEmission();
        float getAverageDay();
        string getAgeGroup();

        //Setter Methods
        void setID(int id);
        void setAge(int ageInput);
        void setTransport();
        void setDistance();
        void setCarbonEmission();
        void setAverageDay();
        void setAgeGroup(int &senior, int &late, int &early, int &university, int &child);
        void setAgeGroup(string ageGroup);
};



#endif