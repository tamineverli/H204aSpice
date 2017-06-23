#ifndef BJT_H
#define BJT_H

#include <Component.h>
#include <string>
#include <complex>
#include <vector>


class BJT : public Component
{
    public:
        BJT();
        virtual ~BJT();
        virtual double BJT::conductanceBC(void);
        virtual double BJT::currentBC (void);
        virtual double BJT::conductanceBE(void); 
        virtual double BJT::currentBC (void);

        unsigned int nodeB, nodeC, nodeE = 0;
        double alfa, alfaR, Isbe, VTbe, Isbc, VTbc, VA, C0be, C1be, C0bc, C1bc, Vbe, Vbc = 0;
        double Vt = 0.6;
        double n = 0.5;

        double GC, GE, IC, IE, iDE, iDC, g= 0;
        double Vbc, Vbr, Vce;

    protected:

    private:
};

#endif // BJT_H
