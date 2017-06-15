#ifndef NETLISTSTRUCTURE_H
#define NETLISTSTRUCTURE_H


class netListStructure
{
    public:
        netListStructure(const string validFilePath);
        virtual ~netListStructure();

		// Array with the same contents of the netlist file, but translated as Elements
		vector< Element *> elementNetlist;

		// Identifier of extraNodes to build the first output file line
		vector< string > extraNodeIdentifier;

		// numRows = numNodes; numColumns = numNodes + 1 (the plus one is for the coupled solution vector as the last column)
		vector< vector<Complex> > nodalAnalysisMatrix;

		// Solution matrix, used in the end to write the output file
		vector<Complex> solutionMatrix;

		string netlistFilePath, stepType;
		unsigned int numNodes, numTotalNodes, solutionIndex;
		double beginFrequency, endFrequency, frequencyStep;
		bool isElementNetlistShared, hasCommandLine, hasMOSFET;

};

#endif // NETLISTSTRUCTURE_H
