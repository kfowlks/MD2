#include <kos.h>
#include "texture.h"
#include "camera.h"
#include "ent.h"
#include <GL/gl.h>
#include <GL/glu.h>

KOS_INIT_FLAGS(INIT_DEFAULT | PVR_ENABLE);

extern uint8 romdisk[];
KOS_INIT_ROMDISK(romdisk);           

int main(int argc, char **argv) {
	cont_cond_t cond;
	uint8	c;

	struct ent *ent;

	//init KOS
	//kos_init_all(THD_ENABLE | IRQ_ENABLE | TA_ENABLE, romdisk);
	
	//init texture.c
	texture_init();

	//init KGL and set the default viewport stuff
	glKosInit();
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45.0f, 640.0f / 480.0f, 0.1f, 100.0f);
	glMatrixMode(GL_MODELVIEW);
	glEnable(GL_TEXTURE_2D);
	glEnable(GL_KOS_AUTO_UV);
	glDisable(GL_CULL_FACE);

	//grab controller 1
	c = maple_first_controller();

	//create a doomguy, pointer to him is returned
	ent=create_ent("/rd/doomguy.md2","/rd/doomguy.jpg");

	//set his position
	ent->x=0;
	ent->z=-5;
	ent->y=-.7;

	//set his first and last animation frame
	ent->anim_start=find_frame(ent->model,"salute01");
	ent->anim_end=find_frame(ent->model,"salute10");

	//engine loop starts here
	while(1) {
		if (cont_get_cond(c, &cond) < 0) {
			printf("Error reading controller\n");
			break;
		}
		if (!(cond.buttons & CONT_START))
			break;

		//allow us to "walk" around.  walk_* functions move depending on what your y_rot() value is
		if (!(cond.buttons & CONT_DPAD_DOWN)) {
			walk_backward();
		}
		if (!(cond.buttons & CONT_DPAD_UP)) {
			walk_forward();
		}
		//rotate_camera take x, y, then z
		//these adjust the y_rot value
		if (!(cond.buttons & CONT_DPAD_LEFT)) {
			rotate_camera(0,-.02,0);
		}
		if (!(cond.buttons & CONT_DPAD_RIGHT)) {
			rotate_camera(0,.02,0);
		}
		//look up / down with Y and A
		if (!(cond.buttons & CONT_Y)) {
			rotate_camera(.01,0,0);
		}
		if (!(cond.buttons & CONT_A)) {
			rotate_camera(-.01,0,0);
		}
		//these allow the camera to strafe left / right depending on what your y_rot value is
		if (!(cond.buttons & CONT_X)) {
			strafe_left();
		}
		if (!(cond.buttons & CONT_B)) {
			strafe_right();
		}

		//tell KGL we're rendering a new frame
		glKosBeginFrame();
		
		//tell ent.c to draw all of the created entities
		draw_ents();

		//tell KOS our frame is done
		glKosFinishFrame();
	}
	//end of engine loop

  //end of program
  return 0;
}
