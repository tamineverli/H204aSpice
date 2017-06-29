#ifndef INDUCTOR_H
#define INDUCTOR_H

#include <SimpleComponent.h>

#include <string>
#include <complex>
#include <vector>

class Inductor : public SimpleComponent
{
    public:
        Inductor(string netlistLine);
		virtual void setTemplate(ComplexVector &nodalSystem, vector<Complex> &previousSolutionVector);
		virtual ~Inductor();

		Complex inductiveReactance;
};

#endif // INDUCTOR_H
