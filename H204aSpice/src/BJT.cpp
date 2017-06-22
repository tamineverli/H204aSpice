//Transistor bipolar: 
//Q<nome> <nóc> <nób> <nóe> <tipo> <α> <αr> <Isbe> <VTbe> <Isbc> <VTbc> <VA> <C0be> <C1be> <C0bc> <C1bc>
// 0        1     2     3     4     5   6     7     8       9      10    11    12     13     14     15

#include "BJT.h"

BJT::BJT(const string netlistLine) : Component(netlistLine) {

	unsigned int blankPosition;
	
	blankPosition = netlistLine.find(" ");
	nodeC = stoi(netlistLine.substr(0, blankPosition));
	
	netlistLine = netlistLine.substr(blankPosition + 1);
	blankPosition = netlistLine.find(" ");
	nodeB = stoi(netlistLine.substr(0, blankPosition));
	
	netlistLine = netlistLine.substr(blankPosition + 1);
	blankPosition = netlistLine.find(" ");
	nodeE = stoi(netlistLine.substr(0, blankPosition));

	netlistLine = netlistLine.substr(blankPosition + 1);
	blankPosition = netlistLine.find(" ");
	type = netlistLine.substr(0, blankPosition));

	netlistLine = netlistLine.substr(blankPosition + 1);
	blankPosition = netlistLine.find(" ");
	alfa = stod(netlistLine.substr(0, blankPosition));

	netlistLine = netlistLine.substr(blankPosition + 1);
	blankPosition = netlistLine.find(" ");
	alfaR = stod(netlistLine.substr(0, blankPosition));

	netlistLine = netlistLine.substr(blankPosition + 1);
	blankPosition = netlistLine.find(" ");
	Isbe = stod(netlistLine.substr(0, blankPosition));

	netlistLine = netlistLine.substr(blankPosition + 1);
	blankPosition = netlistLine.find(" ");
	VTbe = stod(netlistLine.substr(0, blankPosition));

	netlistLine = netlistLine.substr(blankPosition + 1);
	blankPosition = netlistLine.find(" ");
	Isbc = stod(netlistLine.substr(0, blankPosition));

	netlistLine = netlistLine.substr(blankPosition + 1);
	blankPosition = netlistLine.find(" ");
	VTbc = stod(netlistLine.substr(0, blankPosition));

	netlistLine = netlistLine.substr(blankPosition + 1);
	blankPosition = netlistLine.find(" ");
	VA = stod(netlistLine.substr(0, blankPosition));

	netlistLine = netlistLine.substr(blankPosition + 1);
	blankPosition = netlistLine.find(" ");
	C0be = stod(netlistLine.substr(0, blankPosition));

	netlistLine = netlistLine.substr(blankPosition + 1);
	blankPosition = netlistLine.find(" ");
	C1be = stod(netlistLine.substr(0, blankPosition));

	netlistLine = netlistLine.substr(blankPosition + 1);
	blankPosition = netlistLine.find(" ");
	C0bc = stod(netlistLine.substr(0, blankPosition));

	netlistLine = netlistLine.substr(blankPosition + 1);
	blankPosition = netlistLine.find(" ");
	C1bc = stod (netlistLine.substr(0, blankPosition));

    Vt = 0.6;
    n = 0.5;

//conductanceBC (GC)
	GC = (Isbc/Vt)*exp(Vbc/Vtbc);

//currentBC
	IC = Isbc*(exp(Vbc/Vtbc)-1) - (GC * Vbc);


//Se PNP, inverte v
	if (type == PNP) {
		Vt = -0.6;
	}

//conductanceBE (GE)
	GE  = (Isbe/Vt)*exp(Vbe/Vtbe); 

//currentBE

	 IE = Isbe*(exp(Vbc/Vtbe)-1) - (GE * Vbe);



void BJT::SetTemplate 

nodalSystem[][] += ;



}

BJT::~BJT()
{
    //dtor
}
