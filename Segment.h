/* 
 * File:   Segment.h
 * Author: mike
 *
 * Created on 26. listopadu 2014, 12:28
 */

#ifndef SEGMENT_H
#define	SEGMENT_H

#include <string>
#include <iostream>
#include "simlib.h"
#include "Lane.h"
#include "Config.h"
#include "Car.h"
#include "ExitSpec.h"


typedef struct s_segStat {
    unsigned long numCars;
    TStat* speed;
} segStat;

class Segment : public Store{
    public: 
        /**
         * 
         * @param name Name of the segment
         * @param len Length of the segment in metres
         * @param numLanes Number of lanes in the segment
         * @param allowedSpeed Allowed speed for the segment in metres per second
         * @param nextSegment Next segment of the highway
         */
        Segment(int len, int numLanes, double allowedSpeed, Segment* nextSegment, ExitSpec* eSpec, char* statName);
        ~Segment();
        
        /**
         * Method for car to drive the whole segment (lane choosing, speed selection, 
         * enterLane, leaveLane, Wait())
         * Lanes: |   |   |   |
         *        | 2 | 1 | 0 |
         *        |   |   |   |
         * 
         * @param car
         * @return Lane chosen
         */
        class Lane* driveSegment(class Car* car);
        
        /**
         * 
         * @return Next segment of the highway
         */
        class Segment* getNextSegment();
        void setNextSegment(class Segment* seg);
        
        
        
    //protected:
        int numLanes;
        int allowedSpeed;
        int len;
        class Lane *lanes;
        class Segment* nextSegment;
        segStat* stat;
        ExitSpec* eSpec;
        
        /**
         * 
         * @param car
         * @return Index of preferred lane for the car
         */
        int pickLane(Car* car); // For cars to choose their lane
};

#endif	/* SEGMENT_H */

