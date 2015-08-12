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


#include "explicit.h"
#include <iterator>
#include <algorithm>

namespace Pareto {

	/*
	 * Explicit confset definition
	 */

	ExplConfset::~ExplConfset(void)
	{
		for (texplconfset::const_iterator i = confset.begin();
			 i != confset.end(); i++)
//			if ((*i)->decRef() == 0)
				delete *i;
	}

	Confset *ExplConfset::copy(void) const
	{
		ExplConfset *C = new ExplConfset(*this);
		C->confset.clear();
		for (texplconfset::const_iterator i = confset.begin();
			 i != confset.end(); i++) {
			C->add(*i);
//			(*i)->incRef();
		}
		C->visible = visible;
		return C;
	}

	void ExplConfset::add(Conf *c)
	{
		if (sig.empty()) {
			confset.insert(c);
//			c->incRef();
			sig = c->signature();
			w = c->size();		
			for (unsigned i = 0; i < w; i++)
				visible.insert(i);
		}
		else if (sig == c->signature()) {
			confset.insert(c->copy());
//			c->incRef();
		}
		else {
			char ex[128];
			sprintf(ex, "Non-matching signature of conf %s. Did not add it.",
					c->str().data());
			throw Exception(ex);
		}
	}

	void ExplConfset::addAndMin(Conf *c)
	{
		Conf *a;
		texplconfset::iterator iter;
		
		// loop thru Cmin
		iter = confset.begin();
		while (iter != confset.end()) {
			a = *iter;
			if (a->dominates(c, visible)) {
				// c is dominated by a conf in Cmin -> don't use c
				delete c;
				return;
			}
			else if (c->dominates(a, visible)) {
				// remove Cmin[i]
				delete a;
				texplconfset::iterator it = iter;
				iter++;
				confset.erase(it);
			}
			else
				iter++;
		}
		
		// c is not dominated by any conf in Cmin -> add c to Cmin
		add(c);
	}
		
	unsigned ExplConfset::remove(Conf *c)
	{
		if (confset.erase(c) > 0)
			return 0;
		else {
			delete c;
			return 1;
		}
	}

	void ExplConfset::hide(unsigned k)
	{
		visible.erase(k);
	}
		
	void ExplConfset::hide(unsigned k, unsigned l)
	{
		for (unsigned i = k; i < l; i++)
			visible.erase(i);
	}
		
	void ExplConfset::unhide(unsigned k)
	{
		visible.insert(k);
	}
		
	void ExplConfset::unhide(unsigned k, unsigned l)
	{
		for (unsigned i = k; i < l; i++)
			visible.insert(i);
	}
		
	Iter *ExplConfset::iter(void)
	{
		return new ExplIter(*this);
	}

	unsigned ExplConfset::size(void) const
	{
		return (unsigned) confset.size();
	}
		
	unsigned ExplConfset::width(void) const
	{
		return w;
	}

	string ExplConfset::str(void) const
	{
		string s;
		if (size() == 0)
			s = "empty confset\n";
		else {
			for (texplconfset::const_iterator i = confset.begin(); i != confset.end(); i++)
				s += (*i)->str() + '\n';
		}
		return s;
	}

	void ExplConfset::clear(void)
	{
		confset.clear();
		w = 0;
		sig.clear();
		visible.clear();
	}
		

	/*
	 * Explicit confset iterator definition
	 */

	Conf *ExplIter::operator*(void)
	{
		return *iter;
	}

	void ExplIter::operator++(int)
	{
		iter++;
	}

	bool ExplIter::done(void)
	{
		return iter == cs.confset.end();
	}

	
	/*
	 * Explicit confset index
	 */
	
	ExplIndex::ExplIndex(ExplConfset *C, unsigned k, bool desc) :
		order(k, desc), col(k)
	{	
		ind.resize(C->confset.size());
		copy(C->confset.begin(), C->confset.end(), ind.begin());
		sort(ind.begin(), ind.end(), order);
	}
	
