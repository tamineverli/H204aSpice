#include "ControlledSource.h"

#include <iostream>

ControlledSource::ControlledSource(string netlistLine) : Component(netlistLine) {

	unsigned int blankPosition;

    blankPosition = netlistLine.find(" ");
	istringstream in(netlistLine.substr(blankPosition + 1));

	//cout << "paramentros lidos " << netlistLine << endl;  //debug
	in >> node1 >> node2 >> referenceNode1 >> referenceNode2 >> gain;
}

void ControlledSource::print() {

	Component::print();

	cout << " Node 1: " << node1 << endl;
	cout << " Node 2: " << node2 << endl;
	cout << " Reference Node 1: " << referenceNode1 << endl;
	cout << " Reference Node 2: " << referenceNode2 << endl;
	cout << " Extra Node: " << extraNode << endl;
	cout << " Gain: " << gain << endl << endl;
}

ControlledSource::~ControlledSource(){
}
