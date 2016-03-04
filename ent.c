#include <kos.h>
#include "texture.h"
#include "ent.h"

struct ent *ent_list=NULL;

struct ent *create_ent(char *model_name, char *skin_name) {
	struct ent *current=ent_list;
	struct ent *prev=NULL;
	while(current!=NULL) {
		prev=current;
		current=current->next;
	}
	current=(struct ent *)malloc(sizeof(struct ent));
	if(prev!=NULL) {
		prev->next=current;
	} else {
		ent_list=current;
	}
	current->model=get_model(model_name);
	if(current->model==NULL) {
		load_md2(model_name);
		current->model=get_model(model_name);
	}
	current->tex_id=find_tex(skin_name);
	if(current->tex_id==-1) {
		jpeg_to_texture(skin_name,next_tex_id());
		current->tex_id=find_tex(skin_name);
	}
	current->x=0;
	current->y=0;
	current->z=0;
	current->blendcount=2;
	current->blendpos=0;
	current->framenum=1;
	current->next=NULL;
	return current;
}

void draw_ents() {
	struct ent *current=ent_list;
	int frame1,frame2;
	char temp[20];
	while(current!=NULL) {
		if(current->framenum<current->anim_start) {
			current->framenum=current->anim_start;
		}
		frame1=current->framenum;
		if(current->framenum==current->anim_end) {
			frame2=current->anim_start;
		} else {
			frame2=current->framenum+1;
		}

		draw_md2(current->model,current->tex_id,current->x,current->y,current->z,frame1,frame2,current->blendpos,current->blendcount);

		current->blendpos++;
		if(current->blendpos>current->blendcount) {
			current->blendpos=0;
			current->framenum++;
		}

		if(current->framenum>current->anim_end) { current->framenum=current->anim_start; }
		current=current->next;
	}
}