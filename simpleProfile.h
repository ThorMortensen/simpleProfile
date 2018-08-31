/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   simpleProfile.h
 * Author: thm
 *
 * Created on July 25, 2017, 12:17 PM
 */

/**
 * This is a very crude profiling tool. It will give you a rough
 * estimate of how long a piece of code is to execute.
 * But TAKE THE RESULTS WITH A GRAIN OF SALT!
 * The main purpose of this profiler is to COMPARE different parts of
 * you code and help you find bottlenecks NOT to give precise timing results.
 *
 * The macro will create the necessary variables and take a timestamp just
 * before the code is executed and just after. It then accumulate the time
 * difference and then prints the average time to stdout. It will increment 
 * the isProfiling var and decrement when it's finished profiling.
 * Enjoy :-)
 *
 * Usage:
 * Encapsulate the code you want to profile in the one of the macro's
 * I.e:
 *
 * PROFILE_MS(<placeholder_name>, <iterations>, <code to profile ... >)
 *
 * Where the <placeholder_name> is the name associated with the that specific
 * profiling and will be shown in the result. (It MUST BE UNIQUE as it is also 
 * the base name for the variables generated internally in the macro.)
 * The <iterations> is the number of times the profiling is run I.e how many
 * samples is used to calculate the average. The <code to profile ... > is the
 * code to be profiled. It can be arbitrary size and lines.
 * The isProfiling var will be 0 when all profiling is done. You can use that
 * var to stop the program when the profiling is finished (or something else)
 * 
 * =========================================================================
 * Example I want to profile a function called mySlowFunction() and get
 * an average runtime of 10000 iterations. Afterwards I want the program to stop:
 * -------------------------------------------------------------------------
 * PROFILE_MS(verySlowFunction, 10000, mySlowFunction();)
 *
 * if(!isProfiling){
 *    exit(1);
 * }
 * -------------------------------------------------------------------------
 * Now I want to profile some other code snippet to compare 
 * the runtime to my mySlowFunction():
 * -------------------------------------------------------------------------
 * PROFILE_MS(verySlowFunction, 10000, mySlowFunction();)
 * PROFILE_MS(otherStuff, 10000,
 *             someOtherFunction_1();
 *             someOtherFunction_2();
 *             foo++;
 *             if(foo == 42){
 *                doSomeStuff();
 *             }
 *             )
 *
 * if(!isProfiling){
 *    exit(1);
 * }
 * -------------------------------------------------------------------------
 * The result will show runtime for mySlowFuncktion() and the other chunk
 * of code (as one result). I can then compare the to.
 * =========================================================================
 */
#ifndef SIMPLEPROFILE_H
#define SIMPLEPROFILE_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <math.h>
#include <stdio.h>
#include <time.h>

#define MS_PROFILE 1000000
#define US_PROFILE 1000

   //CLOCK_MONOTONIC
   //CLOCK_REALTIME
#define PROF_TIME_MODE CLOCK_REALTIME

   extern uint32_t isProfiling;

    

#define ___INTERNAL_USE_ONLY_PROFILE_HELPER(placeholderName, iterations, timeDiv, unitStr, stuffToProfile)\
         static uint32_t avarage_##placeholderName = 0;\
         static uint32_t profileRuns_##placeholderName = 0;\
         static uint32_t itCount_##placeholderName = 0;\
         static uint32_t minTime_##placeholderName = 0xFFFFFFFF;\
         static uint32_t maxTime_##placeholderName = 0;\
         static uint32_t minLapNumber_##placeholderName = 0;\
         static uint32_t maxLapNumber_##placeholderName = 0;\
         static bool initCount_##placeholderName = true;\
         struct timespec timeStart_##placeholderName;\
         struct timespec timeEnd_##placeholderName;\
         if(initCount_##placeholderName){\
            isProfiling += 1;\
            initCount_##placeholderName = false;\
         }\
         clock_gettime(PROF_TIME_MODE, &timeStart_##placeholderName);\
         stuffToProfile \
         clock_gettime(PROF_TIME_MODE, &timeEnd_##placeholderName);\
         if (timeEnd_##placeholderName.tv_nsec > timeStart_##placeholderName.tv_nsec) {\
            uint32_t lapTime = ((timeEnd_##placeholderName.tv_nsec / timeDiv) - (timeStart_##placeholderName.tv_nsec / timeDiv));\
            avarage_##placeholderName += lapTime;\
            if (lapTime < minTime_##placeholderName  ){\
               minTime_##placeholderName = lapTime;\
               minLapNumber_##placeholderName = profileRuns_##placeholderName;\
            }\
            if (lapTime > maxTime_##placeholderName){\
               maxTime_##placeholderName = lapTime;\
               maxLapNumber_##placeholderName = profileRuns_##placeholderName;\
            }\
            profileRuns_##placeholderName++;\
         } \
         if (profileRuns_##placeholderName == iterations) {\
            printf("\n}------- "#placeholderName" -------{\n"\
                   "  Iterrations ["#iterations"], Time-div "#timeDiv"\n"\
                   "  Min time at lap%u  ~~>  [%u]\n"\
                   "  Max time at lap%u  ~~>  [%u]\n"\
                   "  Average time       ==>  [%u]\n"\
                   "----------------------------------\n",\
                   minLapNumber_##placeholderName, minTime_##placeholderName, \
                   maxLapNumber_##placeholderName, maxTime_##placeholderName, \
                   (avarage_##placeholderName / profileRuns_##placeholderName));\
            isProfiling -= 1;\
         };

#define PROFILE(placeholderName, iterations, timeDiv, stuffToProfile) \
         ___INTERNAL_USE_ONLY_PROFILE_HELPER(placeholderName, iterations, timeDiv, "", stuffToProfile)

#define PROFILE_MS(placeholderName, iterations, stuffToProfile) \
         ___INTERNAL_USE_ONLY_PROFILE_HELPER(placeholderName, iterations, MS_PROFILE, "ms", stuffToProfile)

#define PROFILE_US(placeholderName, iterations, stuffToProfile) \
         ___INTERNAL_USE_ONLY_PROFILE_HELPER(placeholderName, iterations, US_PROFILE, "us", stuffToProfile)


#ifdef __cplusplus
}
#endif

#endif /* SIMPLEPROFILE_H */

