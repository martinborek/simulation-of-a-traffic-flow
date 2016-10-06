/* 
 * File:   Highway.cpp
 * Author: mike
 * 
 * Created on 28. listopadu 2014, 21:46
 */

#include "Highway.h"
#include "Segment.h"
#include <exception>
#include <regex>
#include <iomanip>
#include <string.h>

class fOpenException: public std::exception {
  virtual const char* what() const throw()
  {
    return "Cannot open file!";
  }
} fOpenEx;

Highway::Highway() {
    this->firstSegment = NULL;
}

Highway::~Highway() {
}


#define check(x) if (word != x){ break; }

int Highway::build(const std::string& fileName){
    std::ifstream ifs;
    ifs.open(fileName.c_str());
    
    if (!ifs.is_open()){
        throw fOpenEx;
    }
    
    std::string word = "";
    ifs >> word;
    
    Segment* lastSegCreated = NULL;
    
    char *charName;
    
    while (!ifs.eof()) {
//        std::cout << "read: " << word << "\n";
        
        if (word == "#"){
            // Comment
            std::getline(ifs, word); //discard line
        }
        else if (word == "SEGMENT"){
            bool entrypoint = false; // If this is the 1st segment of highway
            int len;
            int lanes;
            double speed;
            ExitSpec* eSpec = NULL;
            std::string* statName = NULL;
            entrypoint = false;
            ifs >> len;
            ifs >> word; //m
            check("m");
            
            ifs >> lanes;
            ifs >> word; //lanes
            check("lanes")
            
            ifs >> word; //@
            check("@")
            
            ifs >> speed;
            speed /= 3.6; // convert to kph
            ifs >> word; // kph
            check("kph");
            
            ifs >> word; // new kw
            
            if (word == "EXIT" || word == "ENTRYPOINT"){
                ExitSpec::EntryType type = ExitSpec::EntryType::NO_PEAK;
                double outPercentage = 0.0;
                double in_mean;
                double truck_percentage;
                
                if (word == "ENTRYPOINT")
                    entrypoint = true;
                
                // Get exit type:
                ifs >> word;
                if (word == "composite"){
                    type = ExitSpec::EntryType::COMPOSITE_PEAK;
                }
                else if (word == "evening"){
                    type = ExitSpec::EntryType::EVENING_PEAK;
                }
                
                if (!entrypoint){
                    ifs >> word; // OUT:
                    check("OUT:");

                    ifs >> outPercentage;
                }
                
                ifs >> word; // IN:
                check("IN:")
                
                ifs >> in_mean;
                
                ifs >> word; //s
                check("s");
                
                ifs >> truck_percentage;
                ifs >> word; // trucks
                check("trucks");
                
                ifs >> word; // for the next round
                
                eSpec = new ExitSpec(type, in_mean, truck_percentage, outPercentage);
            }
            if (word == "STAT"){
                std::getline(ifs, word); //rest of line
                int idx1 = word.find_first_of("\"");
                statName = new std::string(word.substr(idx1+1, word.length() - idx1 - 2));
                charName = new char [statName->length()+1];
                strcpy(charName, statName->c_str());
                charName[statName->length()] = 0;
                
                ifs >> word; // for the next round
            }
            
            // Create a new segment (partition into segments of SEGMENT_LEN):
            int segCount = 0;
            
            if (eSpec == NULL){ // No exit on this segment
                segCount = ((len-1) / SEGMENT_LEN) +1; // Number of segments to devide in
            }
            else{
                if (len < EXIT_SEGMENT_LEN){
                    std::cout << "ERROR: Segment length " << len << " is shorter than minimum length of exit segment!";
                    break;
                }
                if (len == EXIT_SEGMENT_LEN){
                    segCount = 1;
                }
                else{
                    segCount = ((len-EXIT_SEGMENT_LEN-1) / SEGMENT_LEN) +2;
                }
            }
            
            if (segCount == 1){ // Creating only one segment
                Segment* seg = new Segment(len, lanes, speed, NULL, entrypoint ? NULL : eSpec, charName); // Entrypoint has to have null espec
                
                // Place generator:
                if (eSpec){
                    this->generators.push_back(eSpec->createGenerator(seg));
                }
                
                // Update previously created segment with reference to the new segment:
                if (lastSegCreated != NULL)
                    lastSegCreated->setNextSegment(seg);
                else
                    this->firstSegment = seg;
                lastSegCreated = seg;

                // Erase pointers for next run:
                eSpec = NULL;
                if (statName != NULL){
                    delete statName;
                    statName = NULL;
                }
                if (len < MIN_SEGMENT_LEN)
                    std::cout << "WARNING: Segment length " << len << " is very short! \n";
            }
            else {
                // Place exit first if there should be an exit:
                if (eSpec){
                    Segment* seg = new Segment(EXIT_SEGMENT_LEN, lanes, speed, NULL, entrypoint ? NULL : eSpec, NULL);
                    
                    // Also add a generator:
                    this->generators.push_back(eSpec->createGenerator(seg));
                
                    // Update previously created segment with reference to the new segment:
                    if (lastSegCreated != NULL)
                        lastSegCreated->setNextSegment(seg);
                    else
                        this->firstSegment = seg;
                    lastSegCreated = seg;

                    // Erase pointers for next run:
                    eSpec = NULL;
                    if (len < MIN_SEGMENT_LEN)
                        std::cout << "WARNING: Segment length " << len << " is very short! \n";
                    segCount--;
                    len -= EXIT_SEGMENT_LEN;
                }
                
                while(segCount > 2){
                    Segment* seg = new Segment(SEGMENT_LEN, lanes, speed, NULL, NULL, NULL);
                
                    // Update previously created segment with reference to the new segment:
                    if (lastSegCreated != NULL)
                        lastSegCreated->setNextSegment(seg);
                    else
                        this->firstSegment = seg;
                    lastSegCreated = seg;

                    segCount--;
                    len -= SEGMENT_LEN;
                }
                // Devide len between the last two segments:
                Segment* seg = new Segment(len/2, lanes, speed, NULL, NULL, NULL);
                
                // Update previously created segment with reference to the new segment:
                if (lastSegCreated != NULL)
                    lastSegCreated->setNextSegment(seg);
                else
                    this->firstSegment = seg;
                lastSegCreated = seg;
                
                
                
                // Place stats on last segment:
                seg = new Segment(len/2, lanes, speed, NULL, NULL, charName);
                
                // Update previously created segment with reference to the new segment:
                if (lastSegCreated != NULL) 
                    lastSegCreated->setNextSegment(seg);
                else
                    this->firstSegment = seg;
                lastSegCreated = seg;

                // Erase pointers for next run:
                eSpec = NULL;
                if (statName != NULL){
                    delete statName;
                    statName = NULL;
                }
            }
            
            
            
            continue;
        }
        else{
            std::cout << "WARNING: Unexpected line: " << word;
            std::getline(ifs, word); //discard line
            std::cout << word << "\n";
        }
        
        ifs >> word;
    }
    
    if (!ifs.eof())
    {
        std::cout << "Error reading highway definition file! ( at char_position = " << ifs.tellg() << ", token = " << word << " )\n";
        return -1;
    }   
    
    ifs.close(); // Close the description file
    return 0;
}

