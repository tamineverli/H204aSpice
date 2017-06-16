#include "../header/Component.h"

#include <math.h>
#include <iostream>
#include <iomanip>

Component::Component(string netlistLine) : netlistLine(netlistLine) {

	unsigned int blankPosition;

	type = netlistLine[0];

	blankPosition = netlistLine.find(" ");
	identifier = netlistLine.substr(0, blankPosition);

	netlistLine = netlistLine.substr(blankPosition + 1);

	//If used, the correct value will be set in subsequent constructors
	extraNode = 0;
}

Component::~Component() {
}

void Component::setFrequency(const double newfrequency) {
	frequency = newfrequency;
}

double Component::getFrequency() {
	return frequency;
}

void Component::print() {
	cout << " Type: " << type << endl;
	cout << " Identifier: " << identifier << endl;
}

double Component::frequency = 0.0;
