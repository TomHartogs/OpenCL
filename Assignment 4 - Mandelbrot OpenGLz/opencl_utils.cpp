#define _CRT_SECURE_NO_WARNINGS
#define _SCL_SECURE_NO_WARNINGS

#include <CL/opencl.h>
#include <stdio.h>
#include <algorithm>
#include <stdarg.h>
#include "opencl_utils.h"

//#define checkError(status, ...) _checkError(__LINE__, __FILE__, status, __VA_ARGS__)


///////////////////////////////
// Error functions
///////////////////////////////

// Print the error associciated with an error code
void printError(cl_int error) {
	// Print error message
	switch (error)
	{
	case -1:
		printf("CL_DEVICE_NOT_FOUND ");
		break;
	case -2:
		printf("CL_DEVICE_NOT_AVAILABLE ");
		break;
	case -3:
		printf("CL_COMPILER_NOT_AVAILABLE ");
		break;
	case -4:
		printf("CL_MEM_OBJECT_ALLOCATION_FAILURE ");
		break;
	case -5:
		printf("CL_OUT_OF_RESOURCES ");
		break;
	case -6:
		printf("CL_OUT_OF_HOST_MEMORY ");
		break;
	case -7:
		printf("CL_PROFILING_INFO_NOT_AVAILABLE ");
		break;
	case -8:
		printf("CL_MEM_COPY_OVERLAP ");
		break;
	case -9:
		printf("CL_IMAGE_FORMAT_MISMATCH ");
		break;
	case -10:
		printf("CL_IMAGE_FORMAT_NOT_SUPPORTED ");
		break;
	case -11:
		printf("CL_BUILD_PROGRAM_FAILURE ");
		break;
	case -12:
		printf("CL_MAP_FAILURE ");
		break;
	case -13:
		printf("CL_MISALIGNED_SUB_BUFFER_OFFSET ");
		break;
	case -14:
		printf("CL_EXEC_STATUS_ERROR_FOR_EVENTS_IN_WAIT_LIST ");
		break;

	case -30:
		printf("CL_INVALID_VALUE ");
		break;
	case -31:
		printf("CL_INVALID_DEVICE_TYPE ");
		break;
	case -32:
		printf("CL_INVALID_PLATFORM ");
		break;
	case -33:
		printf("CL_INVALID_DEVICE ");
		break;
	case -34:
		printf("CL_INVALID_CONTEXT ");
		break;
	case -35:
		printf("CL_INVALID_QUEUE_PROPERTIES ");
		break;
	case -36:
		printf("CL_INVALID_COMMAND_QUEUE ");
		break;
	case -37:
		printf("CL_INVALID_HOST_PTR ");
		break;
	case -38:
		printf("CL_INVALID_MEM_OBJECT ");
		break;
	case -39:
		printf("CL_INVALID_IMAGE_FORMAT_DESCRIPTOR ");
		break;
	case -40:
		printf("CL_INVALID_IMAGE_SIZE ");
		break;
	case -41:
		printf("CL_INVALID_SAMPLER ");
		break;
	case -42:
		printf("CL_INVALID_BINARY ");
		break;
	case -43:
		printf("CL_INVALID_BUILD_OPTIONS ");
		break;
	case -44:
		printf("CL_INVALID_PROGRAM ");
		break;
	case -45:
		printf("CL_INVALID_PROGRAM_EXECUTABLE ");
		break;
	case -46:
		printf("CL_INVALID_KERNEL_NAME ");
		break;
	case -47:
		printf("CL_INVALID_KERNEL_DEFINITION ");
		break;
	case -48:
		printf("CL_INVALID_KERNEL ");
		break;
	case -49:
		printf("CL_INVALID_ARG_INDEX ");
		break;
	case -50:
		printf("CL_INVALID_ARG_VALUE ");
		break;
	case -51:
		printf("CL_INVALID_ARG_SIZE ");
		break;
	case -52:
		printf("CL_INVALID_KERNEL_ARGS ");
		break;
	case -53:
		printf("CL_INVALID_WORK_DIMENSION ");
		break;
	case -54:
		printf("CL_INVALID_WORK_GROUP_SIZE ");
		break;
	case -55:
		printf("CL_INVALID_WORK_ITEM_SIZE ");
		break;
	case -56:
		printf("CL_INVALID_GLOBAL_OFFSET ");
		break;
	case -57:
		printf("CL_INVALID_EVENT_WAIT_LIST ");
		break;
	case -58:
		printf("CL_INVALID_EVENT ");
		break;
	case -59:
		printf("CL_INVALID_OPERATION ");
		break;
	case -60:
		printf("CL_INVALID_GL_OBJECT ");
		break;
	case -61:
		printf("CL_INVALID_BUFFER_SIZE ");
		break;
	case -62:
		printf("CL_INVALID_MIP_LEVEL ");
		break;
	case -63:
		printf("CL_INVALID_GLOBAL_WORK_SIZE ");
		break;
	default:
		printf("UNRECOGNIZED ERROR CODE (%d)", error);
	}
}

