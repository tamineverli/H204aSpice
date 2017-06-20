#include "../header/netListStructure.h"
#include "../header/Resistor.h"
#include "../header/Capacitor.h"
#include "../header/Inductor.h"
#include "../header/VoltageSrc.h"
#include "../header/CurrentSrc.h"
#include "../header/AmpOp.h"

#include <stdlib.h>
#include <fstream>
#include <iostream>
#include <iomanip>
#include <math.h>
#include <string>
#include <vector>
#include <sstream>

using namespace std;

netlistStructure::netlistStructure(const string validFilePath) : netlistFilePath(validFilePath)
{

	ifstream netlistFile;
	string fileLine;

	Component *newComponent;
	char elementType;

	bool setExtraNode, wrongElement, hasTransformer;

	netlistFile.open(validFilePath.c_str());

	// First line is the number of nodes
	getline(netlistFile, fileLine);
	istringstream in(fileLine);
	in >> numNodes; // = stoi(fileLine);

	// If a transformer is found, a scan must be made after the netlist initialization
	//hasCommandLine = false;
	//hasTransformer = false;

	//hasBJT = false;

	while (getline(netlistFile, fileLine)) {

		elementType = fileLine[0];
		setExtraNode = false;
		wrongElement = false;

		switch (elementType) {

		    /*Simple Components*/
			case 'R':   //resistor
				newComponent = new Resistor(fileLine);
				break;
			case 'L':   //inductor
				newComponent = new Inductor(fileLine);
				setExtraNode = true;
				break;
			case 'C':   //capacitor
				newComponent = new Capacitor(fileLine);
				break;

            /*Independent Sources*/
            case 'I':   //current source
				newComponent = new CurrentSrc(fileLine);
				break;
			case 'V':   //voltage source
				newComponent = new VoltageSrc(fileLine);
				setExtraNode = true;
				break;

            /*Operational Amplifier*/
			case 'O':
				newComponent = new AmpOp(fileLine);
				setExtraNode = true;
				break;

			default:
				cout << "\nALERTA: elemento invalido identificado no NETLIST.\nComplicado..." << endl;
				wrongElement = true;

		}

		/* Add new component to netlist */
		elementNetlist.push_back(newComponent);

		newComponent->print();
	}

	// Initializes the nodalAnalysisMatrix filled with zeros
	//numTotalNodes = numNodes + extraNodeIdentifier.size();
	cout << "\nNETLIST com " << elementNetlist.size() << " componentes e " << numNodes << " nos.\n" << endl;
	cout << "_____________________________________________________\n" << endl;
	
	newComponent = NULL;
	netlistFile.close();

	// The plus one and plus two in the size is to accept the line and column zero as the ground node
	//nodalAnalysisMatrix = vector< vector< Complex > > (numTotalNodes + 1, vector<Complex>(numTotalNodes + 2, 0.0));

	// Allocates the size of the solutionMatrix (the plus two is because the first element of solution if the frequency)
	//solutionMatrix = vector< Complex > (numTotalNodes + 1, 0.0);

	// Element netlist is created in this object
	//isElementNetlistShared = false;

	//calculateOperatingPoint();

}

netlistStructure::~netlistStructure()
{
    //dtor
}
