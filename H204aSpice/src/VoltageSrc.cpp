#include "VoltageSrc.h"

VoltageSrc::VoltageSrc(string netlistLine) : IndependentSource(netlistLine) {
}

void VoltageSrc::setTemplate(ComplexVector &nodalSystem, vector<Complex> &previousSolutionVector) {

	Complex voltage;

	if (frequency == 0.0)
		voltage = valueDC;
	else
		voltage = polar(magnitude, phase);

	nodalSystem[node1][extraNode] += 1.0;
	nodalSystem[node2][extraNode] += -1.0;
	nodalSystem[extraNode][node1] += -1.0;
	nodalSystem[extraNode][node2] += 1.0;

	nodalSystem[extraNode][nodalSystem.size()] += (-1.0)*voltage;
}

VoltageSrc::~VoltageSrc() {
}
