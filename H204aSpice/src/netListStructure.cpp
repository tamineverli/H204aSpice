#include "../header/netListStructure.h"
#include "../header/Resistor.h"

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

	Component *tempComponent;
	char elementType;

	bool setExtraNode, wrongElement, hasTransformer;

	netlistFile.open(validFilePath.c_str());


	//if (!netlistFile) {
	//	cout << "Erro: nao foi possivel abrir o arquivo de entrada." << endl;
	//	exit(EXIT_FAILURE);
	//}

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
			case 'R':
				tempComponent = new Resistor(fileLine);
				break;

			default:
				cout << "\nALERTA: elemento invalido identificado no NETLIST.\nComplicado..." << endl;
				wrongElement = true;
		}

		/* Add element to netlist */
		elementNetlist.push_back(tempComponent);

		tempComponent->print();

	}

	// Initializes the nodalAnalysisMatrix filled with zeros
	//numTotalNodes = numNodes + extraNodeIdentifier.size();
	cout << "Leitura da NETLIST completa. Circuito com " << numNodes << " nos e " << elementNetlist.size() << " elementos." << endl;

	tempComponent = NULL;
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
