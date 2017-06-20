#include "IndependentSource.h"

#include <iostream>

IndependentSource::IndependentSource(string netlistLine) : Component(netlistLine) {

	unsigned int blankPosition;

	blankPosition = netlistLine.find(" ");
	istringstream in(netlistLine.substr(blankPosition + 1));

	//cout << "paramentros lidos " << netlistLine << endl;  //debug
	in >> node1 >> node2 >> magnitude >> phase >> valueDC;
}

void IndependentSource::print() {

	Component::print();

	cout << " Node 1: " << node1 << endl;
	cout << " Node 2: " << node2 << endl;
	cout << " Extra Node: " << extraNode << endl;
	cout << " Magnitude: " << magnitude << endl;
	cout << " Phase: " << phase << endl;
	cout << " Value DC: " << valueDC << endl << endl;
}

IndependentSource::~IndependentSource() {
}
