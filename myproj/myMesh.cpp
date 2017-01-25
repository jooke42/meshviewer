#include "myMesh.h"
#include <fstream>
#include <iostream>
#include <sstream>
#include <map>
#include <utility>
#include <GL/glew.h>
#include "myvector3d.h"

using namespace std;

myMesh::myMesh(void)
{
	/**** TODO ****/
}


myMesh::~myMesh(void)
{
	clear();
}

void myMesh::inflateMesh(double dist) {
	for (vector<myVertex*>::iterator it = vertices.begin(); it != vertices.end(); it++) {
		*((*it)->point) = *(*it)->point + *((*it)->normal)*dist;
	}
}

void myMesh::smoothenMesh(double dist) {
	vector<myPoint3D*> NbSums;
	for (vector<myVertex*>::iterator it = vertices.begin(); it != vertices.end(); it++) {
		myPoint3D* X = new myPoint3D(0,0,0);
		myHalfedge* e = (*it)->originof->twin;
		int nbB = 0;
		do {
			*X += *(e->source->point);
			nbB++;
			e = e->next->twin;
		} while (e != (*it)->originof->twin);
		*X /= nbB;
		NbSums.push_back(X);
	}
	for (int i = 0; i < vertices.size();i++) {
		*(vertices[i]->point) = (*(vertices[i]->point))*(1 - dist) + (*NbSums[i])*dist;
	}
		
}



void myMesh::clear()
{
	for (unsigned int i = 0; i < vertices.size(); i++) if (vertices[i]) delete vertices[i];
	for (unsigned int i = 0; i < halfedges.size(); i++) if (halfedges[i]) delete halfedges[i];
	for (unsigned int i = 0; i < faces.size(); i++) if (faces[i]) delete faces[i];

	vector<myVertex *> empty_vertices;    vertices.swap(empty_vertices);
	vector<myHalfedge *> empty_halfedges; halfedges.swap(empty_halfedges);
	vector<myFace *> empty_faces;         faces.swap(empty_faces);
}


void myMesh::checkMesh()
{
	bool error;
	unsigned int count;

	cout << "Checking mesh for errors...\n";
	cout << "\tNumber of vertices: " << vertices.size() << endl;
	cout << "\tNumber of halfedges: " << halfedges.size() << endl;
	cout << "\tNumber of faces: " << faces.size() << endl << endl;

	cout << "\tChecking for NULL vertices. " << endl;
	for (unsigned int i = 0; i < vertices.size(); i++)
	{
		if (vertices[i] == NULL)
			cout << "\t\tError: vertex " << i << " is NULL.\n";
		else if (vertices[i]->originof == NULL)
			cout << "\t\tError: the originof halfedge of vertex " << i << " is NULL.\n";
	}
	cout << "\t  Ended check.\n\n";

	cout << "\tChecking the halfedges. " << endl;
	error = false;
	count = 0;
	for (unsigned int i = 0; i < halfedges.size(); i++)
	{
		if (halfedges[i]->source == NULL)
			cout << "\tError: Source is NULL for halfedge " << i << endl;
		if (halfedges[i]->twin == NULL) count++;
		if (halfedges[i]->next == NULL || halfedges[i]->prev == NULL)
			cout << "\tError: Next/prev NULL for halfedge " << i << endl;
		if (halfedges[i]->next->prev != halfedges[i] || halfedges[i]->prev->next != halfedges[i])
			cout << "\tError: Next/prev not set properly for halfedge " << i << endl;
		if (halfedges[i]->twin != NULL && halfedges[i] != halfedges[i]->twin->twin)
			cout << "\tError: Twin pair not set properly for halfedge " << i << endl;
	}
	if (count > 0) cout << "\tThis mesh has boundary edges.\n";
	cout << "\t  Ended check.\n\n";

	cout << "\tChecking fans of each vertex.\n";
	for (unsigned int i = 0; i<vertices.size(); i++) {
		myVertex *v = vertices[i];

		myHalfedge *e1 = v->originof;

		unsigned int k = 0;
		do {
			k++;
			e1 = e1->prev->twin;
			if (k > 100000) cout << "\t\tError: Infinite loop when checking adjacent edges for vertex " << i << endl;
		} while (e1 != NULL && e1 != v->originof);
	}
	cout << "\t  Ended check.\n\n";

	cout << "\tChecking edges of each face.\n";
	unsigned int num_incidentedgesoverallfaces = 0;
	bool istriangular = true;
	for (unsigned i = 0; i<faces.size(); i++) {
		myHalfedge *e1 = faces[i]->adjacent_halfedge;
		unsigned int k = 0;
		do {
			k++;
			if (e1 == NULL) cout << "\t\tError: Found NULL edge on boundary of face " << i << endl;
			e1 = e1->next;
			if (k > 100000) cout << "\t\tError: Infinite loop when checking adjacent edges for face " << i << endl;
		} while (e1 != faces[i]->adjacent_halfedge);
		num_incidentedgesoverallfaces += k;
		if (k>3) istriangular = false;
	}
	if (istriangular) cout << "\t\tThe mesh is triangular.\n";
	else cout << "\t\tThe mesh is not triangular.\n";
	if (num_incidentedgesoverallfaces != halfedges.size())
		cout << "\t\tSuspicious: the total number of halfedges is not equal to the sum at each face.\n";
	cout << "\t  Ended check.\n\n";

	cout << "  Ended check of mesh.\n";
}


