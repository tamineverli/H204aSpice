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

	//Set Vbc, Vbe and Vce for first iteration
	Vbc = 0;
	Vbe = 0;
	Vce = 0;


	//If PNP, invert many things
	if (bjt_type.compare("PNP") == 0) {
		IsBc = -IsBc;
		IsBe = -IsBe;
		VtBc = -VtBc;
		VtBe = -VtBe;
		VA = -VA;
		bjt_Vt = -bjt_Vt;
    }
}

void BJT::print() {

	Component::print();

	cout << " Tipo de BJT: " << bjt_type << endl;
	cout << " No Coletor: " << nodeCollector << endl;
	cout << " No Base: " << nodeBase << endl;
	cout << " No Emissor: " << nodeEmitter << endl << endl;

}

//1. Methods to compute the parameters of the linear model needed to set the templates

	double BJT::setConductanceBC() {
		return (IsBc/VtBc)*exp(vbcAux/VtBc);
	}

	double BJT::setCurrentBC (){
		return  IsBc*(exp(vbcAux/VtBc)-1) - (setConductanceBC() * vbcAux);
	}

	double BJT::setConductanceBE() {
		return (IsBe/VtBe)*exp(vbeAux/VtBe);
	}

	double BJT::setCurrentBE() {
		return  IsBe*(exp(vbeAux/VtBe)-1) - (setConductanceBE() *vbeAux);
	}

//2. Early Effect

	double BJT::sourceG1(){
		return alfa*setConductanceBE()*Vce/VA;
	}

	double BJT::sourceG2(){
		return -1*setConductanceBC()*Vce/VA;
	}

	double BJT::sourceG3(){
		return  (alfa*iDE() - iDC())/VA;
	}

	double BJT::sourceI0(){
	   return - (sourceG1()*Vbe) - (sourceG2()*Vbc);
	}

	//Current through diode between base and emitter
	double BJT::iDE(){
		return setCurrentBE() + setConductanceBE()*Vbe;
	}

	//Current through diode between base and collector
	double BJT::iDC() {
		return setCurrentBC() + setConductanceBC()*Vbc;
	}

