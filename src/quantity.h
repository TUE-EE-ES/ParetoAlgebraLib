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


#ifndef quantity_H
#define quantity_H

#include <bdd-relation.h>
using namespace gbdd;


namespace Pareto
{
	// Quantity Class
	class quantity : public BddSet
	{
		
	public:
		//constructor
		quantity() {}
		//constructor with a set
		quantity(const StructureRelation& r):BddSet(r) {};
		//constructor with a set and domain
		quantity(const Domain &d, Bdd rel_bdd):BddSet(d, rel_bdd) {};
		//constructor with a quantity and domain
		quantity(const Domain &d, const quantity& r):BddSet(d, r) {};
		//constructor with a space
		quantity(Space* space):BddSet(space) {};
		//constructor with a space and the number of values
		quantity(Space* space, unsigned int v):BddSet(space, v) {};
		//constructor with a space and range of values
		quantity(Space* space, unsigned int from_v, unsigned int to_v):BddSet(space,from_v, to_v) {};
		//constructor with a quantity and new value
		quantity(const quantity& s, unsigned int v):BddSet(s, v) {};
		//constructor with a quantity and a range
		quantity(const quantity& s, unsigned int from_v, unsigned int to_v):BddSet(s, from_v, to_v) {};

		// Number of values 
		unsigned int NumberOfValues();

		//Get domain of the quantity
		const Bdd get_dominance(const Domain& d) const;
		const Bdd get_add(const Domain& d1,const Domain& d2) const;

		//Printing
		friend ostream& operator<<(ostream &out, const quantity &r);

	}	;

	/// Tuple of quantities used to type Relation
	class quantities
	{
		// a vector of quantities
		vector<quantity> qs;
	public:
		// Constructor
		quantities(unsigned int n = 0);
		// Constructor with a quantity
		quantities(quantity d1);

		// Checks if the number of quantities is finite
		bool is_some_infinite() const;

		// Union of quantities
		friend quantities operator*(quantities ds1, quantities ds2);

		// // Get a specified Quantity
		quantity& operator[](unsigned int idx);
		const quantity& operator[](unsigned int idx) const;

		// Checks if two set of quantitiea are equal
		friend bool operator==(quantities ds1, quantities ds2);

		/// Iterator
		typedef vector<quantity>::const_iterator const_iterator;

		/// Get iterator pointing to beginning
		const_iterator begin() const { return qs.begin(); }

		/// Get iterator pointing to end
		const_iterator end() const { return qs.end(); }

		/// Get size of quantities
		unsigned int size() const { return qs.size(); }

	};

		
}

#endif /* quantity_H */
