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


#ifndef BddConfset_H
#define BddConfset_H

#include <bdd-relation.h>
#include <quantity.h>
#include <paretobase.h>
#include <explicit.h>
#include <conf.h>

using namespace gbdd;

namespace Pareto
{
//	class gbdd::BddSet;
	class quantity;

	class BddConfset : public BddRelation
	{
	public:
		//empty constructor
		BddConfset() {}

		/// Copy constructor
		BddConfset(const StructureRelation& r):BddRelation(r) {};

		/// Constructor for empty BddConfset
		BddConfset(const Bdd::FiniteVars& vs, bool value = false):
			BddRelation(vs, value)
			{}
		/// Constructor
		BddConfset(const Bdd::FiniteVars& vs, Bdd rel_bdd):BddRelation(vs, rel_bdd) {}

		/// Constructor
		BddConfset(const Domains &ds, Bdd rel_bdd):BddRelation(ds, rel_bdd) {};

		/// Adapt domains with automatic renaming

		BddConfset(const Domains &ds, const BddConfset& r):BddRelation(ds, r) {};
		/// Create empty BddConfset
		/**
		* This can be used to create an empty BddConfset which can be populated with the insert method, which
		* automatically extends the domains
		*/
		BddConfset(Space* space, unsigned int arity):
			BddRelation(space , arity)
			{};

		/// Create empty BddConfset with quantities
		/**
		* This can be used to create an empty BddConfset which can be populated with the insert method
		*/
		BddConfset(quantities qs);

		// Bddbolic Minimize
		BddConfset Bddbolic_Minimize(Space* space, std::vector<BddConfset> Dominates,  Bdd::FiniteVars zs);

		//Abstract quantity Q from BddConfsets
		BddConfset Abstract(const quantity& Q);

		//Abstract domain d from BddConfsets
		BddConfset Abstract_Domain(const Domain& d);

		// returns number of BddConfsets in the set
		unsigned int size();

		// Product Operator
		friend BddConfset operator*(const BddConfset& rel1, const BddConfset& rel2);

		// Alternative Operator
		friend BddConfset operator|(const BddConfset& rel1, const BddConfset& rel2);

		// COnstrain Operator
		friend BddConfset operator&(const BddConfset& rel1, const BddConfset& rel2);

		//Printing
		friend ostream& operator<<(ostream &out, const BddConfset &r);

		//Get list of quantities
		const quantities get_quants() const;

		//Set list of quantities
		void set_quants( const quantities &qs);

		string str(void) const;

		Confset *conv_exp(void) const;

		//Mnimization.
		BddConfset minimise(std::vector<bool>);
		BddConfset advanced_minimise(std::vector<bool>);
		BddConfset *constrain(const BddConfset C, const BddConfset D, std::vector<unsigned int> R);
		BddConfset* add_values(const BddConfset C, const BddConfset D);
		BddConfset mmkp(std::vector<BddConfset> S,std::vector<BddConfset> V, std::vector<unsigned int> R);

		//Mnimization.
		BddConfset mmkp_minimise(std::vector<bool>);

		BddConfset my_abstract(unsigned int );
		BddConfset my_abstract(unsigned int,unsigned int );

		BddConfset join(const BddConfset& , int , int, bool);

		// producer_consumer
		//BddConfset ProdCons(const BddConfset& , int , int );
		BddConfset ProdCons(const BddConfset&, int, int,
								  Value *(*f)(const Value*));

		Bdd get_add(const Domain& d0, const Domain& d1, const Domain& d2, int a) ;
		Bdd get_add(const Domain& d0, const Domain& d1, const Domain& d2) ;
		Bdd get_add(const BddSet& s0, const BddSet& s1, int a); 
		Bdd get_add(const BddSet& s0, const BddSet& s1); 
		BddConfset *Conv_exp(Confset *);
		unique_ptr<Bdd> get_dominance(const Domain& d0, const Domain& d1, bool gl, bool disjoined);

		



	protected:
		quantities quants;

	};

}

#endif /* BddConfset_H */
