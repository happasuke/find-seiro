#ifndef __define
#define __define

typedef struct _pos{
	int x;
	int y;
}pos;

typedef struct _myline{
//near 0,0 point is start
	pos start;
	pos end;
}myline;

typedef struct _intervalTag{
	float angle;
	pos start;
	pos end;
}intervalTag;

#endif