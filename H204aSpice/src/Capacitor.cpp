#include "../header/Capacitor.h"

Capacitor::Capacitor(string netlistLine) : SimpleComponent(netlistLine) {
}

void Capacitor::setTemplate(ComplexVector &nodalSystem) {

	capacitiveAdmittance = Complex(0.0, frequency*value);

	nodalSystem[node1][node1] += capacitiveAdmittance;
	nodalSystem[node1][node2] += conj(capacitiveAdmittance);
	nodalSystem[node2][node1] += conj(capacitiveAdmittance);
	nodalSystem[node2][node2] += capacitiveAdmittance;
}

Capacitor::~Capacitor() {
}
