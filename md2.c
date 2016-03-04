#include <kos.h>
#include <GL/gl.h>
#include <math.h>
#include "md2.h"
#include "camera.h"

struct md2_model_node *model_list=NULL;

extern const float piover180;

int find_frame(struct md2_model_node *model, char *name) {
	int i;
	//printf("Searching for: %s...",name);
	for(i=0;i<model->framecount;i++) {
		//printf("Compare: \"%s\" and \"%s\"\n",name,model->frames[i].name);
		if(!strcmp(name,model->frames[i].name)) {
			//printf("match!\n");
			return i;
		}
	}
	//printf("no match!\n");
	return -1;
}

struct md2_model *get_model(char *filename) {
	struct md2_model_node *current=model_list;
	while(current!=NULL) {
		if(!strcmp(current->filename,filename)) {
			return current;
		}
		current=current->next;
	}
	return NULL;
}

void load_md2(char *filename) {
	int fd;
	int i,j;
	short s,t;
	struct md2_model_hdr model_hdr;
	struct md2_frame_hdr frame_hdr;
	struct md2_pt point;
	struct md2_model_node *current=model_list;
	struct md2_model_node *prev=NULL;
	fd=fs_open(filename,O_RDONLY);
	while(current!=NULL) {
		prev=current;
		current=current->next;
	}
	current=(struct md2_model_node *)malloc(sizeof(struct md2_model_node));
	if(prev!=NULL) {
		prev->next=current;
	} else {
		model_list=current;
	}
	current->filename=(char *)malloc(strlen(filename));
	current->next=NULL;
	strcpy(current->filename,filename);

	fs_read(fd,&model_hdr,sizeof(struct md2_model_hdr));

	current->framecount=model_hdr.numFrames;
	current->trianglecount=model_hdr.numTriangles;

	printf("Loading %i frames of %i vertices...\n",model_hdr.numFrames,model_hdr.numVertices);
	current->frames=(struct frame *)malloc(sizeof(struct frame) * model_hdr.numFrames);
	for(i=0;i<model_hdr.numFrames;i++) {
		current->frames[i].points=(struct pt *)malloc(sizeof(struct pt) * model_hdr.numVertices);
		fs_seek(fd,model_hdr.offsetFrames+(model_hdr.frameSize*i),SEEK_SET);

		fs_read(fd,&frame_hdr,sizeof(struct md2_frame_hdr));
		strcpy(current->frames[i].name,frame_hdr.name);
		//printf("Frame name: %s\n",frame_hdr.name);
		
		for(j=0;j<model_hdr.numVertices;j++) {
			fs_read(fd,&point,sizeof(struct md2_pt));

			current->frames[i].points[j].x=(point.vertex[0] * frame_hdr.scale[0] + frame_hdr.translate[0])/20.0f;
			current->frames[i].points[j].y=(point.vertex[1] * frame_hdr.scale[1] + frame_hdr.translate[1])/20.0f;
			current->frames[i].points[j].z=(point.vertex[2] * frame_hdr.scale[2] + frame_hdr.translate[2])/20.0f;
		}
	}

	//load up U and V values
	fs_seek(fd,model_hdr.offsetTexCoords,SEEK_SET);
	printf("Loading %i texture coordinates...\n",model_hdr.numTexCoords);
	current->texcoord=(struct tex_coord *)malloc(sizeof(struct tex_coord)*model_hdr.numTexCoords);
	for(i=0;i<model_hdr.numTexCoords;i++) {
		fs_read(fd,&s,sizeof(short));
		fs_read(fd,&t,sizeof(short));
		current->texcoord[i].s=(float)s/(float)model_hdr.skinWidth;
		current->texcoord[i].t=(float)t/(float)model_hdr.skinHeight;
	}

	fs_seek(fd,model_hdr.offsetTriangles,SEEK_SET);

	printf("Loading %i triangles...\n",model_hdr.numTriangles);
	current->triangles=(struct triangle *)malloc(sizeof(struct triangle)*model_hdr.numTriangles);
	for(i=0;i<model_hdr.numTriangles;i++) {
		fs_read(fd,&current->triangles[i],sizeof(struct triangle));
	}
	//current->texid=next_tex_id();
	//jpeg_to_texture(skinname,model.texid);
}

void draw_md2(struct md2_model_node *model, int texid, float mx, float my, float mz, int frame1, int frame2, int blendpos, int blendcount) {
	int i,j;
	unsigned short vert;
	unsigned short text;
	char temp[30];
	float x,y,z;
	float x1,y1,z1;
	float x2,y2,z2;
	float light,dx,dy,dz;
	switch_tex(texid);
	glDisable(GL_KOS_AUTO_UV);

	glLoadIdentity();
	glRotatef(cam_xrot(),1.0f,0.0f,0.0f);
	glRotatef(cam_zrot(),0.0f,0.0f,1.0f);
	glRotatef(cam_yrot(),0.0f,1.0f,0.0f);
	glTranslatef(cam_x(),cam_y(),cam_z());
	glTranslatef(mx,my,mz);
	glRotatef(-90,1.0f,0.0f,0.0f);
	glBegin(GL_TRIANGLES);
	for(i=0;i<model->trianglecount;i++) {
		for(j=0;j<3;j++) {
			vert=model->triangles[i].vertex[j];
			text=model->triangles[i].texture[j];
			x1=model->frames[frame1].points[vert].x;
			x2=model->frames[frame2].points[vert].x;
			x=x1-((x1-x2)/blendcount)*blendpos;
			y1=model->frames[frame1].points[vert].y;
			y2=model->frames[frame2].points[vert].y;
			y=y1-((y1-y2)/blendcount)*blendpos;
			z1=model->frames[frame1].points[vert].z;
			z2=model->frames[frame2].points[vert].z;
			z=z1-((z1-z2)/blendcount)*blendpos;

			glTexCoord2f(model->texcoord[text].s,model->texcoord[text].t);
			dx=cam_x()-(mx+x);
			dy=cam_y()-(my+y);
			dz=cam_z()-(mz+z);
			light=1;//1-(sqrt(dx*dx+dy*dy+dz*dz)/8);
			//printf("Light: %f\n",light);
			if(light<0) { light=0; }
			glColor3f(light,light,light);
			glVertex3f(x,y,z);
		}
	}
	glEnd();
	glEnable(GL_KOS_AUTO_UV);
}
