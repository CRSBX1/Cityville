#include "header.hpp"

resident::resident(){}

resident::resident(string id, int ageInput, string transport, float distance, float carbonEmission, float averageDay)
    : residentID(id),
      age(ageInput),
      transportMode(transport),
      dailyDistance(distance),
      carbonEmissionFactor(carbonEmission),
      averageDayPerMonth(averageDay) {
        ageGroup = "Not set yet";
        if(transport=="Car"){
            encodedTransport = 0;
        } else if (transport=="Bus"){
            encodedTransport = 1;
        } else if (transport=="Bicycle"){
            encodedTransport = 2;
        } else if (transport=="Walking"){
            encodedTransport = 3;
        } else if (transport=="School Bus"){
            encodedTransport = 4;
        } else{
            encodedTransport = 5;
        }
      }

string resident::getID(){
    return residentID;
}

int resident::getAge(){
    return age;
}

int resident::getTransportEncode(){
    return encodedTransport;
}

string resident::getTransport(){
    return transportMode;
}

float resident::getDistance(){
    return dailyDistance;
}

float resident::getCarbonEmission(){
    return carbonEmissionFactor;
}

float resident::getAverageDay(){
    return averageDayPerMonth;
}

string resident::getAgeGroup(){
    return ageGroup;
}

//Setter Methods
void setAge();
void resident::setTransport(string transport){
    transportMode = transport;
}
void resident::setDistance(float distance){
    dailyDistance = distance;
};
void setCarbonEmission();
void setAverageDay();

void resident::setAgeGroup(int &senior, int &late, int &early, int &university, int &child){
    if(getAge() <= 100 & getAge() > 60){
        ageGroup = "Senior Citizens/Retirees";
        senior++;
    }
    else if(getAge() <= 60 & getAge() > 45){
        ageGroup = "Working Adults (Late Career)";
        late++;
    }
    else if(getAge() <= 45 & getAge() > 25){
        ageGroup = "Working Adults (Early Career)";
        early++;
    }
    else if(getAge() <=25 & getAge() > 17){
        ageGroup = "University Students/Young Adults";
        university++;
    }
    else if(getAge() <=17 & getAge() > 5){
        ageGroup = "Children & Teenagers";
        child++;
    }
    else{
        ageGroup = "Invalid";
    }
}

void resident::setAgeGroup(string ageGroup){
    this->ageGroup = ageGroup;
}

void resident::setTransportEncode(int encode){
    encodedTransport = encode;
}