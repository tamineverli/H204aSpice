/*


PARTES QUE DEPENDEM DE MOSFET COMENTADAS
1) ANÁLISE DE PONTO DE OPERAÇÃO 
2) ESCRITA DO PONTO DE OPERAÇÃO NO ARQUIVO DE SAÍDA
3)

*/


//
//  analyzer.cpp
//  Implementation of the netList class to be used in the main program.
//
//	g++ -std=c++11 -Wall -o test analyzer.cpp elements.cpp
//  

#include <fstream>
#include <iostream>
#include <iomanip>
#include <math.h>
#include "analyzer.h"

#ifndef NOT_WINDOWS
#include <windows.h>
#include <conio.h>
#endif

const double PI = 3.141592653589793;

using namespace std;

// Opens the netlist file and creates both the elementNetlist and nodalAnalysisMatrix
Analyzer::Analyzer(const string validNetlistFilePath) : netlistFilePath(validNetlistFilePath) {
	
	ifstream netlistFile;
	string netlistLine;
	char elementType;
	Element *tempElement;
	bool setExtraNode, wrongElement, hasTransformer;

	netlistFile.open(netlistFilePath);

	if (!netlistFile) {
		cout << "Erro: nao foi possivel abrir o arquivo de entrada." << endl;
		exit(EXIT_FAILURE);
	}

	// First line is the number of nodes
	getline(netlistFile, netlistLine);
	numNodes = stoi(netlistLine);

	// If a transformer is found, a scan must be made after the netlist initialization
	hasCommandLine = false;
	hasTransformer = false;

//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!-------------ADAPTADO PARA BJT-----------!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	hasBJT = false;

	while (getline(netlistFile, netlistLine)) {

		elementType = netlistLine[0];
		setExtraNode = false;
		wrongElement = false;

		switch (elementType) {
			case 'R':
				tempElement = new Resistor(netlistLine);
				break;
			case 'L':
				tempElement = new Inductor(netlistLine);
				setExtraNode = true;
				break;
			case 'C':
				tempElement = new Capacitor(netlistLine);
				break;
			case 'K':
				tempElement = new Transformer(netlistLine);
				hasTransformer = true;
				break;
			case 'E':
				tempElement = new VoltageAmplifier(netlistLine);
				setExtraNode = true;
				break;
			case 'F':
				tempElement = new CurrentAmplifier(netlistLine);
				setExtraNode = true;
				break;
			case 'G':
				tempElement = new TransconductanceAmplifier(netlistLine);
				break;
			case 'H':
				tempElement = new TransresistanceAmplifier(netlistLine);
				setExtraNode = true;
				break;
			case 'I':
				tempElement = new CurrentSource(netlistLine);
				break;
			case 'V':
				tempElement = new VoltageSource(netlistLine);
				setExtraNode = true;
				break;
			case 'O':
				tempElement = new AmpOp(netlistLine);
				setExtraNode = true;
				break;
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!-------------ADAPTADO PARA BJT-----------!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
			case 'B':
				tempElement = new BJT(netlistLine);
				hasBJT = true;
				break;
			case '.':
				initializeCommandLine(netlistLine);
				wrongElement = true;
				break;
			case '*':
				wrongElement = true;
				break;

			default:
				cout << "Erro: tipo de elemento incorreto. Elemento nao incluido na analise." << endl;
				wrongElement = true;
		}

		if (!wrongElement) {

			elementNetlist.push_back(tempElement);

			// If the element needs an extra node, initializes the extra node in extraNodeIdentifier vector and in the element itself
			if (setExtraNode) {
				
				// If the element is a TransresistanceAmplifier, it needs two extra nodes
				if (elementType == 'H') {
					
					TransresistanceAmplifier *tempTransresistanceAmplifier = dynamic_cast <TransresistanceAmplifier *> (tempElement);

					extraNodeIdentifier.push_back("j" + tempTransresistanceAmplifier->identifier + "-1");
					tempTransresistanceAmplifier->extraNode = numNodes + extraNodeIdentifier.size();

					extraNodeIdentifier.push_back("j" + tempTransresistanceAmplifier->identifier + "-2");
					tempTransresistanceAmplifier->extraNode2 = numNodes + extraNodeIdentifier.size();

					tempTransresistanceAmplifier = NULL;
				}
				else {
					
					extraNodeIdentifier.push_back("j" + tempElement->identifier);
					tempElement->extraNode = numNodes + extraNodeIdentifier.size();
				}
			}
		}
	}

	// Scan all elements to initialize the transformers parameters
	if (hasTransformer) {

		Transformer *tempTransformer;

		for (unsigned int index = 0; index < elementNetlist.size(); ++index) {
			if (elementNetlist.at(index)->type == 'K') {

				tempTransformer = dynamic_cast <Transformer *> (elementNetlist.at(index));
				tempTransformer->getInductanceValues(elementNetlist);
			}
		}

		tempTransformer = NULL;
	}

	// Initializes the nodalAnalysisMatrix filled with zeros
	numTotalNodes = numNodes + extraNodeIdentifier.size();
	cout << "Leitura da NETLIST completa. Circuito com " << numNodes << " nos, " << numTotalNodes << " variaveis e " << elementNetlist.size() << " elementos." << endl;

	tempElement = NULL;
	netlistFile.close();

	// The plus one and plus two in the size is to accept the line and column zero as the ground node
	nodalAnalysisMatrix = vector< vector< Complex > > (numTotalNodes + 1, vector<Complex>(numTotalNodes + 2, 0.0));

	// Allocates the size of the solutionMatrix (the plus two is because the first element of solution if the frequency)
	solutionMatrix = vector< Complex > (numTotalNodes + 1, 0.0);

	// Element netlist is created in this object
	isElementNetlistShared = false;

	calculateOperatingPoint();
}


