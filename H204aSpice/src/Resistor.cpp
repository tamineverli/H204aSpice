#include "../header/Resistor.h"

Resistor::Resistor(string netlistLine) : SimpleComponent(netlistLine) {
}

void Resistor::setTemplate(ComplexVector &nodalAnalysisMatrix) {

	Complex conductance(1.0/value, 0.0);

	nodalAnalysisMatrix[node1][node1] += conductance;
	nodalAnalysisMatrix[node1][node2] += (-1.0)*conductance;
	nodalAnalysisMatrix[node2][node1] += (-1.0)*conductance;
	nodalAnalysisMatrix[node2][node2] += conductance;
}

Resistor::~Resistor() {
}
