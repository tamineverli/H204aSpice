#ifndef CURRENTSRC_H
#define CURRENTSRC_H

#include <IndependentSource.h>

#include <string>
#include <complex>
#include <vector>

class CurrentSrc : public IndependentSource
{
    public:
        CurrentSrc(string netlistLine);
        virtual void setTemplate(ComplexVector &nodalSystem);
        virtual ~CurrentSrc();
};

#endif // CURRENTSRC_H
