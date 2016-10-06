/* 
 * File:   Car.h
 * Author: mike
 *
 * Created on 26. listopadu 2014, 16:47
 */

#ifndef CAR_H
#define	CAR_H

#include "simlib.h"
#include "Lane.h"

class Car : public Process {
public:
    enum CarType {
        CAR,
        TRUCK,
    };
    
    /**
     * 
     * @param type Car or truck?
     * @param speedFactor How much the driver obeys speed limits (1.0 => drives exactly at speed limit)
     * @param maxSpeed How fast the car is willing to go (maximum limit for speed)
     * @param startLane (Lane to start in)
     */
    Car(CarType type, double speedFactor, double maxSpeed, double safetyGap, class Segment* startSegment);
    
    /**
     * Sets Time as Lane enter time
     */
    void markEnterTime();
    
    /**
     * 
     * @return Lane enter time
     */
    double getEnterTime();
    double getEnterTimeOnPrev();
    
    /**
     * 
     * @return Personal car or truck?
     */
    CarType getType();
    
    /**
     * 
     * @return How much the driver obeys speed limits (1.0 => drives exactly at speed limit)
     */
    double getSpeedFactor();
    
    /**
     * 
     * @return How fast the car is willing to go (maximum limit for speed)
     */
    double getMaxSpeed();
    
    void Behavior();
    
    /**
     * 
     * @return the lane that the car is currently going in
     */
    class Lane* getLane();
    
    /**
     * Assign a new lane to the car
     * @param lane
     */
    void setLane(class Lane* lane);
    
    /**
     * 
     * @return Length of the car
     */
    int getCarLength();
    
    class Segment* getExitSegment();
    
    class Segment* getStartSegment();
    
    void setExitSegment(class Segment* seg);
    
    int getMetresTaken();
    int getMetresTakenOnPrev();
    
    void setMetresTaken(int metres);
    
    double getSafetyGap();
    
private:
    double laneEnterTime;
    double laneEnterTimeOnPrev; // on prev segment
    double speedFactor;
    double maxSpeed;
    double safetyGap;
    class Lane* currentLane;
    class Segment* startSegment;
    class Segment* exitSegment;
    CarType type;
    
    /**
     * Metres that the car took from the lane
     */
    int metresTaken;
    int metresTakenBefore; // on previous segment
};

#endif	/* CAR_H */

