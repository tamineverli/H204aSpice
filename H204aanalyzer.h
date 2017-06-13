//
//  analyzer.h
//  Definition of the netList class to be used in the main program.
//  

#ifndef ANALYZER_H
#define ANALYZER_H

#include <complex>
#include <string>
#include <vector>
#include "elements.h"

using namespace std;

class Analyzer {

	public:
		// Opens the netlist file and creates both the elementNetlist and nodalAnalisysMatrix
		Analyzer(const string validNetlistFilePath);

		// Used to initialize the class for calculus of operating point
		Analyzer(vector< Element *> &copiedElementNetlist, const unsigned int numTotalNodes);
		~Analyzer();
		
		// Initializes the frequency analysis parameters from a command line
		void initializeCommandLine(string commandLine);

		// Creates the nodalAnalysisMatrix from the elementNetlist array for the new frequency value
		void buildNodalAnalysisMatrix(const double frequency);

		// Finds the voltage and currents for the actual frequency used to build the nodalAnalysisMatrix, and append the results to the solutionMatrix
		int solveNodalAnalysisMatrix();

		// Calculates the operating point (internal function called within constructor)
		void calculateOperatingPoint();

		// Writes the result of the operating point to a file
		void writeOperatingPoint();

		// Calls the functions buildNodalAnalysisMatrix and solveNodalAnalisysMatrix for each frequency to build the hole analisys
		void calculateFrequencyAnalysis();

		// Debug function
		void printNodalAnalysisMatrix();

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

#endif