#ifndef NETLISTSTRUCTURE_H
#define NETLISTSTRUCTURE_H

#include <string>
#include <complex>
#include <vector>

#include <Component.h>

class netlistStructure
{
    public:
        netlistStructure(const string validFilePath);
        virtual ~netlistStructure();

		// Array containing the components read from the netlist and their parameters
		vector<Component*> elementNetlist;

		// Identifier of extraNodes to build the first output file line
		vector<string> extraNodeIdentifier;

		// numRows = numNodes; numColumns = numNodes + 1 (the plus one is for the coupled solution vector as the last column)
		vector < vector<Complex> > nodalAnalysisMatrix;

		// Solution matrix, used in the end to write the output file
		vector<Complex> solutionMatrix;

		string netlistFilePath, stepType;
		unsigned int numNodes, numTotalNodes, solutionIndex;
		double beginFrequency, endFrequency, frequencyStep;
		bool isElementNetlistShared, hasCommandLine, hasMOSFET;

};

#endif // NETLISTSTRUCTURE_H
