#pragma once

#include "bubble.h"
#include "imgui_drawing_tools.h"
#include "selectible_image_window.h"
#include "image_gallerey.h"
#include "image_window.h"

#include "file_dialog.h"
#include "image_processing.h"
#include "bread.h"
#include "gen_alg.h"

struct MainWindow : Module
{
	Ref<Texture2D> mImage;
	Ref<SelectibleImageWindow> mSelectibleImageWindow;

	MainWindow()
	{
		mWindowFlags |= ImGuiWindowFlags_NoCollapse;
      gpu::Image image = cpu::RGB2YCbCr(cpu::Image(OpenFileDialog("jpg,png")));
		Texture2D output;

		// Lines outlinening
      Kernel kernel_10{
			{ -1, -1, -1 },
			{  2,  2,  2 },
			{ -1, -1, -1 }
      };
      output = gpu::ApplyKernel(kernel_10, image);
      UI::AddModule<ImageWindow>("Lines 10", std::move(output));

      Kernel kernel_11{
      	{ -1, -1,  2 },
      	{ -1,  2, -1 },
      	{  2, -1, -1 }
      };
      output = gpu::ApplyKernel(kernel_11, image);
		UI::AddModule<ImageWindow>("Lines 11", std::move(output));

		Kernel kernel_12{
      	{ -1, 2, -1 },
      	{ -1, 2, -1 },
      	{ -1, 2, -1 }
      };
      output = gpu::ApplyKernel(kernel_12, image);
		UI::AddModule<ImageWindow>("Lines 12", std::move(output));
		
		Kernel kernel_13{
      	{  2, -1, -1 },
      	{ -1,  2, -1 },
      	{ -1, -1,  2 }
      };
      output = gpu::ApplyKernel(kernel_13, image);
		UI::AddModule<ImageWindow>("Lines 13", std::move(output));



		// Gradient masks
		Kernel kernel_gradient10{
      	{  1,  1,  1 },
      	{  1, -2,  1 },
      	{ -1, -1, -1 }
      };
      output = gpu::ApplyKernel(kernel_gradient10, image);
		UI::AddModule<ImageWindow>("Gradient 10", std::move(output));

		Kernel kernel_gradient11{
      	{  1,  1,  1 },
      	{ -1, -2,  1 },
      	{ -1, -1, -1 }
      };
      output = gpu::ApplyKernel(kernel_gradient11, image);
		UI::AddModule<ImageWindow>("Gradient 11", std::move(output));

		Kernel kernel_gradient12{
      	{ -1,  1,  1 },
      	{ -1, -2,  1 },
      	{ -1,  1,  1 }
      };
      output = gpu::ApplyKernel(kernel_gradient12, image);
		UI::AddModule<ImageWindow>("Gradient 12", std::move(output));

		Kernel kernel_gradient13{
      	{ -1, -1,  1 },
      	{ -1, -2,  1 },
      	{  1,  1,  1 }
      };
      output = gpu::ApplyKernel(kernel_gradient13, image);
		UI::AddModule<ImageWindow>("Gradient 13", std::move(output));

		Kernel kernel_gradient14{
      	{ -1, -1, -1 },
      	{  1, -2,  1 },
      	{  1,  1,  1 }
      };
      output = gpu::ApplyKernel(kernel_gradient14, image);
		UI::AddModule<ImageWindow>("Gradient 14", std::move(output));

		Kernel kernel_gradient15{
      	{ 1, -1, -1 },
      	{ 1, -2, -1 },
      	{ 1,  1,  1 }
      };
      output = gpu::ApplyKernel(kernel_gradient12, image);
		UI::AddModule<ImageWindow>("Gradient 15", std::move(output));

		Kernel kernel_gradient16{
      	{ 1,  1,  -1 },
      	{ 1, -2,  -1 },
      	{ 1,  1,  -1 }
      };
      output = gpu::ApplyKernel(kernel_gradient16, image);
		UI::AddModule<ImageWindow>("Gradient 16", std::move(output));

		Kernel kernel_gradient17{
      	{ 1,  1,  1 },
      	{ 1, -2, -1 },
      	{ 1, -1, -1 }
      };
      output = gpu::ApplyKernel(kernel_gradient17, image);
		UI::AddModule<ImageWindow>("Gradient 17", std::move(output));
		
		// Laplas
		Kernel kernel_laplas10{
      	{  0, -1,  0 },
      	{ -1,  4, -1 },
      	{  0, -1,  0 }
      };
      output = gpu::ApplyKernel(kernel_laplas10, image);
		UI::AddModule<ImageWindow>("Laplas 10", std::move(output));
		
		Kernel kernel_laplas11{
      	{ -1, -1, -1 },
      	{ -1,  8, -1 },
      	{ -1, -1, -1 }
      };
      output = gpu::ApplyKernel(kernel_laplas11, image);
		UI::AddModule<ImageWindow>("Laplas 11", std::move(output));

		Kernel kernel_laplas13{
      	{  1, -2,  1 },
      	{ -2,  4, -2 },
      	{  1, -2,  1 }
      };
      output = gpu::ApplyKernel(kernel_laplas13, image);
		UI::AddModule<ImageWindow>("Laplas 13", std::move(output));

		// Sobel
		Kernel kernel_sobel10{
      	{ -1, 0, 1 },
      	{ -1, 0, 1 },
      	{ -1, 0, 1 }
      };
      output = gpu::ApplyKernel(kernel_sobel10, image);
		UI::AddModule<ImageWindow>("Sobel 10", std::move(output));

		Kernel kernel_sobel11{
      	{ -1, -1, -1 },
      	{  0,  0,  0 },
      	{  1,  1,  1 }
      };
      output = gpu::ApplyKernel(kernel_sobel11, image);
		UI::AddModule<ImageWindow>("Sobel 11", std::move(output));

		Kernel kernel_sobel12{
      	{ -1, 0, 1 },
      	{ -2, 0, 2 },
      	{ -2, 0, 1 }
      };
      output = gpu::ApplyKernel(kernel_sobel12, image);
		UI::AddModule<ImageWindow>("Sobel 12", std::move(output));

		Kernel kernel_sobel13{
      	{ -1, -2, -1 },
      	{  0,  0,  0 },
      	{  1,  2,  1 }
      };
      output = gpu::ApplyKernel(kernel_sobel13, image);
		UI::AddModule<ImageWindow>("Sobel 13", std::move(output));

		Kernel kernel_privet10{
      	{  0,  1,  1 },
      	{ -1,  0,  1 },
      	{ -1, -1,  0 }
      };
      output = gpu::ApplyKernel(kernel_privet10, image);
		UI::AddModule<ImageWindow>("Privet 10", std::move(output));

		Kernel kernel_privet11{
      	{ -1, -1,  0 },
      	{ -1,  0,  1 },
      	{  0,  1,  1 }
      };
      output = gpu::ApplyKernel(kernel_privet11, image);
		UI::AddModule<ImageWindow>("Privet 11", std::move(output));

		Kernel kernel_privet12{
      	{  0,  1,  2 },
      	{ -1,  0,  1 },
      	{ -2, -1,  0 }
      };
      output = gpu::ApplyKernel(kernel_privet12, image);
		UI::AddModule<ImageWindow>("Privet 12", std::move(output));

		Kernel kernel_privet13{
      	{ -2, -1,  0 },
      	{ -1,  0,  1 },
      	{  0,  1,  2 }
      };
      output = gpu::ApplyKernel(kernel_privet13, image);
		UI::AddModule<ImageWindow>("Privet 13", std::move(output));

	}

	void Draw(DeltaTime dt) override
	{
	}

};