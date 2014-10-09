#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <opencv/cv.h>
#include <opencv/highgui.h>
#include <math.h>
#include <unistd.h>
#include <termios.h>
#include <fcntl.h>
#include <time.h>

#include "findCrossPoint.h"
#include "angleFilter2hgtwid.h"
#include "splitBlack.h"

#include "utility.h"
#include "define.h"

#define usefirewire

using namespace std;

void init();
void endo();

//serial
char changeValue();
int serialOpen();
void serialClose();
void sendMixPoints();
void serialWriteChar(unsigned char c);
int fd;
struct termios oldtio, newtio;

CvCapture    *src;
IplImage     *img, *img_gray, *img_base;
CvMemStorage *storage = cvCreateMemStorage(0);
CvSeq        *contours = 0, *cvtemp, *lines;
CvPoint2D32f point[1000];
CvRect       rect;
CvPoint      *p;

//capture
static int capturewidth  = 640;
static int captureheight = 480;


//straight filter
pos position[100000];
int sentinel = -1, pi = 0;

pos mixpoint;

int main(){
    int rx,ry,rw,rh;
    
    int  orderBlack = 7;
    int noSerial = 0;
	//find cross point 
	intervalTag tatesen[100], yokosen[100];
    mixpoint.x = mixpoint.y = 2000;
    
	//canny
	int  cannyfirst = 30, cannytouch = 40;

    //find line
    int tatei,yokoi;
    
	//other
	int timestamp = 0;
	char key;

    //watch timer
    clock_t start,end;
    
    init();
    
    cvSetCaptureProperty(src,CV_CAP_PROP_FRAME_WIDTH,capturewidth);
    cvSetCaptureProperty(src,CV_CAP_PROP_FRAME_HEIGHT,captureheight);

	if (-1 == serialOpen()){
        noSerial = 1;
	}

	while (++timestamp){       
        
        start = clock();
		printf("%5d", timestamp);
        
		img = cvQueryFrame(src);
//		if (img == NULL)break;

        if(mixpoint.x<1500 && mixpoint.y<1500){
            rx = mixpoint.x-250;
            ry = mixpoint.y-250;
            if(rx < 0)rx=0;
            if(ry < 0)ry=0;
            if((rw = mixpoint.x+50 )>capturewidth) rw = capturewidth;
            if((rh = mixpoint.y+50 )>captureheight)rh = captureheight;
            
            rect = cvRect(rx,ry,rw,rh);

            cvSetImageROI(img     ,rect);
            cvSetImageROI(img_gray,rect);
            cvSetImageROI(img_base,rect);
        }
        
        //flat
//		cvSmooth(img, img, CV_MEDIAN, 3, 0, 0);

        img_base = cvCloneImage(img);

		//cvCvtColor(img, img_gray_base, CV_RGB2GRAY);

		splitBlack(orderBlack,img);
		
        //color -> gray
		cvCvtColor(img, img_gray, CV_BGR2GRAY);

		//nitika
		cvThreshold(img_gray, img_gray, 100, 255, CV_THRESH_BINARY);
        
        //edge
		cvCanny(img_gray, img_gray, cannyfirst, cannytouch, 5);
		//std::cout << "caniF:" << cannyfirst << " caniT:" << cannytouch << " ";
        
        //haugh
		lines = cvHoughLines2(img_gray, storage, CV_HOUGH_PROBABILISTIC, 1, CV_PI / 180, 50, 35, 10);
        
        yokoi = tatei = 0;
        for (int i = 0; i < lines->total; i++) {
            p = (CvPoint *) cvGetSeqElem (lines, i);
            
            //tate yoko filter
            //angle filter
            if((abs(p[0].x - p[1].x)/(sqrt(pow((double)abs(p[0].x - p[1].x),2)+pow((double)abs(p[0].y - p[1].y),2)))) < cos45){
                yokosen[yokoi].start.x = p[0].x;
                yokosen[yokoi].start.y = p[0].y;
                yokosen[yokoi].end.x   = p[1].x;
                yokosen[yokoi].end.y   = p[1].y;
                yokoi++;
                //cvLine (img_base, p[0], p[1], CV_RGB (255, 0, 0), 3, 8, 0);
            }else{
                tatesen[tatei].start.x = p[0].x;
                tatesen[tatei].start.y = p[0].y;
                tatesen[tatei].end.x   = p[1].x;
                tatesen[tatei].end.y   = p[1].y;
                tatei++;
                //cvLine (img_base, p[0], p[1], CV_RGB (255, 0, 0), 3, 8, 0);
            }
        }
        
       // printf("ti:%2d yi:%2d ",tatei,yokoi);
        
        mixpoint = findCrossPoint(tatesen,yokosen,tatei,yokoi);
        /*
        if(mixpoint.x>0&&mixpoint.y>0){
            cvCircle(img_base, cvPoint(mixpoint.x, mixpoint.y), 3, CV_RGB(255, 255, 255), 2);
        }*/


		//if ('q' == changeValue())break;

		if(noSerial==0)sendMixPoints();

        end = clock();
        
        cvResetImageROI(img);
        cvResetImageROI(img_gray);
        cvResetImageROI(img_base);
    
      //  cvShowImage("input", img_base);
        
        printf(" x:%4d y:%4d\n",mixpoint.x,mixpoint.y);
      //  cout << " time:" << (double)(end-start)/CLOCKS_PER_SEC << endl;
 
    }

	endo();

	return 0;
}


