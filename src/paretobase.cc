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


#include "paretobase.h"

namespace Pareto {
	/**
	 * Default implementation.
	 */
	void Conf::abstract(unsigned k, unsigned l)
	{
		for (unsigned i = k; i < l; i++)
			abstract(i);
	}

	/**
	 * Default implementation.
	 */
	bool Conf::totalCompare(const Conf *c) const
	{
		return str() < c->str();
	}

	/**
	 * Default implementation.
	 */
	void Confset::addAndMin(Conf *c)
	{
		add(c);
		calc.minimise(this);
	}
		
	/**
	 * Default implementation.
	 */
	void Confset::hide(unsigned k, unsigned l)
	{
		for (unsigned i = k; i < l; i++)
			hide(i);
	}
		
	/**
	 * Default implementation.
	 */
	void Confset::unhide(unsigned k, unsigned l)
	{
		for (unsigned i = k; i < l; i++)
			unhide(i);
	}
	
	/**
	 * Default implementation.
	 */
	Confset *Calculator::abstract(Confset *C, unsigned k, unsigned l)
	{
		Confset *D = abstract(C, k);
		for (unsigned i = k + 1; i < l; i++) {
			Confset *E = abstract(D, i);
			disposeConfset(D);
			D = E;
		}
		return D;
	}
}
