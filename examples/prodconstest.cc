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

Value *f(const Value *v) {
	FloatValue *v0 = (FloatValue*) v;
	return new FloatValue(1. / v0->getFloat());
}

int main()
{
	// create an explicit or symbolic Pareto calculator object
	ExplCalculator calc;
//	SymCalculator calc;
	
	// create a new configuration set and add some confs
	Confset *V = calc.newConfset();
	FloatConf *c;
	
	c = new FloatConf();
	c->append(1.);
	c->append(.8);
	V->add(c);
	
	c = new FloatConf();
	c->append(3.);
	c->append(.5);
	V->add(c);
		
	c = new FloatConf();
	c->append(5.);
	c->append(.4);
	V->add(c);
	
	c = new FloatConf();
	c->append(8.);
	c->append(.3);
	V->add(c);
	
	Confset *H = calc.newConfset();	
	
	c = new FloatConf();
	c->append(2.);
	c->append(.8);
	H->add(c);
	
	c = new FloatConf();
	c->append(4.);
	c->append(.4);
	H->add(c);
	
	c = new FloatConf();
	c->append(6.);
	c->append(.2);
	H->add(c);
	
	c = new FloatConf();
	c->append(8.);
	c->append(.1);
	H->add(c);

	calc.identicalQuantities(H, 1, V, 1);
		
	cout << "P:" << endl << H << endl;
	cout << "C:" << endl << V << endl;

	cout << "P x C" << endl;
	cout << *H * *V << endl;
	
	cout << "prodcons(P, 1, C, 0, f):\n";
	Confset *Cpc = calc.prodcons(H, 1, V, 0, f);
	cout << Cpc << endl;
	
	cout << "joinAndAbstract(P, 1, C, 1):\n";
	Confset *Cjoin = calc.joinAndAbstract(H, 1, V, 1);
	cout << Cjoin << endl;

	cin.get();
	
	return 0;
}