//!!!!!!!!!!!!!!!!!!!!!!------------------TEM QUE ADAPTAR PARA BJT-------------------!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
//Constructor to be used by the calculateOperatingPoint of MOSFET element
Analyzer::Analyzer(vector< Element *> &copiedElementNetlist, const unsigned int newNumTotalNodes) : numTotalNodes(newNumTotalNodes) {
	
	elementNetlist = vector< Element *> (copiedElementNetlist);

	nodalAnalysisMatrix = vector< vector< Complex > > (numTotalNodes + 1, vector<Complex>(numTotalNodes + 2, 0.0));
	solutionMatrix = vector< Complex > (numTotalNodes + 1, 0.0);

	// ElementNetlist of this objetc is shared, thus cannot be deleted by destructor
	isElementNetlistShared = true;
}

Analyzer::~Analyzer() {
	
	// Deletes the element netlist, if not shared with other objects

	if (!isElementNetlistShared) {
		for (unsigned int index = 0; index < elementNetlist.size(); ++index) {
			delete (elementNetlist.at(index));
		}
	}
}

// Initializes the frequency analysis parameters from a command line
void Analyzer::initializeCommandLine(string commandLine) {

	unsigned int blankPosition;

	// Ignores the "AC", first and only option of the netlist string
	blankPosition = commandLine.find(" ");
	commandLine = commandLine.substr(blankPosition + 1);

	// Determines if the frequencyStep is calculated as LIN (absolute), by DECade or by OCTave.
	blankPosition = commandLine.find(" ");
	stepType = commandLine.substr(0, blankPosition);

	if ((stepType != "DEC") && (stepType != "OCT") && (stepType != "LIN")) {
		cout << "Erro: tipo de analise em frequencia (DEC/OCT/LIN) invalido." << endl << endl;
		hasCommandLine = false;
		return;
	}

	commandLine = commandLine.substr(blankPosition + 1);
	blankPosition = commandLine.find(" ");
	frequencyStep = stod(commandLine.substr(0, blankPosition));

	commandLine = commandLine.substr(blankPosition + 1);
	blankPosition = commandLine.find(" ");
	beginFrequency = stod(commandLine.substr(0, blankPosition));

	commandLine = commandLine.substr(blankPosition + 1);
	endFrequency = stod(commandLine);

	// Sets the static variable frequency to the begin frequency
	Element::setFrequency(beginFrequency);
	
	cout << "Linha de comando recebida: parametros de analise em frequencia configurados." << endl << endl;
	hasCommandLine = true;
}

