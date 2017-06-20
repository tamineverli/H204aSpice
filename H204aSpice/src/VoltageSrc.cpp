#include "VoltageSrc.h"

VoltageSrc::VoltageSrc(string netlistLine) : IndependentSource(netlistLine) {
}

void VoltageSrc::setTemplate(ComplexVector &nodalAnalysisMatrix) {

	Complex voltage;

	if (frequency == 0.0)
		voltage = valueDC;
	else
		voltage = polar(magnitude, phase);

	nodalAnalysisMatrix[node1][extraNode] += 1.0;
	nodalAnalysisMatrix[node2][extraNode] += -1.0;
	nodalAnalysisMatrix[extraNode][node1] += -1.0;
	nodalAnalysisMatrix[extraNode][node2] += 1.0;

	nodalAnalysisMatrix[extraNode][nodalAnalysisMatrix.size()] += (-1.0)*voltage;
}

VoltageSrc::~VoltageSrc() {
}
