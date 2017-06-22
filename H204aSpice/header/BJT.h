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
        virtual double BJT::conductanceBC(/*inserir*/);
        virtual double BJT::currentBC (/*inserir*/);
        virtual double BJT::conductanceBE(/*inserir*/); 
        virtual double BJT::currentBC (/*inserir*/);

        unsigned int nodeB, nodeC, nodeE = 0;
        double alfa, alfaR, Isbe, VTbe, Isbc, VTbc, VA, C0be, C1be, C0bc, C1bc, Vbe, Vbc = 0;
        double Vt = 0.6;
        double n = 0.5;

        double GC, GE, IC, IE, iDE, iDC, g= 0;

    protected:

    private:
};

#endif // BJT_H
