#pragma once
#include "mypoint3d.h"
#include <vector>
#include <string>

class myHalfedge;
class myVector3D;

class myVertex
{
public:
	myPoint3D *point;
	myHalfedge *originof;

	int index;  //use as you wish.

	myVector3D *normal;

	void computeNormal();
	myVertex(void);
	~myVertex(void);
};
