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

    	//Find L1 and L2 and compute the mutual inductance
		void locateInductors(vector< Component *> &nodalSystem);
		virtual void setTemplate(ComplexVector &nodalSystem, vector<Complex> &previousSolutionVector);
		virtual void print();
		virtual ~CoupledInductors();

		string L1Name, L2Name;
		double L1, L2, k;
		unsigned int extraNode2;
		Complex mutualInductance;
};

#endif // COUPLEDINDUCTORS_H
