#include "../header/Capacitor.h"

Capacitor::Capacitor(string netlistLine) : SimpleComponent(netlistLine) {
}

void Capacitor::setTemplate(ComplexVector &nodalSystem, vector<Complex> &previousSolutionVector) {

	if (frequency > 0) {
		capacitiveAdmittance = Complex(0.0, frequency*value);
	}
	else{
		capacitiveAdmittance = CAPACITIVE_ADMITTANCE_DC;
	}

	nodalSystem[node1][node1] += capacitiveAdmittance;
	nodalSystem[node1][node2] += (-1.0)*capacitiveAdmittance;
	nodalSystem[node2][node1] += (-1.0)*capacitiveAdmittance;
	nodalSystem[node2][node2] += capacitiveAdmittance;
}

Capacitor::~Capacitor() {
}
