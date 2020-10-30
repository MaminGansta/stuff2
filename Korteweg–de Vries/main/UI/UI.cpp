
#include "UI.h"


namespace Bubble
{
	UI::UI(SDL_WINDOW* window)
		: mImGuiControll(window)
	{
		mImGuiControll.OnAttach();
	}

	UI::~UI()
	{
		mImGuiControll.OnDetach();
	}

	void UI::Draw(DeltaTime dt)
	{
		mImGuiControll.Begin();

		mImGuiControll.BeginMenuBar();
		DrawMenuBar();
		mImGuiControll.EndMenuBar();

		ImGui::Begin("Draw graphic");

		static auto F = [](float x, float t, float k, float x0)
		{
			return (2 * k * k) / pow(coshf(k * (x - 4 * k * k * t - x0)), 2);
		};

		static float from = 0.0f, to = 100.0f;
		static float step = 0.1f;
		static float k = 0.5f;
		static float x0 = 1.0f;
		static float t = 0;
		static std::vector<float> values;

		values.resize((to - from) / step);
		t += dt.GetSeconds();

		for (int i = 0; i < (to - from) / step; i++)
		{
			values[i] = F(from + step * i, t, k, x0);
		}

		ImGui::PlotLines("Graph", values.data(), values.size(), 0, NULL, 0.0f, 1.0f, ImVec2(0, 400.0f));

		// Controls
		if (ImGui::Button("restart", { 100, 50 })) { t = 0; }
		ImGui::SliderFloat("k", &k, 0.01f, 1.0f);
		ImGui::SliderFloat("x0", &x0, 0.1f, 100.0f);

		ImGui::End();


		// DemoWindow
		//ImGui::ShowDemoWindow();

		mImGuiControll.End();
	}


	void UI::DrawMenuBar()
	{

	}

}
