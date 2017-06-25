/*
Universidade Federal do Rio de Janeiro
Departamento de Engenharia Eletrônica

Programa que analisa circuitos lineares contendo transistores bipolares, encontrando o ponto de
operação e a resposta em frequência para pequenos sinais

Por: Marina Torres (marinat94@poli.ufrj.br)
     Tamine Verli (tamine.verli@poli.ufrj.br)
     Victor Barros (victorbarros@poli.ufrj.br)


 * Resistor    -  R<name> <node 1> <node 2> <value>
 * Capacitor   -  C<name> <node 1> <node 2> <value>
 * Inductor    -  L<name> <node 1> <node 2> <value>
 *
 * Transformer -  K<name> <node 1+> <node 1-> <node 2+> <node 2-> <n>
 *
 * Op.Amp.     -  O<name> <node out+> <node out-> <node in+> <node in->
 *
 * V.C.V.S.    -  E<name> <node out+> <node out-> <node in+> <node in-> <Gain>
 * C.C.C.S.    -  F<name> <node out+> <node out-> <node in+> <node in-> <Gain>
 * V.C.C.S.    -  G<name> <node out+> <node out-> <node in+> <node in-> <Gain>
 * C.C.V.S.    -  H<name> <node out+> <node out-> <node in+> <node in-> <Gain>
 *
 * Diode       -  D<name> <node +> <node ->
 * Switch      -  $<name> <node +> <node -> <control node +> <control node -> <limit voltage>
*/


#include <iostream>
#include <stdlib.h>
#include <fstream>
#include <iomanip>
#include <string>
#include <vector>

#include "../header/supportFunctions.h"
#include "../header/netListStructure.h"


using namespace std;

int main(int argc, char** argv)
{

//	0. WELCOME!
	welcomeHeader();

//  1. DECLARE MAIN PROGRAM VARIABLES

//  2. READ NETLIST

	//	2.1 Ask for a valid netlist file name
		string netlistPath;
		ifstream netlistFile;

	//	2.2 Ask for a valid netlist file name
		programHeader();
		netlistPath = askNetlistPath();

	//	2.3 Check if the file exists
	    netlistFile.open(netlistPath.c_str());

	//	2.4 If file name is invalid, try again
		while (!netlistFile) {
	        programHeader();
			cout << "ARQUIVO INVALIDO \n" << endl;

			netlistPath = askNetlistPath();
		    netlistFile.open(netlistPath.c_str());
		}

	// 2.5 Call netlist builder
		programHeader();
		cout << "\nLendo NETLIST...\n\n" << endl;

		netlistStructure myCircuit(netlistPath); //This is where the fun begins

		cout << " Pronto para iniciar a analise!" << endl;
		cout << " Pressione qualquer tecla para continuar..." << endl;
		cin.get();

//	3. OPERATING POINT
		programHeader();
		cout << "\nAnalisando o circuito...\n\n" << endl;

		if(myCircuit.hasBJT) {

			if(myCircuit.newtonRaphson()) {

				cout << "___________________________________________\n" << endl;
				cout << "Ponto de operacao dos transistores:\n" << endl;
				myCircuit.findOperatingPoint();
				
				cout << "\n___________________________________________\n" << endl;
				cout << " Pressione qualquer tecla para prosseguir para a analise de frequencia..." << endl;
				cin.get();

			} else {
				cout << "O sistema nodal nao convergiu: ponto de operacao nao encontrado.\n" << endl;
				cin.get();
				return(0);
			}
		}

//	4. FREQUENCY ANALYSIS
		
		//programHeader();
		//cout << "\nAnalise em frequencia...\n\n" << endl;
		//myCircuit.freqAnalysis();
		//myCircuit.printFreqAnalysis();

  	return 0;

}
