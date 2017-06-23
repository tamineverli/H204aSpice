#ifndef BJT_H
#define BJT_H

#include <Component.h>

#include <string>
#include <complex>
#include <vector>


class BJT : public Component
{
    public:
        BJT(string netlistLine);
        virtual ~BJT();
        void SetTemplate(ComplexVector &nodalSystem);
        virtual double conductanceBC();
        virtual double currentBC();
        virtual double conductanceBE();
        virtual double currentBE();
        virtual double iDE();
        virtual double iDC();
        virtual double fonteG1();
        virtual double fonteG2();
        virtual double fonteG3();
        virtual double fonteI0();

        unsigned int nodeB, nodeC, nodeE;
        double alfa, alfaR, Isbe, VTbe, Isbc, VTbc, VA, C0be, C1be, C0bc, C1bc;
        double Vt;
        double n;

        char *BJT_type;

        double GC, GE, IC, IE, g;
        double Vbc, Vbe, Vce;

};

#endif // BJT_H
