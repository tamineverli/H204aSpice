#include "BJT.h"

BJT::BJT(const string netlistLine) : Component(netlistLine) {

unsigned int blankPosition;

    blankPosition = netlistLine.find(" ");
	istringstream in(netlistLine.substr(blankPosition + 1));

	//cout << "paramentros lidos " << netlistLine << endl;  //debug
	in >> nodeC >> nodeB >> nodeE >> type >> alfa >> alfaR >> Isbe >> VTbe >> Isbc >> VTbc >> VA >> C0be >> C1be >> C0bc >> C1bc;

    Vt = 0.6;
    n = 0.5;

//Primeios valores de Vbc e Vbe são chutados
    Vbc = 1;
    Vbe = 1;
}





//*************************************************************
//Cálculo de parâmetros do modelo linearizado necessários para montar a estampa
//*************************************************************

double BJT::conductanceBC (void){
	GC = (Isbc/Vt)*exp(Vbc/Vtbc);
}

double BJT::currentBC (void){
	IC = Isbc*(exp(Vbc/Vtbc)-1) - (GC * Vbc);
}

//Se PNP, inverte v
if (type == "PNP") {
	Vt = -Vt;
}

double BJT::conductanceBE (void){
	GE  = (Isbe/Vt)*exp(Vbe/Vtbe); 
}

double BJT::currentBC (void){
	IE = Isbe*(exp(Vbc/Vtbe)-1) - (GE * Vbe);
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
}

double BJT::fonteG1(void){
	V2 = Vt;

	G1 = (alfa*(Isbe/Vtbe)*exp(Vbe/Vtbe)*(V2/VA));

}

double BJT::fonteG2(void){
	G2 = -1*((Isbc/Vtbc)*exp(Vb/Vtbe)*(V2/VA));
}

double BJT::fonteG3(void){
	G3 = ((alfa*iDE() - iDC())/(VA));
}

double BJT::fonteI0(void){
   I0 = (fonteG3()*Vce - fonteG1()*Vbe - fonteG2()*Vbc - fonteG3()*Vce);
}

void BJT::SetTemplate {
        //Estampa do Transistor
            //Base Coletor
            // Diodo
            //Resistor
            g=GC;
            nodalSystem[node1][node1]+=g;
            nodalSystem[node2][node2]+=g;
            nodalSystem[node1][node2]-=g;
            nodalSystem[node2][node1]-=g;
            //Fonte de corrente
            g=IC;
            nodalSystem[node1][nv+1]+=g;
            nodalSystem[node2][nv+1]-=g;
            //Fonte de corrente controlada
            g=alfa*GE;
            nodalSystem[node1][node2]+=g;
            nodalSystem[node2][node3]+=g;
            nodalSystem[node1][node3]-=g;
            nodalSystem[node2][node2]-=g;
            //Fonte de corrente
            g=alfa*IE;
            nodalSystem[node1][nv+1]-=g;
            nodalSystem[node2][nv+1]+=g;
            //Base Emissor
            // Diodo
            //Resistor
            g=GE;
            nodalSystem[node2][node2]+=g;
            nodalSystem[node3][node3]+=g;
            nodalSystem[node2][node3]-=g;
            nodalSystem[node3][node2]-=g;
            //Fonte de corrente
            g=IE;
            nodalSystem[node2][nv+1]-=g;
            nodalSystem[node3][nv+1]+=g;
            //Fonte de corrente controlada
            g=alfaR*GC();
            nodalSystem[node2][node2]-=g;
            nodalSystem[node3][node1]-=g;
            nodalSystem[node2][node1]+=g;
            nodalSystem[node3][node2]+=g;
            //Fonte de corrente
            g=alfaR*IC;
            nodalSystem[node2][nv+1]+=g;
            nodalSystem[node3][nv+1]-=g;

#ifdef ativarEarly
            //Elementos entre da Coletor e Emissor
            //implementaçao de early
            //Fonte de corrente
            // I0

            g = I0; //(1 - alfa*node].alfaR));
            if(!strcmp(type,"PNP")) g=-g;
            //printf("%d  ", g);
            nodalSystem[node1][nv+1]-=g;
            nodalSystem[node3][nv+1]+=g;

            // G1*VBE
            g = G1; //(1 - alfa*node].alfaR));
            if(!strcmp(type,"PNP")) g=-g;
            // printf("%d  ", g);
            nodalSystem[node].1][node2]+=g;
            nodalSystem[node3][node3]+=g;
            nodalSystem[node1][node3]-=g;
            nodalSystem[node3][node2]-=g;

            // G2*GBC
            g = G2; //(1 - alfa*node].alfaR));
            if(!strcmp(type,"PNP")) g=-g;
            // printf("%d  ", g);
            nodalSystem[node1][node2]+=g;
            nodalSystem[node3][node1]+=g;
            nodalSystem[node1][node1]-=g;
            nodalSystem[node3][node2]-=g;

            // G3*GCE
            g = G3; //(1 - alfa*node].alfaR));
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
