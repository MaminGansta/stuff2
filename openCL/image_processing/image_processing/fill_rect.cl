

__kernel void fill_rect( __global uchar4* image_out,
 						 int from_x,
						 int from_y,
						 int image_w,
						 int image_h,
						 uchar4 color)
{
	int x = get_global_id(0);
	int y = get_global_id(1);
	
	image_out[(y + from_y) * image_w + x + from_x] = color;
}