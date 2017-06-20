#include "CurrentSrc.h"

CurrentSrc::CurrentSrc(string netlistLine) : IndependentSource(netlistLine) {
}

void CurrentSrc::setTemplate(ComplexVector &nodalAnalysisMatrix) {

	Complex current;

	if (frequency == 0.0)
		current = valueDC;
	else
		current = polar(magnitude, phase);

	nodalAnalysisMatrix[node1][nodalAnalysisMatrix.size()] += (-1.0)*current;
	nodalAnalysisMatrix[node2][nodalAnalysisMatrix.size()] += current;
}

CurrentSrc::~CurrentSrc() {
}
