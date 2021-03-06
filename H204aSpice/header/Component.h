#ifndef COMPONENT_H
#define COMPONENT_H

#include <string>
#include <complex>
#include <vector>
#include <iostream>
#include <fstream>

#define CAPACITIVE_ADMITTANCE_DC 1e-9

using namespace std;

typedef complex<double> Complex;
typedef vector< vector<Complex> > ComplexVector;

class Component
{
	public:
		Component(string netlistLine);

		static void setFrequency(const double newfrequency);
		static double getFrequency();
		virtual void setTemplate(ComplexVector &nodalSystem, vector<Complex> &previousSolutionVector) = 0;
		virtual void print();
		virtual ~Component();

		string name;			//Component identification
		string netlistLine;		//Line extracted from netlist
		char type;				//Component type

		unsigned int extraNode;		//For components that need an extra node

		static double frequency;	//Frequency - static (applies for all circuit components)
};

#endif // COMPONENT_H
