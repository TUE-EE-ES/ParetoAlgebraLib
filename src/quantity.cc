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


#include <quantity.h>
#include <gbdd.h>
#include <typeinfo>
#include <iostream>
#include <fstream>

using namespace gbdd;


namespace Pareto
{
	// # of BddConfsets
		unsigned int quantity::NumberOfValues() 
		{
			int n=0;
			for(quantity::const_iterator i=begin(); i != end(); ++i)
				++n;
			return n;
		}

	ostream& operator<<(ostream &out, const quantity &r)
	{
		out << "{";
		for(quantity::const_iterator i=r.begin(); i != r.end(); ++i)
		{
			out<<","<<*i;
		}	
		out << "}";

		return out;
	}


	const Bdd quantity::get_add(const Domain& d0, const Domain& d1) const
	{
//		unsigned int n = get_domain().size();
		Domain vd1 = get_domain();
		Domain vd2 = d0;
		Domain vs=Domain(1,d0.size()+1) + (get_domain()|d0).highest();
		Bdd ci(get_space(),false);
		Bdd s(get_space(),true);
		Domain::const_iterator i2 = vd2.begin();
		Domain::const_iterator si = vs.begin();

		for(Domain::const_iterator i1 = vd1.begin(); i1 != vd1.end(); ++i1)
		{
			
			Bdd s0 = !Bdd::var_equal(get_space(), *i1, *i2);
			Bdd t=Bdd::bdd_product((s0 ^ ci), Bdd::var_true(get_space(),*si), StructureConstraint::fn_iff);
			s &= Bdd::bdd_product((s0 ^ ci), Bdd::var_true(get_space(),*si), StructureConstraint::fn_iff);
					
			ci = (s0&ci) | (Bdd::var_true(get_space(), (*i1)) & Bdd::var_true(get_space(),(*i2)));
			
			
			++i2;
			++si;
		}
		
		return s;
	}

	const Bdd quantity::get_dominance(const Domain& d) const
	{
//		unsigned int n = get_domain().size();
		Domain vs1 = get_domain();
		Domain vs2 = d;
		Bdd r = Bdd::vars_equal(get_space(),vs1,vs2);
		if(get_domain().RealVal.IsUnordered)
			return r;	
		vector<Space::Var> vvs1;
		vector<Space::Var> vvs2;
		for(Domain::const_iterator i1 = vs1.begin(); i1 != vs1.end(); ++i1)
			vvs1.push_back(*i1);
		for(Domain::const_iterator i1 = vs2.begin(); i1 != vs2.end(); ++i1)
			vvs2.push_back(*i1);

		
		Bdd p(get_space(), false);

		for(int i=(vvs1.size()-1); 0<=i; --i)
		{
			Bdd q(get_space(), true);
			for(int j=(vvs1.size()-1); i<j; --j)
				q &= Bdd::var_equal(get_space(), vvs1[j], vvs2[j]);
			q &= Bdd::var_true(get_space(), vvs1[i]) & Bdd::var_false(get_space(), vvs2[i]);
			p|=q;

		}
		
		return p|r;
	}


	/// Create a sequence of empty quantities

	quantities::quantities(unsigned int n):
		qs(n)
	{}

	/// Create quantities with one element

	quantities::quantities(quantity d1):
		qs(1, d1)
	{}

	/// Cross product of quantities

	quantities operator*(quantities ds1, quantities ds2)
	{
		ds1.qs.insert(ds1.qs.end(), ds2.qs.begin(), ds2.qs.end());

		return ds1;
	}

	/// Get one of the quantities

	quantity& quantities::operator[](unsigned int idx)
	{
		assert(idx >= 0 && idx < size());
		return qs[idx];
	}

	/// Get one of the quantities


	const quantity& quantities::operator[](unsigned int idx) const
	{
		return qs[idx];
	}

	/// Equantity

	bool operator==(quantities ds1, quantities ds2)
	{
		return ds1.qs == ds2.qs;
	}



}