	pair<unsigned,unsigned> ExplIndex::getRange(const Value *v)
	{
		unsigned l;
		vector<Conf*>::iterator iter;
		for (iter = ind.begin();
			 iter != ind.end() && !v->dominates((*iter)->get(col)); iter++);
		l = unsigned(iter - ind.begin());
		for (; iter != ind.end() && (*iter)->get(col)->dominates(v); iter++);
		return make_pair(l, unsigned(iter - ind.begin()));
	}

	/*
	 * Explicit Pareto calculator definition
	 */
	 
	Confset *ExplCalculator::newConfset(void)
	{
		ExplConfset *cs = new ExplConfset(*this);
		csList.push_back(cs);
		return cs;
	}
		
		
	Confset *ExplCalculator::newConfset(const Confset *C) 
	{
		ExplConfset *cs = (ExplConfset*) C->copy();
		csList.push_back(cs);
		return cs;	
	}

	Confset *ExplCalculator::minimise(Confset *C)
	{
		switch (minalgo) {
		case DC:
			return minimiseDivideNConquer(C);
			break;
		case SC:
		default:
			return minimiseSimpleCull(C);
			break;
		}
	}

	Confset *ExplCalculator::minimiseSimpleCull(Confset *C)
	{
		ExplConfset *C0 = (ExplConfset*) C;
		Conf *a, *b;
		bool add;
		texplconfset min;
		texplconfset::iterator iterC, iterM;
		
		for (iterC = C0->confset.begin(); iterC != C0->confset.end(); iterC++) {
			a = *iterC;
			add = true;
			
			iterM = min.begin();
			while (iterM != min.end()) {
				b = *iterM;
				if (a->dominates(b, C0->visible)) {
					// remove b from Cmin
					delete b;
					texplconfset::iterator it = iterM;
					iterM++;
					min.erase(it);
				}
				else if (b->dominates(a, C0->visible)) {
					// do not add a to Cmin
					add = false;
					break;
				}
				else
					iterM++;
			}
			
			if (add == true)
				min.insert(a);
			else
				delete a;
		}
		
		C0->confset = min;
		return C;
	}

	/**
	 * \todo Not yet ported.
	 */
	Confset *ExplCalculator::minimiseDivideNConquer(Confset *C)
	{
		cout << "divide & conquer minimisation" << endl;
		return C;
	}

	Confset *ExplCalculator::multiply(const Confset *C, const Confset *D)
	{
		ExplConfset *C0 = (ExplConfset*) C;
		ExplConfset *D0 = (ExplConfset*) D;
		texplconfset::const_iterator iter1, iter2;	
		ExplConfset *prod = (ExplConfset*) newConfset();

		for (iter1 =  C0->confset.begin(); iter1 != C0->confset.end(); iter1++) {
			for (iter2 = D0->confset.begin(); iter2 != D0->confset.end(); iter2++) {
				Conf *c = (*iter1)->copy();
				c->append((*iter2)->copy());
				prod->add(c);
			}
		}
		return prod;
	}

	Confset *ExplCalculator::abstract(Confset *C, unsigned k)
	{
		ExplConfset *C0 = (ExplConfset*) C;
		texplconfset::const_iterator iter;
		
		if (k < C->width()) {
			for (iter = C0->confset.begin(); iter != C0->confset.end(); iter++)
				(*iter)->abstract(k);
			
			C0->w--;
			C0->sig.erase(C0->sig.begin() + k + 1);
			C0->visible.erase(k);
			VisibleList::const_iterator i = C0->visible.begin();
			while (i != C0->visible.end()) {
				if (*i > k) {
					C0->visible.insert(*i - 1);
					unsigned m = *i;
					i++;
					C0->visible.erase(m);
				}
				else
					i++;
			}
		}
		
		return C;
	}
		
