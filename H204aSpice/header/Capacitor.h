#ifndef CAPACITOR_H
#define CAPACITOR_H

#include <SimpleComponent.h>

#include <string>
#include <complex>
#include <vector>

class Capacitor : public SimpleComponent
{
    public:
        Capacitor(string netlistLine);
        virtual void setTemplate(ComplexVector &nodalAnalysisMatrix);
        virtual ~Capacitor();
		Complex capacitiveAdmittance;

};

#endif // CAPACITOR_H