bool myMesh::readFile(std::string filename)
{
 
	string s, t, u;
	vector<int> faceids;
	myHalfedge **hedges;

	ifstream fin(filename);
	if (!fin.is_open()) {
		cout << "Unable to open file!\n";
		return false;
	}
	name = filename;

	map<pair<int, int>, myHalfedge *> twin_map;
	map<pair<int, int>, myHalfedge *>::iterator it;
	int i = 1;
	while (getline(fin, s))
	{
		stringstream myline(s);
		myline >> t;
		if (t == "g") {}
		else if (t == "v")
		{
			myPoint3D* point = new myPoint3D();
			myVertex* v = new myVertex();
			myline >> u;
			point->X = stod(u.substr(0, u.find("/")));
			myline >> u;
			point->Y = stod(u.substr(0, u.find("/")));
			myline >> u;
			point->Z = stod(u.substr(0, u.find("/")));
			v->index = i;
			v->point = point;
			this->vertices.push_back(v);
			i++;

		}
		else if (t == "mtllib") {}
		else if (t == "usemtl") {}
		else if (t == "s") {}
		else if (t == "f")
		{
			myFace* face = new myFace();
			vector<myHalfedge*> face_edges;
			vector<int> face_indices;


			while (myline >> u) {
				int currentVertexIndex = atoi((u.substr(0, u.find("/"))).c_str())-1;
				face_indices.push_back(currentVertexIndex);
				face_edges.push_back(new myHalfedge());
			}
			for (int i = 0; i < face_indices.size();i++) {
				int ipo = (i + 1) % face_indices.size();
				int imo = (i - 1 + face_indices.size()) % face_indices.size();
				face_edges[i]->next = face_edges[ipo];
				face_edges[i]->prev = face_edges[imo];
				face_edges[i]->adjacent_face = face;


				face_edges[i]->source = vertices[face_indices[i]];
				vertices[face_indices[i]]->originof = face_edges[i];

				this->halfedges.push_back(face_edges[i]);
				twin_map.insert(std::pair<std::pair<int,int>,myHalfedge*>(std::pair<int,int>(face_indices[i],face_indices[ipo]), face_edges[i]));
				it = twin_map.find(std::pair<int, int>(face_indices[ipo], face_indices[i]));
				if (it != twin_map.end()) {
					face_edges[i]->twin=it->second;
					it->second->twin = face_edges[i];
				}
					
			}

			face->adjacent_halfedge = face_edges[0];
			this->faces.push_back(face);
			cout << endl;
		}
	}

	checkMesh();
	normalize();

	return true;
}


