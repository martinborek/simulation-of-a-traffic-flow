/* 
 * File:   Lane.h
 * Author: mike
 *
 * Created on 26. listopadu 2014, 12:45
 */

#ifndef LANE_H
#define	LANE_H

#include "simlib.h"
#include "Segment.h"
#include "Config.h"
#include "Car.h"

class Lane : public Store{
public:
    Lane();
    
    /**
     * Bind the lane to a segment of a highway
     * @param seg
     */
    void registerSegment(class Segment* seg);
    
    /**
     * 
     * @return Segment that this lane is part of
     */
    class Segment* getSegment();
    
    /**
     * 
     * @return Maximum speed for this lane (influenced by traffic conditions)
     */
    double getLaneSpeed(class Car* car);
    
    int getCarsIn();
    
    /**
     * 
     * @return Is lane jammed with cars?
     */
    bool isJammed();
    
    unsigned long getCarsInQueue();
    
    /**
     * Enter a lane at given speed
     * @param car
     * @param speed
     */
    void enterLane(Car* car, double speed);
    
    void enterLaneAndExitHighway(Car* car);
    
    /**
     * Leave a lane
     * @param car
     */
    void leaveLane(Car* car);
    
    double getLastSpeedIn();
    
private:
    class Segment* mySegment;
    int carsIn;
    double lastSpeedIn;
    double lastTimeIn;
    class Facility entryQueue; // Store lets cars with low requirements skip the other cars, this will solve it
    double lastAvgSpeed; // Average speed of the last car that left the segment, for this segment
};

#endif	/* LANE_H */

