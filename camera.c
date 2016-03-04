float camera_x=0.0f;
float camera_y=0.0f;
float camera_z=0.0f;
float camera_xrot=0.0f;
float camera_yrot=0.0f;
float camera_zrot=0.0f;
float walkbias=0.0f;

float fsin(float in);
float fcos(float in);

int walkbiasangle;

const float piover180 = 0.0174532925f;
const float rad_to_deg = 57.29577951f;

void walk_forward() {
		camera_x -= (fsin(camera_yrot)*rad_to_deg) * 0.002f;
		camera_z += (fcos(camera_yrot)*rad_to_deg) * 0.002f;
		walkbiasangle+= 10;
		walkbiasangle%=360;
		walkbias=fsin(walkbiasangle * piover180)*0.25f;
}

void walk_backward() {
		camera_x += (fsin(camera_yrot)*rad_to_deg) * 0.002f;
		camera_z -= (fcos(camera_yrot)*rad_to_deg) * 0.002f;
		walkbiasangle-= 10;
		walkbiasangle%=360;
		walkbias=fsin(walkbiasangle * piover180)*0.25f;
}

void strafe_left() {
		camera_x += fsin(camera_yrot+(90*piover180)) * 0.1f;
		camera_z -= fcos(camera_yrot+(90*piover180)) * 0.1f;
		walkbiasangle+= 10;
		walkbiasangle%=360;
		walkbias=fsin(walkbiasangle * piover180)*0.25f;
}

void strafe_right() {
		camera_x += fsin(camera_yrot+(90*piover180)) * 0.1f;
		camera_z += fcos(camera_yrot+(90*piover180)) * 0.1f;
		walkbiasangle-= 10;
		walkbiasangle%=360;
		walkbias=fsin(walkbiasangle * piover180)*0.25f;
}

void move_camera(float xstep, float ystep, float zstep) {
	camera_x+=xstep;
	camera_y+=ystep;
	camera_z+=zstep;
}

void rotate_camera(float xstep, float ystep, float zstep) {
	camera_xrot+=xstep;
	camera_yrot+=ystep;
	camera_zrot+=zstep;
}

float cam_x() {
	return camera_x;
}

float cam_y() {
	return camera_y;//+walkbias;
}
float cam_z() {
	return camera_z;
}

float cam_xrot() {
	return camera_xrot*rad_to_deg;
}

float cam_yrot() {
	return camera_yrot*rad_to_deg;
}

float cam_zrot() {
	return camera_zrot*rad_to_deg;
}