void myMesh::computeNormals()
{
	for (vector<myFace *>::iterator it = faces.begin(); it != faces.end(); it++) {
		(*it)->computeNormal();
	}
	for (vector<myVertex *>::iterator it = vertices.begin(); it != vertices.end(); it++) {
		(*it)->computeNormal();
	}
}

void myMesh::normalize()
{
	if (vertices.size() < 1) return;

	int tmpxmin = 0, tmpymin = 0, tmpzmin = 0, tmpxmax = 0, tmpymax = 0, tmpzmax = 0;

	for (unsigned int i = 0; i < vertices.size(); i++) {
		if (vertices[i]->point->X < vertices[tmpxmin]->point->X) tmpxmin = i;
		if (vertices[i]->point->X > vertices[tmpxmax]->point->X) tmpxmax = i;

		if (vertices[i]->point->Y < vertices[tmpymin]->point->Y) tmpymin = i;
		if (vertices[i]->point->Y > vertices[tmpymax]->point->Y) tmpymax = i;

		if (vertices[i]->point->Z < vertices[tmpzmin]->point->Z) tmpzmin = i;
		if (vertices[i]->point->Z > vertices[tmpzmax]->point->Z) tmpzmax = i;
	}

	double xmin = vertices[tmpxmin]->point->X, xmax = vertices[tmpxmax]->point->X,
		ymin = vertices[tmpymin]->point->Y, ymax = vertices[tmpymax]->point->Y,
		zmin = vertices[tmpzmin]->point->Z, zmax = vertices[tmpzmax]->point->Z;

	double scale = (xmax - xmin) > (ymax - ymin) ? (xmax - xmin) : (ymax - ymin);
	scale = scale > (zmax - zmin) ? scale : (zmax - zmin);

	for (unsigned int i = 0; i < vertices.size(); i++) {
		vertices[i]->point->X -= (xmax + xmin) / 2;
		vertices[i]->point->Y -= (ymax + ymin) / 2;
		vertices[i]->point->Z -= (zmax + zmin) / 2;

		vertices[i]->point->X /= scale;
		vertices[i]->point->Y /= scale;
		vertices[i]->point->Z /= scale;
	}
}


void myMesh::splitFaceTRIS(myFace *f, myPoint3D *p)
{
	myHalfedge *e = f->adjacent_halfedge;
	myVertex *v1 = e->source;
	myVertex *v2 = e->next->source;
	
	myHalfedge *h1 = new myHalfedge();
	myHalfedge *h1Twin = new myHalfedge();
	myHalfedge *h2 = new myHalfedge();
	myHalfedge *h2Twin = new myHalfedge();
	myVertex *v3 = new myVertex();

	myFace *fn = new myFace();
	
	f->adjacent_halfedge = e->prev;
	
	//v3
	v3->point = p;
	v3->originof = h2;

	//h1
	h1->adjacent_face = f;
	h1->next = h2;
	h1->prev = e->prev;
	h1->source = v1;
	h1->twin = h1Twin;
	
	//h2
	h2->adjacent_face = f;
	h2->next = e->next;
	h2->prev = h1;
	h2->source = v3;
	h2->twin = h2Twin;

	//h1Twin
	h1Twin->adjacent_face = fn;
	h1Twin->next = e;
	h1Twin->prev = h2Twin;
	h1Twin->source = v3;
	h1Twin->twin = h1;

	//h2Twin
	h2Twin->adjacent_face = fn;
	h2Twin->next = h1Twin;
	h2Twin->prev = e;
	h2Twin->source = v2;
	h2Twin->twin = h2;

	//eNext
	e->next->prev = h2;

	//ePrev
	e->prev->next = h1;
	
	//fn
	fn->adjacent_halfedge = e;

	//e
	e->next = h2Twin;
	e->prev = h1Twin;

	//push all new Face, point and Halfedges

	faces.push_back(fn);
	halfedges.push_back(h1);
	halfedges.push_back(h1Twin);
	halfedges.push_back(h2);
	halfedges.push_back(h2Twin);
	vertices.push_back(v3);

	triangulate(f);

}