void sendMixPoints(){
	unsigned char d[4] = { 0x00, 0x40, 0x80, 0xC0 };
	for (int j = 0; j<6; j++){
		if ((mixpoint.x >> j) & 1 == 1){
			d[0] |= (1 << j);
		}
		if ((mixpoint.x >> (j + 6)) & 1 == 1){
			d[1] |= (1 << j);
		}
		if ((mixpoint.y >> j) & 1 == 1){
			d[2] |= (1 << j);
		}
		if ((mixpoint.y >> (j + 6)) & 1 == 1){
			d[3] |= (1 << j);
		}
	}

	for (int i = 0; i<4; i++){
		serialWriteChar(d[i]);
	}
}


int serialOpen() {
	fd = open("/dev/cu.usbserial-A90164E3", O_WRONLY);
	if (fd < 0) {
		printf("open error about serial connection\n");
		return -1;
	}
	tcgetattr(fd, &oldtio); /* save current port settings */
	bzero(&newtio, sizeof(newtio));
	newtio.c_cflag = 0x0000000D | CRTSCTS | CS8 | CLOCAL | CREAD;
	newtio.c_iflag = IGNPAR;
	newtio.c_oflag = 0;
	/* set input mode (non-canonical, no echo,...) */
	newtio.c_lflag = 0;
	newtio.c_cc[VTIME] = 0;   /* inter-character timer unused */
	newtio.c_cc[VMIN] = 1;   /* blocking read until 1 chars received */
	tcflush(fd, TCIFLUSH);
	tcsetattr(fd, TCSANOW, &newtio);
	return 0;
}


void serialWriteChar(unsigned char c) {
	int res = 0;
	tcflush(fd, TCIFLUSH); //clear the receiving buffer
	res = write(fd, &c, 1); //blocked until reading 1 byte.
}


void serialClose() {
	tcsetattr(fd, TCSANOW, &oldtio); //restore previous port setting
	close(fd);
}


void init(){
	int width, height;

	cvNamedWindow("input", CV_WINDOW_AUTOSIZE);

	src = cvCaptureFromCAM(0);
	cvSetCaptureProperty(src, CV_CAP_PROP_FPS, 60);
	if (src == NULL){ puts("no camera"); exit(1); }

	img = cvQueryFrame(src);

	width = capturewidth; height = captureheight;

	img_gray = cvCreateImage(cvSize(width, height), IPL_DEPTH_8U, 1);
	img_base = cvCreateImage(cvSize(width, height), IPL_DEPTH_8U, 3);
}


void endo(){
	cvDestroyAllWindows();
	cvReleaseCapture(&src);
	cvReleaseImage(&img);
	cvReleaseImage(&img_gray);
	cvReleaseImage(&img_base);
}


char changeValue(){
	char key;
	//change value
	key = cvWaitKey(33);

	if (key == 'q'){
		return 'q';
		//ninkitohyou
	}
	return 0;
}

