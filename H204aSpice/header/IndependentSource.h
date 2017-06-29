#ifndef INDEPENDENTSOURCE_H
#define INDEPENDENTSOURCE_H

#include <Component.h>

#include <string>
#include <complex>
#include <vector>

class IndependentSource : public Component
{
    public:
        IndependentSource(string netlistLine);
		virtual void setTemplate(ComplexVector &nodalSystem, vector<Complex> &previousSolutionVector) = 0;
		virtual void print();
		virtual ~IndependentSource();

		//Node1 is the positive one
		unsigned int node1, node2;
		double magnitude, phase, valueDC;
};

#endif // INDENPENDENTSOURCE_H
