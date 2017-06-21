#ifndef SIMPLECOMPONENT_H
#define SIMPLECOMPONENT_H

#include <Component.h>

#include <string>
#include <complex>
#include <vector>

class SimpleComponent : public Component {

	public:
		SimpleComponent(string netlistLine);
		virtual void setTemplate(ComplexVector &nodalSystem) = 0;
		virtual void print();
		virtual ~SimpleComponent();

		unsigned int node1, node2;
		double value;			//Resistance, inductance, capacitante, etc
};

#endif // SIMPLECOMPONENT_H
