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

		//Array that stores all the components extracted from the netlist
		vector<Component*> componentNetlist;

		//Vector to support identification of extra nodes
		vector<string> extraNodeVector;

		// numRows = numNodes; numColumns = numNodes + 1 (the plus one is for the coupled solution vector as the last column)
		vector < vector<Complex> > nodalSystem;

		// Solution matrix, used in the end to write the output file
		vector<Complex> solutionMatrix;

		string netlistFilePath, stepType;
		unsigned int numNodes, numTotalNodes, solutionIndex;
		double beginFrequency, endFrequency, frequencyStep;
		bool hasBJT, validCommandLine, isElementNetlistShared;

};

#endif // NETLISTSTRUCTURE_H
