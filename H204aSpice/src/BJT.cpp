#include "BJT.h"

BJT::BJT(const string netlistLine) : Component(netlistLine) {

unsigned int blankPosition;

    blankPosition = netlistLine.find(" ");
	istringstream in(netlistLine.substr(blankPosition + 1));

	//cout << "paramentros lidos " << netlistLine << endl;  //debug
	in >> nodeC >> nodeB >> nodeE >> type >> alfa >> alfaR >> Isbe >> VTbe >> Isbc >> VTbc >> VA >> C0be >> C1be >> C0bc >> C1bc;

//Primeios valores de Vbc e Vbe são chutados
    Vbc = 1;
    Vbe = 1;
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

void BJT::SetTemplate {
        //Estampa do Transistor
            //Base Coletor
            // Diodo
            //Resistor
            g=conductanceBC();
            nodalSystem[node1][node1]+=g;
            nodalSystem[node2][node2]+=g;
            nodalSystem[node1][node2]-=g;
            nodalSystem[node2][node1]-=g;
            //Fonte de corrente
            g=currentBC();
            nodalSystem[node1][nv+1]+=g;
            nodalSystem[node2][nv+1]-=g;
            //Fonte de corrente controlada
            g=alfa*conductanceBE();
            nodalSystem[node1][node2]+=g;
            nodalSystem[node2][node3]+=g;
            nodalSystem[node1][node3]-=g;
            nodalSystem[node2][node2]-=g;
            //Fonte de corrente
            g=alfa*currentBE();
            nodalSystem[node1][nv+1]-=g;
            nodalSystem[node2][nv+1]+=g;
            //Base Emissor
            // Diodo
            //Resistor
            g=conductanceBE();
            nodalSystem[node2][node2]+=g;
            nodalSystem[node3][node3]+=g;
            nodalSystem[node2][node3]-=g;
            nodalSystem[node3][node2]-=g;
            //Fonte de corrente
            g=currentBE();
            nodalSystem[node2][nv+1]-=g;
            nodalSystem[node3][nv+1]+=g;
            //Fonte de corrente controlada
            g=alfaR*conductanceBC();
            nodalSystem[node2][node2]-=g;
            nodalSystem[node3][node1]-=g;
            nodalSystem[node2][node1]+=g;
            nodalSystem[node3][node2]+=g;
            //Fonte de corrente
            g=alfaR*currentBC();
            nodalSystem[node2][nv+1]+=g;
            nodalSystem[node3][nv+1]-=g;

#ifdef ativarEarly
            //Elementos entre da Coletor e Emissor
            //implementaçao de early
            //Fonte de corrente
            // fonteI0()

            g = fonteI0(); //(1 - alfa*node].alfaR));
            if(!strcmp(type,"PNP")) g=-g;
            //printf("%d  ", g);
            nodalSystem[node1][nv+1]-=g;
            nodalSystem[node3][nv+1]+=g;

            // G1*VBE
            g = finteG1(); //(1 - alfa*node].alfaR));
            if(!strcmp(type,"PNP")) g=-g;
            // printf("%d  ", g);
            nodalSystem[node].1][node2]+=g;
            nodalSystem[node3][node3]+=g;
            nodalSystem[node1][node3]-=g;
            nodalSystem[node3][node2]-=g;

            // G2*GBC
            g = fonteG2(); //(1 - alfa*node].alfaR));
            if(!strcmp(type,"PNP")) g=-g;
            // printf("%d  ", g);
            nodalSystem[node1][node2]+=g;
            nodalSystem[node3][node1]+=g;
            nodalSystem[node1][node1]-=g;
            nodalSystem[node3][node2]-=g;

            // G3*GCE
            g = fonteG3(); //(1 - alfa*node].alfaR));
            if(!strcmp(type,"PNP")) g=-g;
            // printf("%d  ", g);
            nodalSystem[node1][node1]+=g;
            nodalSystem[node3][node3]+=g;
            nodalSystem[node1][node3]-=g;
            nodalSystem[node3][node1]-=g;

}


BJT::~BJT()
{
    //dtor
}