void Highway::unbuild() {
    // Deallocate segments:
    Segment* prevSeg = NULL;
    Segment* seg = this->firstSegment;
    while (seg != NULL){
        prevSeg = seg;
        seg = seg->getNextSegment();
        delete prevSeg;
    }
    
    // Deallocate exitSpecs and generators:
    for (std::vector<CarGenerator*>::iterator it = this->generators.begin();
         it != this->generators.end(); ++it){
        delete (*it)->getExitSpec();
        delete (*it);
    }
}

void Highway::activateGenerators(){
    for (std::vector<CarGenerator*>::iterator it = this->generators.begin();
         it != this->generators.end(); ++it){
        (*it)->Activate();
    }
}

void Highway::printHighway(){
    Segment* seg = this->firstSegment;
    std::cout << "\n\nTime " << Time << " ( weekday " << Weekday << ", time " << Hour << " hours)\n";
    std::cout << "START\n";
    std::cout << "  LANES     LENGTH   CARS IN LANES (CARS / FREE METRES @LAST CAR'S SPEED)\n";
    while(seg != NULL){
        std::ostringstream lanes;
        std::ostringstream lanesInfo;
        // Print lanes:
        if (seg->eSpec)
            lanes << "=<";
        else
            lanes << "  ";
        
        lanes << "|";
        lanesInfo << std::setw(6) << seg->len << "m ";
        
        for (int i = 0; i < 4; i++){
            if (seg->numLanes <= i)
                lanes << "  ";
            else{
                lanes << i << "|";
                lanesInfo << std::right << "L" << i << ":" << std::setw(3) << seg->lanes[i].getCarsIn();
                lanesInfo << "/" << std::setw(4) << std::left << seg->lanes[i].Free();
                lanesInfo << " @" << std::setw(4) << std::left << std::setprecision(3) << seg->lanes[i].getLastSpeedIn() * 3.6;
                lanesInfo << (seg->lanes[i].isJammed() ? "! " : "  ");
            }
        }
        
        
        if (seg->stat){
            lanesInfo << seg->stat->speed->Name();
            lanesInfo << " @" << seg->stat->speed->LastValue() << "(last)" << " @" << seg->stat->speed->MeanValue() << "(mean)";
            lanesInfo << " " << seg->stat->numCars << "(sum) cars";
        }
        
        
        std::cout << lanes.str() << lanesInfo.str() << "\n";
        seg = seg->getNextSegment();
    }
    std::cout << "END\n";
}

void Highway::resetStats(){
    Segment* seg = this->firstSegment;
    while(seg){
        if (seg->stat){
            seg->stat->numCars = 0;
            seg->stat->speed->Clear();
        }
        seg = seg->getNextSegment();
    }
}


void Highway::printStatsHeader(std::ofstream& ofs){
    Segment* seg = this->firstSegment;
    ofs << "TIME,WEEKDAY,HOUR,";
    while(seg){
        if (seg->stat){
            //std::cout << "\n\nTRAFFIC DENSITY (" << seg->stat->numCars->Name() << "):\n";
            //seg->stat->numCars->Output();
            ofs << "CARS(" << seg->stat->speed->Name() << "),MIN(" << seg->stat->speed->Name() << "),MAX(" << seg->stat->speed->Name() << "),AVG(" << seg->stat->speed->Name() << "),";
        }
        seg = seg->getNextSegment();
    }
    ofs << "\n";
}

void Highway::printAndResetStats(std::ofstream& ofs){
    Segment* seg = this->firstSegment;
    ofs << Time << "," << Weekday << "," << Hour << ",";
    while(seg){
        if (seg->stat){
            //std::cout << "\n\nTRAFFIC DENSITY (" << seg->stat->numCars->Name() << "):\n";
            //seg->stat->numCars->Output();
            ofs << seg->stat->numCars << "," << seg->stat->speed->Min() << "," << seg->stat->speed->Max() << "," << seg->stat->speed->MeanValue() << ",";
            seg->stat->speed->Clear();
            seg->stat->numCars = 0;
        }
        seg = seg->getNextSegment();
    }
    ofs << "\n";
}
