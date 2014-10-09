#ifndef __utility
#define __utility

#define BELM(IMG,X,Y) ((uchar*)((IMG)->imageData + (IMG)->widthStep*(Y)))[(X)*3]
#define GELM(IMG,X,Y) ((uchar*)((IMG)->imageData + (IMG)->widthStep*(Y)))[(X)*3+1]
#define RELM(IMG,X,Y) ((uchar*)((IMG)->imageData + (IMG)->widthStep*(Y)))[(X)*3+2]

#define cos45 0.525321989

#endif