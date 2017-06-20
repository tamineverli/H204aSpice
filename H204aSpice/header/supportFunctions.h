#ifndef SUPPORTFUNCTIONS_H_INCLUDED
#define SUPPORTFUNCTIONS_H_INCLUDED

#include <iostream>
#include <stdlib.h>
#include <string>

using namespace std;
using std::string;

	//Gives a warm welcome to the user
	void welcomeHeader();

	//Cleans the console and prints the program header
    void programHeader();

	//Asks the user for a valid netlist name and returns its path
    string askNetlistPath();

#endif // SUPPORTFUNCTIONS_H_INCLUDED


