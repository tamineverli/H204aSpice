#include "../header/ControlledSourceImplementations.h"

using namespace std;

/* This file contains the source codes for all controlled source classes:
		
		1. Voltage_to_Voltage;
		2. Current_to_Current;
		3. Voltage_to_Current (transconductance);
		4. Current_to_Voltage (transresistance);
*/

// 	1. Voltage to Voltage
	Voltage_to_Voltage::Voltage_to_Voltage(string netlistLine) : ControlledSource(netlistLine) {
	}

	void Voltage_to_Voltage::setTemplate(ComplexVector &nodalSystem, vector<Complex> &previousSolutionVector) {

		nodalSystem[node1][extraNode] += 1.0;
		nodalSystem[node2][extraNode] += -1.0;

		nodalSystem[extraNode][node1] += -1.0;
		nodalSystem[extraNode][node2] += 1.0;
		nodalSystem[extraNode][referenceNode1] += gain;
		nodalSystem[extraNode][referenceNode2] += (-1.0)*gain;
	}

	Voltage_to_Voltage::~Voltage_to_Voltage() {
	}

//	2. Current to Current
	Current_to_Current::Current_to_Current(string netlistLine) : ControlledSource(netlistLine) {
	}

	void Current_to_Current::setTemplate(ComplexVector &nodalSystem, vector<Complex> &previousSolutionVector) {

		nodalSystem[node1][extraNode] += gain;
		nodalSystem[node2][extraNode] += (-1.0)*gain;
		nodalSystem[referenceNode1][extraNode] += 1.0;
		nodalSystem[referenceNode2][extraNode] += -1.0;

		nodalSystem[extraNode][referenceNode1] += -1.0;
		nodalSystem[extraNode][referenceNode2] += 1.0;
	}

	Current_to_Current::~Current_to_Current() {
	}

//	3. Voltage to Current (transconductance)
	Voltage_to_Current::Voltage_to_Current(string netlistLine) : ControlledSource(netlistLine) {
	}

	void Voltage_to_Current::setTemplate(ComplexVector &nodalSystem, vector<Complex> &previousSolutionVector) {

		nodalSystem[node1][referenceNode1] += gain;
		nodalSystem[node1][referenceNode2] += (-1.0)*gain;
		nodalSystem[node2][referenceNode1] += (-1.0)*gain;
		nodalSystem[node2][referenceNode2] += gain;
	}

	Voltage_to_Current::~Voltage_to_Current() {
	}

//	4. Current to Voltage (transresistance)
	Current_to_Voltage::Current_to_Voltage(string netlistLine) : ControlledSource(netlistLine) {
	}

	void Current_to_Voltage::setTemplate(ComplexVector &nodalSystem, vector<Complex> &previousSolutionVector) {

		nodalSystem[node1][extraNode2] += 1.0;
		nodalSystem[node2][extraNode2] += -1.0;
		nodalSystem[referenceNode1][extraNode] += 1.0;
		nodalSystem[referenceNode2][extraNode] += -1.0;

		nodalSystem[extraNode][referenceNode1] += -1.0;
		nodalSystem[extraNode][referenceNode2] += 1.0;
		nodalSystem[extraNode2][node1] += -1.0;
		nodalSystem[extraNode2][node2] += 1.0;

		nodalSystem[extraNode2][extraNode] += gain;
	}

	Current_to_Voltage::~Current_to_Voltage() {
	}
