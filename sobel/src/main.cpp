
#include "kernel/sobel.h"
#include "time/timer.h"


int main()
{
	Image image("resources/baby_yoda_8K.jpg");
	Bubble::Timer timer;

	sobel_sigle_core(image);
	printf("Single core: %fs\n", timer.Update().GetSeconds());

	image = sobel(image);
	printf("Multi core: %fs\n", timer.Update().GetSeconds());

	image.save("baby_yoda");
	return 0;
}