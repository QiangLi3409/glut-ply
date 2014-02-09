#ifndef __UTILS_H__
#define __UTILS_H__


typedef struct JamesPoint
{
	float x;
	float y;
	float z;
	int status;

} JAMESDATA;


// save PLY data

	   typedef struct Vertex {
		float x,y,z;             /* the usual 3-space position of a vertex */
		} Vertex;

		typedef struct Face {
		  unsigned char intensity; /* this user attaches intensity to faces */
		  unsigned char nverts;    /* number of vertex indices in list */
		  int *verts;              /* vertex index list */
		} Face;

			

#endif 