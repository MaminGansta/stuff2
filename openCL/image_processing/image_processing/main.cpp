
#define FULL_SCREAN_CANVAS
#include "guiAlexandrov/include.h"

#include "openCL/inlucde.h"

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE no, LPSTR args, int cmdShow)
{
	al_init(hInstance);

	Window* window = new Window(L"window", 800, 600, 
		[](HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam, Args arg)->LRESULT 
		{ 
			return (LRESULT)DefWindowProc(hwnd, msg, wParam, lParam);
		});


	// OpenCL
	auto [program, context, device, error] = CreateProgram("fill_rect.cl", VENDOR_AMD);

	if (error)
	{
		output(getErrorString(error));
		return 1;
	}


	// gpu
	cl::Kernel kernel(program, "fill_rect", &error);

	uint32_t work_group_size = kernel.getWorkGroupInfo<CL_KERNEL_WORK_GROUP_SIZE>(device);


	int x = 0, y = 0;
	int width = 100, height = 100;
	Color rect_color(255, 0, 0);
	
	// allocate image buffer on gpu
	cl::Buffer image(context, CL_MEM_WRITE_ONLY | CL_MEM_HOST_READ_ONLY, sizeof(Color) * window->canvas.capacity, NULL, &error);

	// Create command queue
	cl::CommandQueue queue(context, device);


	if (error)
	{
		output(getErrorString(error));
		return 1;
	}

	Timer timer(0.033f);
	while (Window::windows.size())
	{
		Window::default_msg_proc();

		
		//Color black(255, 0, 0);
		//cl::Buffer image_width(context, CL_MEM_READ_ONLY | CL_MEM_HOST_NO_ACCESS | CL_MEM_COPY_HOST_PTR, sizeof(int), &window->canvas.width, &error);
		//cl::Buffer image_height(context, CL_MEM_READ_ONLY | CL_MEM_HOST_NO_ACCESS | CL_MEM_COPY_HOST_PTR, sizeof(int), &window->canvas.height, &error);
		//cl::Buffer color(context, CL_MEM_READ_ONLY | CL_MEM_HOST_NO_ACCESS | CL_MEM_COPY_HOST_PTR, sizeof(Color), &black, &error);
		//
		//// set args
		//error = kernel.setArg(0, image);
		//error = kernel.setArg(1, image_width);
		//error = kernel.setArg(2, image_height);
		//error = kernel.setArg(3, color);
		//
		//// clear screen
		//error = queue.enqueueNDRangeKernel(kernel, cl::NullRange, cl::NDRange(window->canvas.width, window->canvas.height), cl::NDRange(work_group_size, 1));
		//error = queue.enqueueReadBuffer(image, GL_TRUE, 0, sizeof(Color) * window->canvas.whole_size, window->canvas.data);

		
		// image widht and rect color
		cl::Buffer from_x(context, CL_MEM_READ_ONLY | CL_MEM_HOST_NO_ACCESS | CL_MEM_COPY_HOST_PTR, sizeof(int), &x, &error);
		cl::Buffer from_y(context, CL_MEM_READ_ONLY | CL_MEM_HOST_NO_ACCESS | CL_MEM_COPY_HOST_PTR, sizeof(int), &y, &error);

		cl::Buffer image_width(context, CL_MEM_READ_ONLY | CL_MEM_HOST_NO_ACCESS | CL_MEM_COPY_HOST_PTR, sizeof(int), &window->canvas.width, &error);
		cl::Buffer image_height(context, CL_MEM_READ_ONLY | CL_MEM_HOST_NO_ACCESS | CL_MEM_COPY_HOST_PTR, sizeof(int), &window->canvas.height, &error);
		cl::Buffer color(context, CL_MEM_READ_ONLY | CL_MEM_HOST_NO_ACCESS | CL_MEM_COPY_HOST_PTR, sizeof(Color), &rect_color, &error);
		
		// set args
		error = kernel.setArg(0, from_x);
		error = kernel.setArg(1, from_y);
							  
		error = kernel.setArg(2, image);
		error = kernel.setArg(3, image_width);
		error = kernel.setArg(4, image_height);
		error = kernel.setArg(5, color);
		
		// draw red rect
		error = queue.enqueueNDRangeKernel(kernel, cl::NullRange, cl::NDRange(width, height), cl::NDRange(work_group_size, 1));
		error = queue.enqueueReadBuffer(image, GL_TRUE, 0, sizeof(Color) * window->canvas.whole_size, window->canvas.data);

		//for (int i = 0; i < 100; i++)
		//	for (int j = 0; j < 100; j++)
		//		window->canvas.data[i * window->canvas.width + j] = rect_color;


		timer.update();
		window->render_canvas();
	}

	return 0;
}