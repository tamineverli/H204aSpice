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

	Vbc = 0;
	Vbe = 0;
	Vce = 0;



	//If PNP, invert bjt_Vt
	if (bjt_type.compare("PNP") == 0) {
		bjt_Vt = -bjt_Vt;
    }
}

void BJT::print() {

	Component::print();

	cout << " BJT Type: " << bjt_type << endl;
	cout << " Collector Node: " << nodeCollector << endl;
	cout << " Base Node: " << nodeBase << endl;
	cout << " Emitter Node: " << nodeEmitter << endl << endl;

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

//3. Early Effect

	//Current through diode between base and emitter
	double BJT::iDE(){
		return setCurrentBE() + setConductanceBE()*Vbe;
	}

	//Current through diode between base and collector
	double BJT::iDC() {
		return setCurrentBC() + setConductanceBC()*Vbc;
	}

//4. Set BJT template
void BJT::setTemplate(ComplexVector &nodalSystem, vector<Complex> &previousSolutionVector) {

	Vbc = real(previousSolutionVector[nodeBase] - previousSolutionVector[nodeCollector]);
	Vbe = real(previousSolutionVector[nodeBase] - previousSolutionVector[nodeEmitter]);
	Vce = real(previousSolutionVector[nodeCollector] - previousSolutionVector[nodeEmitter]);

	vbcAux = ((Vbc) > 0.7)? 0.7:(Vbc);
	vbeAux = ((Vbe) > 0.7)? 0.7:(Vbe);

	//Base Coletor - Diodo - Resistor
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

	//Fonte de corrente controlada
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

	//Base Emissor - Diodo - Resistor
	bjt_g = setConductanceBE();
	bjt_g = real(bjt_g);
	nodalSystem[nodeBase][nodeBase] += bjt_g;
	nodalSystem[nodeEmitter][nodeEmitter] += bjt_g;
	nodalSystem[nodeBase][nodeEmitter] -= bjt_g;
	nodalSystem[nodeEmitter][nodeBase] -= bjt_g;

	//Current source - if AC, ignore this
	if (frequency == 0) {
		//Fonte de corrente
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

		//Elementos entre da Coletor e Emissor
		//implementaçao de early

		//If PNP, invert bjt_g
		if (bjt_type.compare("PNP") == 0) {
			bjt_g = - sourceI0();
		}
	    else bjt_g = sourceI0();

		nodalSystem[nodeCollector][nodalSystem.size()] -= bjt_g;
		nodalSystem[nodeEmitter][nodalSystem.size()] += bjt_g;
	}

	//If PNP, invert bjt_g
	if (bjt_type.compare("PNP") == 0) {
		bjt_g = - sourceG1();
    }
    else {
		bjt_g = sourceG1();
    }
	nodalSystem[nodeCollector][nodeBase] += bjt_g;
	nodalSystem[nodeEmitter][nodeEmitter] += bjt_g;
	nodalSystem[nodeCollector][nodeEmitter] -= bjt_g;
	nodalSystem[nodeEmitter][nodeBase] -= bjt_g;

	//If PNP, invert bjt_g
	if (bjt_type.compare("PNP") == 0) {
		bjt_g = - sourceG2();
    }
    else {
		bjt_g = sourceG2();
    }
	nodalSystem[nodeCollector][nodeBase] += bjt_g;
	nodalSystem[nodeEmitter][nodeCollector] += bjt_g;
	nodalSystem[nodeCollector][nodeCollector] -= bjt_g;
	nodalSystem[nodeEmitter][nodeBase] -= bjt_g;

	//If PNP, invert bjt_g
	if (bjt_type.compare("PNP") == 0) {
		bjt_g = - sourceG3();
    }
    else {
		bjt_g = sourceG3();
    }
	nodalSystem[nodeCollector][nodeCollector] += bjt_g;
	nodalSystem[nodeEmitter][nodeEmitter] += bjt_g;
	nodalSystem[nodeCollector][nodeEmitter] -= bjt_g;
	nodalSystem[nodeEmitter][nodeCollector] -= bjt_g;


//Capacitances BC

	Complex capacitiveAdmittance;

	//Calculate CbcR
	if (Vbc > bjt_Vt/2) {
		CbcR = C0bc/pow(1/2,bjt_n);
	}
	else {
		CbcR = C0bc/pow(1-(Vbc/bjt_Vt),bjt_n);
	}

	//Only use CbcR when AC
	if (frequency > 0) {
		capacitiveAdmittance = Complex(0.0, frequency*CbcR);
	}
	else capacitiveAdmittance = 0.5*CAPACITIVE_ADMITTANCE_DC;

	nodalSystem[nodeBase][nodeBase] += capacitiveAdmittance;
	nodalSystem[nodeBase][nodeCollector] += (-1.0)*(capacitiveAdmittance);
	nodalSystem[nodeCollector][nodeBase] += (-1.0)*(capacitiveAdmittance);
	nodalSystem[nodeCollector][nodeCollector] += capacitiveAdmittance;


	if (Vbc > 0) {
		CbcD = C1bc*(exp(Vbc/VtBc)-1);

		//Only use CbcD when AC
		if (frequency > 0) {
			capacitiveAdmittance = Complex(0.0, frequency*CbcD);
		}
		else capacitiveAdmittance = 0.5*CAPACITIVE_ADMITTANCE_DC;

		nodalSystem[nodeBase][nodeBase] += capacitiveAdmittance;
		nodalSystem[nodeBase][nodeCollector] += (-1.0)*(capacitiveAdmittance);
		nodalSystem[nodeCollector][nodeBase] += (-1.0)*(capacitiveAdmittance);
		nodalSystem[nodeCollector][nodeCollector] += capacitiveAdmittance;
	}


//Capacitances BE

	if (Vbe > bjt_Vt/2) {
		CbeR = C0be/pow(1/2,bjt_n);
	}
	else {
		CbeR = C0be/pow(1-(Vbe/bjt_Vt),bjt_n);
	}

	//Only use CbeR when AC
	if (frequency > 0) {
		capacitiveAdmittance = Complex(0.0, frequency*CbeR);
	}
	else capacitiveAdmittance = 0.5*CAPACITIVE_ADMITTANCE_DC;

	nodalSystem[nodeBase][nodeBase] += capacitiveAdmittance;
	nodalSystem[nodeBase][nodeEmitter] += (-1.0)*(capacitiveAdmittance);
	nodalSystem[nodeEmitter][nodeBase] += (-1.0)*(capacitiveAdmittance);
	nodalSystem[nodeEmitter][nodeEmitter] += capacitiveAdmittance;

	if (Vbe > 0) {
		CbeD = C1be*(exp(Vbe/VtBe)-1);

		//Only use CbeD when AC
		if (frequency > 0) {
			capacitiveAdmittance = Complex(0.0, frequency*CbeD);
		}
		else capacitiveAdmittance = 0.5*CAPACITIVE_ADMITTANCE_DC;

		nodalSystem[nodeBase][nodeBase] += capacitiveAdmittance;
		nodalSystem[nodeBase][nodeEmitter] += (-1.0)*(capacitiveAdmittance);
		nodalSystem[nodeEmitter][nodeBase] += (-1.0)*(capacitiveAdmittance);
		nodalSystem[nodeEmitter][nodeEmitter] += capacitiveAdmittance;
	}

	//for (int i = 0; i < nodalSystem.capacity(); ++i) {
      //  for (int j  = 0; j < nodalSystem.at(i).capacity(); ++j) {
        //    cout << nodalSystem[i][j];
        //} cout << endl;
	//}


	cout << "\n Transistor: " << name << " | Vbe: " << Vbe << " | VbeAux: " << vbeAux << " | Vbc: " <<
				Vbc << " | VbcAux: " << vbcAux << " | Vce: " << Vce << " | Gc: " << setConductanceBC() << " | Ic: " << setCurrentBC() << " | Ge: "<<
				setConductanceBE() << " | Ie: " << setCurrentBE() << " | G1: " << sourceG1() << " | G2: " << sourceG2()
				<< " | G3: " << sourceG3() << " | I0: " << sourceI0() << endl;

}
BJT::~BJT(){
}
