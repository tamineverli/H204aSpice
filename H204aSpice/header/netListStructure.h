#ifndef NETLISTSTRUCTURE_H
#define NETLISTSTRUCTURE_H

#include <string>
#include <complex>
#include <vector>

#include "Component.h"

class netlistStructure
{
    public:
        netlistStructure(const string validFilePath);
        //Set frequency analysis parameters according to a command line
        void setCommandLineParameters(string commandLine);
        //Build nodal system for a given frequency
        void buildNodalSystem(const double frequency);
        //Solve the nodal system matrix
        int solveNodalSystem();
        //Compute BJT tensions using Newton Raphson method
        int newtonRaphson();
        //Find operating point for all transistors
		void findOperatingPoint();
		//Perform frequency analysis
		//void freqAnalysis();
		//Frequency analysis output
		//void printFreqAnalysis();

        virtual ~netlistStructure();

		//Array that stores all the components extracted from the netlist
		vector<Component*> componentNetlist;

		//Vector to support identification of extra nodes
		vector<string> extraNodeVector;

		// numRows = numNodes; numColumns = numNodes + 1 (the plus one is for the coupled solution vector as the last column)
		vector < vector<Complex> > nodalSystem;

		// Solution vector, used in the end to write the output file
		vector<Complex> nodalSolutionVector;

		string netlistFilePath, stepType;
		unsigned int numNodes, numTotalNodes, solutionIndex;
		double beginFrequency, endFrequency, frequencyStep;
		bool hasBJT, validCommandLine, isElementNetlistShared;

        double step, inicialFrequency, finalFrequency;

};

#endif // NETLISTSTRUCTURE_H
