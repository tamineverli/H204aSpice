#include "../header/netListStructure.h"
#include "../header/Resistor.h"
#include "../header/Capacitor.h"
#include "../header/Inductor.h"
#include "../header/VoltageSrc.h"
#include "../header/CurrentSrc.h"
#include "../header/AmpOp.h"
#include "../header/ControlledSourceImplementations.h"
#include "../header/CoupledInductors.h"
#include "../header/BJT.h"

#include <stdlib.h>
#include <fstream>
#include <iostream>
#include <iomanip>
#include <math.h>
#include <string>
#include <vector>
#include <sstream>

#define MIN_ERROR_THRESHOLD 1e-9
#define NEWTON_RAP_MAX_ITERATIONS 200
#define NEWTON_RAP_MAX_RAND 100
#define NEWTON_RAP_STEP 0.05
#define MAX_ERROR 0.05
#define PI 3.14159265359


using namespace std;

netlistStructure::netlistStructure(const string validFilePath) : netlistFilePath(validFilePath) {

	//	1. DECLARE LOCAL VARIABLES

		ifstream netlistFile;	//netlist input by user
		string fileLine;		//to assist in reading each netlist line

		Component *newComponent;	//to store new components
		char componentType;			//to store new components' types

		bool setExtraNode, validComponent;

	//	2. PRESET NETLIST VARIABLES

		hasBJT = false;
		validCommandLine = false;

	//	3. OPEN NETLIST FILE AND LET THE PARTY START

		netlistFile.open(validFilePath.c_str());

		//Get number of nodes from the first line in the netlist
		getline(netlistFile, fileLine);
		istringstream in(fileLine);
		in >> numNodes;

		//Now we get the circuit components by reading the netlist line by line
		while (getline(netlistFile, fileLine)) {

			componentType = fileLine[0];
			validComponent = true;
			setExtraNode = false;

			switch (componentType) {

			    //Simple Components
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

	            //Independent Sources
		            case 'I':   //current source
						newComponent = new CurrentSrc(fileLine);
						break;
					case 'V':   //voltage source
						newComponent = new VoltageSrc(fileLine);
						setExtraNode = true;
						break;

	            //Operational Amplifier
					case 'O':
						newComponent = new AmpOp(fileLine);
						setExtraNode = true;
						break;

				//Controlled Sources
					case 'E':	//Voltage_to_Voltage
						newComponent = new Voltage_to_Voltage(fileLine);
						setExtraNode = true;
						break;
					case 'F':	//Current_to_Current
						newComponent = new Current_to_Current(fileLine);
						setExtraNode = true;
						break;
					case 'G':	//Voltage_to_Current (transconductance)
						newComponent = new Voltage_to_Current(fileLine);
						break;
					case 'H':	//Current_to_Voltage (transresistance)
						newComponent = new Current_to_Voltage(fileLine);
						setExtraNode = true;
						break;

				//Inductor Coupling
					case 'K':	//Coupled inductors
						newComponent = new CoupledInductors(fileLine);

						CoupledInductors *tempCouple;

						tempCouple = dynamic_cast <CoupledInductors *> (newComponent);

						tempCouple->locateInductors(componentNetlist);	//Locate coupled inductors in netlist
						tempCouple->mutualInductance = 0;				//Initialize mutual inductance

						tempCouple = NULL;
						break;

				//BJT
					case 'Q':
						newComponent = new BJT(fileLine);
						hasBJT = true;
						break;

				//Command Line and Comments
					case '.':
						setCommandLineParameters(fileLine);
						validComponent = false;
						break;

				//Comments
					case '*':
						cout << " Comentario: " << fileLine << endl << endl;
						validComponent = false;
						break;

				//Defaut
					default:
						cout << "\nALERTA: elemento invalido identificado no NETLIST.\nComplicado...\n" << endl;
						validComponent = false;
			}

			if (validComponent) {

				// Add new component to netlist
				componentNetlist.push_back(newComponent);
				newComponent->print();

				// If the element needs an extra node, initializes the extra node in extraNodeVector vector and in the element itself
				if (setExtraNode) {

					// If the element is a Current_to_Voltage controlled source, it needs two extra nodes
					if (componentType == 'H') {

						Current_to_Voltage *tempCurrent_to_Voltage = dynamic_cast <Current_to_Voltage *> (newComponent);

						extraNodeVector.push_back("j" + tempCurrent_to_Voltage->name + " 1");
						tempCurrent_to_Voltage->extraNode = numNodes + extraNodeVector.size();

						extraNodeVector.push_back("j" + tempCurrent_to_Voltage->name + " 2");
						tempCurrent_to_Voltage->extraNode2 = numNodes + extraNodeVector.size();

						tempCurrent_to_Voltage = NULL;
					}
					else {

						extraNodeVector.push_back("j" + newComponent->name);
						newComponent->extraNode = numNodes + extraNodeVector.size();
					}
				}
			}
		}

		newComponent = NULL;
		netlistFile.close();

		cout << "_______________________________________________________________\n" << endl;

		//Update total number of nodes, considering extra nodes
			numTotalNodes = numNodes + extraNodeVector.size();

			//Check if frequency analysis parameters have been set
			if (!validCommandLine) {
				setCommandLineParameters("defautParameters");
			}

		cout << "\n NETLIST com " << componentNetlist.size() << " componentes e " << numNodes << " nos.\n" << endl;
		cout << "_______________________________________________________________\n" << endl;

	//	4 . SET NODAL SYSTEM

		//Allocate memory for the output matrix - first line stores frequency values
		nodalSolutionVector = vector< Complex > (numTotalNodes + 1, 0.0);

}

