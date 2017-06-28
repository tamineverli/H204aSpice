#ifndef BJT_H
#define BJT_H

#include <Component.h>

#include <cmath>
#include <string>
#include <complex>
#include <vector>

using namespace std;

class BJT : public Component
{
    public:
        BJT(string netlistLine);
        virtual void setTemplate(ComplexVector &nodalSystem);

        //Set and Get methods for Vbc, Vbe and Vce
        //void setTerminalVoltages(ComplexVector &nodalSolutionVector);
        double getVbc();
        double getVbe();
        double getVce();

        //Calculate parameters of the linear model needed to set the templates
        double setConductanceBC();
        double setCurrentBC();
        double setConductanceBE();
        double setCurrentBE();
        double sourceG1();
        double sourceG2();
        double sourceG3();
        double sourceI0();

        //Early Effect
        double iDE();
        double iDC();


        virtual ~BJT();

        string bjt_type;
        unsigned int nodeCollector, nodeBase, nodeEmitter;
        double alfa, alfaR;
        double  IsBe, VtBe, IsBc, VtBc, VA;
        double  CbeR, CbeD, CbcR, CbcD, C0be, C1be, C0bc, C1bc;

        double bjt_Vt, bjt_n, bjt_g;

        double GC, GE, IC, IE;
        double Vbc, Vbe, Vce;

};

#endif // BJT_H
