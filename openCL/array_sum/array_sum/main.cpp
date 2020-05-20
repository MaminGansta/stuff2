
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


	if (error)
	{
		printf(getErrorString(error));
		return 1;
	}

	cl::Kernel kernel(program, "ArraySum", &error);

	uint32_t work_group_size = kernel.getWorkGroupInfo<CL_KERNEL_WORK_GROUP_SIZE>(device);
	uint32_t num_work_groups = data.size() / work_group_size;


	cl::Buffer values_per_item(context, CL_MEM_READ_ONLY | CL_MEM_HOST_NO_ACCESS | CL_MEM_COPY_HOST_PTR, sizeof(int), &num_work_groups, &error);
	cl::Buffer inBuffer(context, CL_MEM_READ_ONLY | CL_MEM_HOST_NO_ACCESS | CL_MEM_COPY_HOST_PTR, sizeof(int) * data.size(), data.data(), &error);
	cl::Buffer outBuffer(context, CL_MEM_WRITE_ONLY | CL_MEM_HOST_READ_ONLY, sizeof(int), nullptr, &error);


	error = kernel.setArg(0, values_per_item);
	error = kernel.setArg(1, inBuffer);
	error = kernel.setArg(2, outBuffer);



	printf("time %0.4f \n", float(clock()) / CLOCKS_PER_SEC);


	cl::CommandQueue queue(context, device);
	error = queue.enqueueNDRangeKernel(kernel, cl::NullRange, cl::NDRange(work_group_size), cl::NDRange(num_work_groups));

	int res = 0;
	error = queue.enqueueReadBuffer(outBuffer, GL_FALSE, 0, sizeof(int), &res);

	cl::finish();

	printf("time %0.4f \n", float(clock()) / CLOCKS_PER_SEC);

	return 0;
}