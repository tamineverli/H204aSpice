#include "../header/AmpOp.h"

#include <iostream>

using namespace std;

AmpOp::AmpOp(string netlistLine) : Component(netlistLine) {

	unsigned int blankPosition;

    blankPosition = netlistLine.find(" ");
	istringstream in(netlistLine.substr(blankPosition + 1));

	//cout << "paramentros lidos " << netlistLine << endl;  //debug
	in >> out_positive >> out_negative >> in_positive >> in_negative;
}

void AmpOp::setTemplate(ComplexVector &nodalSystem, vector<Complex> &previousSolutionVector) {

	nodalSystem[out_positive][extraNode] += 1.0;
	nodalSystem[out_negative][extraNode] += -1.0;
	nodalSystem[extraNode][in_positive] += 1.0;
	nodalSystem[extraNode][in_negative] += -1.0;
}

void AmpOp::print() {

	Component::print();

    cout << " Input Node (+): " << in_positive << endl;
	cout << " Input Node (-): " << in_negative << endl << endl;

	cout << " Output Node (+): " << out_positive << endl;
	cout << " Output Node (-): " << out_negative << endl;

}

AmpOp::~AmpOp() {
}