// Creates the nodalAnalysisMatrix from the elementNetlist array for the new frequency value
void Analyzer::buildNodalAnalysisMatrix(const double frequency) {

	Element::setFrequency(frequency);

	// Fills the nodalAnalysisMatrix with zeros. The plus one and plus two in the size is to accept the line and column zero as the ground node
	nodalAnalysisMatrix = vector< vector< Complex > > (numTotalNodes + 1, vector<Complex>(numTotalNodes + 2, 0.0));

	for (unsigned int index = 0; index < elementNetlist.size(); ++index) {
		elementNetlist.at(index)->setTemplate(nodalAnalysisMatrix);
	}
}

// Finds the voltage and currents for the actual frequency used to build the nodalAnalysisMatrix
int Analyzer::solveNodalAnalysisMatrix() {
	
	const double ZERO_ERROR_THRESHOLD = 1e-9;

	unsigned int i, j, l, a;
	Complex temp, pivot;

	for (i = 1; i <= numTotalNodes; ++i) {

		temp = 0.0;
		a = i;

		for (l = i; l < numTotalNodes + 1; ++l) {
			if (abs(nodalAnalysisMatrix[l][i]) > abs(temp)) {
				a = l;
				temp = nodalAnalysisMatrix[l][i];
			}
		}

		if (i != a) {
			for (l = 1; l < numTotalNodes + 2; ++l) {
				pivot = nodalAnalysisMatrix[i][l];
				nodalAnalysisMatrix[i][l] = nodalAnalysisMatrix[a][l];
				nodalAnalysisMatrix[a][l] = pivot;
			}
		}

		if (abs(temp) < ZERO_ERROR_THRESHOLD) {
			cout << "Erro: sistema singular.";
			return 1;
		}
		
		// To diagonalize the matrix, the condition below must be "j > 0". However, for this programm, it is enought to solve the system, done with the condition "j > i"
		for (j = numTotalNodes + 1; j > 0; --j) {
			
			nodalAnalysisMatrix[i][j] /= temp;
			pivot = nodalAnalysisMatrix[i][j];

			if (abs(pivot) != 0.0) {
				for (l = 1; l < numTotalNodes + 1; ++l) {
					if (l != i)
						nodalAnalysisMatrix[l][j] -= nodalAnalysisMatrix[l][i] * pivot;
				}
			}
		}
	}

	// The first element of each line of the solutionMatrix is the frequency
	solutionMatrix.at(0) = Element::getFrequency()/(2*PI);

	// Writes the solution to the solutionMatrix to be writen to the output file later
	for (i = 1; i < solutionMatrix.capacity(); ++i) {
		solutionMatrix.at(i) = nodalAnalysisMatrix[i][nodalAnalysisMatrix.at(i).capacity() - 1];
	}

	return 0;
}


//!!!!!!!!!!!!!!!!!!!!!!------------------TEM QUE ADAPTAR PARA BJT-------------------!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!


