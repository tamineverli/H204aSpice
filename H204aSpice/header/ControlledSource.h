#ifndef CONTROLLEDSOURCE_H
#define CONTROLLEDSOURCE_H

#include <Component.h>

#include <string>
#include <complex>
#include <vector>

class ControlledSource : public Component
{
    public:
        ControlledSource(string netlistLine);
		virtual void setTemplate(ComplexVector &nodalSystem, vector<Complex> &previousSolutionVector) = 0;
		virtual void print();
		virtual ~ControlledSource();

		unsigned int node1, node2, referenceNode1, referenceNode2, extraNode;
		double gain;

};

#endif // CONTROLLEDSOURCE_H
