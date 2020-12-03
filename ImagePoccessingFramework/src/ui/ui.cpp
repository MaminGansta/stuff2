
#include "ui.h"


namespace Bubble
{
	UI::UI(Window* window)
		: mImGuiControll(window)
	{
		mImGuiControll.OnAttach();

		mShader = ShaderLoader::Load("resources/shaders/shader.glsl");
		mImage = CreateRef<Texture2D>("resources/images/nature.jpg");

		float quadVertices[] = {
			// positions        // texture Coords
			-1.0f,  1.0f, 0.0f, 0.0f, 1.0f,
			-1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
			 1.0f,  1.0f, 0.0f, 1.0f, 1.0f,
			 1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
		};

		VertexBuffer vb(quadVertices, 4);
		VertexArray va;

		BufferLayout layout
		{
			{ GLSLDataType::Float3, "Position" },
			{ GLSLDataType::Float2, "TexCoords" }
		};
		vb.SetLayout(layout);
		va.AddVertexBuffer(std::move(vb));
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
		
		ImGui::Begin("Test");
		//ImGui::Button("Button", { 100, 50 });
		ImGui::Image((ImTextureID)mImage->mRendererID, { 500, 500 });
		ImGui::End();

		mImGuiControll.End();
	}


	void UI::DrawMenuBar()
	{

	}


	void gaussian_blur_func(Framebuffer& framebuffer, Framebuffer& buffer, Shader& gauss, int FullScreenQuadVAO)
	{
		glDisable(GL_DEPTH_TEST);

		// horizontal
		buffer.Bind();
		//glActiveTexture(GL_TEXTURE0);
		//glBindTexture(GL_TEXTURE_2D, framebuffer.m_ColorAttachment);
		//gauss.SetTexture2D("image", buffer.mColorAttachment);
		//gauss.SetUni1i("horizontal", 0);
		//
		//glBindVertexArray(FullScreenQuadVAO);
		//glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
		//glBindVertexArray(0);
		//
		//// vertical
		//framebuffer.Bind();
		//glActiveTexture(GL_TEXTURE0);
		//glBindTexture(GL_TEXTURE_2D, buffer.m_ColorAttachment);
		//gauss.setUni1i("image", 0);
		//gauss.setUni1i("horizontal", 1);

		glBindVertexArray(FullScreenQuadVAO);
		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
		glBindVertexArray(0);

		glEnable(GL_DEPTH_TEST);
	}


}
