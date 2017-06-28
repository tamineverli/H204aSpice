#include "../header/Inductor.h"

Inductor::Inductor(string netlistLine) : SimpleComponent(netlistLine) {
}

void Inductor::setTemplate(ComplexVector &nodalSystem) {

	if (frequency > 0) {
		inductiveReactance = Complex(0.0, frequency*value);
	}
	else{
		//CRIAR MACRO DEPOIS
		inductiveReactance = 1e-9;
	}

	nodalSystem[node1][extraNode] += 1.0;
	nodalSystem[node2][extraNode] += -1.0;
	nodalSystem[extraNode][node1] += -1.0;
	nodalSystem[extraNode][node2] += 1.0;
	nodalSystem[extraNode][extraNode] += inductiveReactance;
}

Inductor::~Inductor() {
}