void myMesh::splitEdge(myHalfedge *e, myPoint3D *p)
{

	myVertex *v1 = e->source;
	myVertex *v2 = e->next->source;
	myHalfedge *eTwin = e->twin;
	myHalfedge *eTwinPrev = e->twin->prev;
	myHalfedge *eNext = e->next;
	myHalfedge *h = new myHalfedge();
	myHalfedge *hTwin = new myHalfedge();
	myVertex *v3 = new myVertex();

	// v3
	v3->point = p;
	v3->originof = eTwin;
	//h
	h->prev = e;
	h->next = e->next;
	h->source = v3;
	h->adjacent_face = e->adjacent_face;
	h->twin = hTwin;
	//hTwin
	hTwin->prev = eTwin->prev;
	hTwin->next = eTwin;
	hTwin->source = eTwin->source;
	hTwin->adjacent_face = eTwin->adjacent_face;
	hTwin->twin = h;
	//eTwinPrev
	eTwinPrev->next = hTwin;
	//eNext
	eNext->prev = h;
	// e
	e->next = h;
	//etwin
	eTwin->prev = hTwin;
	eTwin->source = v3;

	//addToStack
	halfedges.push_back(h);
	halfedges.push_back(hTwin);
	vertices.push_back(v3);
}

void myMesh::splitFaceQUADS(myFace *f, myPoint3D *p)
{
	/**** TODO ****/
}


void myMesh::subdivisionCatmullClark()
{
	/**** TODO ****/
}


void myMesh::triangulate()
{
	std::vector<myFace*> oldFaces = this->faces;
	for (auto it = oldFaces.begin();
		it != oldFaces.end();
		it++) {
		triangulate(*it);
	}
}


//return false if already triangle, true othewise.
bool myMesh::triangulate(myFace *f)
{
	int n = 0;
	myHalfedge *e = f->adjacent_halfedge;
	// compute n, the number of vertices in the face f
	do {
		n = n + 1;
		e = e->next;
	} while (e != f->adjacent_halfedge); // used to count the vertices of the face given
	if (n == 3)
		return false; // it's already a triangle, that's all for today folks
					  // otherwise, we have to triangulate the face given in parameter
	vector<myHalfedge *> in;
	vector<myHalfedge *> out;
	vector<myFace *> nf;
	myVertex *start_v = f->adjacent_halfedge->source;
	for (int i = 0; i < n - 3; i++) {
		in.push_back(new myHalfedge());
		out.push_back(new myHalfedge());
		halfedges.push_back(in[i]);
		halfedges.push_back(out[i]);
	}
	in.push_back(e->prev->twin);
	out.push_back(e->prev);
	for (int i = 0; i < n - 3; i++) {
		nf.push_back(new myFace());
		faces.push_back(nf[i]);
	}
	nf.push_back(f);
	e = f->adjacent_halfedge->next;
	for (int i = 0; i < n - 2; i++) {
		int ipo = (i + 1) % (n - 2);
		int imo = (i - 1 + (n - 2)) % (n - 2);
		if (i != n - 3)
		{
			in[i]->next = e->next;
			in[i]->prev = out[ipo];
			in[i]->twin = out[i];
			in[i]->source = start_v;
			in[i]->adjacent_face = nf[ipo];
		}
		out[i]->next = (i == 0 ? e->prev : in[imo]);

		out[i]->prev = e;
		out[i]->twin = in[i];
		out[i]->source = e->next->source;
		out[i]->adjacent_face = nf[i];
		myHalfedge *enext = e->next;
		e->next = out[i];
		if (i != 0) e->prev = in[imo];

		e->adjacent_face = nf[i];
		e = enext;
		nf[i]->adjacent_halfedge = out[i];
	}
	out[0]->next->prev = out[0];
	return true;
}