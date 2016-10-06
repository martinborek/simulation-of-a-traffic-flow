/* 
 * File:   ExitSpec.cpp
 * Author: mike
 * 
 * Created on 28. listopadu 2014, 13:51
 */

#include "ExitSpec.h"
#include "Car.h"

ExitSpec::ExitSpec(EntryType inType, double expMean, double trucksPercentage, double outPercentage) {
    this->in_type = inType;
    this->mean = expMean;
    this->in_trucksPercentage = trucksPercentage;
    this->out_percentage = outPercentage;
}

ExitSpec::ExitSpec(const ExitSpec& orig) {
}

ExitSpec::~ExitSpec() {
}

bool ExitSpec::willCarLeave(){
    return ( Random() < this->out_percentage);
}

double ExitSpec::getTruckPercentage(){
    return this->in_trucksPercentage;
}

double ExitSpec::getExpMean(){
    return this->mean;
}

int ExitSpec::getType(){
    return this->in_type;
}

CarGenerator* ExitSpec::createGenerator(class Segment* seg){
    return new CarGenerator(this, seg);
}





CarGenerator::CarGenerator(class ExitSpec* eSpec, class Segment* seg){
    this->eSpec = eSpec;
    this->seg = seg;
}


// weekdayFlow: Relative number of traffic (weight) for each day:
//                       Mon  Tue  Wed  Thu  Fri  Sat  Sun
double weekdayFlow[7] = {0.86471, 0.82528, 0.93528, 0.99355, 1.00000, 0.60013, 0.56814};

// GenSpec = nominal values for every type of generator. What percentage of specified
//           traffic to generate at which hour of day (e.g. 00h == 00:00-00:59   13h == 13:00-13:59)
double genSpecHours[3][24] = {
    { // COMPOSITE_PEAK
      // 00h  01h  02h  03h  04h  05h  06h  07h  08h  09h  10h  11h
         0.13701, 0.11192, 0.10564, 0.12017, 0.17044, 0.34618,
         0.67894, 0.89553, 0.86943, 0.76825, 0.70600, 0.71233,
         //0.1, 0.1, 0.1, 0.2, 0.3, 0.5, 0.8, 1.0, 1.0, 0.7, 0.6, 0.5,
     
      // 12h  13h  14h  15h  16h  17h  18h  19h  20h  21h  22h  23h
         0.73098, 0.79076, 0.87828, 0.98388, 1.00000, 0.93000,
         0.78999, 0.58899, 0.40605, 0.30514, 0.24002, 0.17916
        //0.5, 0.5, 0.5, 0.4, 0.4, 0.4, 0.4, 0.4, 0.4, 0.4, 0.2, 0.1
    },
        
    { // EVENING_PEAK
      // 00h  01h  02h  03h  04h  05h  06h  07h  08h  09h  10h  11h
         0.1, 0.1, 0.1, 0.2, 0.3, 0.5, 0.5, 0.5, 0.5, 0.4, 0.4, 0.4,
     
      // 12h  13h  14h  15h  16h  17h  18h  19h  20h  21h  22h  23h
         0.5, 0.5, 0.6, 0.8, 1.0, 0.9, 0.8, 0.6, 0.4, 0.4, 0.2, 0.1
    },
        
    { // NO_PEAK
      // 00h  01h  02h  03h  04h  05h  06h  07h  08h  09h  10h  11h
         0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 
     
      // 12h  13h  14h  15h  16h  17h  18h  19h  20h  21h  22h  23h
         0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5
    },
};

void CarGenerator::Behavior(){
    double safetyGap = Normal(safetyGapMu, safetyGapSigma); // Generate safety gap for the driver
    
    // Cut inapropriate values:
    if (safetyGap < 0.2)
        safetyGap = 0.2;
    else if (safetyGap > 3.0)
        safetyGap = 3.0;
    
    // Generate truck or car?
    if (Random() > this->eSpec->getTruckPercentage()){
        // Generate car
        double speedFactor = Normal(1.12, 0.18); // Based on statistics
        if (speedFactor < 0.5 ) {speedFactor = 0.5;}
        if (speedFactor > 3.0 ) {speedFactor = 3.0;}
        
        Car *c = new Car(Car::CAR, speedFactor, SPEED_UNLIMITED, safetyGap, this->seg);
        c->Activate();
    }
    else{
        // Generate truck
        double speedFactor = Normal(0.99, 0.15); // Based on statistics
        if (speedFactor < 0.5 ) {speedFactor = 0.5;}
        if (speedFactor > 3.0 ) {speedFactor = 3.0;}
        
        Car *c = new Car(Car::TRUCK, speedFactor, 80/3.6, safetyGap, this->seg);
        c->Activate();
    }
    Activate(Time + Exponential(this->eSpec->getExpMean()
            / (weekdayFlow[Weekday] * genSpecHours[this->eSpec->getType()][Hour])));
}

ExitSpec* CarGenerator::getExitSpec(){
    return this->eSpec;
}
