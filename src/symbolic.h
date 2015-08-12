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



#ifndef SymConfset_H
#define SymConfset_H

#include "bdd-relation.h"
#include "quantity.h"
#include "bddconfset.h"
#include "paretobase.h"
#include "explicit.h"
using namespace gbdd;

namespace Pareto
{
	
	class gbdd::BddSet;
	class gbdd::BddRelation;
	class BddConfset;
	class quantity;

	//Space *space = Space::create_default();

	class SymConfset : public Confset 
	{
	public:

		SymConfset(Calculator &c) : Confset(c), conv(false) {}
		virtual ~SymConfset(void);
		virtual Confset *copy(void) const;
		virtual void add(Conf*);
		virtual unsigned remove(Conf*);
		virtual void hide(unsigned int);
		virtual void hide(unsigned int, unsigned int);
		virtual void unhide(unsigned int);
		virtual void unhide(unsigned int, unsigned int);
		virtual unsigned int size(void) const;
		virtual unsigned int width(void) const;
		virtual string str(void) const;
		virtual Iter *iter(void);
		virtual void clear(void);
		virtual void addAndMin(Conf*);

		///Get the configurations in term of BDD
		const BddConfset* get_rel() const;
		/// Set the configurations in form of BDD
		void set_rel(const BddConfset *r);
		///Convert symbolic representation to explicit one
		void exp2sym(void);
		///Convert explicit representation to symbolic one
		void sym2exp(void);
		///Check if symbolic conversion is done
		void conv2sym(void);

	protected:
		Hidevec hidden;
		friend class SymCalculator;

		BddConfset *rel;
		Confset *econf;
		bool conv;
		ExplCalculator calce;
		Hidevec setQuants;
		vector<RealQuant*> rquants;

	};

	/// Symbolic Pareto calculator
	class SymCalculator : public Calculator
	{
	public:
		
		virtual ~SymCalculator() {}

		virtual Confset *newConfset(void);
		virtual Confset *newConfset(const Confset*);

		// elementary operations
		virtual Confset *minimise(Confset*);
		virtual Confset *advanced_minimise(const Confset*);

		virtual Confset *multiply(const Confset*, const Confset*);
		virtual Confset *abstract(Confset*, unsigned int);
		virtual Confset *abstract(Confset*, unsigned int, unsigned int);
		virtual Confset *constrain(Confset*, const Confset*);
		virtual Confset *constrain(Confset*, bool (*)(Conf*));
		virtual Confset *constrain(Confset*, const Conf*);
		virtual Confset *unite(const Confset*, const Confset*);

		// compound operations
		virtual Iter *prodgen(vector<Confset*>);
		virtual Iter *prodgen(Confset*, Confset*);
		
		virtual Confset *addDerived(Confset *C, Value *(*f)(Conf*, const void*), const void* = NULL);
		virtual Confset *addDerivedVector(Confset*, Conf *(*F)(Conf*, const void*), const void* = NULL);
		
		// compound operations
		virtual Confset *prodcons(const Confset*, unsigned, const Confset*, unsigned,
								  Value *(*f)(const Value*));
		virtual Confset *join(const Confset*, unsigned, const Confset*, unsigned,
							  bool abstr=false);
		virtual void identicalQuantities(const Confset *C, unsigned k, const Confset *D, unsigned l);
	};
}

#endif /* SymConfset_H */