// Calculates the operating point and writes the result to a file
void Analyzer::calculateOperatingPoint() {

	const unsigned int NUM_MAX_ITERATIONS_TO_CONVERGE = 50;
	const double VOLTAGE_CONVERGE_THRESHOLD = 0.0003;

	// Initializes a MOSFET vector considering the worst case of every element in elementNetlist being a MOS transistor
	vector <MOSFET *> mosfets;
	unsigned int mosfetIndex, temp;
	bool hasConverged = false;
	
	for (unsigned int index = 0; index < elementNetlist.size(); ++index) {
		if (elementNetlist.at(index)->type == 'M') {
			mosfets.push_back(dynamic_cast <MOSFET *> (elementNetlist.at(index)));
		}
	}

	// Solves the system on and again until it converges or reaches max number of trials (solutionMatrix.capacity())
	for (solutionIndex = 0; ((solutionIndex < NUM_MAX_ITERATIONS_TO_CONVERGE) && (!hasConverged)); ++solutionIndex) {

		// Build the nodalAnalysisMatrix for DC (frequency = 0.0)
		buildNodalAnalysisMatrix(0.0);
		if (solveNodalAnalysisMatrix()) {
			cout << "Nao foi possivel calcular o ponto de operacao." << endl << endl;
			return;
		}

		hasConverged = true;

		// If all of the MOSFETs node voltages converge, abort the loop
		for (mosfetIndex = 0; mosfetIndex < mosfets.size(); ++mosfetIndex) {
			
			if ((abs(solutionMatrix.at(mosfets.at(mosfetIndex)->nodeD) - mosfets.at(mosfetIndex)->Vd) > VOLTAGE_CONVERGE_THRESHOLD) ||
				(abs(solutionMatrix.at(mosfets.at(mosfetIndex)->nodeG) - mosfets.at(mosfetIndex)->Vg) > VOLTAGE_CONVERGE_THRESHOLD) ||
				(abs(solutionMatrix.at(mosfets.at(mosfetIndex)->nodeS) - mosfets.at(mosfetIndex)->Vs) > VOLTAGE_CONVERGE_THRESHOLD) ||
				(abs(solutionMatrix.at(mosfets.at(mosfetIndex)->nodeB) - mosfets.at(mosfetIndex)->Vb) > VOLTAGE_CONVERGE_THRESHOLD)) {
				
				hasConverged = false;
			}
		
			if (((mosfets.at(mosfetIndex)->MOSType == "NMOS") && (real(solutionMatrix.at(mosfets.at(mosfetIndex)->nodeD)) < real(solutionMatrix.at(mosfets.at(mosfetIndex)->nodeS)))) ||
				((mosfets.at(mosfetIndex)->MOSType == "PMOS") && (real(solutionMatrix.at(mosfets.at(mosfetIndex)->nodeD)) > real(solutionMatrix.at(mosfets.at(mosfetIndex)->nodeS))))) {
					temp = mosfets.at(mosfetIndex)->nodeD;
					mosfets.at(mosfetIndex)->nodeD = mosfets.at(mosfetIndex)->nodeS;
					mosfets.at(mosfetIndex)->nodeS = temp;
					mosfets.at(mosfetIndex)->inverted = !mosfets.at(mosfetIndex)->inverted;
			}

			mosfets.at(mosfetIndex)->Vd = real(solutionMatrix.at(mosfets.at(mosfetIndex)->nodeD));
			mosfets.at(mosfetIndex)->Vg = real(solutionMatrix.at(mosfets.at(mosfetIndex)->nodeG));
			mosfets.at(mosfetIndex)->Vs = real(solutionMatrix.at(mosfets.at(mosfetIndex)->nodeS));
			mosfets.at(mosfetIndex)->Vb = real(solutionMatrix.at(mosfets.at(mosfetIndex)->nodeB));

			mosfets.at(mosfetIndex)->Vds = mosfets.at(mosfetIndex)->Vd - mosfets.at(mosfetIndex)->Vs;
			mosfets.at(mosfetIndex)->Vgs = mosfets.at(mosfetIndex)->Vg - mosfets.at(mosfetIndex)->Vs;
			mosfets.at(mosfetIndex)->Vbs = mosfets.at(mosfetIndex)->Vb - mosfets.at(mosfetIndex)->Vs;

			mosfets.at(mosfetIndex)->calculateParameters();
		}
	}

	if (!hasConverged) {
		cout << "Erro: sistema nao convergiu na definicao do ponto de operacao." << endl;
		exit(EXIT_FAILURE);
	}

	cout << "Sistema convergiu com " << solutionIndex + 1 << " iteracoes." << endl << endl;
}



