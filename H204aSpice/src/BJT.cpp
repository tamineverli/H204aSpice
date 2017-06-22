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

double BJT::conductanceBC (/*inserir*/){
	GC = (Isbc/Vt)*exp(Vbc/Vtbc);
}

double BJT::currentBC (/*inserir*/){
	IC = Isbc*(exp(Vbc/Vtbc)-1) - (GC * Vbc);
}

//Se PNP, inverte v
if (type == "PNP") {
	Vt = -Vt;
}

double BJT::conductanceBE (/*inserir*/){
	GE  = (Isbe/Vt)*exp(Vbe/Vtbe); 
}

double BJT::currentBC (/*inserir*/){
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
            Yn[nodalSystem].a][nodalSystem].a]+=;
            Yn[nodalSystem].b][nodalSystem].b]+=g;
            Yn[nodalSystem].a][nodalSystem].b]-=g;
            Yn[nodalSystem].b][nodalSystem].a]-=g;
            //Fonte de corrente
            g=IC;
            Yn[nodalSystem].a][nv+1]+=g;
            Yn[nodalSystem].b][nv+1]-=g;
            //Fonte de corrente controlada
            g=nodalSystem].alfa*GE;
            Yn[nodalSystem].a][nodalSystem].b]+=g;
            Yn[nodalSystem].b][nodalSystem].c]+=g;
            Yn[nodalSystem].a][nodalSystem].c]-=g;
            Yn[nodalSystem].b][nodalSystem].b]-=g;
            //Fonte de corrente
            g=nodalSystem].alfa*IE;
            Yn[nodalSystem].a][nv+1]-=g;
            Yn[nodalSystem].b][nv+1]+=g;
            //Base Emissor
            // Diodo
            //Resistor
            g=GE;
            Yn[nodalSystem].b][nodalSystem].b]+=g;
            Yn[nodalSystem].c][nodalSystem].c]+=g;
            Yn[nodalSystem].b][nodalSystem].c]-=g;
            Yn[nodalSystem].c][nodalSystem].b]-=g;
            //Fonte de corrente
            g=IE;
            Yn[nodalSystem].b][nv+1]-=g;
            Yn[nodalSystem].c][nv+1]+=g;
            //Fonte de corrente controlada
            g=nodalSystem].alfar*GC();
            Yn[nodalSystem].b][nodalSystem].b]-=g;
            Yn[nodalSystem].c][nodalSystem].a]-=g;
            Yn[nodalSystem].b][nodalSystem].a]+=g;
            Yn[nodalSystem].c][nodalSystem].b]+=g;
            //Fonte de corrente
            g=nodalSystem].alfar*IC;
            Yn[nodalSystem].b][nv+1]+=g;
            Yn[nodalSystem].c][nv+1]-=g;

#ifdef ativarEarly
            //Elementos entre da Coletor e Emissor
            //implementaçao de early
            //Fonte de corrente
            // I0

            g = I0; //(1 - nodalSystem].alfa*nodalSystem].alfar));
            if(!strcmp(nodalSystem].tipo,"PNP")) g=-g;
            //printf("%d  ", g);
            Yn[nodalSystem].a][nv+1]-=g;
            Yn[nodalSystem].c][nv+1]+=g;

            // G1*VBE
            g = G1; //(1 - nodalSystem].alfa*nodalSystem].alfar));
            if(!strcmp(nodalSystem].tipo,"PNP")) g=-g;
            // printf("%d  ", g);
            Yn[nodalSystem].a][nodalSystem].b]+=g;
            Yn[nodalSystem].c][nodalSystem].c]+=g;
            Yn[nodalSystem].a][nodalSystem].c]-=g;
            Yn[nodalSystem].c][nodalSystem].b]-=g;

            // G2*GBC
            g = G2; //(1 - nodalSystem].alfa*nodalSystem].alfar));
            if(!strcmp(nodalSystem].tipo,"PNP")) g=-g;
            // printf("%d  ", g);
            Yn[nodalSystem].a][nodalSystem].b]+=g;
            Yn[nodalSystem].c][nodalSystem].a]+=g;
            Yn[nodalSystem].a][nodalSystem].a]-=g;
            Yn[nodalSystem].c][nodalSystem].b]-=g;

            // G3*GCE
            g = G3; //(1 - nodalSystem].alfa*nodalSystem].alfar));
            if(!strcmp(nodalSystem].tipo,"PNP")) g=-g;
            // printf("%d  ", g);
            Yn[nodalSystem].a][nodalSystem].a]+=g;
            Yn[nodalSystem].c][nodalSystem].c]+=g;
            Yn[nodalSystem].a][nodalSystem].c]-=g;
            Yn[nodalSystem].c][nodalSystem].a]-=g;





}


BJT::~BJT()
{
    //dtor
}
