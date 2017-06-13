/*


ARQUIVO ELEMENTS DO DANIEL COM A PARTE DE MOSFET COMENTADA NO FINAL 



*/

#include <math.h>
#include <iostream>
#include <iomanip>
#include "elements.h"
#include "analyzer.h"

using namespace std;

const double PI = 3.141592653589793;

double Element::frequency = 0.0;

Element::Element(const string newNetlistLine) : netlistLine (newNetlistLine) {
	
	unsigned int blankPosition;

	type = netlistLine[0];

	blankPosition = netlistLine.find(" ");
	identifier = netlistLine.substr(0, blankPosition);

	netlistLine = netlistLine.substr(blankPosition + 1);

	//If used, the correct value will be set in subsequent constructors
	extraNode = 0;
}

void Element::setFrequency(const double newfrequency) {
	frequency = newfrequency;
}

double Element::getFrequency() {
	return frequency;
}

void Element::print() {
	cout << " Type: " << type << endl;
	cout << " Identifier: " << identifier << endl;
}

Element::~Element() {
}

BasicElement::BasicElement(const string newNetlistLine) : Element(newNetlistLine) {

	unsigned int blankPosition;

	blankPosition = netlistLine.find(" ");
	node1 = stoi(netlistLine.substr(0, blankPosition));
	
	netlistLine = netlistLine.substr(blankPosition + 1);
	blankPosition = netlistLine.find(" ");
	node2 = stoi(netlistLine.substr(0, blankPosition));

	netlistLine = netlistLine.substr(blankPosition + 1);
	value = stod(netlistLine);
}

void BasicElement::print() {

	Element::print();

	cout << " Node1: " << node1 << endl;
	cout << " Node2: " << node2 << endl;
	cout << " ExtraNode: " << extraNode << endl;
	cout << " Value: " << value << endl << endl;
}

BasicElement::~BasicElement() {
}

Amplifier::Amplifier(const string newNetlistLine) : Element(newNetlistLine) {

	unsigned int blankPosition;

	blankPosition = netlistLine.find(" ");
	node1 = stoi(netlistLine.substr(0, blankPosition));
	
	netlistLine = netlistLine.substr(blankPosition + 1);
	blankPosition = netlistLine.find(" ");
	node2 = stoi(netlistLine.substr(0, blankPosition));

	netlistLine = netlistLine.substr(blankPosition + 1);
	blankPosition = netlistLine.find(" ");
	controlNode1 = stoi(netlistLine.substr(0, blankPosition));

	netlistLine = netlistLine.substr(blankPosition + 1);
	blankPosition = netlistLine.find(" ");
	controlNode2 = stoi(netlistLine.substr(0, blankPosition));

	netlistLine = netlistLine.substr(blankPosition + 1);
	gain = stod(netlistLine);
}

void Amplifier::print() {

	Element::print();

	cout << " Node1: " << node1 << endl;
	cout << " Node2: " << node2 << endl;
	cout << " ControlNode1: " << controlNode1 << endl;
	cout << " ControlNode2: " << controlNode2 << endl;
	cout << " ExtraNode: " << extraNode << endl;
	cout << " Gain: " << gain << endl << endl;
}

Amplifier::~Amplifier() {
}

Source::Source(const string newNetlistLine) : Element(newNetlistLine) {

	unsigned int blankPosition;

	blankPosition = netlistLine.find(" ");
	node1 = stoi(netlistLine.substr(0, blankPosition));
	
	netlistLine = netlistLine.substr(blankPosition + 1);
	blankPosition = netlistLine.find(" ");
	node2 = stoi(netlistLine.substr(0, blankPosition));

	netlistLine = netlistLine.substr(blankPosition + 1);
	blankPosition = netlistLine.find(" ");
	magnitude = stod(netlistLine.substr(0, blankPosition));

	netlistLine = netlistLine.substr(blankPosition + 1);
	blankPosition = netlistLine.find(" ");
	phase = stod(netlistLine.substr(0, blankPosition))*PI/180;

	netlistLine = netlistLine.substr(blankPosition + 1);
	DCValue = stod(netlistLine);
}

