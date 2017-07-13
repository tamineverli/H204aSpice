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
#include <string.h>
#include <vector>
#include <sstream>

#define MIN_ERROR_THRESHOLD 1e-12
#define NEWTON_RAP_MAX_ITERATIONS 200
#define NEWTON_RAP_MAX_RAND 100
#define NEWTON_RAP_STEP 0.05
#define MAX_ERROR_ABSOLUTE 1e-11
#define MAX_ERROR 1e-9
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

						//Locate coupled inductors in netlist
						CoupledInductors *tempCouple;
						tempCouple = dynamic_cast <CoupledInductors *> (newComponent);

						tempCouple->locateInductors(componentNetlist);
						tempCouple->mutualInductance = 0;				//Initialize mutual inductance

						tempCouple = NULL;
						break;

				//BJT
						case('Q'):
							newComponent = new BJT(fileLine);
							hasBJT = true;
							break;

				//Command Line and Comments
						case '.':
							setCommandLineParameters(fileLine);
							validComponent = false;
							break;

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
		nodalSolutionVector = vector< Complex > (numTotalNodes + 1, 0.1);

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
	const double defautFinalFreq = 60;
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

		commandLineInput >> stepType;

		//If step type is not valid, set it with its defaut value
		if ((stepType != "DEC") && (stepType != "OCT") && (stepType != "LIN")) {
			cout << "\n ALERTA: tipo de analise nao reconhecido. Opcao padrao selecionada." << endl;
			stepType = defautStepType;
		}

		//Set defaut value for frequency points if input is invalid
		if (commandLineInput.eof())
			frequencyPoints = defautPoints;			//set defaut value
		else commandLineInput >> frequencyPoints;	//set input value

		//Set defaut value for frequency range if input is invalid
		if (commandLineInput.eof()) {
			inicialFrequency = defautInicialFreq;	//set defaut value
			finalFrequency = defautFinalFreq;		//set defaut value
		}
		else commandLineInput >> inicialFrequency >> finalFrequency;	//set input value

		validCommandLine = true;

		cout << "\n Parametros de analise em frequencia configurados com sucesso:" << endl;
	}

	cout << " > Pontos = " << frequencyPoints << endl;
	cout << " > Tipo de Passo = " << stepType << endl;
	cout << " > Faixa de Frequencia: " << inicialFrequency << "Hz to " << finalFrequency << "Hz\n" << endl;

	return;
}

//Build nodal system for a given frequency
void netlistStructure::buildNodalSystem(const double frequency, vector<Complex> &previousSolutionVector) {

	Component::setFrequency(frequency);

	//Initialize nodal system matrix with zeros - line and column [0,0] represent the ground node
	nodalSystem = vector< vector< Complex > > (numTotalNodes + 1, vector<Complex>(numTotalNodes + 2, 0.0));

	//Save templates to file
	//char buf[1000],tmp[1000];

	for (unsigned int index = 0; index < componentNetlist.size(); ++index) {
		componentNetlist.at(index)->setTemplate(nodalSystem, previousSolutionVector);
        /*
		estampaLogFile.open("templateLogFile.txt", ios::app);

		estampaLogFile << "\nSistema apos a estampa de " << componentNetlist.at(index)->name;
		estampaLogFile << " (" << frequency << ") " << endl;

		for (int k=1; k<=numTotalNodes; k++) {
			strcpy(buf,"");
			for (int j=1; j<=numTotalNodes+1; j++) {
				if (abs(nodalSystem[k][j])!=0) sprintf(tmp,"%+6.2e%+6.2ei ",real(nodalSystem[k][j]),imag(nodalSystem[k][j]));
				else sprintf(tmp,"..... ..... ");
				strcat(buf,tmp);
			}
			estampaLogFile << endl;
			estampaLogFile << buf;
		}

		estampaLogFile.close(); */
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
			cout << "\nErro: sistema singular." << endl;
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
	//nodalSolutionVector.at(0) = Component::getFrequency()/(2*PI);

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
	previousSolutionVector = vector< Complex > (numTotalNodes + 1, 0.1);

	previousSolutionVector[0] = 0.0;

	bool converged = false;
	unsigned int countIterations = 0;
	unsigned int countRandomizations = 0;

	while(!converged) {

		//1. Build and solve the nodal system
		buildNodalSystem(0.0, previousSolutionVector);
		if(!solveNodalSystem()) return(0);

			//Reset solution vector with ramdom values if maximum number of iterations is reached
		if( (countIterations >= NEWTON_RAP_MAX_ITERATIONS) && (countRandomizations < NEWTON_RAP_MAX_RAND)) {

			for (unsigned int index = 1; index <= numTotalNodes; ++index) {
				nodalSolutionVector[index]=((rand()%100)/100.0);
				++countRandomizations;
				countIterations = 0;
			}

			//Close program in case maximum number of randomizations is reached
		} else if(countRandomizations == NEWTON_RAP_MAX_RAND) {
			return(0);
		}

		//Compare new solution with solution from previous iteration
		nodalSolutionVector[0] = 0;
		converged = true;
		for (unsigned int index = 1; index <= numTotalNodes; ++index) {

            //debug
			//cout << "\nAntiga: " << previousSolutionVector[index] << " Atual: " << nodalSolutionVector[index] << endl;

            // RELATIVE ERROR = (current solution - previous solution) / previous solution
			if (abs( (previousSolutionVector[index] - nodalSolutionVector[index])
				/ previousSolutionVector[index]) > MAX_ERROR) {

				converged = false;
			}
		}

		//Copy new solution to nodalSolutionVector, for comparison in the next iteration
		previousSolutionVector = nodalSolutionVector;

		//Use new solution as input to update Vbc, Vbe and Vce for every BJT in the circuit
		BJT *tempBJT;
		for (unsigned int index = 0; index < componentNetlist.size(); ++index) {
		            //If component is BJT, update Vbc, Vbe and Vce
			if (componentNetlist.at(index)->type == 'Q') {
				tempBJT = dynamic_cast <BJT *> (componentNetlist.at(index));
						//tempBJT->setTerminalVoltages(nodalSolutionVector);

				tempBJT->Vbc = abs(nodalSolutionVector[tempBJT->nodeBase] - nodalSolutionVector[tempBJT->nodeCollector]);
				tempBJT->Vbe = abs(nodalSolutionVector[tempBJT->nodeBase] - nodalSolutionVector[tempBJT->nodeEmitter]);
				tempBJT->Vce = abs(nodalSolutionVector[tempBJT->nodeCollector] - nodalSolutionVector[tempBJT->nodeEmitter]);
			}
		}

		tempBJT = NULL;
		++countIterations;
	}

    //If algorithm reaches this point, solution must have converged
	return(1);
}

//Print BJT operating point
void netlistStructure::findBJTOperatingPoint() {

	cout << " \nTRANSISTORES:\n" << endl;

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

				cout << " Transistor: " << tempBJT->name << endl;
				cout << " > Vce = " << tempBJT->Vce << endl;
				cout << " > Ic = " << Ic << endl;
				cout << " > Ib = " << Ib << endl;
				cout << endl;
			}
		}
		tempBJT = NULL;
	}

}

