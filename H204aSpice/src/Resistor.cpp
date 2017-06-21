#include "../header/Resistor.h"

Resistor::Resistor(string netlistLine) : SimpleComponent(netlistLine) {
}

void Resistor::setTemplate(ComplexVector &nodalSystem) {

	Complex conductance(1.0/value, 0.0);

	nodalSystem[node1][node1] += conductance;
	nodalSystem[node1][node2] += (-1.0)*conductance;
	nodalSystem[node2][node1] += (-1.0)*conductance;
	nodalSystem[node2][node2] += conductance;
}

Resistor::~Resistor() {
}
