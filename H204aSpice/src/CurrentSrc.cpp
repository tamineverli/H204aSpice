#include "CurrentSrc.h"

CurrentSrc::CurrentSrc(string netlistLine) : IndependentSource(netlistLine) {
}

void CurrentSrc::setTemplate(ComplexVector &nodalSystem, vector<Complex> &previousSolutionVector) {

	Complex current;

	if (frequency == 0.0)
		current = valueDC;
	else
		current = polar(magnitude, phase);

	nodalSystem[node1][nodalSystem.size()] += (-1.0)*current;
	nodalSystem[node2][nodalSystem.size()] += current;


}

CurrentSrc::~CurrentSrc() {
}
