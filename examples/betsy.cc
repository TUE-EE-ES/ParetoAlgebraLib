/*
 * The Pareto Calculator
 * Copyright (c) 2008, TU Eindhoven
 * Eindhoven university of Technology
 * Eindhoven, The Netherlands
 * Contact: m.c.w.geilen@tue.nl
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <gbdd.h>
#include <pareto.h>
#include <iostream>
#include <fstream>


using namespace gbdd;
using namespace Pareto;


// Function for Producer-Consumer constrain.
Value *f(const Value *v) {
	FloatValue *v0 = (FloatValue*) v;
	return new FloatValue(1. / v0->getFloat());
}

//
Value *aggregate_pow(Conf *c, const void *x = NULL)
{
	double s = 0;
	s +=  ((FloatValue *)c->get(2))->getFloat();
	s +=  ((FloatValue *)c->get(5))->getFloat();
	return new FloatValue(s);
}

int main()
{
	// Defining the Calculator(Explicit or Symbolic)
	
	ExplCalculator calc;
//	SymCalculator calc;

	srand(10);
	unsigned int p,d,e,t;
	cout<<"\n Please enter number of configurations\n p(Processor):";
	cin>>p;
	cout<<"\n d(Decoder):";
	cin>>d;
	cout<<"\n t(Transmission):";
	cin>>t;
	cout<<"\n e(Encoder):";
	cin>>e;

	// Defining quantities and their corresponding values

	vector<double> Pce;
	vector<double> Pp;

	vector<double> Dce;

	vector<double> Tbr;
	vector<double> Tl;
	vector<double> Tbc;
	vector<double> Tp;

	vector<double> Emp;
	vector<double> Emq;
	vector<double> Embr;

	while(Pce.size()< p)
		Pce.push_back((rand()%1000));
	while(Pp.size()< p)
		Pp.push_back((rand()%1000000));

	while(Dce.size()< d)
		Dce.push_back((rand()%10000000)/100000000.0);

	while(Tbr.size()< t)
		Tbr.push_back((rand()%100));
	while(Tl.size()< t)
		Tl.push_back((rand()%1000000));
	while(Tbc.size()< t)
		Tbc.push_back((rand()%1000000));
	while(Tp.size()< t)
		Tp.push_back((rand()%1000000));

	while(Emq.size()< e)
		Emq.push_back(-rand()%10000000);
	while(Embr.size()< e)
		Embr.push_back((rand()%10000000)/100000000.0);

	
	// Defining Configurations

	Confset *Proc = calc.newConfset();
	Confset *Dec = calc.newConfset();
	Confset *Trans = calc.newConfset();
	Confset *Enc = calc.newConfset();
	GenConf *c;
	FloatValue *v;
	UnorderedFloatValue *uv;

	// Adding values to configurations

	for(unsigned int i = 0; i<p; ++i)
	{
		c = new GenConf();
		v = new FloatValue(Pce[i]);
		c->append(v);
		uv = new UnorderedFloatValue(i);
		c->append(uv);
		v = new FloatValue(Pp[i]);
		c->append(v);
		Proc->add(c);
	}

	for(unsigned int i = 0; i<d; ++i)
	{
		c = new GenConf();
		uv = new UnorderedFloatValue(i);
		c->append(uv);
		v = new FloatValue(Dce[i]);
		c->append(v);
		Dec->add(c);
	}

	for(unsigned int i = 0; i<t; ++i)
	{
		c = new GenConf();
		v = new FloatValue(Tbr[i]);
		c->append(v);
		v = new FloatValue(Tl[i]);
		c->append(v);
		v = new FloatValue(Tbc[i]);
		c->append(v);
		v = new FloatValue(Tp[i]);
		c->append(v);
		Trans->add(c);
	}

	for(unsigned int i = 0; i<e; ++i)
	{
		c = new GenConf();
		uv = new UnorderedFloatValue(i);
		c->append(uv);
		v = new FloatValue(Emq[i]);
		c->append(v);
		v = new FloatValue(Embr[i]);
		c->append(v);
		Enc->add(c);
	}

	cout<<"Processor:\n"<<Proc<<endl;
	cout<<"Decoder:\n"<<Dec<<endl;
	cout<<"Transmission:\n"<<Trans<<endl;
	cout<<"Encoder:\n"<<Enc<<endl;

	// Defining identical quantities

	calc.identicalQuantities(Dec,0,Enc,0);

	// Computation Chain:

	cout<<"\nProducer-consumer(Dec,Proc)....\n";
	Confset *Cprod1 = calc.prodcons(Dec, 1, Proc, 0, f);
	
	cout<<"\nAbstract(Decoder Comp. Effort)....\n";
	Confset *Cabs11 = calc.abstract(Cprod1, 1);

	cout<<"\nAbstract(Processor Comp. Effort)....\n";
	Confset *Cabs12 = calc.abstract(Cabs11, 1);

	Cabs12->hide(1);

	cout<<"\nMinimization....\n";
	Confset *Cmin1 = calc.minimise(Cabs12);
	cout << Cmin1;
	
	cout<<"\nProducer-consumer(Trans,Enc)....\n";
	Confset *Cprod2 = calc.prodcons(Trans, 0, Enc, 2, f);

	cout<<"\nAbstract(Transmission Bit Rate)....\n";
	Confset *Cabs21 = calc.abstract(Cprod2, 0);

	cout<<"\nAbstract(Encoder Bit Rate)....\n";
	Confset *Cabs22 = calc.abstract(Cabs21, 5);
	
	Cabs22->hide(3);

	cout<<"\nMinimization....\n";
	Confset *Cmin2 = calc.minimise(Cabs22);
	cout << Cmin2;
	
	cout<<"\nJoin(on MPEG Parameter)....\n";
	Confset *Cjoin = calc.joinAndAbstract(Cmin1,0, Cmin2, 3);
	cout << Cjoin;
	
	Cjoin->hide(0);

	cout<<"\nMinimization....\n";
	Confset *Cmin = calc.minimise(Cjoin);

	cout << "\nAdding aggregated power...." << endl;
	Confset *Cres = calc.addDerived(Cmin, aggregate_pow);

	cout<<"\nResult Encoding....\n \n"<< Cres;
	cin.get();
	cin.get();

	return 0;
}
