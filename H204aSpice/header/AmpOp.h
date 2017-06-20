#ifndef AMPOP_H
#define AMPOP_H

#include <Component.h>

#include <string>
#include <complex>
#include <vector>

class AmpOp : public Component
{
    public:
        AmpOp(const string netlistLine);
		virtual void setTemplate(ComplexVector &nodalAnalysisMatrix);
		virtual void print();
		virtual ~AmpOp();

		unsigned int out_positive, out_negative;
		unsigned int in_positive, in_negative;
};

#endif // AMPOP_H
