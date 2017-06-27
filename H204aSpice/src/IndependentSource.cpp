#include "IndependentSource.h"

#include <iostream>

IndependentSource::IndependentSource(string netlistLine) : Component(netlistLine) {

	unsigned int blankPosition;

	blankPosition = netlistLine.find(" ");
	istringstream in(netlistLine.substr(blankPosition + 1));

	in >> node1 >> node2 >> magnitude;

	//Phase and Value DC are optional arguments, so we need to test if they exist
	if (in.eof()) {
		phase = 0;			//set defaut value
		valueDC = 0;		//set defaut value
	}
	else {
		in >> phase;
		if (in.eof()) {
			valueDC = 0;	//set defaut value
		}
		else in >> valueDC;
	}
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
