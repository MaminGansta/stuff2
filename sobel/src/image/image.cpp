

#include "image.h"
#include <string.h>

	
int convert_wchar_to_utf8(char* buffer, size_t buffer_len, const wchar_t* output)
{
	return stbi_convert_wchar_to_utf8(buffer, buffer_len, output);
}

uint8_t* load_image(const char* filename, int* x, int* y, int* comp, int req_comp)
{
	return stbi_load(filename, x, y, comp, req_comp);
}

void image_free(void* ptr)
{
	stbi_image_free(ptr);
}


Image_base<uint8_t> BGRA_2_RBGA(const Image_base<uint8_t>& image)
{
	Image_base<uint8_t> temp(image.width, image.height);

	for (int i = 0; i < image.height; i++)
	{
		for (int j = 0; j < image.width; j++)
		{
			const Color& color = image[i * image.width + j];
			temp[i * image.width + j] = Color(color.b, color.g, color.r, color.a);
		}
	}
	return temp;
}



int write_image(const char* filename, const Image_base<uint8_t>& image, ImageType type, int flip, int png_qulity)
{
	stbi_flip_vertically_on_write(flip);

	char extention[255] = { '\0' };
	strcat_s(extention, 255, filename);

	switch (type)
	{
		case ImageType::PNG:
			strcat_s(extention, 255, ".png");
			return stbi_write_png(extention, image.width, image.height, 4, image.data, image.width * sizeof(int));
		case ImageType::BMP:
			strcat_s(extention, 255, ".bmp");
			return stbi_write_bmp(extention, image.width, image.height, 4, image.data);
		case ImageType::TGA:
			strcat_s(extention, 255, ".tga");
			return stbi_write_tga(extention, image.width, image.height, 4, image.data);
		case ImageType::JPG:
			strcat_s(extention, 255, ".jpg");
			return stbi_write_jpg(extention, image.width, image.height, 4, image.data, png_qulity);
	}
	return 0;
}


template struct Image_type<uint8_t>;
template struct Image_type<float>;

