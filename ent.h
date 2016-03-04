#include "md2.h"

struct ent {
	float x;
	float y;
	float z;
	struct md2_model_node *model;
	int tex_id;
	int anim_start;
	int anim_end;
	int framenum;
	int blendpos;
	int blendcount;
	struct ent *next;
};