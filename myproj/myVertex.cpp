#include "myVertex.h"
#include "myvector3d.h"
#include "myHalfedge.h"
#include "myFace.h"

myVertex::myVertex(void)
{
	point = NULL;
	originof = NULL;
	normal = new myVector3D(1.0,1.0,1.0);
}

myVertex::~myVertex(void)
{
	if (normal) delete normal;
}

void myVertex::computeNormal()
{
	myHalfedge *e = originof;
	normal->clear();
	do {
		*normal += *e->adjacent_face->normal;

		e = e->prev->twin;
	} while (e != originof);
	normal->normalize();

}
