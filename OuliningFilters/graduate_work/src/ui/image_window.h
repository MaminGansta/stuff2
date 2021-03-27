#pragma once

#include "bubble.h"

struct ImageWindow : Module
{
	std::string mName;
	Ref<Texture2D> mImage;

	ImageWindow(const Ref<Texture2D>& image)
		: mImage(image)
	{
		mName = "Image + " + std::to_string(mImage->mRendererID);
	}

	ImageWindow(Texture2D&& image)
		: mImage(CreateRef<Texture2D>(std::move(image)))
	{
		mName = "Image + " + std::to_string(mImage->mRendererID);
	}

	ImageWindow(const std::string& name, Texture2D&& image)
		: mName(name),
		  mImage(CreateRef<Texture2D>(std::move(image)))
	{}

	void Draw(DeltaTime dt)
	{
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{ 0, 0 });
		ImGui::Begin(mName.c_str(), &mIsOpen);
		{
			ImVec2 size = ImGui::GetContentRegionAvail();
			ImGui::Image((void*)mImage->mRendererID, size);
		}
		ImGui::End();
		ImGui::PopStyleVar();
	}

};