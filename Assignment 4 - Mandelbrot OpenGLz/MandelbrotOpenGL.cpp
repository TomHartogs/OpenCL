#include <windows.h>
#include <stdio.h>
#include <CL/cl.h>
#include "opencl_utils.h"
#include <GL/glut.h>  // GLUT, include glu.h and gl.h
#include "OpenGL_functions.h"

#pragma region OpenGL/glut defines
#define WIDTH 800
#define HEIGHT 600
#define WINDOW_START_POS_X 50
#define WINDOW_START_POS_Y 50
#define WINDOW_NAME "Mandelbrot openGL"
#pragma endregion

#pragma region OpenCL defines
#define MAX_PLATFORMS 2
#define WORKGROUP_THREADS 128
#pragma endregion


int getChosenPlatform(cl_platform_id *platform_id, int maxPlatforms);
void printDeviceNames(cl_device_id device_id, int ret_num_devices);
int getChosenKernel(int maxAmountOfKernels);

void display();

int main(int argc, char** argv) {
#pragma region OpenGL/glut initialization
	glutInit(&argc, argv); // Initialize GLUT
	glutCreateWindow(WINDOW_NAME); // Create a window with the given title
	glutInitWindowSize(WIDTH, HEIGHT); // Set the window's initial width & height
	glutInitWindowPosition(WINDOW_START_POS_X, WINDOW_START_POS_Y); // Position the window's initial top-left corner
	glutDisplayFunc(display); // Register display callback handler for window re-paint  

	init_gl(WIDTH, HEIGHT);
#pragma endregion OpenGL/glut initialization

#pragma region OpenCL initialization
cl_device_id device_id = NULL;
cl_platform_id platform_id[MAX_PLATFORMS];
cl_program program = NULL;
cl_kernel kernel = NULL;
cl_uint ret_num_devices;
cl_mem memobj = NULL;
cl_int ret;
#pragma endregion OpenCL initialization

	glutMainLoop(); // Enter the event-processing loop
	return 0;
}

void display()
{
	glClearColor(1.0f, 0.0f, 0.0f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT);
	glFlush();
}