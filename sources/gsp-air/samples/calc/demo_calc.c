#include <demo_calc.h>

static int demo_calc_add( char* endpoint, void* data, double operand1, double operand2, double* result )
{

  *result = operand1 + operand2;

  return ( AIR_ERROR_OK );
}

static int demo_calc_sub( char* endpoint, void* data, double operand1, double operand2, double* result )
{

  *result = operand1 - operand2;

  return ( AIR_ERROR_OK );
}

static int demo_calc_mul( char* endpoint, void* data, double operand1, double operand2, double* result )
{

  *result = operand1 * operand2;

  return ( AIR_ERROR_OK );
}

static int demo_calc_div( char* endpoint, void* data, double operand1, double operand2, double* result )
{

  *result = operand1 / operand2;

  return ( AIR_ERROR_OK );
}

static int demo_calc_add_complex( char* endpoint, void* data, calc_complex operand1, calc_complex operand2, calc_complex* result )
{
  result->real = operand1.real + operand2.real;
  result->imaginary = operand1.imaginary + operand2.imaginary;
  return ( AIR_ERROR_OK );
}

static int demo_calc_sub_complex( char* endpoint, void* data, calc_complex operand1, calc_complex operand2, calc_complex* result )
{
  result->real = operand1.real - operand2.real;
  result->imaginary = operand1.imaginary - operand2.imaginary;
  return ( AIR_ERROR_OK );
}

static int demo_calc_mul_complex( char* endpoint, void* data, calc_complex operand1, calc_complex operand2, calc_complex* result )
{
  result->real = ( operand1.real * operand2.real ) - ( operand1.imaginary * operand2.imaginary );
  result->imaginary = ( operand1.real * operand2.imaginary ) + ( operand2.real * operand1.imaginary );
  return ( AIR_ERROR_OK );
}

static int demo_calc_div_complex( char* endpoint, void* data, calc_complex operand1, calc_complex operand2, calc_complex* result )
{
  float denom = ( operand2.real * operand2.real ) + ( operand2.imaginary * operand2.imaginary );
  result->real = ( ( operand1.real * operand2.real ) + ( operand1.imaginary * operand2.imaginary ) ) / denom;
  result->imaginary = ( ( operand1.real * ( -operand2.imaginary ) ) + ( operand2.real * operand1.imaginary ) ) / denom;
  return ( AIR_ERROR_OK );
}

const void* demo_calc_provider[] = {
  calc_rpc, 
  demo_calc_add, 
  demo_calc_sub, 
  demo_calc_mul, 
  demo_calc_div, 
  demo_calc_add_complex, 
  demo_calc_sub_complex, 
  demo_calc_mul_complex, 
  demo_calc_div_complex, 
};
