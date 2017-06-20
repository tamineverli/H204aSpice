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


#include "supportFunctions.h"
#include "netListStructure.h"

using namespace std;

int main(int argc, char** argv)
{

//	0. WELCOME!
	welcomeHeader();

//  1. DECLARE MAIN PROGRAM VARIABLES

	//int numero_variaveis;
	//string metodo = "DC";
	//double passo = 0.1;
	//double tempo_final = 1.0;
	//unsigned passos_por_ponto = 1;

	//int totalNodeNum;
	//string netlistName, commandLine, outputFileName;


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
		netlistStructure netlistStructure(netlistPath); //This is where the fun begins

		cout << "Pronto para iniciar a analise!" << endl;
		cout << "Pressione qualquer tecla para continuar..." << endl;
		cin.get();

/*


	vector<string> lista;
	vector<Elemento> netlist(1);
	std::vector<std::vector<long double>> resultados;
	// vector<Elemento> componentesVariantes;
	// vector<Elemento> componentesNaoLineares;



    // Reading the netlist - can take optional command line argument as filename*/
/*		string nomeArquivo = "";
	if (argc == 2)
		nomeArquivo = argv[1];
	readNetList(lista, netlist, componentesVariantes, argc, nomeArquivo, numero_variaveis, tempo_final, passo, metodo, passos_por_ponto, componentesNaoLineares);

#ifdef DEBUG
	cout << "Metodo de simulacao: " << metodo << endl;
#endif

	/* DC */
	/* Lista tudo */
/*		vector<vector<long double>> Yn(numero_variaveis+1, vector<long double>(numero_variaveis+2));

	listarVariaveis(lista, numero_variaveis);
	cin.get();

	mostrarNetlist(netlist);
	cin.get();

	/* Monta o sistema nodal modificado */
//	montarSistemaDC(netlist, Yn, numero_variaveis);

	/* Resolve o sistema
	if (resolverSistema(Yn, numero_variaveis))
	{
		cin.get();
		exit(ERRO_RESOLUCAO_SISTEMA);
	}
	#ifdef DEBUG
		/*Opcional: Mostra o sistemar resolvido*/
//		mostrarSistema("Sistema resolvido: ", Yn, numero_variaveis);
	//	cin.get();
//	#endif

	/* Mostra solucao
	cout << "Solucao:" << endl;
	string txt = "Tesao";

	for (int i=1; i<=numero_variaveis; i++)
	{
		if (i==numero_nos+1)
		{
			txt = "Corrente";
		}
		cout << txt << " " << lista[i] << ": " << Yn[i][numero_variaveis+1] << endl;
	}

	cin.get();
*/
  return 0;

}
