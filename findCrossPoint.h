#ifndef __findFourLine
#define __findFourLine

#include <stdio.h>
#include <iostream>
#include <opencv/highgui.h>
#include "define.h"
#include "math.h"
#include "utility.h"

pos findCrossPoint(intervalTag *tatesen, intervalTag *yokosen,int tatei,int yokoi);
void findTwoLine(intervalTag *tatesen,intervalTag *yokosen,int tatei,int yokoi,myline *lineBox);
pos findMixPoint(myline *lineBox);

#endif