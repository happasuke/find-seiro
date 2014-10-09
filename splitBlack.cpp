#include "splitBlack.h"

void splitBlack(int orderBlack,IplImage *img){
	int height = img->height;
	int width = img->width;
    
    for (int j = 0; j<height; j++){
		for (int i = 0; i<width; i++){
			if (RELM(img, i, j)>orderBlack && GELM(img, i, j)>orderBlack && BELM(img, i, j)>orderBlack){
				BELM(img, i, j) = 255;
				GELM(img, i, j) = 255;
				RELM(img, i, j) = 255;
			}
			else{
				BELM(img, i, j) = 0;
				GELM(img, i, j) = 0;
				RELM(img, i, j) = 0;
			}
		}
	}
}