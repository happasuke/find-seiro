#Makefile
CC   = g++
LFLG = -L/opt/local/lib -L/usr/lib
LIBS = -fopenmp -lm -lopencv_highgui -lopencv_core -lopencv_ml -lopencv_video -lopencv_imgproc -lopencv_calib3d -lopencv_objdetect
OBJS = findCrossPoint.cpp splitBlack.cpp
HEAD = findCrossPoint.h splitBlack.h utility.h define.h

find:$(OBJS) main.cpp $(HEAD)
	$(CC) main.cpp $(OBJS) $(LFLG) $(LIBS) -o find
clean:
	rm -r find
