#include "../header/Inductor.h"

Inductor::Inductor(string netlistLine) : SimpleComponent(netlistLine) {
}

void Inductor::setTemplate(ComplexVector &nodalSystem) {

	inductiveReactance = Complex(0.0, frequency*value);

	nodalSystem[node1][extraNode] += 1.0;
	nodalSystem[node2][extraNode] += -1.0;
	nodalSystem[extraNode][node1] += -1.0;
	nodalSystem[extraNode][node2] += 1.0;
	nodalSystem[extraNode][extraNode] += inductiveReactance;
}

Inductor::~Inductor() {
}
