
#define CL_USE_DEPRECATED_OPENCL_2_0_APIS
#include <CL/cl.hpp>

#include <cstdio>
#include <fstream>

int main(void)
{

	std::vector<cl::Platform> platforms;

	cl::Platform::get(&platforms);

	_ASSERT(platforms.size() > 0);

	auto platform = platforms.front();

	std::vector<cl::Device> devices;
	platform.getDevices(CL_DEVICE_TYPE_GPU, &devices);

	_ASSERT(devices.size() > 0);

	auto device = devices.front();
	auto vendor = device.getInfo<CL_DEVICE_VENDOR>();
	auto version = device.getInfo<CL_DEVICE_VERSION>();


	std::ifstream helloWorldFile("HelloWorld.cl");
	if (!helloWorldFile.is_open()) return 1;


	std::string src (std::istreambuf_iterator<char>(helloWorldFile), (std::istreambuf_iterator<char>()) );


	cl::Program::Sources sources(1, std::make_pair(src.c_str(), src.length() + 1));

	cl::Context context(device);
	cl::Program program(context, sources);

	auto error = program.build("-cl-std=CL1.2");

	char buffer[16];
	cl::Buffer memBuf(context, CL_MEM_WRITE_ONLY | CL_MEM_HOST_READ_ONLY, sizeof(buffer));
	cl::Kernel kernel(program, "HelloWorld", &error);
	kernel.setArg(0, memBuf);

	cl::CommandQueue queue(context, device);
	queue.enqueueTask(kernel);
	queue.enqueueReadBuffer(memBuf, GL_TRUE, 0, sizeof(buffer), buffer);

	printf_s(buffer);

	return 0;
}