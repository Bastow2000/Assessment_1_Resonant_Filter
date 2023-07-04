#include <cmath>
#include "Filter_Blocks.h"

Filter_Block::Filter_Block(){}; //Default Constructor
Filter_Block::~Filter_Block(){}; //Default Deconstructor


//Obtaining seperate Coefficients values per class instance use
void Filter_Block::obtainingCoefficients(float b0,float b1, float a1)
{
	B0_ = b0; //clearing state and obtainingCoefficient for b0
	B1_ = b1; //clearing state and obtainingCoefficient for b1
	A1_ = a1; //clearing state and obtainingCoefficient for a1
}

//Calculating each filter block, returning the output for further use
float Filter_Block::process(float input )  //input = x[n]
{
	//y[n] = B_0 * x[n] + B_1 * x[n-1] - A_1* y[n-1] Differnce Equation
	float output = (B0_*input +B1_ * getLastX1_ - A1_ * getLastY1_); //output = y[n]
		
	//Getting the last state of x[n] and y[n]
	getLastX1_ = input; //getLastX1 = x[n-1]
    getLastY1_ = output; //getLastY1 = y[n-1]
	return output; //returning output for further use
} 