#ifndef VOLTAGESRC_H
#define VOLTAGESRC_H

#include <IndependentSource.h>

#include <string>
#include <complex>
#include <vector>

class VoltageSrc : public IndependentSource
{
    public:
        VoltageSrc( string netlistLine);
		virtual void setTemplate(ComplexVector &nodalSystem, vector<Complex> &previousSolutionVector);
		virtual ~VoltageSrc();
};

#endif // VOLTAGESRC_H
