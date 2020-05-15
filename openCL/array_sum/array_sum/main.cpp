
#define CL_USE_DEPRECATED_OPENCL_2_0_APIS
#include "CL/cl.hpp"

#include <cstdio>
#include <fstream>

#include "OCLerrors.cpp"
#include "OCLprogram.cpp"


int main(void)
{

	auto [program, context, device, error] = CreateProgram("HelloWorld.cl", VENDOR_AMD);
	
	if (error)
	{
		printf(getErrorString(error));
		return 1;
	}


	char buffer[16];
	cl::Buffer memBuf(context, CL_MEM_WRITE_ONLY | CL_MEM_HOST_READ_ONLY, sizeof(buffer));
	cl::Kernel kernel(program, "HelloWorld", &error);
	kernel.setArg(0, memBuf);

	cl::CommandQueue queue(context, device);
	queue.enqueueTask(kernel);
	queue.enqueueReadBuffer(memBuf, GL_TRUE, 0, sizeof(buffer), buffer);


	printf(buffer);

	return 0;
}