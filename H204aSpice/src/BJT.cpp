#include "BJT.h"

#include <iostream>

using namespace std;

BJT::BJT(string netlistLine) : Component(netlistLine) {

	unsigned int blankPosition;

    blankPosition = netlistLine.find(" ");
	istringstream in(netlistLine.substr(blankPosition + 1));

	in >> nodeCollector >> nodeBase >> nodeEmitter >> bjt_type;

	//The following arguments are optional - test eof before reading and set defaut values if necessary
	if (in.eof()) {
		alfa = 0.995;	alfaR = 0.5;	IsBe = 1e-9;	VtBe = 43.43e-3;	IsBc = 1e-9;	VtBc = 43.43e-3;	VA = 100;
		C0be = 5e-12;	C1be = 100e-18;	C0bc = 5e-12;	C1bc = 100e-1;
	} else {
		in >> alfa >> alfaR >> IsBe >> VtBe >> IsBc >> VtBc >> VA >> C0be >> C1be >> C0bc >> C1bc;
	}

	bjt_Vt = 0.6;
	bjt_n = 0.5;

	Vbc = 0.1;
	Vbe = 0.1;
	Vce = 0.1;

	//If PNP, invert bjt_Vt
	if (bjt_type.compare("PNP") == 0) {
		bjt_Vt = -bjt_Vt;
    }
}


//1. Set and Get methods for Vbc, Vbe and Vce
/*
	//Set new values for ddp between BJT terminals
	void BJT::setTerminalVoltages(ComplexVector &nodalSolutionVector) {
		Vbc = abs(nodalSolutionVector[nodeBase]) - abs(nodalSolutionVector[nodeCollector]);
		Vbe = abs(nodalSolutionVector[nodeBase]) - abs(nodalSolutionVector[nodeEmitter]);
		Vce = abs(nodalSolutionVector[nodeCollector]) - abs(nodalSolutionVector[nodeEmitter]);
	}
*/
    double BJT::getVbc() {
		return(Vbc);
    }

    double BJT::getVbe() {
		return(Vbc);
    }

    double BJT::getVce() {
		return(Vbc);
    }

//2. Methods to compute the parameters of the linear model needed to set the templates

	double BJT::setConductanceBC() {
		return (IsBc/bjt_Vt)*exp(Vbc/VtBc);
	}

	double BJT::setCurrentBC (){
		return  IsBc*(exp(Vbc/VtBc)-1) - (setConductanceBC() * Vbc);
	}

	double BJT::setConductanceBE() {
		return (IsBe/bjt_Vt)*exp(Vbe/VtBe);
	}

	double BJT::setCurrentBE() {
		return  IsBe*(exp(Vbc/VtBe)-1) - (setConductanceBE() *Vbe);
	}

	double BJT::sourceG1(){
		return alfa*(IsBe/VtBe)*exp(Vbe/VtBe)*(bjt_Vt/VA);
	}

	double BJT::sourceG2(){
		return -1*(IsBc/VtBc)*exp(Vbc/VtBe)*(bjt_Vt/VA);
	}

	double BJT::sourceG3(){
		return  (alfa*iDE() - iDC())/VA;
	}

	double BJT::sourceI0(){
	//    Vce = nodalAnalysisMatrix[!!!!!!!!!COLETOR!!!!!!!!!!!!] - nodalAnalysisMatrix[!!!!!!!!!!!!EMISSOR!!!!!!!!!]; //vCE

	   return sourceG3()*Vce - sourceG1()*Vbe - sourceG2()*Vbc - sourceG3()*Vce;
	}

	// double BJT:: currentIc() {

	// 	return - Vbc / GC
	// }

//3. Early Effect

	//Current through diode between base and emitter
	double BJT::iDE(){
		return ((IsBe)*exp(Vbe/VtBe) - 1);
	}

	//Current through diode between base and collector
	double BJT::iDC(){
		return ((IsBc)*exp(Vbc/VtBc) - 1);
	}

