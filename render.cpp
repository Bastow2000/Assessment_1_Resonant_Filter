// Queen Mary University of London
// ECS7012 - Music and Audio Programming
// Spring 2022
//
// Assignment 1: Synth Filter
// This project contains template code for implementing a resonant filter with
// parameters adjustable in the Bela GUI, and waveform visible in the scope

#include <Bela.h>
#include <libraries/Gui/Gui.h>
#include <libraries/GuiController/GuiController.h>
#include <libraries/Scope/Scope.h>
#include <libraries/math_neon/math_neon.h>
#include <cmath>
#include "Wavetable.h"
#include "Filter_Blocks.h"
//										 Class Calling || Instances 
//------------------------------------------------------------------------------------------------------
// Browser-based GUI to adjust parameters
Gui gGui;
GuiController gGuiController;

// Browser-based oscilloscope to visualise signal
Scope gScope;

// Oscillator objects
Wavetable gSineOscillator, gSawtoothOscillator;

// First order blocks 
Filter_Block block1,block2,block3,block4; 

//------------------------------------------------------------------------------------------------------
//										Filter State Global
float gLastX1 = 0.f; //x[n-1] 
float gLastY1 = 0.f; //y[n-1]
//										Cutoff & Resonance Global
constexpr float gComp = 0.5f; //Given fixed value
float gRes = 0.f;
//										Coefficients Global
float gA1 = 0.f; 
float gB0 = 0.f, gB1 = 0.f;
//										Amplitude Global
int gType=1;
float gA=0.f,gB=0.f,gC=0.f,gD=0.f,gE=0.f;
//------------------------------------------------------------------------------------------------------

void calculate_coefficients(float sampleRate, float frequencyHz, float resonance)
{
	//Given formula for omega_c
	double omega_c = ((2.0f*M_PI) * (frequencyHz / sampleRate)); 
	
	//Given updated formula for gRes
	gRes = resonance*(1.0029f)+(0.0526f*omega_c)-(0.0926f*powf(omega_c,2))+(0.0218f*powf(omega_c,3));
	
	//Given updated formula for g
	float g = (0.9892f*omega_c)-(0.4342f*powf(omega_c,2))+(0.1381f*powf(omega_c,3))-(0.0202f*powf(omega_c,4));
	
	// Calculate coefficients
     gB0 = (g/1.3f); 		//g(x[n]/1.3)
     gB1 = ((0.3f*g)/1.3f);	//g(0.3*x[n]/1.3)
     gA1 = g-1; 			//g(y[n-1])
}


bool setup(BelaContext *context, void *userData)
{
	   

	std::vector<float> wavetable;
	const unsigned int wavetableSize = 1024;
		
	// Populate a buffer with the first 64 harmonics of a sawtooth wave
	wavetable.resize(wavetableSize);
	
	// Generate a sawtooth wavetable (a ramp from -1 to 1)
	for(unsigned int n = 0; n < wavetableSize; n++)
	{
		wavetable[n] = -1.0f + 2.0f * (float)n / (float)(wavetableSize - 1);
	}
	
	// Initialise the sawtooth wavetable, passing the sample rate and the buffer
	gSawtoothOscillator.setup(context->audioSampleRate, wavetable);

	// Recalculate the wavetable for a sine
	for(unsigned int n = 0; n < wavetableSize; n++)
	{
		wavetable[n] = sin(2.0f * M_PI * (float)n / (float)wavetableSize);
	}	
	
	// Initialise the sine oscillator
	gSineOscillator.setup(context->audioSampleRate, wavetable);
	// Set up the sine Oscillator

	gGui.setup(context->projectName);
	gGuiController.setup(&gGui, "Oscillator and Filter Controls");	
	
	// Arguments: name, default value, minimum, maximum, increment
	// Create sliders for oscillator and filter settings
	gGuiController.addSlider("Oscillator Frequency", 440, 40, 8000, 1);
	gGuiController.addSlider("Oscillator Amplitude", 0.5f, 0.f, 2.0f, 0.f);
	gGuiController.addSlider("Filter Cutoff", 1000, 100, 5000, 1);
	gGuiController.addSlider("resonance",0.2f, 0.f ,1.1f, 0.1f);
		gGuiController.addSlider("Filter Type",1, 1 ,6, 1);
	

	// Set up the scope
	gScope.setup(2, context->audioSampleRate);

	return true;
}

