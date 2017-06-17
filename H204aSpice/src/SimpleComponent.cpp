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

	cout << " Node1: " << node1 << endl;
	cout << " Node2: " << node2 << endl;
	cout << " ExtraNode: " << extraNode << endl;
	cout << " Value: " << value << endl << endl;
}

SimpleComponent::~SimpleComponent() {
}
