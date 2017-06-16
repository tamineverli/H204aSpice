#include "../header/SimpleComponent.h"

#include <iostream>

SimpleComponent::SimpleComponent(string netlistLine) : Component(netlistLine) {

	unsigned int blankPosition;

	blankPosition = netlistLine.find(" ");
	node1 = stoi(netlistLine.substr(0, blankPosition));

	netlistLine = netlistLine.substr(blankPosition + 1);
	blankPosition = netlistLine.find(" ");
	node2 = stoi(netlistLine.substr(0, blankPosition));

	netlistLine = netlistLine.substr(blankPosition + 1);
	value = stod(netlistLine);
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
