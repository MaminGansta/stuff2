
#define CL_USE_DEPRECATED_OPENCL_2_0_APIS
#include "CL/cl.hpp"

#include <cstdio>
#include <fstream>

#include <time.h>

#include "OCLerrors.cpp"
#include "OCLprogram.cpp"


int main(void)
{

	auto [program, context, device, error] = CreateProgram("array_sum.cl", VENDOR_AMD);
	

	printf("compile time %0.4f \n", float(clock()) / CLOCKS_PER_SEC);


	if (error)
	{
		printf(getErrorString(error));
		return 1;
	}


	std::vector<int> data(1024, 1);



	cl::Buffer inBuffer(context, CL_MEM_READ_ONLY | CL_MEM_HOST_NO_ACCESS | CL_MEM_COPY_HOST_PTR, sizeof(int) * data.size(), data.data(), &error);
	cl::Buffer outBuffer(context, CL_MEM_WRITE_ONLY | CL_MEM_HOST_READ_ONLY, sizeof(int), nullptr, &error);


	if (error)
	{
		printf(getErrorString(error));
		return 1;
	}

	cl::Kernel kernel(program, "ArraySum", &error);
	error = kernel.setArg(0, inBuffer);
	error = kernel.setArg(1, outBuffer);

	printf("time %0.4f \n", float(clock()) / CLOCKS_PER_SEC);


	cl::CommandQueue queue(context, device);
	queue.enqueueNDRangeKernel(kernel, cl::NullRange, cl::NDRange(data.size()));

	int res = 0;
	queue.enqueueReadBuffer(outBuffer, GL_FALSE, 0, sizeof(int), &res);

	cl::finish();

	printf("time %0.4f \n", float(clock()) / CLOCKS_PER_SEC);

	return 0;
}