	Confset *ExplCalculator::abstract(Confset *C, unsigned k, unsigned l)
	{
		ExplConfset *C0 = (ExplConfset*) C;
		texplconfset::const_iterator iter;
		
		if (l > C->width())
			l = C->width();
		
		if (k < l) {
			for (iter = C0->confset.begin(); iter != C0->confset.end(); iter++)
				(*iter)->abstract(k, l);
			
			C0->w -= l - k;
			for (unsigned i = k; i < l; i++) {
				C0->sig.erase(C0->sig.begin() + k + 1);
				C0->visible.erase(i);
			}
			VisibleList::const_iterator i = C0->visible.begin();
			while (i != C0->visible.end()) {
				if (*i > l) {
					C0->visible.insert(*i - (l - k));
					unsigned m = *i;
					i++;
					C0->visible.erase(m);
				}
				else
					i++;
			}
		}
		
		return C;
	}
		
	Confset *ExplCalculator::constrain(Confset *C, const Confset *D)
	{
		ExplConfset *C0 = (ExplConfset*) C;
		ExplConfset *D0 = (ExplConfset*) D;
		texplconfset::const_iterator i0 = C0->confset.begin();
		texplconfset::const_iterator i1 = D0->confset.begin();
		ltconf cmp;
		
		while (i0 != C0->confset.end() && i1 != D0->confset.end()) {
			if ((*i0)->equals(*i1, C0->visible)) {
				i0++;
				i1++;
			}
			else if (cmp(*i0, *i1)) {
				Conf *c = *i0;
				i0++;
				C0->remove(c);
			}
			else {
				i1++;
			}
		}
		
		return C;
	}

	Confset *ExplCalculator::constrain(Confset *C, bool (*check)(Conf*))
	{
		ExplConfset *C0 = (ExplConfset*) C;
		texplconfset::const_iterator i = C0->confset.begin();

		while (i != C0->confset.end()) {
			if (!check(*i)) {
				Conf *c = *i;
				i++;
				C0->remove(c);
			}
			else
				i++;
		}
		
		return C;
	}

	Confset *ExplCalculator::constrain(Confset *C, const Conf *d)
	{
		ExplConfset *C0 = (ExplConfset*) C;
		texplconfset::const_iterator i = C0->confset.begin();
		
		while (i != C0->confset.end()) {
			if (!(*i)->dominates(d)) {
				Conf *c = *i;
				i++;
				C0->remove(c);
			}
			else
				i++;
		}
		
		return C;
	}

	Confset *ExplCalculator::unite(const Confset *C, const Confset *D)
	{
		ExplConfset *Cu = (ExplConfset*) newConfset(C);
		ExplConfset *D0 = (ExplConfset*) D;
		texplconfset::const_iterator i;
		
		for (i = D0->confset.begin(); i != D0->confset.end(); i++)
			Cu->add((*i)->copy());

		return Cu;
	}

	Iter *ExplCalculator::prodgen(vector<Confset*> Clist)
	{
		return new ExplProdGen(Clist);
	}

	Iter *ExplCalculator::prodgen(Confset *C0, Confset *C1)
	{
		return new ExplProdGen(C0, C1);
	}

	Confset *ExplCalculator::addDerived(Confset *C, Value *(*f)(Conf*, const void*), const void *x)
	{
		ExplConfset *C0 = (ExplConfset*) C;
		texplconfset::const_iterator iter;
		Conf *c;

		if (C->size() > 0) {
			for (iter = C0->confset.begin(); iter != C0->confset.end(); iter++) {
				c = *iter;
				c->append(f(c, x));
			}

			C0->w++;
			C0->sig = c->signature();
			C0->visible.insert(C0->w - 1);
		}
		
		return C;
	}
		
	Confset *ExplCalculator::addDerivedVector(Confset *C, Conf *(*F)(Conf*, const void*), const void *x)
	{
		ExplConfset *C0 = (ExplConfset*) C;
		texplconfset::const_iterator iter;
		Conf *c;
		
		if (C->size() > 0) {
			for (iter = C0->confset.begin(); iter != C0->confset.end(); iter++) {
				c = *iter;
				c->append(F(c, x));
			}
			
			for (unsigned i = C0->w; i < c->size(); i++)
				C0->visible.insert(i);
			C0->w = c->size();
			C0->sig = c->signature();
		}
		
		return C;
	}

