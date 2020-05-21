

__kernel void fill_rect( __global int* from_x,
						 __global int* from_y,
						 __global uchar4* image_out,
						 __global int* image_w,
						 __global int* image_h,
						 __global uchar4* color)
{
	int x = get_global_id(0);
	int y = get_global_id(1);
	
	image_out[(y + *from_y) * *(image_w) + x + *from_x] = *color;
}