// Print line, file name, and error code if there is an error. Exits the
// application upon error.
void _checkError(int line,
	const char *file,
	cl_int error,
	const char *msg,
	...) {
	// If not successful
	if (error != CL_SUCCESS) {
		// Print line and file
		printf("ERROR: ");
		printError(error);
		printf("\nLocation: %s:%d\n", file, line);

		// Print custom message.
		va_list vl;
		va_start(vl, msg);
		vprintf(msg, vl);
		printf("\n");
		va_end(vl);
		getchar();

		// Cleanup and bail.
		exit(error);
	}
}

// Create program from a file and compile it
cl_program build_program(cl_context context, cl_device_id device, const char* filename) {

	cl_program program;
	FILE *program_handle;
	char *program_buffer, *program_log;
	size_t program_size, log_size;
	cl_int err;

	// Read program file and place content into buffer
	program_handle = fopen(filename, "rb");
	if (program_handle == NULL) {
		perror("Couldn't find the program file");
		getchar(); exit(1);
	}
	fseek(program_handle, 0, SEEK_END);
	program_size = ftell(program_handle);
	rewind(program_handle);
	program_buffer = (char*)malloc(program_size + 1);
	program_buffer[program_size] = '\0';
	fread(program_buffer, sizeof(char), program_size, program_handle);
	fclose(program_handle);

	// Create program from file
	program = clCreateProgramWithSource(context, 1, (const char**)&program_buffer, &program_size, &err);
	checkError(err, "Couldn't create the program");
	free(program_buffer);

	// Build program
	err = clBuildProgram(program, 0, NULL, NULL, NULL, NULL);
	if (err < 0) {

		// Find size of log and print to std output
		clGetProgramBuildInfo(program, device, CL_PROGRAM_BUILD_LOG,
			0, NULL, &log_size);
		program_log = (char*)malloc(log_size + 1);
		program_log[log_size] = '\0';
		clGetProgramBuildInfo(program, device, CL_PROGRAM_BUILD_LOG,
			log_size + 1, program_log, NULL);
		printf("%s\n", program_log);
		free(program_log);
		getchar(); exit(1);
	}

	return program;
}

int getChosenPlatform(cl_platform_id *platform_id, int maxPlatforms)
{
	cl_uint ret_num_platforms;
	cl_int ret;
	char* info;
	size_t infoSize;
	const cl_platform_info attributeTypes[1] =
	{ CL_PLATFORM_NAME };

	ret = clGetPlatformIDs(maxPlatforms, platform_id, &ret_num_platforms);
	checkError(ret, "Could not get platform ID's");

	for (int i = 0; i < ret_num_platforms; i++)
	{
		ret = clGetPlatformInfo(platform_id[i], attributeTypes[0], 0, NULL, &infoSize);
		checkError(ret, "Couldn't get platform attribute value size");

		info = (char*)malloc(infoSize);

		// get platform attribute value
		ret = clGetPlatformInfo(platform_id[i], attributeTypes[0], infoSize, info, NULL);
		checkError(ret, "Couldn't get platform attribute value");

		printf("[%i]%s\n", i, info);
		free(info);
	}
	int chosenPlatform = 0;
	char input[2];
	scanf("")
	int ch = getchar();

	switch (ch)
	{
		//input a number
	case '1':
	case '2':
	case '3':
	case '4':
	case '5':
	case '6':
	case '7':
	case '8':
	case '9':
	{
		ch -= '0';
		if (ch < ret_num_platforms)
			chosenPlatform = ch;
	}
	}
	return chosenPlatform;
}
int getChosenKernel(int amountOfKernels)
{
	printf("Enter kernel number: \n");
	int chosenKernel = 0;
	int ch = getchar();

	switch (ch)
	{
		//input a number
	case '1':
	case '2':
	case '3':
	case '4':
	case '5':
	case '6':
	case '7':
	case '8':
	case '9':
	{
		ch -= '0';
		if (ch <= amountOfKernels)
			chosenKernel = ch;
	}
	}
	return chosenKernel;
}

void printDeviceNames(cl_device_id deviceId, int numDevices)
{
	cl_int ret;
	char* info;
	size_t infoSize;

	printf("Available devices:\n");

	for (int i = 0; i < numDevices; i++)
	{
		const cl_device_info attributeTypes[1] =
		{ CL_DEVICE_NAME };

		ret = clGetDeviceInfo(deviceId, attributeTypes[0], 0, NULL, &infoSize);
		checkError(ret, "Couldn't get platform attribute value size");

		info = (char*)malloc(infoSize);

		// get platform attribute value
		ret = clGetDeviceInfo(deviceId, attributeTypes[0], infoSize, info, NULL);
		checkError(ret, "Couldn't get platform attribute value");

		printf("[%i]%s\n", i, info);
		free(info);
	}
}
