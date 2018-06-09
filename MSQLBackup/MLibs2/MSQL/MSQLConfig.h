#pragma once

#define MSQLOutputNone
//#define MSQLOutputprintf
//#define MSQLOutputMLog



#ifdef MSQLOutputNone
// all output disabled
#define MSQLLog //
#define MSQLLogFirstParameter 0
#endif


#ifdef MSQLOutputprintf
// for debugging logging printf
#define MSQLprintf(firstparameter, ...) printf(##__VA_ARGS__); printf("\n")
#define MSQLLog MSQLprintf
#define MSQLLogFirstParameter 0
#endif


#ifdef MSQLOutputMLog
// for debugging logging with MLog
//#define MSQLLog MLogS
//#define MSQLLogFirstParameter MLUser
#endif