void Source::print() {

	Element::print();

	cout << " Node1: " << node1 << endl;
	cout << " Node2: " << node2 << endl;
	cout << " ExtraNode: " << extraNode << endl;
	cout << " Magnitude: " << magnitude << endl;
	cout << " Phase: " << phase << endl;
	cout << " DC Value: " << DCValue << endl << endl;
}

Source::~Source() {
}

Resistor::Resistor(const string newNetlistLine) : BasicElement(newNetlistLine) {
}

void Resistor::setTemplate(ComplexVector &nodalAnalysisMatrix) {
	
	Complex conductance(1.0/value, 0.0);

	nodalAnalysisMatrix[node1][node1] += conductance;
	nodalAnalysisMatrix[node1][node2] += (-1.0)*conductance;
	nodalAnalysisMatrix[node2][node1] += (-1.0)*conductance;
	nodalAnalysisMatrix[node2][node2] += conductance;
}

Resistor::~Resistor() {
}

Inductor::Inductor(const string newNetlistLine) : BasicElement(newNetlistLine) {
}

void Inductor::setTemplate(ComplexVector &nodalAnalysisMatrix) {
	
	inductiveReactance = Complex(0.0, frequency*value);

	nodalAnalysisMatrix[node1][extraNode] += 1.0;
	nodalAnalysisMatrix[node2][extraNode] += -1.0;
	nodalAnalysisMatrix[extraNode][node1] += -1.0;
	nodalAnalysisMatrix[extraNode][node2] += 1.0;
	nodalAnalysisMatrix[extraNode][extraNode] += inductiveReactance;
}

Inductor::~Inductor() {
}

Capacitor::Capacitor(const string newNetlistLine) : BasicElement(newNetlistLine) {
}

void Capacitor::setTemplate(ComplexVector &nodalAnalysisMatrix) {
	
	capacitiveAdmittance = Complex(0.0, frequency*value);

	nodalAnalysisMatrix[node1][node1] += capacitiveAdmittance;
	nodalAnalysisMatrix[node1][node2] += conj(capacitiveAdmittance);
	nodalAnalysisMatrix[node2][node1] += conj(capacitiveAdmittance);
	nodalAnalysisMatrix[node2][node2] += capacitiveAdmittance;
}

Capacitor::~Capacitor() {
}

Transformer::Transformer(const string newNetlistLine) : Element(newNetlistLine) {

	unsigned int blankPosition;

	//The attribute netlistLine will be already trimmed to contain only the unitialized values
	blankPosition = netlistLine.find(" ");
	L1Name = netlistLine.substr(0, blankPosition);

	netlistLine = netlistLine.substr(blankPosition + 1);
	blankPosition = netlistLine.find(" ");
	L2Name = netlistLine.substr(0, blankPosition);
	
	netlistLine = netlistLine.substr(blankPosition + 1);
	k = stod(netlistLine);
}

//Gets the value of L1 and L2 from the elementNetlist vector
void Transformer::getInductanceValues(vector< Element *> &elementNetlist) {

	Inductor *tempInductor;

	for (unsigned int index = 0; index < elementNetlist.size(); index++)	{
		if (elementNetlist.at(index)->type == 'L') {
			
			tempInductor = dynamic_cast <Inductor *> (elementNetlist.at(index));

			if (tempInductor->identifier == L1Name) {
				L1 = tempInductor->value;
				extraNode = tempInductor->extraNode;
			}
			else if (tempInductor->identifier == L2Name) {
				L2 = tempInductor->value;
				extraNode2 = tempInductor->extraNode;
			}
		}
	}

	tempInductor = NULL;
}

void Transformer::setTemplate(ComplexVector &nodalAnalysisMatrix) {

	mutualInductance = Complex(0.0, frequency*k*sqrt(L1*L2));

	nodalAnalysisMatrix[extraNode][extraNode2] += mutualInductance;
	nodalAnalysisMatrix[extraNode2][extraNode] += mutualInductance;
}