//Set frequency analysis parameters according to a command line
void netlistStructure::setCommandLineParameters(string commandLine) {

	/* COMMAND LINE PARAMETERS:
		1. stepType (determines how the frequency step is calculated):
			- LIN (absolute) / DEC (decade) / OCT (octave)
		2. step
		3. inicialFrequency
		4. finalFrequency
	 */

	const string defautStepType = "LIN";
	const double defautInicialFreq = 5;
	const double defautFinalFreq = 60000;
	const double defautPoints = 5;

	//Set parameters with defaut values if necessary
	if (commandLine == "defautParameters") {
		stepType = defautStepType;
		frequencyPoints = defautPoints;
		inicialFrequency = defautInicialFreq;
		finalFrequency = defautFinalFreq;

		Component::setFrequency(inicialFrequency);
		validCommandLine = true;

		cout << "\n Parametros de analise em frequencia configurados com valores padrao:" << endl;
	}
	else {
		// First option should be "AC", so we can skip it
		unsigned int blankPosition;
		blankPosition = commandLine.find(" ");
		istringstream commandLineInput(commandLine.substr(blankPosition + 1));

		commandLineInput >> stepType >> frequencyPoints >> inicialFrequency >> finalFrequency;

		//If step type is not valid, set it with its defaut value
		if ((stepType != "DEC") && (stepType != "OCT") && (stepType != "LIN")) {
			cout << "\n ALERTA: tipo de analise invalido. Opcao padrao selecionada." << endl;
			stepType = defautStepType;
		}

		// Sets the static variable frequency to the begin frequency
		Component::setFrequency(inicialFrequency);
		validCommandLine = true;

		cout << "\n Parametros de analise em frequencia configurados com sucesso:" << endl;
	}

	cout << " > Points = " << frequencyPoints << endl;
	cout << " > StepType = " << stepType << endl;
	cout << " > Frequency Range: " << inicialFrequency << " to " << finalFrequency << endl << endl;

	return;
}


//Build nodal system for a given frequency
void netlistStructure::buildNodalSystem(const double frequency) {

	Component::setFrequency(frequency);

	//Initialize nodal system matrix with zeros - line and column [0,0] represent the ground node
	nodalSystem = vector< vector< Complex > > (numTotalNodes + 1, vector<Complex>(numTotalNodes + 2, 0.0));

	for (unsigned int index = 0; index < componentNetlist.size(); ++index) {
		componentNetlist.at(index)->setTemplate(nodalSystem);
	}
}

//Solve the nodal system matrix
int netlistStructure::solveNodalSystem() {

	unsigned int i, j, l, a;
	Complex temp, pivot;

	for (i = 1; i <= numTotalNodes; ++i) {

		temp = 0.0;
		a = i;

		for (l = i; l < numTotalNodes + 1; ++l) {
			if (abs(nodalSystem[l][i]) > abs(temp)) {
				a = l;
				temp = nodalSystem[l][i];
			}
		}

		if (i != a) {
			for (l = 1; l < numTotalNodes + 2; ++l) {
				pivot = nodalSystem[i][l];
				nodalSystem[i][l] = nodalSystem[a][l];
				nodalSystem[a][l] = pivot;
			}
		}

		if (abs(temp) < MIN_ERROR_THRESHOLD) {
			cout << "Erro: sistema singular." << endl;
			return(0);
		}

		// To diagonalize the matrix, the condition below must be "j > 0". However, for this programm, it is enought to solve the system, done with the condition "j > i"
		for (j = numTotalNodes + 1; j > 0; --j) {

			nodalSystem[i][j] /= temp;
			pivot = nodalSystem[i][j];

			if (abs(pivot) != 0.0) {
				for (l = 1; l < numTotalNodes + 1; ++l) {
					if (l != i)
						nodalSystem[l][j] -= nodalSystem[l][i] * pivot;
				}
			}
		}
	}

	// The first element of each line of the nodalSolutionVector is the frequency
	nodalSolutionVector.at(0) = Component::getFrequency()/(2*PI);

	// Writes the solution to the nodalSolutionVector to be writen to the output file later
	for (i = 1; i < nodalSolutionVector.capacity(); ++i) {
		nodalSolutionVector.at(i) = nodalSystem[i][nodalSystem.at(i).capacity() - 1];
	}
	return(1);
}

