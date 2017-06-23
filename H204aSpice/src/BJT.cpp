#include "BJT.h"

BJT::BJT(const string netlistLine) : Component(netlistLine) {

unsigned int blankPosition;

    blankPosition = netlistLine.find(" ");
	istringstream in(netlistLine.substr(blankPosition + 1));

	//cout << "paramentros lidos " << netlistLine << endl;  //debug
	in >> nodeC >> nodeB >> nodeE >> type >> alfa >> alfaR >> Isbe >> VTbe >> Isbc >> VTbc >> VA >> C0be >> C1be >> C0bc >> C1bc;

}


BJT nome_BJT = new BJT(string_doarquivo)


//*************************************************************
//Cálculo de parâmetros do modelo linearizado necessários para montar a estampa
//*************************************************************
double BJT::conductanceBC (void){
	return (Isbc/Vt)*exp(Vbc/Vtbc);
}

double BJT::currentBC (void){
	return  Isbc*(exp(Vbc/Vtbc)-1) - (conductanceBC() * Vbc);
}

//Se PNP, inverte v
if (type == "PNP") {
	Vt = -Vt;
}

double BJT::conductanceBE (void){
	return (Isbe/Vt)*exp(Vbe/Vtbe); 
}

double BJT::currentBE (void){
	return  Isbe*(exp(Vbc/Vtbe)-1) - (conductanceBE() *Vbe);
}



//*************************************************************
// Adicionando o efeito Early 
//*************************************************************

//Corrente que passa no diodo entre base e emissor
double BJT::iDE(void){

/*  !!!!!   TENHO QUE COLOCAR ISSO NA PRIMEIRA ITERAÇÃO LÁ FORA	   !!!!!!
	(Vbe*) = Vt;

	if (type == "PNP"){
		Vt=-Vt;
	}
*/
	return ((Isbe)*exp(Vbe/Vtbe) - 1);
}

//Corrente que passa no diodo entre base e coletor
 double BJT::iDC(void){
/*  !!!!!   TENHO QUE COLOCAR ISSO NA PRIMEIRA ITERAÇÃO LÁ FORA	   !!!!!!
	Vbc = Vt;

	if (type == "PNP"){
		Vt=-Vt;
	}
*/

	return ((Isbc)*exp(Vbc/Vtbc) - 1);
}

double BJT::fonteG1(void){
	return (alfa*(Isbe/Vtbe)*exp((Vbe/Vtbe)*(Vt/VA));


double BJT::fonteG2(void){
	return -1*((Isbc/Vtbc)*exp(Vbc/Vtbe)*(Vt/VA));
}

double BJT::fonteG3(void){
	return  ((alfa*iDE() - iDC())/(VA));
}

double BJT::fonteI0(void){
    
//    Vce = nodalAnalysisMatrix[!!!!!!!!!COLETOR!!!!!!!!!!!!] - nodalAnalysisMatrix[!!!!!!!!!!!!EMISSOR!!!!!!!!!]; //vCE

   return (fonteG3()*Vce - fonteG1()*(Vbe*) - fonteG2()*Vbc - fonteG3()*Vce);
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

            // G1*(Vbe*)
            g = fonteG1(); //(1 - alfa*node].alfaR));
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

}

BJT::~BJT()
{
    //dtor
}