void Transformer::print() {

	Element::print();

	cout << " L1: " << L1Name << endl;
	cout << " L2: " << L2Name << endl;
	cout << " Indutancia mutua: " << mutualInductance << endl << endl;
}

Transformer::~Transformer() {
}

VoltageAmplifier::VoltageAmplifier(const string newNetlistLine) : Amplifier(newNetlistLine) {
}

void VoltageAmplifier::setTemplate(ComplexVector &nodalAnalysisMatrix) {
	
	nodalAnalysisMatrix[node1][extraNode] += 1.0;
	nodalAnalysisMatrix[node2][extraNode] += -1.0;

	nodalAnalysisMatrix[extraNode][node1] += -1.0;
	nodalAnalysisMatrix[extraNode][node2] += 1.0;
	nodalAnalysisMatrix[extraNode][controlNode1] += gain;
	nodalAnalysisMatrix[extraNode][controlNode2] += (-1.0)*gain;
}

VoltageAmplifier::~VoltageAmplifier() {
}

CurrentAmplifier::CurrentAmplifier(const string newNetlistLine) : Amplifier(newNetlistLine) {
}

void CurrentAmplifier::setTemplate(ComplexVector &nodalAnalysisMatrix) {
	
	nodalAnalysisMatrix[node1][extraNode] += gain;
	nodalAnalysisMatrix[node2][extraNode] += (-1.0)*gain;
	nodalAnalysisMatrix[controlNode1][extraNode] += 1.0;
	nodalAnalysisMatrix[controlNode2][extraNode] += -1.0;

	nodalAnalysisMatrix[extraNode][controlNode1] += -1.0;
	nodalAnalysisMatrix[extraNode][controlNode2] += 1.0;
}

CurrentAmplifier::~CurrentAmplifier() {
}

TransconductanceAmplifier::TransconductanceAmplifier(const string newNetlistLine) : Amplifier(newNetlistLine) {
}

void TransconductanceAmplifier::setTemplate(ComplexVector &nodalAnalysisMatrix) {
	
	nodalAnalysisMatrix[node1][controlNode1] += gain;
	nodalAnalysisMatrix[node1][controlNode2] += (-1.0)*gain;
	nodalAnalysisMatrix[node2][controlNode1] += (-1.0)*gain;
	nodalAnalysisMatrix[node2][controlNode2] += gain;
}

TransconductanceAmplifier::~TransconductanceAmplifier() {
}

TransresistanceAmplifier::TransresistanceAmplifier(const string newNetlistLine) : Amplifier(newNetlistLine) {
}

void TransresistanceAmplifier::setTemplate(ComplexVector &nodalAnalysisMatrix) {
	
	nodalAnalysisMatrix[node1][extraNode2] += 1.0;
	nodalAnalysisMatrix[node2][extraNode2] += -1.0;
	nodalAnalysisMatrix[controlNode1][extraNode] += 1.0;
	nodalAnalysisMatrix[controlNode2][extraNode] += -1.0;

	nodalAnalysisMatrix[extraNode][controlNode1] += -1.0;
	nodalAnalysisMatrix[extraNode][controlNode2] += 1.0;
	nodalAnalysisMatrix[extraNode2][node1] += -1.0;
	nodalAnalysisMatrix[extraNode2][node2] += 1.0;

	nodalAnalysisMatrix[extraNode2][extraNode] += gain;
}

TransresistanceAmplifier::~TransresistanceAmplifier() {
}

CurrentSource::CurrentSource(const string newNetlistLine) : Source(newNetlistLine) {
}

void CurrentSource::setTemplate(ComplexVector &nodalAnalysisMatrix) {

	Complex currentValue;

	if (frequency == 0.0)
		currentValue = DCValue;
	else
		currentValue = polar(magnitude, phase);

	nodalAnalysisMatrix[node1][nodalAnalysisMatrix.size()] += (-1.0)*currentValue;
	nodalAnalysisMatrix[node2][nodalAnalysisMatrix.size()] += currentValue;
}

