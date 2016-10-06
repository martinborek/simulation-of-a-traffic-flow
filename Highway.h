/* 
 * File:   Highway.h
 * Author: mike
 *
 * Created on 28. listopadu 2014, 21:46
 */

#ifndef HIGHWAY_H
#define	HIGHWAY_H

#include <vector>
#include <string>
#include <iostream>
#include <fstream>
#include "ExitSpec.h"

class Highway {
public:
    Highway();
    virtual ~Highway();
    
    int build(const std::string& fileName);
    
    void unbuild();
    
    void activateGenerators();
    
    void printHighway();
    
    void resetStats();
    
    void printAndResetStats(std::ofstream& ofs);
    
    void printStatsHeader(std::ofstream& ofs);
    
private:
    class Segment* firstSegment;
    std::vector<CarGenerator*> generators; // Generators have references to ExitSpecs
};

#endif	/* HIGHWAY_H */

