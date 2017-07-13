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
        void buildNodalSystem(const double frequency, vector<Complex> &previousSolutionVector);
        //Solve the nodal system matrix
        int solveNodalSystem();
		//Print nodal system
        void printNodalSystem();
        //Compute BJT tensions using Newton Raphson method
        int newtonRaphson();
        //Print BJT operating point
		void findBJTOperatingPoint();
		//Perform frequency analysis onscreen
		//void freqAnalysis();
		//Frequency analysis output to file
		void freqAnalysisToFile();
		ofstream estampaLogFile;
		//Used to debug
		void printOutputToFile(std::vector<std::complex<double>>);

        virtual ~netlistStructure();

		//Array that stores all the components extracted from the netlist
		vector<Component*> componentNetlist;

		//Vector to support identification of extra nodes
		vector<string> extraNodeVector;

		// numRows = numNodes / numColumns = numNodes + 1 (to save the solution)
		vector < vector<Complex> > nodalSystem;

		// Solution vector, used in the end to write the output file
		vector<Complex> nodalSolutionVector;

		// Solution vector, used in the end to write the output file
		vector<Complex> previousSolutionVector;

		string netlistFilePath, stepType;
		unsigned int numNodes, numTotalNodes, solutionIndex;
		double frequencyPoints, inicialFrequency, finalFrequency;
		bool hasBJT, validCommandLine, isElementNetlistShared;

		ofstream logFile;

};

#endif // NETLISTSTRUCTURE_H