CurrentSource::~CurrentSource() {
}

VoltageSource::VoltageSource(const string newNetlistLine) : Source(newNetlistLine) {
}

void VoltageSource::setTemplate(ComplexVector &nodalAnalysisMatrix) {

	Complex voltageValue;

	if (frequency == 0.0)
		voltageValue = DCValue;
	else
		voltageValue = polar(magnitude, phase);

	nodalAnalysisMatrix[node1][extraNode] += 1.0;
	nodalAnalysisMatrix[node2][extraNode] += -1.0;
	nodalAnalysisMatrix[extraNode][node1] += -1.0;
	nodalAnalysisMatrix[extraNode][node2] += 1.0;

	nodalAnalysisMatrix[extraNode][nodalAnalysisMatrix.size()] += (-1.0)*voltageValue;
}

VoltageSource::~VoltageSource() {
}

AmpOp::AmpOp(const string newNetlistLine) : Element(newNetlistLine) {

	unsigned int blankPosition;

	blankPosition = netlistLine.find(" ");
	outputNodePositive = stoi(netlistLine.substr(0, blankPosition));
	
	netlistLine = netlistLine.substr(blankPosition + 1);
	blankPosition = netlistLine.find(" ");
	outputNodeNegative = stoi(netlistLine.substr(0, blankPosition));

	netlistLine = netlistLine.substr(blankPosition + 1);
	blankPosition = netlistLine.find(" ");
	inputNodePositive = stoi(netlistLine.substr(0, blankPosition));

	netlistLine = netlistLine.substr(blankPosition + 1);
	inputNodeNegative = stoi(netlistLine);
}

void AmpOp::setTemplate(ComplexVector &nodalAnalysisMatrix) {

	nodalAnalysisMatrix[outputNodePositive][extraNode] += 1.0;
	nodalAnalysisMatrix[outputNodeNegative][extraNode] += -1.0;
	nodalAnalysisMatrix[extraNode][inputNodePositive] += 1.0;
	nodalAnalysisMatrix[extraNode][inputNodeNegative] += -1.0;
}

void AmpOp::print() {

	Element::print();

	cout << " OutputNode+: " << outputNodePositive << endl;
	cout << " OutputNode-: " << outputNodeNegative << endl;
	cout << " InputNode+: " << inputNodePositive << endl;
	cout << " InputNode-: " << inputNodeNegative << endl << endl;
}

AmpOp::~AmpOp() {
}