//Used to debug
void netlistStructure::printOutputToFile(std::vector<std::complex<double>> v) {

	logFile.open(".\logFile.txt",ios::app);

	if (!logFile) {
		cout << "\n\nErro: nao foi possivel abrir o arquivo." << endl;
		exit(EXIT_FAILURE);
	}

	for (int i = 0; i < numTotalNodes; ++i)
	{
		logFile << v[i] << "\t";
	}
	logFile << endl;
	logFile.close();

}

//Print nodal system
void netlistStructure::printNodalSystem() {

	char buf[1000],tmp[1000];

	printf("\n Sistema resolvido:\n");

	for (unsigned int k = 1; k <= numTotalNodes; k++) {

		strcpy(buf,"");

		for (unsigned int j=1; j <= numTotalNodes + 1; j++) {

			if (abs(nodalSystem[k][j]) != 0) sprintf( tmp, "%+6.2f ", nodalSystem[k][j]);
			else sprintf(tmp,"...... ");
			strcat(buf,tmp);
		}
		cout << endl;
		printf(buf);
	}
}

//
void netlistStructure::freqAnalysisToFile() {

	double frequency, step, scaleFactor;;

	string outputFileName;
	ofstream outputFile;

	//1. Create and open the file
		outputFileName = netlistFilePath.substr(0, netlistFilePath.find("."));
		outputFileName += " - Frequency Analysis.tab";

		outputFile.open(outputFileName);

		if (!outputFile) {
			cout << "\n\nErro: nao foi possivel abrir o arquivo." << endl;
			exit(EXIT_FAILURE);
		}

	//2. Calculate scale factor
		if (stepType == "LIN") {
			step = (finalFrequency - inicialFrequency)/(frequencyPoints - 1);
		}
		else if (stepType == "DEC") {
			scaleFactor = pow(10, (1.0/(frequencyPoints - 1)));
		}
		else if (stepType == "OCT") {
			scaleFactor = pow(2, (1.0/(frequencyPoints - 1)));
		}

	//3. Write the header of the output table, containing frequency, voltage and current identifiers

		outputFile << "f";

		for (unsigned int index = 0; index < numNodes; ++index) {
			outputFile << " " << (index + 1) << "m " << (index + 1) << "f";
		}

		for (unsigned int index = 0; index < extraNodeVector.size(); ++index) {
			outputFile << " " << extraNodeVector[index] << "m " << extraNodeVector[index] << "f";
		}

		outputFile << endl;

	//4. Write the output for a range of frequencies
		for (frequency = inicialFrequency; frequency <= finalFrequency; ) {

			buildNodalSystem(2*PI*frequency, previousSolutionVector);
			solveNodalSystem(); //no need to test if solution is right, as we did it before

			outputFile << scientific << setprecision(10) << frequency;

			for (unsigned int column = 1; column < nodalSolutionVector.size(); ++column) {
				outputFile << " " << scientific << setprecision(15) << abs(nodalSolutionVector.at(column))
				<< " " << fixed << setprecision(15) << arg(nodalSolutionVector.at(column))*(180/PI);
			}

			outputFile << endl;

			//Set step for next iteration
			if (stepType == "LIN")
				frequency += step;
			else
				frequency *= scaleFactor;
		}

		outputFile.close();

		cout << "\n Arquivo gravado com sucesso em:\n " << outputFileName << endl << endl;
		cout << " Pressione qualquer tecla para continuar...";
		cin.get();
	}

	netlistStructure::~netlistStructure() {
	}


