/* 
 * File:   ExitSpec.h
 * Author: mike
 *
 * Created on 28. listopadu 2014, 13:51
 */

#ifndef EXITSPEC_H
#define	EXITSPEC_H

#include "simlib.h"

class ExitSpec {
public:
    enum EntryType{
        COMPOSITE_PEAK = 0,
        EVENING_PEAK = 1,
        NO_PEAK = 2 // !!! HAS TO BE LAST !!!
    };
    
    ExitSpec(EntryType inType, double expMean, double trucksPercentage, double outPercentage);
    ExitSpec(const ExitSpec& orig);
    virtual ~ExitSpec();
    
    class CarGenerator* createGenerator(class Segment* seg);
    
    bool willCarLeave();
    
    int getType();
    
    double getTruckPercentage();
    
    double getExpMean();
    
private:
    double out_percentage; // Percentage of cars in previous segment that leave at this exit
    double in_trucksPercentage; // Percentage of trucks generated
    EntryType in_type;
    double mean;
};

class CarGenerator : public Event {
public:
    CarGenerator(class ExitSpec* eSpec, class Segment* seg);
    
    void Behavior();
    
    class ExitSpec* getExitSpec();
    
private:
    class ExitSpec* eSpec;
    class Segment* seg;
};

#endif	/* EXITSPEC_H */

