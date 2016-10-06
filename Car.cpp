/* 
 * File:   Car.cpp
 * Author: mike
 * 
 * Created on 26. listopadu 2014, 16:47
 */

#include "Car.h"

Car::Car(CarType type, double speedFactor, double maxSpeed, double safetyGap, Segment* startSegment) : Process() {
    this->type = type;
    this->laneEnterTime = 0;
    this->speedFactor = speedFactor;
    this->currentLane = NULL;
    this->maxSpeed = maxSpeed;
    this->metresTaken = 0;
    this->startSegment = startSegment;
    this->exitSegment = NULL;
    this->safetyGap = safetyGap;
}


void Car::Behavior(){
    this->startSegment->driveSegment(this);
    while (currentLane->getSegment()->getNextSegment() != NULL){
        this->currentLane->getSegment()->getNextSegment()->driveSegment(this);
        if (currentLane->getSegment() == getExitSegment()){ break; }
    }
    
    if (currentLane->getSegment()->getNextSegment() == NULL){
        // Went off las segment of the highway:
        this->currentLane->Leave(this->getMetresTaken() - this->getCarLength()); // Free safety gap
        this->currentLane->leaveLane(this); // Free the last lane
    }
}


void Car::markEnterTime(){
    this->laneEnterTimeOnPrev = this->laneEnterTime;
    this->laneEnterTime = Time;
}

double Car::getEnterTime(){
    return this->laneEnterTime;
}

double Car::getEnterTimeOnPrev(){
    return this->laneEnterTimeOnPrev;
}

double Car::getSpeedFactor(){
    return this->speedFactor;
}


double Car::getMaxSpeed(){
    return this->maxSpeed;
}

double Car::getSafetyGap(){
    return this->safetyGap;
}

void Car::setMetresTaken(int metres){
    this->metresTakenBefore = this->metresTaken;
    this->metresTaken = metres;
}


int Car::getMetresTaken(){
    return this->metresTaken;
}


int Car::getMetresTakenOnPrev(){
    return this->metresTakenBefore;
}

int Car::getCarLength(){
    switch (this->type){
        case CAR:
            return CAR_LENGTH; // 5 Metres
        case TRUCK:
            return TRUCK_LENGTH;
        default:
            return CAR_LENGTH;
    }
}

class Segment* Car::getExitSegment(){
    return this->exitSegment;
}

class Segment* Car::getStartSegment(){
    return this->startSegment;
}
    
void Car::setExitSegment(Segment* seg){
    this->exitSegment = seg;
}

void Car::setLane(Lane* lane){
    this->currentLane = lane;
}


Lane* Car::getLane(){
    return this->currentLane;
}

Car::CarType Car::getType(){
    return Car::CarType::CAR;
}
