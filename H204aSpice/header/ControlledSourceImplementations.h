#ifndef CONTROLLEDSOURCEIMPLEMENTATIONS_H_INCLUDED
#define CONTROLLEDSOURCEIMPLEMENTATIONS_H_INCLUDED

#include "ControlledSource.h"

#include <iostream>
#include <stdlib.h>
#include <string>

using namespace std;

class Voltage_to_Voltage : public ControlledSource {

	public:
		Voltage_to_Voltage(const string netlistLine);
		virtual void setTemplate(ComplexVector &nodalAnalysisMatrix);
		virtual ~Voltage_to_Voltage();
};

class Current_to_Current : public ControlledSource {

	public:
		Current_to_Current(const string newNetlistLine);
		virtual void setTemplate(ComplexVector &nodalAnalysisMatrix);
		virtual ~Current_to_Current();
};

class Voltage_to_Current : public ControlledSource {

	public:
		Voltage_to_Current(const string newNetlistLine);
		virtual void setTemplate(ComplexVector &nodalAnalysisMatrix);
		virtual ~Voltage_to_Current();
};

class Current_to_Voltage : public ControlledSource {

	public:
		Current_to_Voltage(const string newNetlistLine);
		virtual void setTemplate(ComplexVector &nodalAnalysisMatrix);
		virtual ~Current_to_Voltage();

		unsigned int extraNode2;
};



#endif // CONTROLLEDSOURCEIMPLEMENTATIONS_H_INCLUDED
