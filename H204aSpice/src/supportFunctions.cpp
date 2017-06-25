#include "../header/supportFunctions.h"

using namespace std;
using std::string;

//Gives a warm welcome to the user
void welcomeHeader() {

    system("cls");	//Cleans the console

	cout << "\nH204a Spice, um trabalho de Circuitos Eletricos II\n" << endl;
	cout << "Versao: 1.1 - lancada em 20.06.2017\n\n" << endl;

	cout << "Organizacao: " << endl;
	cout << "Universidade Federal do Rio de Janeiro\n" << endl;

	cout << "Autores: " << endl;
	cout << "- Marina Torres \n- Tamine Verli \n- Victor Costa \n" << endl;

	cout << "Seja bem vindo! Pressione qualquer tecla para comecar..." << endl;
	cin.get();
}

//Cleans the console and prints the program header
void programHeader() {

    system("cls");	//Cleans the console

	cout << "\nH204a Spice - Circuitos Eletricos II\n" << endl;
}

//Asks the user for a valid netlist name and returns its path
string askNetlistPath() {

	string netlistName;

	cout << "Informe o nome do arquivo de NETLIST que deseja analisar: ";
	getline(cin, netlistName);
	return("C:/H204aSpice/bin/Debug/netlists/" + netlistName);
	//return("netlists/" + netlistName);
}