//------------MOSFET---------------------------
/*
MOSFET::MOSFET(const string newNetlistLine) : Element(newNetlistLine) {

	unsigned int blankPosition;
	
	blankPosition = netlistLine.find(" ");
	nodeD = stoi(netlistLine.substr(0, blankPosition));
	
	netlistLine = netlistLine.substr(blankPosition + 1);
	blankPosition = netlistLine.find(" ");
	nodeG = stoi(netlistLine.substr(0, blankPosition));

	netlistLine = netlistLine.substr(blankPosition + 1);
	blankPosition = netlistLine.find(" ");
	nodeS = stoi(netlistLine.substr(0, blankPosition));

	netlistLine = netlistLine.substr(blankPosition + 1);
	blankPosition = netlistLine.find(" ");
	nodeB = stoi(netlistLine.substr(0, blankPosition));

	netlistLine = netlistLine.substr(blankPosition + 1);
	blankPosition = netlistLine.find(" ");
	MOSType = netlistLine.substr(0, blankPosition);

	netlistLine = netlistLine.substr(blankPosition + 1);
	blankPosition = netlistLine.find(" ");
	L = stod(netlistLine.substr(netlistLine.find("=") + 1, blankPosition));

	netlistLine = netlistLine.substr(blankPosition + 1);
	blankPosition = netlistLine.find(" ");
	W = stod(netlistLine.substr(netlistLine.find("=") + 1, blankPosition));

	netlistLine = netlistLine.substr(blankPosition + 1);
	blankPosition = netlistLine.find(" ");
	K = stod(netlistLine.substr(0, blankPosition));

	netlistLine = netlistLine.substr(blankPosition + 1);
	blankPosition = netlistLine.find(" ");
	Vt0 = stod(netlistLine.substr(0, blankPosition));

	netlistLine = netlistLine.substr(blankPosition + 1);
	blankPosition = netlistLine.find(" ");
	lambda = stod(netlistLine.substr(0, blankPosition));

	netlistLine = netlistLine.substr(blankPosition + 1);
	blankPosition = netlistLine.find(" ");
	gama = stod(netlistLine.substr(0, blankPosition));

	netlistLine = netlistLine.substr(blankPosition + 1);
	blankPosition = netlistLine.find(" ");
	phi = stod(netlistLine.substr(0, blankPosition));

	netlistLine = netlistLine.substr(blankPosition + 1);
	Ld = stod(netlistLine);

	inverted = false;

	// Forces every transistor MOS to start in triode region
	Vgs = 10.0;
	Vds = 0.1;
	Vbs = 0.1;

	if (MOSType == "PMOS") {
		Vgs = -10.0;
		Vds = -0.1;
		Vbs = -0.1;
	}
	else if (MOSType != "NMOS") {
		cout << "Erro: tipo do MOSFET (NMOS/PMOS) invalido. Abortando programa..." << endl;
		exit(EXIT_FAILURE);
	}
	
	calculateParameters();
}

// This function would be used in case of single-MOS circuits, but is not active on the present programm.
void MOSFET::calculateOperatingPoint(vector< Element *> &elementNetlist, const unsigned int numTotalNodes) {
	
	const unsigned int NUM_MAX_ITERATIONS_TO_CONVERGE = 50;
	const double VOLTAGE_CONVERGE_THRESHOLD = 0.005;

	unsigned int solutionIndex, temp;
	bool hasConverged = false;
	
	Analyzer auxiliar(elementNetlist, numTotalNodes);

	// Solves the system on and again until it converges or reaches max number of trials (solutionMatrix.capacity())
	for (solutionIndex = 0; ((solutionIndex < NUM_MAX_ITERATIONS_TO_CONVERGE) && (!hasConverged)); solutionIndex++) {
		
		// Build the nodalAnalysisMatrix for DC (frequency = 0.0)
		auxiliar.buildNodalAnalysisMatrix(0.0);
		auxiliar.solveNodalAnalysisMatrix();
		auxiliar.printNodalAnalysisMatrix();

		// If all of the MOSFET node voltages converge, abort the loop
		if ((abs(auxiliar.solutionMatrix.at(nodeD) - Vd) < VOLTAGE_CONVERGE_THRESHOLD) && 
			(abs(auxiliar.solutionMatrix.at(nodeG) - Vg) < VOLTAGE_CONVERGE_THRESHOLD) && 
			(abs(auxiliar.solutionMatrix.at(nodeS) - Vs) < VOLTAGE_CONVERGE_THRESHOLD) && 
			(abs(auxiliar.solutionMatrix.at(nodeB) - Vb) < VOLTAGE_CONVERGE_THRESHOLD)) {
			
			hasConverged = true;
		}

		if ((MOSType == "PMOS") && (abs(auxiliar.solutionMatrix.at(nodeD)) > abs(auxiliar.solutionMatrix.at(nodeS)))) {
			temp = nodeD;
			nodeD = nodeS;
			nodeS = temp;
			inverted = true;
		}

		Vd = real(auxiliar.solutionMatrix.at(nodeD));
		Vg = real(auxiliar.solutionMatrix.at(nodeG));
		Vs = real(auxiliar.solutionMatrix.at(nodeS));
		Vb = real(auxiliar.solutionMatrix.at(nodeB));

		Vds = Vd - Vs;
		Vgs = Vg - Vs;
		Vbs = Vb - Vs;

		calculateParameters();
	}

	if (!hasConverged) {
		cout << "Erro: sistema nao convergiu na definicao do ponto de operacao do MOSFET '" << identifier << "'." << endl;
		exit(EXIT_FAILURE);
	}

	cout << "Sistema convergiu com " << solutionIndex << " iteracoes: MOSFET '" << identifier << "' inicializado." << endl << endl;
}



void MOSFET::setTemplate(ComplexVector &nodalAnalysisMatrix) {

	Complex CgsAdmittance, CgdAdmittance, CbgAdmittance;

	//Current Source, to be added only when calculating operating point
	if (frequency == 0.0) {
		nodalAnalysisMatrix[nodeD][nodalAnalysisMatrix.size()] += (-1.0)*I0;
		nodalAnalysisMatrix[nodeS][nodalAnalysisMatrix.size()] += I0;
	}

	//Resistor
	nodalAnalysisMatrix[nodeD][nodeD] += Gds;
	nodalAnalysisMatrix[nodeD][nodeS] += (-1.0)*Gds;
	nodalAnalysisMatrix[nodeS][nodeD] += (-1.0)*Gds;
	nodalAnalysisMatrix[nodeS][nodeS] += Gds;

	//Controled Current Source 1
	nodalAnalysisMatrix[nodeD][nodeG] += Gm;
	nodalAnalysisMatrix[nodeD][nodeS] += (-1.0)*Gm;
	nodalAnalysisMatrix[nodeS][nodeG] += (-1.0)*Gm;
	nodalAnalysisMatrix[nodeS][nodeS] += Gm;

	//Controled Current Source 2
	nodalAnalysisMatrix[nodeD][nodeB] += Gmb;
	nodalAnalysisMatrix[nodeD][nodeS] += (-1.0)*Gmb;
	nodalAnalysisMatrix[nodeS][nodeB] += (-1.0)*Gmb;
	nodalAnalysisMatrix[nodeS][nodeS] += Gmb;

	//Insert the parasite capacitance
	CgsAdmittance = Complex(0.0, frequency*Cgs);
	CgdAdmittance = Complex(0.0, frequency*Cgd);
	CbgAdmittance = Complex(0.0, frequency*Cbg);

	nodalAnalysisMatrix[nodeG][nodeG] += CgsAdmittance;
	nodalAnalysisMatrix[nodeG][nodeS] += conj(CgsAdmittance);
	nodalAnalysisMatrix[nodeS][nodeG] += conj(CgsAdmittance);
	nodalAnalysisMatrix[nodeS][nodeS] += CgsAdmittance;

	nodalAnalysisMatrix[nodeG][nodeG] += CgdAdmittance;
	nodalAnalysisMatrix[nodeG][nodeD] += conj(CgdAdmittance);
	nodalAnalysisMatrix[nodeD][nodeG] += conj(CgdAdmittance);
	nodalAnalysisMatrix[nodeD][nodeD] += CgdAdmittance;

	nodalAnalysisMatrix[nodeB][nodeB] += CbgAdmittance;
	nodalAnalysisMatrix[nodeB][nodeG] += conj(CbgAdmittance);
	nodalAnalysisMatrix[nodeG][nodeB] += conj(CbgAdmittance);
	nodalAnalysisMatrix[nodeG][nodeG] += CbgAdmittance; 
}


void MOSFET::calculateParameters() {
	
	//dVt is Vt derivative in function of Vbs
	double SqrtPhiMinusVbs, VgsMinusVt, KtimesWdividedL, onePlusLambdaTimesVds, mi, Cox, CoxTimesWTimesLd, CoxTimesWTimesL;

	if (abs(Vbs) > phi/2) {
		if (MOSType == "NMOS")
			//Vbs = phi/2;
			Vbs = -(1.67)*phi;
		else
			//Vbs = phi/2;
			Vbs = 0.0;
	}

	SqrtPhiMinusVbs = sqrt(phi - Vbs);
	Vt = Vt0 + gama*(SqrtPhiMinusVbs - sqrt(phi));
	
	if (MOSType == "NMOS") {
		
		VgsMinusVt = Vgs - Vt;
		mi = 0.05;

		if (Vgs < Vt)
			operatingRegion = "cortado";
		else if (Vds < VgsMinusVt)
			operatingRegion = "triodo";
		else
			operatingRegion = "saturado";
	}
	else {

		Vt *= -1.0;
		VgsMinusVt = Vgs - Vt;
		mi = 0.02;

		if (Vgs > Vt)
			operatingRegion = "cortado";
		else if (Vds > VgsMinusVt)
			operatingRegion = "triodo";
		else
			operatingRegion = "saturado";

		VgsMinusVt *= -1.0;
		Vds *= -1.0;
	}

	Cox = 2*K/mi;
	CoxTimesWTimesLd = Cox*W*Ld;
	CoxTimesWTimesL = Cox*W*L;
	KtimesWdividedL = K*W/L;
	onePlusLambdaTimesVds = 1 + lambda*Vds;

	if (operatingRegion == "cortado") {
		Id = 0.0;
		Gds = 0.0;
		Gm = 0.0;
		Gmb = 0.0;
		Cgs = CoxTimesWTimesLd;
		Cgd = CoxTimesWTimesLd;
		Cbg = CoxTimesWTimesL;
	}
	else if (operatingRegion == "triodo") {
		Id = KtimesWdividedL*(2*VgsMinusVt*Vds - pow(Vds, 2))*onePlusLambdaTimesVds;
		Gds = KtimesWdividedL*(2*VgsMinusVt - 2*Vds + 4*lambda*VgsMinusVt*Vds - 3*lambda*pow(Vds, 2));
		Gm = KtimesWdividedL*2*Vds*onePlusLambdaTimesVds;
		Gmb = Gm*gama/(2*SqrtPhiMinusVbs);		
		Cgs = CoxTimesWTimesLd + 0.5*CoxTimesWTimesL;
		Cgd = Cgs;
		Cbg = 0.0;
	}
	else {
		Id = KtimesWdividedL*pow(VgsMinusVt, 2)*onePlusLambdaTimesVds;
		Gds = KtimesWdividedL*pow(VgsMinusVt, 2)*lambda;
		Gm = KtimesWdividedL*2*VgsMinusVt*onePlusLambdaTimesVds;
		Gmb = Gm*gama/(2*SqrtPhiMinusVbs);
		Cgs = CoxTimesWTimesLd + (2.0/3.0)*CoxTimesWTimesL;
		Cgd = CoxTimesWTimesLd;
		Cbg = 0.0;
	}

	if (MOSType == "PMOS") {
		Vds *= -1.0;
		Vt *= -1.0;
		Id *= -1.0;
	}

	I0 = Id - Gm*Vgs - Gmb*Vbs - Gds*Vds;	
}

void MOSFET::print() {

	Element::print();

	cout << scientific << setprecision(4);

	cout << " NodeD: " << nodeD << endl;
	cout << " NodeG: " << nodeG << endl;
	cout << " NodeS: " << nodeS << endl;
	cout << " NodeB: " << nodeB << endl;
	cout << " MOSType: " << MOSType << endl;
	cout << " Regiao de operacao: ";
	if (inverted)
		cout << "invertido ";
	cout  << operatingRegion << endl;
	cout << " L: " << L << "  \t\t" << " Vt0: " << Vt0 << endl;
	cout << " W: " << W << "  \t\t" << " Lambda: " << lambda << endl;
	cout << " K: " << K << "  \t\t" << " Gama: " << gama << endl;
	cout << " Ld: " << Ld << " \t\t" << " Phi: " << phi << endl;
	cout << " Vt: " << Vt << " \t\t" << " Id: " << Id << endl;
	cout << " Vgs: " << Vgs << "\t\t" << " Vds: " << Vds << "\t\t" << " Vbs: " << Vbs << endl;
	cout << " Gm: " << Gm << " \t\t" << " Gds: " << Gds << "\t\t" << " Gmb: " << Gmb << endl;
	cout << " Cgs: " << Cgs << "\t\t" << " Cgd: " << Cgd << "\t\t" << " Cbg: " << Cbg << endl << endl;
}

MOSFET::~MOSFET() {
*/
}