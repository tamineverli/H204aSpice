#include "CoupledInductors.h"

#include <iostream>

CoupledInductors::CoupledInductors(string netlistLine) : Component(netlistLine) {

	unsigned int blankPosition;

	blankPosition = netlistLine.find(" ");
	istringstream in(netlistLine.substr(blankPosition + 1));

	//cout << "parametros lidos " << netlistLine << endl;  //debug
	in >> L1Name >> L2Name >> k;
}

//Gets the value of L1 and L2 from the componentNetlist vector
void CoupledInductors::locateInductors(vector< Component *> &componentNetlist) {

	Inductor *tempInductor;

	for (unsigned int index = 0; index < componentNetlist.size(); index++)	{
		if (componentNetlist.at(index)->type == 'L') {

			tempInductor = dynamic_cast <Inductor *> (componentNetlist.at(index));

			if (tempInductor->name == L1Name) {
				L1 = tempInductor->value;
				extraNode = tempInductor->extraNode;
			}
			else if (tempInductor->name == L2Name) {
				L2 = tempInductor->value;
				extraNode2 = tempInductor->extraNode;
			}
		}
	}

	tempInductor = NULL;
}

void CoupledInductors::setTemplate(ComplexVector &nodalSystem) {

	mutualInductance = Complex(0.0, frequency*k*sqrt(L1*L2));

	nodalSystem[extraNode][extraNode2] += mutualInductance;
	nodalSystem[extraNode2][extraNode] += mutualInductance;
}

void CoupledInductors::print() {

	//Component::print();

	cout << " Acoplamento entre Indutores: " << L1Name << " e " << L2Name << endl << endl;
}

CoupledInductors::~CoupledInductors() {
}
