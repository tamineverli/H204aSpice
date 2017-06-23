#include "BJT.h"

#include <cstring>

using namespace std;

BJT::BJT(string netlistLine) : Component(netlistLine) {

unsigned int blankPosition;

    blankPosition = netlistLine.find(" ");
	istringstream in(netlistLine.substr(blankPosition + 1));

	//cout << "paramentros lidos " << netlistLine << endl;  //debug
    in >> nodeC >> nodeB >> nodeE >> type >> alfa >> alfaR >> Isbe >> VTbe >> Isbc >> VTbc >> VA >> C0be >> C1be >> C0bc >> C1bc;

    Vt = 0.6;
    n = 0.5;

    Vbc = 0.1;
    Vbe = 0.1;
    Vce = 0.1;

    //Se PNP, inverte v
    if (!strcmp(BJT_type,"PNP")) {
        Vt = -Vt;
    }
}

//Calculo de parâmetros do modelo linearizado necessários para montar a estampa
double BJT::conductanceBC() {
	return (Isbc/Vt)*exp(Vbc/VTbc);
}

double BJT::currentBC (){
	return  Isbc*(exp(Vbc/VTbc)-1) - (conductanceBC() * Vbc);
}

double BJT::conductanceBE() {
	return (Isbe/Vt)*exp(Vbe/VTbe);
}

double BJT::currentBE() {
	return  Isbe*(exp(Vbc/VTbe)-1) - (conductanceBE() *Vbe);
}

//*************************************************************
// Adicionando o efeito Early
//*************************************************************

//Corrente que passa no diodo entre base e emissor
double BJT::iDE(){
	return ((Isbe)*exp(Vbe/VTbe) - 1);
}

//Corrente que passa no diodo entre base e coletor
 double BJT::iDC(){
/*  !!!!!   TENHO QUE COLOCAR ISSO NA PRIMEIRA ITERAÇÃO LÁ FORA	   !!!!!!
	Vbc = Vt;

	if (type == "PNP"){
		Vt=-Vt;
	}
*/

	return ((Isbc)*exp(Vbc/VTbc) - 1);
}

double BJT::fonteG1(){
	return alfa*(Isbe/VTbe)*exp(Vbe/VTbe)*(Vt/VA);

}


double BJT::fonteG2(){
	return -1*(Isbc/VTbc)*exp(Vbc/VTbe)*(Vt/VA);
}

double BJT::fonteG3(){
	return  (alfa*iDE() - iDC())/VA;
}

double BJT::fonteI0(){

//    Vce = nodalAnalysisMatrix[!!!!!!!!!COLETOR!!!!!!!!!!!!] - nodalAnalysisMatrix[!!!!!!!!!!!!EMISSOR!!!!!!!!!]; //vCE

   return fonteG3()*Vce - fonteG1()*Vbe - fonteG2()*Vbc - fonteG3()*Vce;
}



void BJT::SetTemplate(ComplexVector &nodalSystem) {
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

            //Elementos entre da Coletor e Emissor
            //implementaçao de early
            //Fonte de corrente
            // fonteI0()

            g = fonteI0(); //(1 - alfa*node].alfaR));
            if(!strcmp(BJT_type,"PNP")) g=-g;
            //printf("%d  ", g);
            nodalSystem[nodeB][extraNode]-=g;
            nodalSystem[nodeE][extraNode]+=g;

            // G1*(Vbe*)
            g = fonteG1(); //(1 - alfa*node].alfaR));
            if(!strcmp(BJT_type,"PNP")) g=-g;
            // printf("%d  ", g);
            nodalSystem[nodeB][nodeC]+=g;
            nodalSystem[nodeE][nodeE]+=g;
            nodalSystem[nodeB][nodeE]-=g;
            nodalSystem[nodeE][nodeC]-=g;

            // G2*GBC
            g = fonteG2(); //(1 - alfa*node].alfaR));
            if(!strcmp(BJT_type,"PNP")) g=-g;
            // printf("%d  ", g);
            nodalSystem[nodeB][nodeC]+=g;
            nodalSystem[nodeE][nodeB]+=g;
            nodalSystem[nodeB][nodeB]-=g;
            nodalSystem[nodeE][nodeC]-=g;

            // G3*GCE
            g = fonteG3(); //(1 - alfa*node].alfaR));
            if(!strcmp(BJT_type,"PNP")) g=-g;
            // printf("%d  ", g);
            nodalSystem[nodeB][nodeB]+=g;
            nodalSystem[nodeE][nodeE]+=g;
            nodalSystem[nodeB][nodeE]-=g;
            nodalSystem[nodeE][nodeB]-=g;


/*
            if (Vbc == Vt/2) {
                    Cr = C0bc/(1-VbcVt)^n;

                  capacitiveAdmittance = Complex(0.0, frequency*Cr);

                  nodalSystem[nodeB][nodeB] += capacitiveAdmittance;
                  nodalSystem[nodeB][nodeC] += conj(capacitiveAdmittance);
                  nodalSystem[nodeC][nodeB] += conj(capacitiveAdmittance);
                  nodalSystem[nodeC][nodeC] += capacitiveAdmittance;
            }


            if (Vbc > 0) {
                  Cd = (C1bc*expVbcVT)-1;
                  capacitiveAdmittance = Complex(0.0, frequency*Cd);

                  nodalSystem[nodeB][nodeB] += capacitiveAdmittance;
                  nodalSystem[nodeB][nodeC] += conj(capacitiveAdmittance);
                  nodalSystem[nodeC][nodeB] += conj(capacitiveAdmittance);
                  nodalSystem[nodeC][nodeC] += capacitiveAdmittance;
            }

            if ( Vbe == Vt/2) {
                    Cr = C0bc/(1-(Vbe/Vt))^n;

                  capacitiveAdmittance = Complex(0.0, frequency*Cr);

                  nodalSystem[nodeB][nodeB] += capacitiveAdmittance;
                  nodalSystem[nodeB][nodeC] += conj(capacitiveAdmittance);
                  nodalSystem[nodeC][nodeB] += conj(capacitiveAdmittance);
                  nodalSystem[nodeC][nodeC] += capacitiveAdmittance;
            }


            if (Vbe* > 0) {
                  Cd = (C1bc*exp(Vbe/VT)-1);
                  capacitiveAdmittance = Complex(0.0, frequency*Cd);

                  nodalSystem[nodeB][nodeB] += capacitiveAdmittance;
                  nodalSystem[nodeB][nodeC] += conj(capacitiveAdmittance);
                  nodalSystem[nodeC][nodeB] += conj(capacitiveAdmittance);
                  nodalSystem[nodeC][nodeC] += capacitiveAdmittance;
            }
*/
}


BJT::~BJT()
{
    //dtor
}
