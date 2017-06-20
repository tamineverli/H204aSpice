#include "../header/Capacitor.h"

Capacitor::Capacitor(string netlistLine) : SimpleComponent(netlistLine) {
}

void Capacitor::setTemplate(ComplexVector &nodalAnalysisMatrix) {

	capacitiveAdmittance = Complex(0.0, frequency*value);

	nodalAnalysisMatrix[node1][node1] += capacitiveAdmittance;
	nodalAnalysisMatrix[node1][node2] += conj(capacitiveAdmittance);
	nodalAnalysisMatrix[node2][node1] += conj(capacitiveAdmittance);
	nodalAnalysisMatrix[node2][node2] += capacitiveAdmittance;
}

Capacitor::~Capacitor() {
}
