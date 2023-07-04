#pragma once 
#include <vector>
class Filter_Block{
public:
	Filter_Block(); //Default Constructor
	~Filter_Block(); //Default Deconstructor
	void obtainingCoefficients(float b0,float b1,float a1); //obtain coefficients for each filter block
	float process(float input); //calculate the output for each filter block
private:
	float getLastX1_{0.f}, getLastY1_{0.f}; //clearing state
	float B0_{0.f}, B1_{0.f}, A1_{0.f}; //clearing state
};