void render(BelaContext *context, void *userData)
{
	// Read the slider values
	float oscFrequency = gGuiController.getSliderValue(0);	
	float oscAmplitude = gGuiController.getSliderValue(1);	
	float cutoffFrequency = gGuiController.getSliderValue(2);
	float resonance = gGuiController.getSliderValue(3);
	float filter_type = gGuiController.getSliderValue(4);
	int type = filter_type * gType; // Allowing the GuiController to scroll back and forth through the switch implementation
//	float amplitude = powf(10.0f, oscAmplitude / 20);
	
	// Set the oscillator frequency
	gSineOscillator.setFrequency(oscFrequency);
	gSawtoothOscillator.setFrequency(oscFrequency);

	// Calculate new filter coefficients
	calculate_coefficients(context->audioSampleRate, cutoffFrequency,resonance);
	
	block1.obtainingCoefficients(gB0,gB1,gA1); //obtainingCoefficient for block1
	block2.obtainingCoefficients(gB0,gB1,gA1); //obtainingCoefficient for block2
	block3.obtainingCoefficients(gB0,gB1,gA1); //obtainingCoefficient for block3
	block4.obtainingCoefficients(gB0,gB1,gA1); //obtainingCoefficient for block4
	
	          	 switch (type) // this switch allows you to change between filters
    {
        case 1:
           if(type== 1) 
             {
            gA=0.f,gB=0.f,gC=1.f,gD=0.f,gE=0.f;//Two pole Low-pass
           }
           else 
           {
           	gA=0.f,gB=0.f,gC=0.f,gD=0.f,gE=0.f; //Clearing its state
           }
           break;
        case 2:    
         if(type ==2) 
          {
           	gA=0.f,gB=0.f,gC=0.f,gD=0.f,gE=1.f;//Four pole Low-pass
           }
           else 
           {
           	gA=0.f,gB=0.f,gC=0.f,gD=0.f,gE=0.f; //Clearing its state
           }
            break;
        case 3:
         if(type == 3) 
          {
            gA=0.f,gB=2.f,gC=-2.f,gD=0.f,gE=0.f;  //Two pole Band-pass
           }
           else 
           {
           	gA=0.f,gB=0.f,gC=0.f,gD=0.f,gE=0.f; //Clearing its state
           }
            break;
        case 4:
         if(type == 4) 
           {
            gA=0.f,gB=0.f,gC=4.f,gD=-8.f,gE=4.f; //Four pole Band-pass
           }
           else 
           {
           	gA=0.f,gB=0.f,gC=0.f,gD=0.f,gE=0.f; //Clearing its state
           }
            break;
        case 5:
         if(type == 5) 
           {
             gA=1.f,gB=-2.f,gC=1.f,gD=0.f,gE=0.f; //Two pole High-pass-pass
           }
           else 
           {
           	gA=0.f,gB=0.f,gC=0.f,gD=0.f,gE=0.f; //Clearing its state
           } 
           break;
        case 6:
         if(type == 6) 
           {
           gA=1.f,gB=-4.f,gC= 6.f,gD=-4.f,gE=1.f;	//Four pole High-pass
           }
           else 
           {
           	gA=0.f,gB=0.f,gC=0.f,gD=0.f,gE=0.f; //Clearing its state
           }
             break;
    }
    for(unsigned int n = 0; n < context->audioFrames; n++) 
    {
    	// Uncomment one line or the other to choose sine or sawtooth oscillator
    	// (or, if you like, add a GUI or hardware control to switch on the fly)
		//	float in = oscAmplitude * gSineOscillator.process();
		float input = oscAmplitude * gSawtoothOscillator.process();
		//float input = amplitude * in;
            
        //(x[n]-4gRes*y[n-1]-x[n]*gComp) Equation for feedback
 		float feedbackInput = (input-(4*gRes*(gLastY1)-(input*gComp)));
 		
		//tanf_neon() is the nonlinearity function
		//for block1 x[n] is the nonlinearity of the feedbackInput
		float output1 = block1.process(tanf_neon(feedbackInput));
		float output2 = block2.process(output1); //for block2 x[n] is the output of block1
		float output3 = block3.process(output2); //for block3 x[n] is the output of block2
		float output4  = block4.process(output3); //for block4 x[n] is the output of block3
		float output = (gA*tanf_neon(feedbackInput))+(gB*output2)+(gC*output3)+(gD*output4)+(gE*output4);
		
        //gLastX1 = in; not used here
        gLastY1 = output; // x[n-1]
        
        // Write the output to every audio channel
    	for(unsigned int channel = 0; channel < context->audioOutChannels; channel++) {
    		audioWrite(context, n, channel, output);
    	}
    	
    	gScope.log(input, output);
    }
}

void cleanup(BelaContext *context, void *userData)
{

}