//Compute BJT tensions using Newton Raphson method
int netlistStructure::newtonRaphson() {

	srand(time(0));

	//Initialize solution vector with zeros
	nodalSolutionVector = vector< Complex > (numTotalNodes + 1, 0.0);

	bool converged = false;
	unsigned int countIterations = 0;
	unsigned int countRandomizations = 0;

    while(!converged) {

		//1. Build and solve the nodal system
			buildNodalSystem(0.0);
			if(!solveNodalSystem()) return(0);

			converged = true;

			//Compare new solution with solution from previous iteration
			for (unsigned int index = 1; index <= numTotalNodes; ++index) {
				if (fabs(nodalSolutionVector[index] - nodalSystem[index][numTotalNodes+1]) > MAX_ERROR) {
					converged = false;
					++countIterations;
				}
			}

		//2. If solution has not converged
			if(!converged) {

				//Reset solution vector with ramdom values if maximum number of iterations is reached
				if( (countIterations == NEWTON_RAP_MAX_ITERATIONS) && (countRandomizations < NEWTON_RAP_MAX_RAND)) {

					for (unsigned int index = 1; index <= numTotalNodes; ++index) {
						nodalSolutionVector[index]=((rand()%100)/100.0);
						++countRandomizations;
						countIterations=0;
					}

				//Close program in case maximum number of randomizations is reached
				} else if(countRandomizations == NEWTON_RAP_MAX_RAND) {
					return(0);

				//Copy new solution to nodalSolutionVector, for comparison in the next iteration
				} else {
					for (unsigned int index = 1; index <= numTotalNodes; ++index) {
						nodalSolutionVector[index] = nodalSystem[index][numTotalNodes+1];
					}
				}
			}

		//3. Use new solution as input to update Vbc, Vbe and Vce for every BJT in the circuit
			BJT *tempBJT;
			for (unsigned int index = 0; index < componentNetlist.size(); ++index) {

	            //If component is BJT, update Vbc, Vbe and Vce
				if (componentNetlist.at(index)->type == 'Q') {
					tempBJT = dynamic_cast <BJT *> (componentNetlist.at(index));
					//tempBJT->setTerminalVoltages(nodalSolutionVector);

					tempBJT->Vbc = abs(nodalSolutionVector[tempBJT->nodeBase]) - abs(nodalSolutionVector[tempBJT->nodeCollector]);
					tempBJT->Vbe = abs(nodalSolutionVector[tempBJT->nodeBase]) - abs(nodalSolutionVector[tempBJT->nodeEmitter]);
					tempBJT->Vce = abs(nodalSolutionVector[tempBJT->nodeCollector]) - abs(nodalSolutionVector[tempBJT->nodeEmitter]);
				}
			}
			tempBJT = NULL;
    }
    //If algorithm reaches this point, solution must have converged
    return(1);
}

//Compute and print operating point for all transistors
void netlistStructure::findOperatingPoint() {

	if (hasBJT) {

		double Ic, Ib;
		BJT *tempBJT;

		//Search for every BJT in the netlist
		for (unsigned int index = 0; index < componentNetlist.size(); ++index) {

			if (componentNetlist.at(index)->type == 'Q') {

				tempBJT = dynamic_cast <BJT *> (componentNetlist.at(index));

				//Calculate and print the operating point
				Ic = tempBJT->alfa * tempBJT->setCurrentBE() - tempBJT->setCurrentBC();
				Ib = Ic * (tempBJT->alfa - 1) / (tempBJT->alfa);	 // ic = beta * ib entao ib = ic / beta, sendo beta = alfa / (alfa - 1)

				cout << " TRANSISTOR NAME: " << tempBJT->name;
				cout << " > Vce = " << tempBJT->Vce << endl;
				cout << " > Ic = " << Ic << endl;
				cout << " > Ib = " << Ib << endl;
			}
		}
		tempBJT = NULL;
	}
}

