
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

	Timer timer(0.0f);
	while (Window::windows.size())
	{
		// set args
		error = kernel.setArg(0, image);

		error = kernel.setArg(1, x);
		error = kernel.setArg(2, y);
		error = kernel.setArg(3, window->canvas.width);
		error = kernel.setArg(4, window->canvas.height);
		error = kernel.setArg(5, rect_color);
		
		
		// draw red rect
		error = queue.enqueueNDRangeKernel( kernel, cl::NullRange, 
											cl::NDRange(window->canvas.width, window->canvas.height),
											cl::NDRange(16, 16));

		error = queue.enqueueReadBuffer(image, GL_TRUE, 0, sizeof(Color) * window->canvas.whole_size, window->canvas.data);


		output("fps %d\n", timer.FPS);

		timer.update();
		window->render_canvas();
		Window::default_msg_proc();
	}

	return 0;
}