// Writes the result of the operating point to a file
void Analyzer::writeOperatingPoint() {

	string outputFileName;
	ofstream outputFile;
	MOSFET *tempMOSFET;

	outputFileName = netlistFilePath.substr(0, netlistFilePath.find("."));
	outputFileName += "-operatingPoint.tab";

	outputFile.open(outputFileName);

	if (!outputFile) {
		cout << "Erro: nao foi possivel abrir o arquivo de saida para gravacao do ponto de operacao." << endl;
		exit(EXIT_FAILURE);
	}

	buildNodalAnalysisMatrix(0.0);
	if (solveNodalAnalysisMatrix()) {
		cout << " Ponto de operacao nulo ou inexistente." << endl << endl;
		return;
	}

	outputFile << "Ponto de operação encontrado após " << solutionIndex << " iterações." << endl;
	outputFile << "Solução da análise nodal:" << endl << endl;

	outputFile << scientific << setprecision(4);

	for (unsigned int index = 1; index < numNodes + 1; ++index) {
		
		if (index < 10)
			outputFile << "\t ";
		else
			outputFile << "\t";

		outputFile << index << ": ";

		if (real(solutionMatrix.at(index)) >= 0.0)
			outputFile << " ";
		else
			outputFile << "-";
		outputFile << abs(solutionMatrix.at(index)) << endl;
	}
	for (unsigned int index = 0; index < extraNodeIdentifier.size(); ++index) {
		
		outputFile << extraNodeIdentifier[index] << ": ";
		
		if (real(solutionMatrix.at(index + numNodes + 1)) >= 0.0)
			outputFile << " ";
		else
			outputFile << "-";
		outputFile << abs(solutionMatrix.at(index + numNodes + 1)) << endl;
	}

	if (hasMOSFET) {
		outputFile << endl << "Parâmetros dos transistores:" << endl << endl;
		for (unsigned int index = 0; index < elementNetlist.size(); ++index) {
			if (elementNetlist.at(index)->type == 'M') {
				
				tempMOSFET = dynamic_cast <MOSFET *> (elementNetlist.at(index));

				outputFile << tempMOSFET->identifier << ": " << tempMOSFET->MOSType << " ";
				if (tempMOSFET->inverted)
					outputFile << "invertido ";
				outputFile << tempMOSFET->operatingRegion << endl;
				outputFile << " Vt = " << tempMOSFET->Vt << "\tId = " << tempMOSFET->Id << endl;
				outputFile << " Vgs = " << tempMOSFET->Vgs << "\tVds = " << tempMOSFET->Vds << "\tVbs = " << tempMOSFET->Vbs << endl;
				outputFile << " Gm = " << tempMOSFET->Gm << "\tGds = " << tempMOSFET->Gds << "\tGmb = " << tempMOSFET->Gmb << endl;
				outputFile << " Cgs = " << tempMOSFET->Cgs << "\tCgd = " << tempMOSFET->Cgd << "\tCgb = " << tempMOSFET->Cbg << endl << endl;
			}
		}
	}

	outputFile.close();
	cout << "Ponto de operacao registrado no arquivo '" << outputFileName << "'." << endl << endl;
}
*/



