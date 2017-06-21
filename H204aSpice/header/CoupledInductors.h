#ifndef COUPLEDINDUCTORS_H
#define COUPLEDINDUCTORS_H

#include <Component.h>
#include <Inductor.h>

#include <string>
#include <complex>
#include <vector>

class CoupledInductors : public Component
{
    public:
        CoupledInductors(string netlistLine);
		virtual void setTemplate(ComplexVector &nodalSystem);
		virtual void print();
		void locateInductors(vector< Component *> &nodalSystem);	//Initializes L1 and L2 Must
		virtual ~CoupledInductors();

		string L1Name, L2Name;
		unsigned int extraNode2;
		double L1, L2, k;
		Complex mutualInductance;
};

#endif // COUPLEDINDUCTORS_H
