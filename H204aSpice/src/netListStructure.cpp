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

#define ZERO_ERROR_THRESHOLD 1e-9
#define NEWTON_RAP_MAX_ITERATIONS 200
#define NEWTON_RAP_MAX_RAND 100
#define NEWTON_RAP_STEP 0.05
#define MAX_ERROR 0.05
#define PI 3.14


using namespace std;

netlistStructure::netlistStructure(const string validFilePath) : netlistFilePath(validFilePath) {

	//	1. DECLARE LOCAL VARIABLES

		ifstream netlistFile;	//netlist input by user
		string fileLine;		//to assist in reading each netlist line

		Component *newComponent;	//to store new components
		char componentType;			//to store new components' types

		bool setExtraNode, validComponent;

	//	2. PRESET CLASS VARIABLES

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
					case 'Q':	//BJT
						newComponent = new BJT(fileLine);
						hasBJT = true;
						break;

				//Command Line
					case '.':
						setCommandLineParameters(fileLine);
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

		cout << "\n NETLIST com " << componentNetlist.size() << " componentes e " << numNodes << " nos.\n" << endl;
		cout << "_____________________________________________________\n" << endl;

	//	4. SET OTHER CIRCUIT PARAMETERS
			//Update total number of nodes, considering extra nodes
			numTotalNodes = numNodes + extraNodeVector.size();

			//Check if frequency analysis parameters have been set
			if (!validCommandLine) {
				setCommandLineParameters("defautParameters");
			}

	//	5 . SET NODAL SYSTEM

		//Initialize the nodal system with zeros - line and column [0,0] represent the ground


		//Build nodal system
		//buildNodalSystem(0.0);

		//Allocate memory for the output matrix - first line stores frequency values
		nodalSolutionVector = vector< Complex > (numTotalNodes + 1, 0.0);

		//Element netlist is created in this object
		//isElementNetlistShared = false;

	//	5. USE NEWTON RAPHSON TO SET BJT VOLTAGES AND CURRENTS
		newtonRaphson();

		printOperatingPoint();

	//	6. UPDATE NODAL SYSTEM

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
	const double defautInicialFreq = 0;
	const double defautFinalFreq = 1;
	const double defautStep = 1;

	//Set parameters with defaut values if necessary
	if (commandLine == "defautParameters") {
		stepType = defautStepType;
		step = defautStep;
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

		commandLineInput >> stepType >> step >> inicialFrequency >> finalFrequency;

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

	cout << "> Step Type = " << stepType << endl;
	cout << "> Step = " << step << endl;
	cout << "> inicialFrequency = " << inicialFrequency << endl;
	cout << "> finalFrequency = " << finalFrequency << endl;

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
	return;
}

//Solve the nodal system matrix
int netlistStructure::solveNodalSystem() {

	//const double ZERO_ERROR_THRESHOLD = 1e-9;

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

		if (abs(temp) < ZERO_ERROR_THRESHOLD) {
			cout << "Erro: sistema singular.";
			return(1);
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
	return(0);
}

//Compute BJT tensions using Newton Raphson method
void netlistStructure::newtonRaphson() {

//nv tem que virar nosso numero total de variáveis
//tem que declarar solução atual e ultima solução, e todo o resto
//mudar as funções de abrir arquivo

	srand(time(0));

	//Initialize solution vector with zeros
	nodalSolutionVector = vector< Complex > (numTotalNodes + 1, 0.0);

    bool converged = false;
	unsigned int countIterations = 0;
	unsigned int countRandomizations = 0;

    // loop para definir o maximo de tentativas de newtempoAtivo-raphson
    while (!converged) {

        if ( (countIterations == NEWTON_RAP_MAX_ITERATIONS) && (countRandomizations < NEWTON_RAP_MAX_RAND)) {

            for (unsigned int index = 1; index <= numTotalNodes; ++index) {
                nodalSolutionVector[index]=((rand()%100)/100.0);
	            ++countRandomizations;
	            countIterations=0;
            }
        }

        if (countRandomizations == NEWTON_RAP_MAX_RAND) {
            printf("\nOs calculos nao convergiram.\n");
            exit(1);
        }

        // monta a matriz de condutancia e resolve o sistema para o passo interno atual
        buildNodalSystem(0.0); //montarEstampas();
        solveNodalSystem();//resolverSistema();

        converged = true;

		for (unsigned int index = 1; index <= numTotalNodes; ++index) {
            if (fabs(nodalSolutionVector[index] - nodalSystem[index][numTotalNodes+1]) > MAX_ERROR) {
                converged = false;
        	    ++countIterations;
			}
        }

        // Copia a solucao atual para o vetor solucao
        for (unsigned int index = 1; index <= numTotalNodes; ++index) {
            nodalSolutionVector[index] = nodalSystem[index][numTotalNodes+1];
        }

    	BJT *tempBJT;

		for (unsigned int index = 0; index < componentNetlist.size(); ++index) {

			if (componentNetlist.at(index)->type == 'Q') {

				tempBJT = dynamic_cast <BJT *> (componentNetlist.at(index));
				tempBJT->Vbc = nodalSolutionVector[tempBJT->nodeB] - nodalSolutionVector[tempBJT->nodeC];
    			tempBJT->Vbe = nodalSolutionVector[tempBJT->nodeB] - nodalSolutionVector[tempBJT->nodeE];
    			tempBJT->Vce = nodalSolutionVector[tempBJT->nodeC] - nodalSolutionVector[tempBJT->nodeE];
			}
		}

		tempBJT = NULL;


    }
}

//Find operating point for all transistors
void netlistStructure::findOperatingPoint() {

	return;
}

//Operating point output
void netlistStructure::printOperatingPoint() {

	if (hasBJT) {

		BJT *tempBJT;

		double ic_temp, Ib;

		for (unsigned int index = 0; index < componentNetlist.size(); ++index) {

			if (componentNetlist.at(index)->type == 'Q') {

				tempBJT = dynamic_cast <BJT *> (componentNetlist.at(index));

				ic_temp = tempBJT->alfa * tempBJT->currentBE() - tempBJT->currentBC();
				Ib = ic_temp * (tempBJT->alfa - 1) / (tempBJT->alfa);	 // ic = beta * ib entao ib = ic / beta, sendo beta = alfa / (alfa - 1)

				cout << "> TRANSISTOR NAME: " << tempBJT->name;
				cout << "> Vce = " << tempBJT->Vce << endl;
				cout << "> Ic = " << tempBJT->Ic << endl;
				cout << "> Ib = " << Ib << endl;
			}
		}

		tempBJT = NULL;
	}
	return;
}

//Perform frequency analysis
void netlistStructure::freqAnalysis() {

	return;
}

//Frequency analysis output
void netlistStructure::printFreqAnalysis() {

	return;
}

netlistStructure::~netlistStructure() {
}
