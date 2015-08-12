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


#include "pareto.h"

using namespace Pareto;

Value *sum(Conf *c, const void *x = NULL)
{
	double s = 0;
	for (unsigned i = 0; i < c->size(); i++)
		s += ((FloatValue*)(c->get(i)))->getFloat();
	return new FloatValue(s);
}

bool atMostTwo(Conf *c)
{
	for (unsigned i = 0; i < c->size(); i++)
		if (((FloatValue*)(c->get(i)))->getFloat() > 2)
			return false;
	return true;
}

int main()
{
	// create an explicit or symbolic Pareto calculator object
	ExplCalculator calc;
	//SymCalculator calc;
	
	// create a new configuration set and add some confs
	Confset *C = calc.newConfset();
	FloatConf *c;
	
	c = new FloatConf();
	c->append(1.);
	c->append(2.);
	c->append(3.);
	C->add(c);
	
	c = new FloatConf();
	c->append(2.);
	c->append(2.);
	c->append(1.);
	C->add(c);
	
	// make a copy of C and add more confs
	Confset *D = calc.newConfset(C);
	
	c = new FloatConf();
	c->append(3.);
	c->append(2.);
	c->append(1.);
	C->add(c);

	c = new FloatConf();
	c->append(0.2);
	c->append(2.3);
	c->append(3.3);
	C->add(c);
	
	c = new FloatConf();
	c->append(0.2);
	c->append(2.);
	c->append(3.3);
	D->add(c);

	c = new FloatConf();
	c->append(3.);
	c->append(2.3);
	c->append(3.3);
	D->add(c);
	calc.identicalQuantities(C,0,D,0);
	calc.identicalQuantities(C,1,D,1);
	calc.identicalQuantities(C,2,D,2);
	
	// exception examples
	try {
		c->append(new Value());
	}
	catch (Exception e) {
		cout << "ERROR:\t" << e.what() << endl;
	}

	try {
		C->add(new FloatConf());
	}
	catch (Exception e) {
		cout << "ERROR:\t" << e.what() << endl;
	}
	
	// keep copy for later
	Confset *Ccopy = C->copy();
	
	// references for use with shortcuts
	Confset &Cr = *C;
	Confset &Dr = *D;
	
	// printing using an iterator
	cout << "C:" << endl;
	for (Iterator iter(C); !iter.done(); iter++)
		cout << "conf " << *iter << endl;
	
	// printing with the stream operator
	cout << "D:" << endl << D << endl;
	
	// minimisation
	cout << "min(C):" << endl;
	//C->hide(2);
	Confset *Cmin = calc.minimise(C);
	cout << Cmin << endl;
	// with shortcut:
	cout << min(Cr) << endl;
	
	// constraint
	cout << "C constrained with D:" << endl;
	Confset *Ccon = calc.constrain(C, D);
	cout << Ccon << endl;
	// with shortcut:
	cout << (Cr & Dr) << endl;

	// constraint based on predicate function
	cout << "C constrained by function 'atMostTwo':" << endl;
	Confset *Ccon2 = calc.constrain(C, atMostTwo);
	cout << Ccon2 << endl;
	// with shortcut:
	cout << constr(*C, atMostTwo) << endl;
	
	// abstraction
	cout << "abstract Q1 from C:" << endl;
	Confset *Cabs = calc.abstract(C, 1);
	cout << Cabs << endl;
	// with shortcut:
	cout << abstr(Cr, 1) << endl;
	
	cout << min(Cr);
	
	// product
	C = Ccopy->copy();
	cout << "C x D:" << endl;
	Confset *Cprod = calc.multiply(C, D);
	cout << Cprod << endl;
	// with shortcut:
	C = Ccopy->copy();
	cout << (*C * Dr) << endl;
	
	C = Ccopy->copy();
	cout << "C x D with product generator:" << endl;
	for (ProdGen p(C, D); !p.done(); p++)
		cout << "generated conf " << *p << endl;		

	// alternatives
	C = Ccopy->copy();
	cout << endl << "union of C and D (alternatives):" << endl;
	Confset *Calt = calc.unite(C, D);
	cout << Calt << endl;
	// with shortcut:
	C = Ccopy->copy();
	cout << (*C | Dr) << endl;

	// derived quantity
	cout << "add derived quantity for function 'sum' to C:" << endl;
	Confset *Cf = calc.addDerived(C, sum);
	cout << Cf << endl;
	// with shortcut:
	cout << derive(*C, sum) << endl;	
	
	// combined
	cout << "test:\n" << min(abstr(*C * Dr, 1)) << endl;
	
//	cin.get();

	return 0;
}