	Confset *ExplCalculator::prodcons(const Confset *P, unsigned k,
									  const Confset *C, unsigned l,
									  Value *(*f)(const Value*))
	{
		ExplConfset *Cpc = (ExplConfset*) newConfset();
		ExplIndex IP((ExplConfset*) P, k);
		ExplIndex IC((ExplConfset*) C, l);
		Conf *c;
		unsigned i = 0;
		int j = (int) IP.size() - 1;
		
		while (i < IC.size() && j >= 0) {
			if (IC[i]->get(l)->dominates(f(IP[j]->get(k)))) {
				c = IP[j]->copy();
				c->append(IC[i]->copy());
				Cpc->add(c);
				i++;
			}
			else {
				j--;
				if (j >= 0) {
					for (unsigned a = 0; a < i; a++) {
						c = IP[j]->copy();
						c->append(IC[a]->copy());
						Cpc->add(c);
					}
				}
			}
		}
		if (i == IC.size()) {
			for (unsigned a = 0; a < IC.size(); a++) {
				for (int b = 0; b < j; b++) {
					c = IP[b]->copy();
					c->append(IC[a]->copy());
					Cpc->add(c);
				}
			}
		}
		return Cpc;
	}
	
	Confset *ExplCalculator::join(const Confset *C, unsigned k, const Confset *D,
								  unsigned l, bool abstr)
	{
		ExplConfset *Cjoin = (ExplConfset*) newConfset();
		unsigned i, il, iu, jl, ju;
		i = il = iu = jl = ju = 0;
		pair<unsigned,unsigned> p;
		
		if (C->size() > 0 && D->size() > 0) {
			ExplIndex IC((ExplConfset*) C, k);
			ExplIndex ID((ExplConfset*) D, l);
			
			while (i < IC.size()) {
				const Value *test = IC[i]->get(k);
				
				p = IC.getRange(test);
				il = p.first;
				iu = p.second;
				p = ID.getRange(test);
				jl = p.first;
				ju = p.second;
				
				for (unsigned m = il; m < iu; m++) {
					for (unsigned n = jl; n < ju; n++) {
						Conf *c = IC[m]->copy();
						c->append(ID[n]->copy());
						if (abstr)
							c->abstract(C->width() + l);
						Cjoin->add(c);
					}
				}
							
				i = iu;
			}
		}
		
		return Cjoin;
	}

	/*
	 * Product generator
	 */
	 
	ExplProdGen::ExplProdGen(vector<Confset*> Clist) : l(Clist)
	{
		init();
	}

	ExplProdGen::ExplProdGen(Confset *C, Confset *D)
	{
		l.push_back(C);
		l.push_back(D);
		init();
	}

	ExplProdGen::~ExplProdGen(void)
	{
		for (unsigned i = 0; i < itervec.size(); i++)
			delete itervec[i];
	}

	void ExplProdGen::init(void)
	{
		for (unsigned i = 0; i < l.size(); i++)
			itervec.push_back(l[i]->iter());
		pconf = NULL;
		stop = false;
	}

	Conf *ExplProdGen::operator*(void)
	{
		if (pconf == NULL) {
			pconf = (**itervec[0])->copy();
			for (unsigned i = 1; i < itervec.size(); i++)
				pconf->append((**itervec[i])->copy());
		}
		return pconf;
	}

	void ExplProdGen::operator++(int)
	{
		(*itervec[itervec.size() - 1])++;
		for (int i = (unsigned) itervec.size() - 1; i >= 0; i--) {
			if (itervec[i]->done()) {
				if (i == 0)
					stop = true;
				else {
					delete itervec[i];
					itervec[i] = l[i]->iter();
					(*itervec[i-1])++;
				}
			}
		}
		pconf = NULL;
	}

}
