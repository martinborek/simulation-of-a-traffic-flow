#include "stdlib.h"
#include "string.h"
#include "iostream"

#include "simlib.h"

#include "Segment.h"
#include "Config.h"
#include "Car.h"
#include "Highway.h"

#include <chrono>
#include <thread>


using namespace std;
using namespace simlib3;

Highway h;
std::ofstream ofs;

char *sourceFile;
char *outputFile;
double simTime;
double safetyGapMu;
double safetyGapSigma;



//class Observer : public Event{
//    // For visualising flow of traffic in commandline. Useful for debugging or as a screensaver.
//    void Behavior(){
//        h.printHighway();
//        std::this_thread::sleep_for(std::chrono::seconds(1));
//        Activate(Time+100);
//    }
//
//};




class StatCollector : public Event{
    // For collectiong hwy statistics each hour.
    void Behavior(){
        std::cout << "\rTime = " << Time << " (" << (int) (Time/simTime*100) << "%)" << std::flush;
        h.printAndResetStats(ofs);
        Activate(Time+3600);
    }
};

int main(int argc, char* argv[]){
    long seed = time(NULL);
    RandomSeed(seed);    
    std::cout << "DEBUG: Random seed = " << seed << "\n";
        
    // Default simulation parameters:
    sourceFile = "test.hdf";
    outputFile = "stat.csv";
    simTime = 864000; // 10 days
    safetyGapMu = 1.5;
    safetyGapSigma = 0.62148;
    
    // Parse args:
    char *helpLine = "USAGE:\n"
                     "    ./model -t simulation_time -s highway_definition.hdf -o output.csv \n"
                     "            -sgmu safetyGap_mu -sgsg safetyGap_sigma\n\n"
    "        simulation_time            Time period to simulate (seconds)\n"
    "        highway_definition.hdf     Definition file with highway description\n"
    "        output.csv                 File to write statistics to\n"
    "        safetyGap_mu               Mu coefficient for safety gap distribution (seconds)\n"
    "        safetyGap_sigma            Sigma coefficient for safety gap distribution (seconds)\n";
    
    for (int i = 1; i < argc; i++){
        if (strcmp(argv[i], "-s") == 0 && argc - i > 1){
            sourceFile = argv[++i];
        }
        else if (strcmp(argv[i], "-o") == 0 && argc - i > 1){
            outputFile = argv[++i];
        }
        else if (strcmp(argv[i], "-t") == 0 && argc - i > 1){
            simTime = atof(argv[++i]);
        }
        else if (strcmp(argv[i], "-sgmu") == 0 && argc - i > 1){
            safetyGapMu = atof(argv[++i]);
        }
        else if (strcmp(argv[i], "-sgsg") == 0 && argc - i > 1){
            safetyGapSigma = atof(argv[++i]);
        }
        else {
            std::cout << helpLine;
            return 0;
        }
    }
    
    // Init model time
    Init(0, simTime);
    
    // Build highway...
    try{
        if(h.build(sourceFile)){
            h.unbuild(); // Something went wrong
            return -1;
        }
    }
    catch (exception& e){
        std::cout << e.what() << "\n";
        return -1;
    }
    
    // Open statistics file for writing:
    ofs.open(outputFile);
    h.printStatsHeader(ofs); // write header with field names
    
    // Activate traffic generators:
    h.activateGenerators();
    
    //(new Observer)->Activate(234000);
    (new StatCollector)->Activate(3599);
    
    // Run the simulation:
    Run();
    
    // Free the freeway!
    h.unbuild();
    
    ofs.close();
    
    std::cout << "\nFinished successfully ( Time = " << Time << " )\n";
    
    return EXIT_SUCCESS;
}
