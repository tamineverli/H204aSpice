#include "../header/Inductor.h"

Inductor::Inductor(string netlistLine) : SimpleComponent(netlistLine) {
}

void Inductor::setTemplate(ComplexVector &nodalAnalysisMatrix) {

	inductiveReactance = Complex(0.0, frequency*value);

	nodalAnalysisMatrix[node1][extraNode] += 1.0;
	nodalAnalysisMatrix[node2][extraNode] += -1.0;
	nodalAnalysisMatrix[extraNode][node1] += -1.0;
	nodalAnalysisMatrix[extraNode][node2] += 1.0;
	nodalAnalysisMatrix[extraNode][extraNode] += inductiveReactance;
}

Inductor::~Inductor() {
}
