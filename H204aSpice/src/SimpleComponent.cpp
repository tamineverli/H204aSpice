#include "../header/SimpleComponent.h"

#include <iostream>

SimpleComponent::SimpleComponent(string netlistLine) : Component(netlistLine) {

	unsigned int blankPosition;

	blankPosition = netlistLine.find(" ");
	istringstream in(netlistLine.substr(blankPosition + 1));

	//cout << "paramentros lidos " << netlistLine << endl;  //debug
	in >> node1 >> node2 >> value;
}

void SimpleComponent::print() {

	Component::print();

	cout << " Node 1: " << node1 << endl;
	cout << " Node 2: " << node2 << endl;
	cout << " Extra Node: " << extraNode << endl;
	cout << " Value: " << value << endl << endl;
}

SimpleComponent::~SimpleComponent() {
}
