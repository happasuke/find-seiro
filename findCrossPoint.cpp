#include "findCrossPoint.h"


pos findCrossPoint(intervalTag *tatesen, intervalTag *yokosen,int tatei,int yokoi){
	myline lineBox[2];
    pos mixpoint;
    
    findTwoLine(tatesen, yokosen,tatei,yokoi,lineBox);
	mixpoint = findMixPoint(lineBox);
	return mixpoint;
}


void findTwoLine(intervalTag *tatesen, intervalTag *yokosen,int tatei,int yokoi,myline *lineBox){
	/*
     i love in it!
          | |
          | |
     _____| |_
     _____   _|
          |_|
     */
    
	lineBox[0].start.y = 2000;
	lineBox[0].start.x = 0;
	lineBox[0].end.y   = 2000;
    lineBox[0].end.x   = 2000;
	lineBox[1].start.y = 0;
  	lineBox[1].start.x = 2000;    
	lineBox[1].end.y   = 2000;   
	lineBox[1].end.x   = 2000;
    
    
#ifdef _OPENMP
#pragma omp parallel
#endif
    {
#ifdef _OPENMP
#pragma omp for
#endif
        for (int i = 0; i<tatei; i++){
            //find most upper line
            //culiculate distance to (0,0)
            if (sqrt((double)pow(abs(tatesen[i].start.y + tatesen[i].end.y) / 2,2) + pow(abs(tatesen[i].start.x + tatesen[i].end.x) / 2,2)) - sqrt((double)pow(abs(lineBox[0].start.y + lineBox[0].end.y) / 2,2) + pow(abs(lineBox[0].start.x + lineBox[0].end.x) / 2,2)) < 0){
                lineBox[0].start.y = tatesen[i].start.y;
                lineBox[0].start.x = tatesen[i].start.x;
                lineBox[0].end.y = tatesen[i].end.y;
                lineBox[0].end.x = tatesen[i].end.x;
            }
        }
        
#ifdef _OPENMP
#pragma omp for
#endif
        for (int i = 0; i<yokoi; i++){
            //find most left line
            if (sqrt((double)pow(abs(yokosen[i].start.x + yokosen[i].end.x) / 2,2) + pow(abs(yokosen[i].start.y + yokosen[i].end.y) / 2,2)) - sqrt((double)pow(abs(lineBox[1].start.x + lineBox[1].end.x) / 2,2) + pow(abs(lineBox[1].start.y + lineBox[1].end.y) / 2,2)) < 0){
                lineBox[1].start.y = yokosen[i].start.y;
                lineBox[1].start.x = yokosen[i].start.x;
                lineBox[1].end.y = yokosen[i].end.y;
                lineBox[1].end.x = yokosen[i].end.x;
            }
        }
    }
}


pos findMixPoint(myline *lineBox){
    pos mixpoint;
	float katamuki[2], seppen[2];
	float pkatamuki[2], pseppen[2];

	//detect equation
	for (int i = 0; i < 2; i++){
		if (lineBox[i].start.y == lineBox[i].end.y){ lineBox[i].end.y++; }
		if (lineBox[i].start.x == lineBox[i].end.x){ lineBox[i].end.x++; }
		katamuki[i] = (float)(lineBox[i].start.y - lineBox[i].end.y) / (float)(lineBox[i].start.x - lineBox[i].end.x);
		seppen[i] = (float)(lineBox[i].start.y) - katamuki[i] * (float)(lineBox[i].start.x);
	}

	//find cross point of 2 lines
	mixpoint.x = int((seppen[1] - seppen[0]) / (katamuki[0] - katamuki[1]));
	mixpoint.y = int((katamuki[0] * seppen[1] - katamuki[1] * seppen[0]) / (katamuki[0] - katamuki[1]));
    
	//cvCircle(img_base, cvPoint(mixpoint.x, mixpoint->y), 3, CV_RGB(255, 255, 255), 2);

    return mixpoint;
}
