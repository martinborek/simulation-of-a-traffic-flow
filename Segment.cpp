/* 
 * File:   Segment.cpp
 * Author: mike
 * 
 * Created on 26. listopadu 2014, 12:28
 */

#include "Segment.h"
#include "assert.h"

Segment::Segment(int len, int numLanes, double allowedSpeed, Segment* nextSegment, ExitSpec* eSpec, char* statName){
    this->numLanes = numLanes;
    this->allowedSpeed = allowedSpeed;
    this->len = len;
    this->lanes = new Lane[numLanes];
    for (int i = 0; i < this->numLanes; i++){
        this->lanes[i].registerSegment(this);
    }
    this->eSpec = eSpec;
    this->nextSegment = nextSegment;
    if (statName){
        this->stat = new segStat;
        this->stat->numCars = 0;//new TStat(statName->c_str());
        this->stat->speed = new TStat(statName);
    }
    else{
        this->stat = NULL;
    }
}


Segment::~Segment(){
    delete[] this->lanes;
    if (this->stat){
        delete [] this->stat->speed->Name();
        delete this->stat->speed;
        delete this->stat;
    }
}


Segment* Segment::getNextSegment(){
    return this->nextSegment;
}


void Segment::setNextSegment(class Segment* seg){
    this->nextSegment = seg;
}


Lane* Segment::driveSegment(Car* car){
    // Is the car leaving on this segment?
    if (car->getExitSegment() == this){
        lanes[0].enterLaneAndExitHighway(car);
        car->setLane(&(lanes[0])); // Not necessary but just in case
        //std::cout << "Car " << car << " got off highway in segment " << this << "\n";
        return &(lanes[0]); // Dummy
    }
    
    // First, we need to choose which lane the car will go in:
    // If the car is about to leave on next exit, it has to go in lane 0
    int laneIdx = 0;
    
    // Is there an exit on the next segment?
    if (getNextSegment() != NULL && getNextSegment()->eSpec != NULL){
        // Is the car going to leave on next exit?:
        if (car->getExitSegment() == NULL && getNextSegment()->eSpec->willCarLeave()){
            car->setExitSegment(getNextSegment());
            laneIdx = 0;
        }
        else
            laneIdx = pickLane(car); // Car is not gonna leave
    }
    else if (car->getStartSegment() == this && this->eSpec != NULL){
        // If we just got on the highway, keep in lane 0 for this segment
        // If we just started at the beginning of highway, go in preferred lane
        laneIdx = 0;
    }
    else{
        laneIdx = pickLane(car);
    }
    
    
    Lane *desiredLane = &(this->lanes[laneIdx]);
    // We cannot go faster than the lane:
    double speed = this->allowedSpeed * car->getSpeedFactor(); // The speed we want to go
    if (speed > car->getMaxSpeed()) {speed = car->getMaxSpeed();} // Cannot go faster than the car allows us to
    if (speed > desiredLane->getLaneSpeed(car)) { speed = desiredLane->getLaneSpeed(car); }
    assert(speed > 0.0);
    
    desiredLane->enterLane(car, speed); // Go in the lane at desired speed
    
//    std::cout << "Car " << car << " entered segment " << this->name << " in lane " << laneIdx << " at Time = " << Time << "\n";
    
    // Now leave the previous segment
    if (car->getLane() != NULL){ car->getLane()->leaveLane(car);} // Free the previous lane
    car->setLane(desiredLane); // Update lane for the car
//    std::cout << "Car " << car << " is driving in segment " << this->name << " in lane " << laneIdx << " at Time = " << Time << "\n";
    car->Wait( ((double) desiredLane->getSegment()->len / speed)); // t = s/v
//    std::cout << "Car " << car << " has finished driving in segment " << this << " in lane " << laneIdx << " at Time = " << Time << "\n";
    
    return desiredLane;// TODO change to void
}


int Segment::pickLane(Car* car){
    // What speed does the car want to go?
    double wantedSpeed = this->allowedSpeed * car->getSpeedFactor();
    if (wantedSpeed > car->getMaxSpeed()){ wantedSpeed = car->getMaxSpeed();}
    
    // Get speed for each lane, then decide which lane to enter
    int max = 0;
    switch (car->getType()){
        case Car::CAR:
            max = this->numLanes;
            break;
        case Car::TRUCK:
            max = this->numLanes - 1; // Trucks cannot go in the fastest lane
        default:
            break;
    }
    
    if (max < 0){ max = 0; }
    
    int lane = 0;
    int lastNotJammed = -1;
    // Lanes: |   |   |   |
    //        | 2 | 1 | 0 |
    //        |   |   |   |
    //for (lane = 0; (lane < max) && this->lanes[lane].isJammed() && (this->lanes[lane].getLaneSpeed() < wantedSpeed); lane++);
    for (lane = 0; lane < max; lane++){
        if (!this->lanes[lane].isJammed())
            lastNotJammed = lane;
        if (!this->lanes[lane].isJammed() && (this->lanes[lane].getLaneSpeed(car) > wantedSpeed))
            break; // The lane is not jammed and the speed is fine for us
    }
    
    if (lane >= max){lane = max-1;}
    
    if (this->lanes[lane].isJammed()){
        if (lastNotJammed != -1)
            lane = lastNotJammed; // If there is a lane that isn't jammed, go for it
        else{
            // Pick the least jammed lane:
            int least = 0;
            for (int i = max-1; i >=0; i--)
                if (this->lanes[i].getCarsInQueue() <= this->lanes[least].getCarsInQueue())
                    least = i;
            lane = least;
//            // Random decision for the lane:
//            lane = (Random())*((double)max);
        }
    }
    
    return lane;
}