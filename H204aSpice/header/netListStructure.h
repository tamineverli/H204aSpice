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
        void setCommandLineParameters(string commandLine);
        virtual ~netlistStructure();

        // Netlist related variables

        	string netlistFilePath;	
        	unsigned int numNodes, numTotalNodes;
			vector<Component*> componentNetlist; 	//array containing components read from the netlist
			vector<string> extraNodeIdentifier;		//stores extra nodes

		// Nodal System related variables
			unsigned int solutionIndex;
			vector < vector<Complex> > nodalSystem;	//numRows = numNodes; numColumns = numNodes + 1 (the plus one is for the coupled solution vector as the last column)
			vector<Complex> solutionMatrix;			//store the system output after all calculations
		
		// Command Line related variables
			string stepType;
			double inicialFrequency, finalFrequency, step;
		
		// Other varaibles
			bool hasBJT, validCommandLine, isElementNetlistShared;
};

#endif // NETLISTSTRUCTURE_H
