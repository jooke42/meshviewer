#include "myFace.h"
#include "myvector3d.h"
#include "myHalfedge.h"
#include "myVertex.h"
#include <GL/glew.h>

myFace::myFace(void)
{
	adjacent_halfedge = NULL;
	normal = new myVector3D(1.0, 1.0, 1.0);
}

myFace::~myFace(void)
{
	if (normal) delete normal;
}

void myFace::computeNormal()
{
	
	myVector3D v1 = (*adjacent_halfedge->next->source->point) - (*adjacent_halfedge->source->point) ;
	myHalfedge *nextEdge= adjacent_halfedge->next;
	myVector3D v2 = (*nextEdge->next->source->point) - (*nextEdge->source->point);
	normal->crossproduct(v1,v2);
	
	normal->normalize();
	
	
}

int myFace::totalEdges() {
	int totalEdges = 0;
	myHalfedge* e = this->adjacent_halfedge;
	do {
		totalEdges++;
		e = e->next;
	} while (e != this->adjacent_halfedge);
	return totalEdges;
}
