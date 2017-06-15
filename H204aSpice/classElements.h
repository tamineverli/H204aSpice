#ifndef CLASSELEMENTS_H_INCLUDED
#define CLASSELEMENTS_H_INCLUDED

#include <string>
#include <complex>
#include <vector>

using namespace std;

typedef complex <double> Complex;
typedef vector <vector<Complex>> ComplexVector;

class Element
{

	public:
		Element(const string newNetlistLine);
		static void setFrequency(const double newfrequency);
		static double getFrequency();
		virtual void setTemplate(ComplexVector &nodalAnalysisMatrix) = 0;
		virtual void print();
		virtual ~Element();

		char type;					//First letter of netlist line read
		string netlistLine;			//Element line read from netlist
		string identifier;			//Element name, found after first letter of netlist line
		unsigned int extraNode;		//For components that need an extra node
		static double frequency;	//Omega, used to build nodalAnalysisMatrix
};

class BasicElement : public Element
{

	public:
		BasicElement(const string newNetlistLine);
		virtual void setTemplate(ComplexVector &nodalAnalysisMatrix) = 0;
		virtual void print();
		virtual ~BasicElement();

		//Node1 is the positive one
		unsigned int node1, node2;
		double value;			//Resistance, inductance, capacitante, etc
};

class Amplifier : public Element
{

	public:
		Amplifier(const string newNetlistLine);
		virtual void setTemplate(ComplexVector &nodalAnalysisMatrix) = 0;
		virtual void print();
		virtual ~Amplifier();

		//Node1 and ControlNode1 are the positive ones
		unsigned int node1, node2, controlNode1, controlNode2, extraNode;
		double gain;
};

class Source : public Element
{

	public:
		Source(const string newNetlistLine);
		virtual void setTemplate(ComplexVector &nodalAnalysisMatrix) = 0;
		virtual void print();
		virtual ~Source();

		//Node1 is the positive one
		unsigned int node1, node2;
		double magnitude, phase, DCValue;
};

class Resistor : public BasicElement
{

	public:
		Resistor(const string newNetlistLine);
		virtual ~Resistor();
		virtual void setTemplate(ComplexVector &nodalAnalysisMatrix);
};

class Inductor : public BasicElement
{

	public:
		Inductor(const string newNetlistLine);
		virtual ~Inductor();
		virtual void setTemplate(ComplexVector &nodalAnalysisMatrix);

		Complex inductiveReactance;
};

class Capacitor : public BasicElement
{

	public:
		Capacitor(const string newNetlistLine);
		virtual ~Capacitor();
		virtual void setTemplate(ComplexVector &nodalAnalysisMatrix);

		Complex capacitiveAdmittance;
};

//Do not have inheritance because the netlistLine of this element does not contais nodes
class Transformer : public Element
{

	public:
		Transformer(const string newNetlistLine);
		virtual ~Transformer();
		virtual void setTemplate(ComplexVector &nodalAnalysisMatrix);
		virtual void print();
		void getInductanceValues(vector< Element *> &elementNetlist);	//Initializes L1 and L2 Must

		string L1Name, L2Name;
		unsigned int extraNode2;
		double L1, L2, k;
		Complex mutualInductance;
};

class VoltageAmplifier : public Amplifier
{

	public:
		VoltageAmplifier(const string newNetlistLine);
		virtual ~VoltageAmplifier();
		virtual void setTemplate(ComplexVector &nodalAnalysisMatrix);
};

class CurrentAmplifier : public Amplifier
{

	public:
		CurrentAmplifier(const string newNetlistLine);
		virtual ~CurrentAmplifier();
		virtual void setTemplate(ComplexVector &nodalAnalysisMatrix);
};

class TransconductanceAmplifier : public Amplifier
{

	public:
		TransconductanceAmplifier(const string newNetlistLine);
		virtual ~TransconductanceAmplifier();
		virtual void setTemplate(ComplexVector &nodalAnalysisMatrix);
};

class TransresistanceAmplifier : public Amplifier
{

	public:
		TransresistanceAmplifier(const string newNetlistLine);
		virtual ~TransresistanceAmplifier();
		virtual void setTemplate(ComplexVector &nodalAnalysisMatrix);

		unsigned int extraNode2;
};

class CurrentSource : public Source
{

	public:
		CurrentSource(const string newNetlistLine);
		virtual ~CurrentSource();
		virtual void setTemplate(ComplexVector &nodalAnalysisMatrix);
};

class VoltageSource : public Source
{

	public:
		VoltageSource(const string newNetlistLine);
		virtual ~VoltageSource();
		virtual void setTemplate(ComplexVector &nodalAnalysisMatrix);
};

class AmpOp : public Element
{

	public:
		AmpOp(const string newNetlistLine);
		virtual ~AmpOp();
		virtual void setTemplate(ComplexVector &nodalAnalysisMatrix);
		virtual void print();

		unsigned int outputNodePositive, outputNodeNegative;
		unsigned int inputNodePositive, inputNodeNegative;
};

class BJT : public Element
{
    public:

};


#endif // CLASSELEMENTS_H_INCLUDED
