/* 
 * File:   Lane.cpp
 * Author: mike
 * 
 * Created on 26. listopadu 2014, 12:45
 */

#include <assert.h>
#include "simlib.h"
#include "Lane.h"


Lane::Lane() : Store() {
    this->mySegment = NULL;
    this->carsIn = 0;
    this->lastSpeedIn = 0;
    this->lastTimeIn = 0;
    this->lastAvgSpeed = 0;
}

void Lane::registerSegment(Segment* seg){
    assert (this->mySegment == NULL);
    this->mySegment = seg;
    SetCapacity(this->mySegment->len); // Free metres in the lane
    this->lastAvgSpeed = seg->allowedSpeed;
}


Segment* Lane::getSegment(){
    return mySegment;
}

double Lane::getLastSpeedIn(){
    return this->lastSpeedIn;
}

double Lane::getLaneSpeed(Car* car){
    if (this->carsIn == 0){
        // If the segment is empty, we can go as fast as we want
        return SPEED_UNLIMITED;
    }
    
    // If the segment is not empty, we need to know how fast we have to go in order to be 2secs 
    // behind the last car in the lane at the end of the lane.
    double tEnter = this->lastTimeIn; // Time of entry of the last car
    int l = this->mySegment->len; // Length of segment
    double v = this->lastSpeedIn; // Speed of the last car
    
    //double speed = ((l - 2*v)*v)/(l-v*(Time - tEnter));
    double speed = (l)/(((l-v*(Time - tEnter))/v) + car->getSafetyGap());
    
    if (speed <= 0){
        // There are cars waiting to get to the next segment -> jamming
        // No matter what speed we choose, we'll probably have to wait anyway,
	// so speed doesn't matter.
        speed = SPEED_UNLIMITED;
    }
    assert(speed>0);
    if (speed>SPEED_UNLIMITED)
        speed = SPEED_UNLIMITED; // for printout, we don't want big numbers
    return speed;
}


int Lane::getCarsIn(){
    return this->carsIn;
}


bool Lane::isJammed(){
    if (this->entryQueue.QueueLen() > 0){
        return true;
    }
    return false;
}


unsigned long Lane::getCarsInQueue(){
    return this->entryQueue.QueueLen();
}


void Lane::enterLaneAndExitHighway(Car* car){
//this->entryQueue.Seize(car);
//    std::cout << "Car " << car << " will seize lane facility at Time = " << Time << "\n";
    car->Seize(this->entryQueue);
//    std::cout << "Car " << car << " has seized lane facility at Time = " << Time << "\n";
    // we're waiting to enter the next segment, give up safety gap (simulate jamming)
    car->getLane()->Leave(car->getMetresTaken() - car->getCarLength());
    car->getLane()->carsIn--;
    
//    std::cout << "Car" << car << " wants to take " << car->getCarLength() + SAFETY_GAP * speed << " metres. Current available space is " << Free() << "\n";
    // Attempt to enter the next segment:
    Enter(car, car->getCarLength());
    
    // Leave previous lane:
    car->getLane()->Leave(car->getCarLength());
    
    // Include function of leaveLane:
    car->Release(this->entryQueue);
    
    Leave(car->getCarLength());
}


void Lane::enterLane(Car* car, double speed){
    //this->entryQueue.Seize(car);
//    std::cout << "Car " << car << " will seize lane facility at Time = " << Time << "\n";
    car->Seize(this->entryQueue);
//    std::cout << "Car " << car << " has seized lane facility at Time = " << Time << "\n";
    // we're waiting to enter the next segment, give up safety gap (simulate jamming)
    if (car->getLane() != NULL){ car->getLane()->Leave(car->getMetresTaken() - car->getCarLength());}
    
//    std::cout << "Car" << car << " wants to take " << car->getCarLength() + SAFETY_GAP * speed << " metres. Current available space is " << Free() << "\n";
    // Attempt to enter the next segment:
    Enter(car, car->getCarLength() + car->getSafetyGap() * speed);
    
    // Free space in the previous segment:
    //if (car->getLane() != NULL){ car->getLane()->Leave(car->getCarLength());} //NO, done in Segment.cpp
    
    car->markEnterTime();
    car->setMetresTaken(car->getCarLength() + car->getSafetyGap() * speed);
    this->carsIn++;
    this->lastSpeedIn = speed;
    this->lastTimeIn = Time;
    //this->entryQueue.Release(car);
    car->Release(this->entryQueue);
//    std::cout << "Car " << car << " has released lane facility at Time = " << Time << "\n";
}

void Lane::leaveLane(Car* car){
    // Log statistics:
    this->lastAvgSpeed = ( ((double) this->mySegment->len) / (Time - car->getEnterTimeOnPrev())); // Avg speed for this segment, including waiting time
    Segment* seg = this->mySegment;
    if (seg->stat){
        seg->stat->speed->operator () (this->lastAvgSpeed*3.6); // log in kph
        seg->stat->numCars++; // Log 1 car crossing STAT segment
    }
    
    // Leave:
    this->carsIn--;
    Leave(car->getCarLength());
//    std::cout << "Car " << car << " has left lane. Now the free capacity of the lane is " << Free() << "\n";
}
