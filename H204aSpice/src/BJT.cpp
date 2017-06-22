#include "BJT.h"

BJT::BJT(const string netlistLine) : Component(netlistLine) {

unsigned int blankPosition;

    blankPosition = netlistLine.find(" ");
	istringstream in(netlistLine.substr(blankPosition + 1));

	//cout << "paramentros lidos " << netlistLine << endl;  //debug
	in >> nodeC >> nodeB >> nodeE >> type >> alfa >> alfaR >> Isbe >> VTbe >> Isbc >> VTbc >> VA >> C0be >> C1be >> C0bc >> C1bc;

//Primeios valores de Vbc e Vbe são chutados
    Vbc = 0.0;
    Vbe = 0.0;
}


BJT nome_BJT = new BJT(string_doarquivo)

//*************************************************************
//Cálculo de parâmetros do modelo linearizado necessários para montar a estampa
//*************************************************************

double BJT::conductanceBC (void){
	GC = (Isbc/Vt)*exp(Vbc/Vtbc);
	return GC;
}

double BJT::currentBC (void){
	IC = Isbc*(exp(Vbc/Vtbc)-1) - (GC * Vbc);
	return IC;
}

//Se PNP, inverte v
if (type == "PNP") {
	Vt = -Vt;
}

double BJT::conductanceBE (void){
	GE  = (Isbe/Vt)*exp(Vbe/Vtbe); 
	return GE;
}

double BJT::currentBE (void){
	IE = Isbe*(exp(Vbc/Vtbe)-1) - (GE * Vbe);
	return IE;
}



//*************************************************************
// Adicionando o efeito Early 
//*************************************************************

//Corrente que passa no diodo entre base e emissor
double BJT::iDE(void){

/*  !!!!!   TENHO QUE COLOCAR ISSO NA PRIMEIRA ITERAÇÃO LÁ FORA	   !!!!!!
	Vbe = Vt;

	if (type == "PNP"){
		Vt=-Vt;
	}
*/
	iDE = ((Isbe)*exp(Vbe/Vtbe) - 1);
	return iDE;
}

//Corrente que passa no diodo entre base e coletor
double BJT::iDC(void){
/*  !!!!!   TENHO QUE COLOCAR ISSO NA PRIMEIRA ITERAÇÃO LÁ FORA	   !!!!!!
	Vbc = Vt;

	if (type == "PNP"){
		Vt=-Vt;
	}
*/

	iDC = ((Isbc)*exp(Vbc/Vtbc) - 1);
	return iDC;
}

double BJT::fonteG1(void){
	V2 = Vt;

	G1 = (alfa*(Isbe/Vtbe)*exp(Vbe/Vtbe)*(V2/VA));
	return G1;
}

double BJT::fonteG2(void){
	G2 = -1*((Isbc/Vtbc)*exp(Vb/Vtbe)*(V2/VA));
	return G2;
}

double BJT::fonteG3(void){
	G3 = ((alfa*iDE() - iDC())/(VA));
	return G3;
}

double BJT::fonteI0(void){
   I0 = (fonteG3()*Vce - fonteG1()*Vbe - fonteG2()*Vbc - fonteG3()*Vce);
   return I0;
}

void BJT::SetTemplate(void) {
        //Estampa do Transistor
            //Base Coletor
            // Diodo
            //Resistor
            g=conductanceBC();
            nodalSystem[nodeB][nodeB]+=g;
            nodalSystem[nodeC][nodeC]+=g;
            nodalSystem[nodeB][nodeC]-=g;
            nodalSystem[nodeC][nodeB]-=g;
            //Fonte de corrente
            g=currentBC();
            nodalSystem[nodeB][extraNode]+=g;
            nodalSystem[nodeC][extraNode]-=g;
            //Fonte de corrente controlada
            g=alfa*conductanceBE();
            nodalSystem[nodeB][nodeC]+=g;
            nodalSystem[nodeC][nodeE]+=g;
            nodalSystem[nodeB][nodeE]-=g;
            nodalSystem[nodeC][nodeC]-=g;
            //Fonte de corrente
            g=alfa*currentBE();
            nodalSystem[nodeB][extraNode]-=g;
            nodalSystem[nodeC][extraNode]+=g;
            //Base Emissor
            // Diodo
            //Resistor
            g=conductanceBE();
            nodalSystem[nodeC][nodeC]+=g;
            nodalSystem[nodeE][nodeE]+=g;
            nodalSystem[nodeC][nodeE]-=g;
            nodalSystem[nodeE][nodeC]-=g;
            //Fonte de corrente
            g=currentBE();
            nodalSystem[nodeC][extraNode]-=g;
            nodalSystem[nodeE][extraNode]+=g;
            //Fonte de corrente controlada
            g=alfaR*conductanceBC();
            nodalSystem[nodeC][nodeC]-=g;
            nodalSystem[nodeE][nodeB]-=g;
            nodalSystem[nodeC][nodeB]+=g;
            nodalSystem[nodeE][nodeC]+=g;
            //Fonte de corrente
            g=alfaR*currentBC();
            nodalSystem[nodeC][extraNode]+=g;
            nodalSystem[nodeE][extraNode]-=g;

#ifdef ativarEarly
            //Elementos entre da Coletor e Emissor
            //implementaçao de early
            //Fonte de corrente
            // fonteI0()

            g = fonteI0(); //(1 - alfa*node].alfaR));
            if(!strcmp(type,"PNP")) g=-g;
            //printf("%d  ", g);
            nodalSystem[nodeB][extraNode]-=g;
            nodalSystem[nodeE][extraNode]+=g;

            // G1*VBE
            g = finteG1(); //(1 - alfa*node].alfaR));
            if(!strcmp(type,"PNP")) g=-g;
            // printf("%d  ", g);
            nodalSystem[nodeB][nodeC]+=g;
            nodalSystem[nodeE][nodeE]+=g;
            nodalSystem[nodeB][nodeE]-=g;
            nodalSystem[nodeE][nodeC]-=g;

            // G2*GBC
            g = fonteG2(); //(1 - alfa*node].alfaR));
            if(!strcmp(type,"PNP")) g=-g;
            // printf("%d  ", g);
            nodalSystem[nodeB][nodeC]+=g;
            nodalSystem[nodeE][nodeB]+=g;
            nodalSystem[nodeB][nodeB]-=g;
            nodalSystem[nodeE][nodeC]-=g;

            // G3*GCE
            g = fonteG3(); //(1 - alfa*node].alfaR));
            if(!strcmp(type,"PNP")) g=-g;
            // printf("%d  ", g);
            nodalSystem[nodeB][nodeB]+=g;
            nodalSystem[nodeE][nodeE]+=g;
            nodalSystem[nodeB][nodeE]-=g;
            nodalSystem[nodeE][nodeB]-=g;

}


BJT::~BJT()
{
    //dtor
}
