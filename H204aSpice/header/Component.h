#ifndef COMPONENT_H
#define COMPONENT_H

#include <string>
#include <complex>
#include <vector>

using namespace std;

typedef complex<double> Complex;
typedef vector< vector<Complex> > ComplexVector;

class Component
{
	public:

		Component(string netlistLine);

		static void setFrequency(const double newfrequency);
		static double getFrequency();
		virtual void setTemplate(ComplexVector &nodalAnalysisMatrix) = 0;
		virtual void print();
		virtual ~Component();

		char type;					//First letter of netlist line read

		string netlistLine;			//Element line read from netlist
		string identifier;			//Element name, found after first letter of netlist line

		unsigned int extraNode;		//For components that need an extra node
		static double frequency;	//Omega, used to build nodalAnalysisMatrix
};

#endif // COMPONENT_H