//3. Set BJT template
void BJT::setTemplate(ComplexVector &nodalSystem, vector<Complex> &previousSolutionVector) {

	//Set Vbc, Vbe and Vce using the most recent solution
	Vbc = real(previousSolutionVector[nodeBase] - previousSolutionVector[nodeCollector]);
	Vbe = real(previousSolutionVector[nodeBase] - previousSolutionVector[nodeEmitter]);
	Vce = real(previousSolutionVector[nodeCollector] - previousSolutionVector[nodeEmitter]);

	//If PNP, invert more things
	if (bjt_type.compare("PNP") == 0) {
		vbcAux = ((Vbc) < -0.7)? -0.7:(Vbc);
		vbeAux = ((Vbe) < -0.7)? -0.7:(Vbe);

		//Calculate CbcR
		if (Vbc < bjt_Vt/2) {
			CbcR = C0bc/pow(0.5,bjt_n);
		}
		else CbcR = C0bc/pow(1.0 - (vbcAux/bjt_Vt),bjt_n);

		if (Vbc < 0) {
			CbcD = C1bc*(exp(vbcAux/VtBc)-1);
		} else CbcD = 0;

		if (Vbe < bjt_Vt/2) {
			CbeR = C0be/pow(0.5,bjt_n);
		}
		else CbeR = C0be/pow(1.0-(vbeAux/bjt_Vt),bjt_n);

		if (Vbe < 0) {
			CbeD = C1be*(exp(vbeAux/VtBe) - 1);
		} else CbeD = 0;

	}	//If NPN...
	else {
		vbcAux = ((Vbc) > 0.7)? 0.7:(Vbc);
		vbeAux = ((Vbe) > 0.7)? 0.7:(Vbe);

		//Calculate CbcR
		if (Vbc > bjt_Vt/2) {
			CbcR = C0bc/pow(0.5,bjt_n);
		}
		else {
			CbcR = C0bc/pow(1.0 - (vbcAux/bjt_Vt),bjt_n);
		}
		if (Vbc > 0) {
			CbcD = C1bc*(exp(vbcAux/VtBc)-1);
		} else CbcD = 0;

		if (Vbe > bjt_Vt/2) {
			CbeR = C0be/pow(0.5,bjt_n);
		}
		else CbeR = C0be/pow(1.0-(vbeAux/bjt_Vt),bjt_n);

		if (Vbe > 0) {
			CbeD = C1be*(exp(vbeAux/VtBe) - 1);
		} else CbeD = 0;
	}

	//Base Collector - Diode - Resistor
	bjt_g = setConductanceBC();
	bjt_g = real(bjt_g);
	nodalSystem[nodeBase][nodeBase] += bjt_g;
	nodalSystem[nodeCollector][nodeCollector] += bjt_g;
	nodalSystem[nodeBase][nodeCollector] -= bjt_g;
	nodalSystem[nodeCollector][nodeBase] -= bjt_g;

	//Current source - if AC, ignore this
	if (frequency == 0) {
		bjt_g = setCurrentBC();
		bjt_g = real(bjt_g);
		nodalSystem[nodeBase][nodalSystem.size()] -= bjt_g;
		nodalSystem[nodeCollector][nodalSystem.size()] += bjt_g;
	}

	//Controlled current source
	bjt_g = alfa * setConductanceBE();
	bjt_g = real(bjt_g);
	nodalSystem[nodeCollector][nodeBase] += bjt_g;
	nodalSystem[nodeCollector][nodeEmitter] -= bjt_g;
	nodalSystem[nodeBase][nodeEmitter] += bjt_g;
	nodalSystem[nodeBase][nodeBase] -= bjt_g;

	//Current source - if AC, ignore this
	if (frequency == 0) {
		bjt_g = alfa * setCurrentBE();
		bjt_g = real(bjt_g);
		nodalSystem[nodeBase][nodalSystem.size()] += bjt_g;
		nodalSystem[nodeCollector][nodalSystem.size()] -= bjt_g;
	}

	//Base Emitter - Diode - Resistor
	bjt_g = setConductanceBE();
	bjt_g = real(bjt_g);
	nodalSystem[nodeBase][nodeBase] += bjt_g;
	nodalSystem[nodeEmitter][nodeEmitter] += bjt_g;
	nodalSystem[nodeBase][nodeEmitter] -= bjt_g;
	nodalSystem[nodeEmitter][nodeBase] -= bjt_g;

	//Current source - if AC, ignore this
	if (frequency == 0) {
		//Current Source
		bjt_g = setCurrentBE();
		bjt_g = real(bjt_g);
		nodalSystem[nodeBase][nodalSystem.size()] -= bjt_g;
		nodalSystem[nodeEmitter][nodalSystem.size()] += bjt_g;
	}

	//Fonte de corrente controlada
	bjt_g = alfaR * setConductanceBC();
	bjt_g = real(bjt_g);
	nodalSystem[nodeBase][nodeBase] -= bjt_g;
	nodalSystem[nodeEmitter][nodeCollector] -= bjt_g;
	nodalSystem[nodeBase][nodeCollector] += bjt_g;
	nodalSystem[nodeEmitter][nodeBase] += bjt_g;

	//Current sources - if AC, ignore this
	if (frequency == 0) {

		bjt_g = alfaR * setCurrentBC();
		bjt_g = real(bjt_g);
		nodalSystem[nodeBase][nodalSystem.size()] += bjt_g;
		nodalSystem[nodeEmitter][nodalSystem.size()] -= bjt_g;

		//Elementos entre Coletor e Emissor
		//implementaçao de early
		bjt_g = sourceI0();

		nodalSystem[nodeCollector][nodalSystem.size()] -= bjt_g;
		nodalSystem[nodeEmitter][nodalSystem.size()] += bjt_g;
	}


	bjt_g = sourceG1();

	nodalSystem[nodeCollector][nodeBase] += bjt_g;
	nodalSystem[nodeEmitter][nodeEmitter] += bjt_g;
	nodalSystem[nodeCollector][nodeEmitter] -= bjt_g;
	nodalSystem[nodeEmitter][nodeBase] -= bjt_g;

	bjt_g = sourceG2();

	nodalSystem[nodeCollector][nodeBase] += bjt_g;
	nodalSystem[nodeEmitter][nodeCollector] += bjt_g;
	nodalSystem[nodeCollector][nodeCollector] -= bjt_g;
	nodalSystem[nodeEmitter][nodeBase] -= bjt_g;

	bjt_g = sourceG3();

	nodalSystem[nodeCollector][nodeCollector] += bjt_g;
	nodalSystem[nodeEmitter][nodeEmitter] += bjt_g;
	nodalSystem[nodeCollector][nodeEmitter] -= bjt_g;
	nodalSystem[nodeEmitter][nodeCollector] -= bjt_g;


	//Capacitances BC

	Complex capacitiveAdmittance;

	//Only use CbcR when AC
	if (frequency > 0) {
		capacitiveAdmittance = Complex(0.0, frequency*CbcR);
	}
	else capacitiveAdmittance = 0.5*CAPACITIVE_ADMITTANCE_DC;

	nodalSystem[nodeBase][nodeBase] += capacitiveAdmittance;
	nodalSystem[nodeBase][nodeCollector] += (-1.0)*(capacitiveAdmittance);
	nodalSystem[nodeCollector][nodeBase] += (-1.0)*(capacitiveAdmittance);
	nodalSystem[nodeCollector][nodeCollector] += capacitiveAdmittance;

	//Only use CbcD when AC
	if (frequency > 0) {
		capacitiveAdmittance = Complex(0.0, frequency*CbcD);
	}
	else capacitiveAdmittance = 0.5*CAPACITIVE_ADMITTANCE_DC;

	nodalSystem[nodeBase][nodeBase] += capacitiveAdmittance;
	nodalSystem[nodeBase][nodeCollector] += (-1.0)*(capacitiveAdmittance);
	nodalSystem[nodeCollector][nodeBase] += (-1.0)*(capacitiveAdmittance);
	nodalSystem[nodeCollector][nodeCollector] += capacitiveAdmittance;

	//Capacitances BE

	//Only use CbeR when AC
	if (frequency > 0) {
		capacitiveAdmittance = Complex(0.0, frequency*CbeR);
	}
	else capacitiveAdmittance = 0.5*CAPACITIVE_ADMITTANCE_DC;

	nodalSystem[nodeBase][nodeBase] += capacitiveAdmittance;
	nodalSystem[nodeBase][nodeEmitter] += (-1.0)*(capacitiveAdmittance);
	nodalSystem[nodeEmitter][nodeBase] += (-1.0)*(capacitiveAdmittance);
	nodalSystem[nodeEmitter][nodeEmitter] += capacitiveAdmittance;

	//Only use CbeD when AC
	if (frequency > 0) {
		capacitiveAdmittance = Complex(0.0, frequency*CbeD);
	}
	else capacitiveAdmittance = 0.5*CAPACITIVE_ADMITTANCE_DC;

	nodalSystem[nodeBase][nodeBase] += capacitiveAdmittance;
	nodalSystem[nodeBase][nodeEmitter] += (-1.0)*(capacitiveAdmittance);
	nodalSystem[nodeEmitter][nodeBase] += (-1.0)*(capacitiveAdmittance);
	nodalSystem[nodeEmitter][nodeEmitter] += capacitiveAdmittance;

	//Print transistor parameters (debug)
	/*
	cout << "\n Transistor: " << name << " | Vbe: " << Vbe << " | VbeAux: " << vbeAux << " | Vbc: " <<
				Vbc << " | VbcAux: " << vbcAux << " | Vce: " << Vce << " | Cbe: " << (CbeD+CbeR) <<
				" | Cbc: " << (CbcD+CbcR) << " | Gc: " << setConductanceBC() << " | Ic: "
				<< setCurrentBC() << " | Ge: "<< setConductanceBE() << " | Ie: " << setCurrentBE() <<
				" | G1: " << sourceG1() << " | G2: " << sourceG2() << " | G3: " << sourceG3() << " | I0: " << sourceI0() << endl;
	*/
}

BJT::~BJT(){
}
