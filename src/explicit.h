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


#ifndef EXPLICIT_H
#define EXPLICIT_H

#include "paretobase.h"

using namespace std;

namespace Pareto {
	// Forward references
	class ExplIter;
	class ExplConfset;

	/// Order function for set objects
	struct ltconf
	{
		bool operator()(const Conf *s1, const Conf *s2) const
		{
			return s1->totalCompare(s2);
		}
	};

	/// Type for configuration set data structure
	typedef set<Conf*, ltconf> texplconfset;

	/// Explicit set of configurations
	class ExplConfset : public Confset {	
	public:
		ExplConfset(Calculator &c) : Confset(c), w(0) {}
		virtual ~ExplConfset(void);
		virtual Confset *copy(void) const;
		virtual void add(Conf*);
		virtual void addAndMin(Conf*);
		virtual unsigned remove(Conf*);
		virtual void hide(unsigned);
		virtual void hide(unsigned, unsigned);
		virtual void unhide(unsigned);
		virtual void unhide(unsigned, unsigned);
		virtual Iter *iter(void);
		virtual unsigned size(void) const;
		virtual unsigned width(void) const;
		virtual string str(void) const;
		virtual void clear(void);
		
		VisibleList visible;
		
	protected:
		texplconfset confset;
		unsigned w;
		
		friend class ExplIter;
		friend class ExplIndex;
		friend class ExplCalculator;
	};


	/// Iterator for ExplConfset
	class ExplIter : public Iter {
	public:
		ExplIter(ExplConfset& C) : cs(C) {iter = cs.confset.begin();}
		virtual ~ExplIter() {}
		virtual Conf *operator*(void);
		virtual void operator++(int);
		virtual bool done(void);
		
	protected:
		ExplConfset &cs;
		texplconfset::const_iterator iter;
	};
		
	struct IndexOrder {
		IndexOrder(unsigned k, bool desc) {k_ = k; desc_ = desc;}
		bool operator()(Conf *c, Conf *d) {
			if (desc_)
				return !d->get(k_)->totalCompare(c->get(k_));
			else
				return !c->get(k_)->totalCompare(d->get(k_));
		}
		unsigned k_;
		bool desc_;
	};
	
	/// Index on ExplConfset
	/**
	 * Creates an sorted index on an ExplConfset on a chosen quantity \a k.
	 * The default sorting order is best-to-worst (\a desc is true). The index
	 * contains pointers to configurations in the set, and can be accessed by
	 * the \a [] operator.
	 */
	class ExplIndex {
	public:
		ExplIndex(ExplConfset *C, unsigned k=0, bool desc=true);
		virtual ~ExplIndex() {}
		virtual pair<unsigned,unsigned> getRange(const Value *v);
		virtual inline Conf *operator[](unsigned k) {return ind[k];}
		virtual inline unsigned size(void) {return ind.size();}
	
	protected:
		std::vector<Conf*> ind;
		IndexOrder order;
		unsigned col;
	};
	
	/// Types of minimisation algorithms
	typedef enum class ExplMinAlgos_ {
		SC,
		DC
	} ExplMinAlgos;

	/// Explicit Pareto calculator
	class ExplCalculator : public Calculator
	{



	public:	
		ExplCalculator(ExplMinAlgos m = ExplMinAlgos_::SC) : minalgo(m) {}
		virtual ~ExplCalculator() {}

		virtual Confset *newConfset(void);
		virtual Confset *newConfset(const Confset*);
		
		// elementary operations
		virtual Confset *minimise(Confset*);
		
		/// Simple-Cull minimisation
		virtual Confset *minimiseSimpleCull(Confset*);
		
		/// Divide & Conquer minimisation
		virtual Confset *minimiseDivideNConquer(Confset*);
		
		/// Select a minimise algorithm; Simple Cull is default
		virtual void setMinimiseAlgo(ExplMinAlgos m) {minalgo = m;}
		
		virtual Confset *multiply(const Confset*, const Confset*);	
		virtual Confset *abstract(Confset*, unsigned);
		virtual Confset *abstract(Confset*, unsigned, unsigned);
		virtual Confset *constrain(Confset*, const Confset*);
		virtual Confset *constrain(Confset*, bool (*)(Conf*));
		virtual Confset *constrain(Confset*, const Conf*);
		virtual Confset *unite(const Confset*, const Confset*);
		
		virtual Iter *prodgen(std::vector<Confset*>);
		virtual Iter *prodgen(Confset*, Confset*);
		
		virtual Confset *addDerived(Confset*, Value *(*)(Conf*, const void*), const void* = NULL);
		virtual Confset *addDerivedVector(Confset*, Conf *(*)(Conf*, const void*), const void* = NULL);
		
		// compound operations
		virtual Confset *prodcons(const Confset*, unsigned, const Confset*, unsigned,
								  Value *(*f)(const Value*));
		virtual Confset *join(const Confset*, unsigned, const Confset*, unsigned, bool abstr=false);
		
	protected:
		ExplMinAlgos minalgo;
	};


	/// Product generator for explicit calculator
	class ExplProdGen : public Iter {
	public:
		ExplProdGen(std::vector<Confset*> Clist);
		ExplProdGen(Confset *C0, Confset *C1);
		virtual ~ExplProdGen(void);
		virtual void init(void);
		virtual Conf *operator*(void);	
		virtual void operator++(int);
		virtual inline bool done(void) {return stop;}
		
	protected:
		std::vector<Confset*> l;
		std::vector<Iter*> itervec;
		Conf *pconf;
		bool stop;
	};
}

#endif // EXPLICIT_H
