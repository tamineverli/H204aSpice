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
#include <fstream>
#include <iomanip>
#include <string>
#include <vector>

#ifndef NOT_WINDOWS
#include <conio.h>
#endif

#include "analyzer.h"
#include "elements.h"

using namespace std;

int main(int argc, char* argv[]) {
	
	string netlistFileName, commandLine, outputFileName;
	ifstream netlistFile;

	// Clear the console
	#ifndef NOT_WINDOWS
	system("cls");
	#endif

	cout << "Trabalho de Circuitos Eletricos II\n\n" << endl;
	cout << "Membros do Grupo: " << endl;
	cout << "Marina Torres, Tamine Verli e Victor Costa" << endl;


	if (argc < 2) {
		cout << "Entre com o nome do arquivo de NETLIST: ";
		getline(cin, netlistFileName);
	}
	else {
		netlistFileName = string(argv[1]);
	}

	netlistFile.open(netlistFileName);
	while (!netlistFile) {
		cout << "Erro: nao foi possivel abrir o arquivo de entrada." << endl;
		cout << "Entre com um nome de arquivo valido: ";
		getline(cin, netlistFileName);
	}

	Analyzer analyzer(netlistFileName);

	if (argc >= 3) {
		commandLine = string(argv[2]);
		analyzer.initializeCommandLine(commandLine);
	}
	
	while (!analyzer.hasCommandLine) {
		cout << "Linha de comando invida ou nao encontrada." << endl;
		cout << "Digite uma linha de comando comecando de formato '.AC <LIN/OCT/DEC> <n_pontos> <freq_inicio> <freq_fim>': ";
		getline(cin, commandLine);
		analyzer.initializeCommandLine(commandLine);
	}

	cout << "Elementos inicializados para a analise nodal:" << endl << endl;
	for (unsigned int index = 0; index < analyzer.elementNetlist.size(); index++) {
		analyzer.elementNetlist.at(index)->print();
	}

	analyzer.writeOperatingPoint();
	
	cout << "Realizando analise em frequencia..." << endl << endl;
	
	analyzer.calculateFrequencyAnalysis();

	cout << "Programa terminado. Pressione uma tecla para fechar..." << endl;

	#ifndef NOT_WINDOWS
	getch();
	system("cls");
	#endif

	return 0;
}