//4. Set BJT template
void BJT::setTemplate(ComplexVector &nodalSystem) {

	//Base Coletor
	//Diodo
	//Resistor
	bjt_g = setConductanceBC();
	nodalSystem[nodeBase][nodeBase] += bjt_g;
	nodalSystem[nodeCollector][nodeCollector] += bjt_g;
	nodalSystem[nodeBase][nodeCollector] -= bjt_g;
	nodalSystem[nodeCollector][nodeBase] -= bjt_g;

	//Fonte de corrente
	bjt_g = setCurrentBC();
	nodalSystem[nodeBase][extraNode] += bjt_g;
	nodalSystem[nodeCollector][extraNode] -= bjt_g;

	//Fonte de corrente controlada
	bjt_g = alfa * setConductanceBE();
	nodalSystem[nodeBase][nodeCollector] += bjt_g;
	nodalSystem[nodeCollector][nodeEmitter] += bjt_g;
	nodalSystem[nodeBase][nodeEmitter] -= bjt_g;
	nodalSystem[nodeCollector][nodeCollector] -= bjt_g;

	//Fonte de corrente
	bjt_g = alfa * setCurrentBE();
	nodalSystem[nodeBase][extraNode] -= bjt_g;
	nodalSystem[nodeCollector][extraNode] += bjt_g;

	//Base Emissor
	//Diodo
	//Resistor
	bjt_g = setConductanceBE();
	nodalSystem[nodeCollector][nodeCollector] += bjt_g;
	nodalSystem[nodeEmitter][nodeEmitter] += bjt_g;
	nodalSystem[nodeCollector][nodeEmitter] -= bjt_g;
	nodalSystem[nodeEmitter][nodeCollector] -= bjt_g;

	//Fonte de corrente
	bjt_g = setCurrentBE();
	nodalSystem[nodeCollector][extraNode] -= bjt_g;
	nodalSystem[nodeEmitter][extraNode] += bjt_g;

	//Fonte de corrente controlada
	bjt_g = alfaR * setConductanceBC();
	nodalSystem[nodeCollector][nodeCollector] -= bjt_g;
	nodalSystem[nodeEmitter][nodeBase] -= bjt_g;
	nodalSystem[nodeCollector][nodeBase] += bjt_g;
	nodalSystem[nodeEmitter][nodeCollector] += bjt_g;

	//Fonte de corrente
	bjt_g = alfaR * setCurrentBC();
	nodalSystem[nodeCollector][extraNode] += bjt_g;
	nodalSystem[nodeEmitter][extraNode] -= bjt_g;

	//Elementos entre da Coletor e Emissor
	//implementaçao de early
	//Fonte de corrente
	// sourceI0()


	//bjt_g = sourceI0(); //(1 - alfa*node].alfaR));
	//if(!strcmp(BJT_type,"PNP")) bjt_g =- bjt_g;

	//If PNP, invert bjt_g
	if (bjt_type.compare("PNP") == 0) {
		bjt_g = - sourceI0();
    }
    else {
		bjt_g = sourceI0();
    }
	nodalSystem[nodeBase][extraNode]-=bjt_g;
	nodalSystem[nodeEmitter][extraNode]+=bjt_g;

	// G1*(Vbe*)
	//bjt_g = sourceG1(); //(1 - alfa*node].alfaR));
	//if(!strcmp(BJT_type,"PNP")) bjt_g = - bjt_g;

	//If PNP, invert bjt_g
	if (bjt_type.compare("PNP") == 0) {
		bjt_g = - sourceG1();
    }
    else {
		bjt_g = sourceG1();
    }
	nodalSystem[nodeBase][nodeCollector] += bjt_g;
	nodalSystem[nodeEmitter][nodeEmitter] += bjt_g;
	nodalSystem[nodeBase][nodeEmitter] -= bjt_g;
	nodalSystem[nodeEmitter][nodeCollector] -= bjt_g;

	// G2*GBC
	//bjt_g = sourceG2(); //(1 - alfa*node].alfaR));
	//if(!strcmp(BJT_type,"PNP")) bjt_g = -bjt_g;

	//If PNP, invert bjt_g
	if (bjt_type.compare("PNP") == 0) {
		bjt_g = - sourceG2();
    }
    else {
		bjt_g = sourceG2();
    }
	nodalSystem[nodeBase][nodeCollector] += bjt_g;
	nodalSystem[nodeEmitter][nodeBase] += bjt_g;
	nodalSystem[nodeBase][nodeBase] -= bjt_g;
	nodalSystem[nodeEmitter][nodeCollector] -= bjt_g;

	// G3*GCE
	//bjt_g = sourceG3(); //(1 - alfa*node].alfaR));
	//if(!strcmp(BJT_type,"PNP")) bjt_g = -bjt_g;

	//If PNP, invert bjt_g
	if (bjt_type.compare("PNP") == 0) {
		bjt_g = - sourceG3();
    }
    else {
		bjt_g = sourceG3();
    }
	nodalSystem[nodeBase][nodeBase] += bjt_g;
	nodalSystem[nodeEmitter][nodeEmitter] += bjt_g;
	nodalSystem[nodeBase][nodeEmitter] -= bjt_g;
	nodalSystem[nodeEmitter][nodeBase] -= bjt_g;


//Capacitâncias Junção BC

	if (Vbc > bjt_Vt/2) {
		CbcR = C0bc/pow(1-(bjt_Vt/2),bjt_n);
	}
	else {
		CbcR = C0bc/pow(1-(Vbc/bjt_Vt),bjt_n);
	}
	
	capacitiveAdmittance = Complex(0.0, frequency*Cr);

	nodalSystem[nodeBase][nodeBase] += capacitiveAdmittance;
	nodalSystem[nodeBase][nodeCollector] += conj(capacitiveAdmittance);
	nodalSystem[nodeCollector][nodeBase] += conj(capacitiveAdmittance);
	nodalSystem[nodeCollector][nodeCollector] += capacitiveAdmittance;
	

	if (Vbc > 0) {
		CbcD = (C1bc*exp((Vbc/VtBc)-1);

		capacitiveAdmittance = Complex(0.0, frequency*Cd);

		nodalSystem[nodeBase][nodeBase] += capacitiveAdmittance;
		nodalSystem[nodeBase][nodeCollector] += conj(capacitiveAdmittance);
		nodalSystem[nodeCollector][nodeBase] += conj(capacitiveAdmittance);
		nodalSystem[nodeCollector][nodeCollector] += capacitiveAdmittance;
	}


//Capacitâncias junção BE

	if (Vbe > bjt_Vt/2) {
		CbeR = C0be/pow(1-(bjt_Vt/2),bjt_n);
	}
	else {
		CbeR = C0be/pow(1-(Vbe/bjt_Vt),bjt_n);
	}

	capacitiveAdmittance = Complex(0.0, frequency*Cr);

	nodalSystem[nodeBase][nodeBase] += capacitiveAdmittance;
	nodalSystem[nodeBase][nodeEmitter] += conj(capacitiveAdmittance);
	nodalSystem[nodeEmitter][nodeBase] += conj(capacitiveAdmittance);
	nodalSystem[nodeEmitter][nodeEmitter] += capacitiveAdmittance;
	
	if (Vbe > 0) {
		CbeD = (C1be*exp((Vbe/VtBe)-1);

		capacitiveAdmittance = Complex(0.0, frequency*Cd);

		nodalSystem[nodeBase][nodeBase] += capacitiveAdmittance;
		nodalSystem[nodeBase][nodeEmitter] += conj(capacitiveAdmittance);
		nodalSystem[nodeEmitter][nodeBase] += conj(capacitiveAdmittance);
		nodalSystem[nodeEmitter][nodeEmitter] += capacitiveAdmittance;
	}




BJT::~BJT() {
}
