/* 
 * File:   Config.h
 * Author: mike
 *
 * Created on 26. listopadu 2014, 16:32
 */

#ifndef CONFIG_H
#define	CONFIG_H

#define SAFETY_GAP 2 // 2 secs
#define SEGMENT_LEN 1000 // 1000 metres
#define MIN_SEGMENT_LEN 500 // Must be bigger than (or equal) SEGMENT_LEN/2
#define EXIT_SEGMENT_LEN 500 // Length (in metres) to be allocated to exit part of the segment
#define SPEED_UNLIMITED 120.0 // 120 m/s

#define CAR_LENGTH 5
#define TRUCK_LENGTH 15


// Date and time definition:
#define Weekday (((int)Time/86400)%7)  /* Weekday == day in week (Mon - Sun) as 0-6 */
#define Hour (((int)Time/3600)%24)


// Default simulation parameters:
extern char *sourceFile;
extern char *outputFile;
extern double simTime;
extern double safetyGapMu;
extern double safetyGapSigma;

#endif	/* CONFIG_H */

