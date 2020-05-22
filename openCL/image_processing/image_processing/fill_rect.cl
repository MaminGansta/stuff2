

__kernel void fill_rect( 
						 //__global uchar4* image_out,
						 __write_only image2d_t image_out,
 						 int from_x,
						 int from_y,
						 int image_w,
						 int image_h,
						 uchar4 color_in)
{
	//uint4 color = (uint4)(color_in[0], color_in[1], color_in[2], color_in[3]);
	uint4 color = (uint4)(255, 0, 0, 255);

	int x = get_global_id(0);
	int y = get_global_id(1);
	
	write_imageui(image_out, (int2)(x + from_x, y + from_y), color);
}