// Calls the functions buildNodalAnalysisMatrix and solveNodalAnaysisMatrix for each frequency to build the hole analisys
void Analyzer::calculateFrequencyAnalysis() {

	string outputFileName;
	ofstream outputFile;
	double frequency, step, stepScaleFactor;
	unsigned int numRows = 0;

	// Writes the results of the hole analysis to the output file
	outputFileName = netlistFilePath.substr(0, netlistFilePath.find("."));
	outputFileName += "-frequencyAnalysis.tab";

	outputFile.open(outputFileName);

	if (!outputFile) {
		cout << "Erro: nao foi possivel abrir o arquivo de saida para gravacao da analise em frequencia." << endl;
		exit(EXIT_FAILURE);
	}

	// Writes the first line, composed by the voltage and current identifiers
	outputFile << "f";
	for (unsigned int index = 0; index < numNodes; ++index) {
		outputFile << " " << (index + 1) << "m " << (index + 1) << "f";
	}
	for (unsigned int index = 0; index < extraNodeIdentifier.size(); ++index) {
		outputFile << " " << extraNodeIdentifier[index] << "m " << extraNodeIdentifier[index] << "f";
	}
	outputFile << endl;

	if (stepType == "LIN") {
		step = ceil((endFrequency - beginFrequency)/(frequencyStep - 1));
		stepScaleFactor = step/beginFrequency + 1;
	}
	else {

		step = 1;

		if (stepType == "DEC") {
			stepScaleFactor = pow(10.0, (1/(frequencyStep - 1)));
		}
		else if (stepType == "OCT") {
			stepScaleFactor = pow(2.0, (1/(frequencyStep - 1)));
		}
	}

	for (frequency = beginFrequency; frequency <= endFrequency; frequency += step) {

		buildNodalAnalysisMatrix(2*PI*frequency);
		if (solveNodalAnalysisMatrix()) {
			cout << " Nao foi possivel calcular a analise na frequencia " << frequency << ". Programa abortado." << endl;
			cout << "Pressione uma tecla para fechar..." << endl;
			
			#ifndef NOT_WINDOWS
			getch();
			system("cls");
			#endif
			
			exit(EXIT_FAILURE);
		}

		outputFile << scientific << setprecision(4) << abs(solutionMatrix.at(0));

		// Writes the contents of the file, spliting the complex values into magnitude and phase (starts in one to jump the ground node voltage)
		for (unsigned int columnIndex = 1; columnIndex < solutionMatrix.size(); ++columnIndex) {
			outputFile << " " << scientific << setprecision(5) << abs(solutionMatrix.at(columnIndex))
					   << " " << fixed << setprecision(3) << arg(solutionMatrix.at(columnIndex))*(180/PI);
		}
		outputFile << endl;

		if (stepType == "LIN")
			stepScaleFactor = step/frequency + 1;
		else
			step = frequency*(stepScaleFactor - 1);

		++numRows;
	}

	outputFile.close();
	cout << "Analise em frequencia realizada com sucesso. Tabela com " << numRows << " pontos registrada no arquivo '" << outputFileName << "'." << endl << endl;
}

void Analyzer::printNodalAnalysisMatrix() {
	
	// Used to change the color of output in windows
	#ifndef NOT_WINDOWS
	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	unsigned int alternativeColor = 4, defaultColor = 7;
	#endif

	cout.precision(1);
	cout << scientific;

	for (unsigned int indexRow = 0; indexRow < nodalAnalysisMatrix.capacity(); indexRow++) {
		cout << " | ";
		for (unsigned int indexColumn = 0; indexColumn < nodalAnalysisMatrix[indexRow].capacity(); indexColumn++) {
			
			#ifndef NOT_WINDOWS
			if ((indexRow == 0) || (indexColumn == 0)) {
				SetConsoleTextAttribute(hConsole, alternativeColor);
			}
			else {
				SetConsoleTextAttribute(hConsole, defaultColor);
			}
			#endif

			if (indexColumn == nodalAnalysisMatrix[indexRow].capacity() - 1)
				cout << "| ";

			if (real(nodalAnalysisMatrix[indexRow][indexColumn]) >= 0.0)
				cout << " ";
			else
				cout << "-";

			cout << abs(real(nodalAnalysisMatrix[indexRow][indexColumn]));

			if (imag(nodalAnalysisMatrix[indexRow][indexColumn]) >= 0.0)
				cout << " + j";
			else
				cout << " - j";

			cout << abs(imag(nodalAnalysisMatrix[indexRow][indexColumn])) << "\t";
		}

		#ifndef NOT_WINDOWS
		SetConsoleTextAttribute(hConsole, defaultColor);
		#endif

		cout << "|" << endl;
	}
	cout << endl;
}