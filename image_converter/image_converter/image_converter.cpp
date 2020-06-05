
#include <cstring>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"


void print_help()
{
	printf("========================== Image converter ===========================\n\n"
		"1) To convert image pass image name in args (full or relative path)\n"
		"2) Image will be converted to jpg by default to set output type\n"
		"   add flag -t with (png, bmp, tga, jpg). Like this -t png\n"
		"3) You can pass more then 1 image <separated by space>(name1.png name2.tga)\n"
		"4) Drop your image/images to .exe and it/they will converted to jpg\n"
		"   output file/files will be in the same dirrectory\n\n"

		"Usage in cmd: imgConv.exe  image.png\n"
		"Usage in cmd: imgConv.exe  -t tga image.jpg\n"
		"Widnows cmd provide drag and drope image name\n");
}


int main(int argc, char* argv[])
{
	char* filename;
	char def[] = "jpg";
	char* convert_to = def;

	// errors counter
	int errors = 0;

	// if no args
	if (argc < 2)
	{
		print_help();
		getchar();
		return 1;
	}


	// proccesss flags
	for (int i = 1; i < argc; i++)
	{
		if (strcmp(argv[i], "-t") == 0)
		{
			i++;
			convert_to = argv[i];
		}
		else if (strcmp(argv[i], "-help") == 0)
		{
			print_help();
			return 0;
		}
	}


	for (int i = 1; i < argc; i++)
	{
		filename = argv[i];

		// Skip flags
		if (strcmp(filename, "-t") == 0)
		{
			i++;
			continue;
		}
		if (strcmp(filename, "-help") == 0)
		{
			continue;
		}

		// ============== open image ===================

		// open image
		int width = 0, height = 0;
		int chanels = 0;

		unsigned char* image = stbi_load(filename, &width, &height, &chanels, 0);

		if (image == NULL)
		{
			errors++;
			printf("Error: Can not open image: %s\n", filename);
			continue;
		}

		// ============== write image ==================

		char extention[255] = { '\0' };
		for (int i = 0; i < 255; i++)
		{
			if (filename[i] == '.') break;
			extention[i] = filename[i];
		}

		int res = 0;
		if (strcmp(convert_to, "png") == 0)
		{
			strcat_s(extention, 255, ".png");
			res = stbi_write_png(extention, width, height, chanels, image, width * chanels);
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
			errors++;
			printf("Error: Illegal format: %s\n", convert_to);
			printf("Avaliable only: png, bmp, tga, jpg\n");

			stbi_image_free(image);
			getchar();
			return 1;
		}

		if (res == 0)
		{
			errors++;
			printf("Error: Can not write image: %s\n", filename);
		}

		stbi_image_free(image);
	}

	// if all ok we dont have any log
	if (errors)
		getchar();

	return 0;
}
