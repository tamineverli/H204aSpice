#ifndef SIMPLECOMPONENT_H
#define SIMPLECOMPONENT_H

#include <Component.h>

#include <string>
#include <complex>
#include <vector>

class SimpleComponent : public Component {

	public:
		SimpleComponent(string netlistLine);
		virtual void setTemplate(ComplexVector &nodalAnalysisMatrix) = 0;
		virtual void print();
		virtual ~SimpleComponent();

		//Node1 is the positive one
		unsigned int node1, node2;
		double value;			//Resistance, inductance, capacitante, etc
};

#endif // SIMPLECOMPONENT_H
