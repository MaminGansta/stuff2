
#include <cstring>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"


int main(int argc, char* argv[])
{

	if (argc < 2) return 0;
	
	char* filename = argv[1];
	char* convert_to;
	char def[] = "jpg";

	if (argc < 3)
		convert_to = def;
	else
		convert_to = argv[2];

	// ============== open image ===================

	// open image
	int width = 0, height = 0;
	int chanels = 0;

	unsigned char* image = stbi_load(filename, &width, &height, &chanels, 0);

	if (image == NULL)
	{
		printf("no such file: %s\n", filename);
		return 1;
	}

	// ============== write image ==================

	char extention[255] = { '\0' };
	
	for (int i = 0; i < 255; i++)
	{
		if (filename[i] == '.') break;
		extention[i] = filename[i];
	}


	int res = 1;
	if (strcmp(convert_to, "png") == 0)
	{
		strcat_s(extention, 255, ".png");
		res = stbi_write_png(extention, width, height, chanels, image, width * sizeof(unsigned) * chanels);
	}
	
	else if (strcmp(convert_to, "bmp") == 0)
	{
		strcat_s(extention, 255, ".bmp");
		res = stbi_write_bmp(extention, width, height, chanels, image);
	}
	
	else if (strcmp(convert_to, "tga") == 0)
	{
		strcat_s(extention, 255, ".tga");
		res = stbi_write_tga(extention, width, height, chanels, image);
	}

	else if (strcmp(convert_to, "jpg") == 0)
	{
		strcat_s(extention, 255, ".jpg");
		res = stbi_write_jpg(extention, width, height, chanels, image, 50);
	}
	else
	{
		printf("no such format: %s\n", convert_to);
		printf("Avaliable only: png, bmp, tga, jpg.\n");
	}

	if (res == 0)
		printf("cant write image\n");


	stbi_image_free(image);
}
