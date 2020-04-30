//http://www.flipcode.com/archives/Fast_Trigonometry_Functions_Using_Lookup_Tables.shtml
#include <stdio.h>
#include <assert.h>
#include <math.h>
#include <conio.h>
#include <time.h>

#define MAX_CIRCLE_ANGLE      512
#define HALF_MAX_CIRCLE_ANGLE (MAX_CIRCLE_ANGLE/2)
#define QUARTER_MAX_CIRCLE_ANGLE (MAX_CIRCLE_ANGLE/4)
#define MASK_MAX_CIRCLE_ANGLE (MAX_CIRCLE_ANGLE - 1)
#define PI 3.14159265358979323846f

float fast_cossin_table[MAX_CIRCLE_ANGLE];           // Declare table of fast cosinus and sinus

// Copied from NVidia web site
inline void FloatToInt(int *int_pointer, float f) 
{
  __asm  fld  f
  __asm  mov  edx,int_pointer
  __asm  FRNDINT
  __asm  fistp dword ptr [edx];

}

inline float fastcos(float n)
{
   float f = n * HALF_MAX_CIRCLE_ANGLE / PI;
   int i;
   FloatToInt(&i, f);
   if (i < 0)
   {
      return fast_cossin_table[((-i) + QUARTER_MAX_CIRCLE_ANGLE)&MASK_MAX_CIRCLE_ANGLE];
   }
   else
   {
      return fast_cossin_table[(i + QUARTER_MAX_CIRCLE_ANGLE)&MASK_MAX_CIRCLE_ANGLE];
   }

   assert(0);
}

inline float fastsin(float n)
{
   float f = n * HALF_MAX_CIRCLE_ANGLE / PI;
   int i;
   FloatToInt(&i, f);
   if (i < 0)
   {
      return fast_cossin_table[(-((-i)&MASK_MAX_CIRCLE_ANGLE)) + MAX_CIRCLE_ANGLE];
   }
   else
   {
      return fast_cossin_table[i&MASK_MAX_CIRCLE_ANGLE];
   }

   assert(0);
}

const long iMaxTests = 10000000;

void buld_lookup_tables()
{

   // Build cossin table
   for (int i = 0 ; i < MAX_CIRCLE_ANGLE; i++)
   {
      fast_cossin_table[i] = (float)sin((double)i * PI / HALF_MAX_CIRCLE_ANGLE);
   }
}