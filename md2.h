struct pt {
        float x;
        float y;
        float z;
};

struct poly {
        struct pt point[8];
        unsigned int vert_count;
        unsigned int tex_id;
        struct poly *next;
};

struct md2_model_hdr { 
   int magic; 
   int version; 
   int skinWidth; 
   int skinHeight; 
   int frameSize; 
   int numSkins; 
   int numVertices; 
   int numTexCoords; 
   int numTriangles; 
   int numGlCommands; 
   int numFrames; 
   int offsetSkins; 
   int offsetTexCoords; 
   int offsetTriangles; 
   int offsetFrames; 
   int offsetGlCommands; 
   int offsetEnd; 
};

struct md2_pt {
   unsigned char vertex[3]; //X, Y, Z
   unsigned char lightNormalIndex;
};

struct md2_frame_hdr {
   float scale[3];
   float translate[3];
   char name[16];
};

struct frame {
	char name[16];
	struct pt *points;
};

struct triangle {
	unsigned short vertex[3];
	unsigned short texture[3];
};

struct tex_coord {
	float s;
	float t;
};

struct md2_model_node {
	int trianglecount;
	int framecount;
	struct frame *frames;
	struct tex_coord *texcoord;
	struct triangle *triangles;
	char frame_class[20];
	char *filename;
	struct md2_model_node *next;
};

struct md2_model *get_model();
