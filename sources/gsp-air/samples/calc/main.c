#include "demo_primitive_calc.h"

#include <pthread.h>
#include "test.h"

int main(int argc, char** argv) 
{
  air_publish( "calc", NULL, demo_primitive_calc_provider );

  /*double operand1 = 5;
  double operand2 = 3;
  double* result = air_alloc( NULL, sizeof( double ) );
  
  int ret = primitive_calc_add( "calc", operand1, operand2, result );
  printf( "Result: %d   Add: %f \n",ret, *result );
  
  ret = primitive_calc_sub( "calc", operand1, operand2, result );
  printf( "Result: %d   Sub: %f\n",ret, *result );
  
  ret = primitive_calc_mul( "calc", operand1, operand2, result );
  printf( "Result: %d   Mul: %f\n",ret, *result );
  
  ret = primitive_calc_div( "calc", operand1, operand2, result );
  printf( "Result: %d   Div: %f\n",ret, *result );
  
  air_free( result );*/
  
  run_calc_test();
  
  char ch;
  scanf("Test", &ch);
}