//Perform frequency analysis
void netlistStructure::freqAnalysis() {

	double frequency, step, scaleFactor;
	char option;

	//1. Define scale factor
		if (stepType == "LIN") {
			step = ceil((finalFrequency - inicialFrequency)/(frequencyPoints - 1));
			scaleFactor = step/inicialFrequency + 1;
		}
		else if (stepType == "DEC") {
			scaleFactor = pow(10.0, (1/(frequencyPoints - 1)));
		}
		else if (stepType == "OCT") {
			scaleFactor = pow(2.0, (1/(frequencyPoints - 1)));
		}

	//2. Write the header row of the output table, containing frequency, voltage and current identifiers

		//Frequency header row
		cout << "F";

		//Voltage header row: module and phase
		for (unsigned int index = 0; index < numNodes; ++index) {
			cout << " " << (index + 1) << "m " << (index + 1) << "f";
		}
		//Current header row: module and phase
		for (unsigned int index = 0; index < extraNodeVector.size(); ++index) {
			cout << " " << extraNodeVector[index] << "m " << extraNodeVector[index] << "f";
		}

		cout << endl;


	//3. Build and solve the system for a range of frequencies
		for (frequency = inicialFrequency; frequency <= finalFrequency; frequency += step) {

			buildNodalSystem(2*PI*frequency);

			if (solveNodalSystem()) {
				cout << " Nao foi possivel calcular a analise na frequencia " << frequency << ". Programa abortado." << endl;
				cout << " Pressione qualquer tecla para fechar..." << endl;
				cin.get();

				exit(EXIT_FAILURE);
			}

			cout << scientific << setprecision(4) << abs(nodalSolutionVector.at(0));

			//Write the output splitting values into magnitude and phase (start in 1 to ignore ground node)
			for (unsigned int column = 1; column < nodalSolutionVector.size(); ++column) {
				cout << " " << scientific << setprecision(5) << abs(nodalSolutionVector.at(column))
						    << " " << fixed << setprecision(3) << arg(nodalSolutionVector.at(column))*(180/PI);
			}
			cout << endl; //this line is done!

			//Set step for next iteration
			if (stepType == "LIN")
				scaleFactor = step/frequency + 1;
			else
				step = frequency*(scaleFactor - 1);
		}
	cout << "\n___________________________________________________________________________\n" << endl;
	cout << " Analise em frequencia realizada com sucesso.\n" << endl;
	cout << " Deseja salvar o resultado? [Y/N]";

	while( (option != 'Y') || (option != 'N')) {
		cin.get(option);
	}
	if(option == 'Y') freqAnalysisToFile(scaleFactor);
}

//
void netlistStructure::freqAnalysisToFile(double scaleFactor) {

	double frequency, step;

	string outputFileName;
	ofstream outputFile;

	//Set the file name
	outputFileName = netlistFilePath.substr(0, netlistFilePath.find("."));
	outputFileName += " - Frequency Analysis.tab";

	outputFile.open(outputFileName);

	if (!outputFile) {
		cout << "\n\nErro: nao foi possivel abrir o arquivo." << endl;
		exit(EXIT_FAILURE);
	}

	//1. Write the header of the output table, containing frequency, voltage and current identifiers

		outputFile << "F";

		for (unsigned int index = 0; index < numNodes; ++index) {
			outputFile << " " << (index + 1) << "m " << (index + 1) << "f";
		}

		for (unsigned int index = 0; index < extraNodeVector.size(); ++index) {
			outputFile << " " << extraNodeVector[index] << "m " << extraNodeVector[index] << "f";
		}

		outputFile << endl;

	//3. Write the output for a range of frequencies
		for (frequency = inicialFrequency; frequency <= finalFrequency; frequency += step) {

			buildNodalSystem(2*PI*frequency);
			solveNodalSystem(); //no need to test if solution is right, as we did it before

			outputFile << scientific << setprecision(4) << abs(nodalSolutionVector.at(0));

			for (unsigned int column = 1; column < nodalSolutionVector.size(); ++column) {
				outputFile << " " << scientific << setprecision(5) << abs(nodalSolutionVector.at(column))
						   << " " << fixed << setprecision(3) << arg(nodalSolutionVector.at(column))*(180/PI);
			}

			outputFile << endl;

			//Set step for next iteration
			if (stepType == "LIN")
				scaleFactor = step/frequency + 1;
			else
				step = frequency*(scaleFactor - 1);
		}

	outputFile.close();
	cout << " Arquivo gravado com sucesso. \n Pressione qualquer tecla para continuar..." << outputFileName << "'." << endl;
	cin.get();
}

netlistStructure::~netlistStructure() {
}


