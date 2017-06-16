#ifndef RESISTOR_H
#define RESISTOR_H

#include <SimpleComponent.h>

#include <string>
#include <complex>
#include <vector>

class Resistor : public SimpleComponent {

	public:
		Resistor(string netlistLine);
		virtual ~Resistor();
		virtual void setTemplate(ComplexVector &nodalAnalysisMatrix);
};

#endif // RESISTOR_H
