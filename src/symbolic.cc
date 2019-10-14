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


#include <symbolic.h>
#include <gbdd.h>
#include <typeinfo>
#include <iostream>
#include <fstream>
#include <pareto.h>

using namespace gbdd;


namespace Pareto
{
	Space *space = Space::create_default();
	
	const BddConfset* SymConfset::get_rel() const
	{
		return rel;
	}

	void SymConfset::set_rel( const BddConfset *r)
	{
		rel = new BddConfset(*r);
		hidden.resize(r->arity(), false);
		setQuants.resize(r->arity(), false);
	}

	SymConfset::~SymConfset(void)
	{
		if(conv)
		{
			delete rel;
		}
		calce.disposeAll();

	}

	Confset *SymConfset::copy(void) const
	{
		SymConfset *C = new SymConfset(*this);
		if(conv)
			C->set_rel(new BddConfset(*rel));
		else
			C->econf = (ExplConfset*) econf->copy();
		C->conv = conv;
		C->hidden = hidden;
		C->setQuants = setQuants;
		return C;
	}

	void SymConfset::add(Conf *c)
	{
		if(conv)
		{
			char ex[128];
			sprintf(ex, "Dynamic addition in symbolic version is not supported. %s",
					c->str().data());
			throw Exception(ex);
		}
		else
		{
			if (sig.empty()){
				Confset *CT = new ExplConfset(calce);
				econf = CT;
				sig = c->signature();
				hidden.resize(c->size(), false);
				setQuants.resize(c->size(), false);
				rquants.resize(c->size(), new RealQuant());
			}
			econf->add(c);
		}
	}

	void SymConfset::exp2sym()
	{
		
		Iterator i0(econf);
		Domain::Var var = 0;
		quantities qs;

		for(unsigned int i=0; i<(*i0)->size(); ++i)
		{
			RealQuant RQ1;
			
			if((setQuants.size()<=i) || !setQuants[i])
			{
				if((*i0)->signature()[i+1] == QFLOAT )
					RQ1.IsReal = true;
				else
					RQ1.IsUnordered = true;
			
				Iterator i1(econf);
				while (!i1.done()) 
				{
					if((*i0)->signature()[i+1] == QFLOAT )
						RQ1.FValues.insert(((FloatValue*)((*i1)->get(i)))->getFloat());
					else
						RQ1.FValues.insert(( (FloatValue*)(*i1)->get(i))->getFloat());
					i1++;
				}
			}
			else
				RQ1 = *rquants[i];

			quantity Q1(space,RQ1.FValues.size());
			Domain new_dom1 = Q1.get_domain();
			new_dom1 = new_dom1 + var +1;
			var = new_dom1.highest();
			new_dom1.RealVal = RQ1;
			Q1 = quantity(new_dom1, Q1.get_bdd());
			qs = qs * Q1;
		}
		rel = new BddConfset(qs);
		while (!i0.done())
		{
			vector<unsigned int> v;
			for(unsigned int i=0; i<(*i0)->size(); ++i)
			{
				set<double> Fval = qs[i].get_domain().RealVal.FValues;
				set<double>::const_iterator it = Fval.find( ((FloatValue*)((*i0)->get(i)))->getFloat());
				unsigned int n = 0;
				for(set<double>::const_iterator i2 = Fval.begin(); it != i2; ++i2)
					++n;
				v.push_back(n);
			}
			rel->insert(v);
			i0++;
		}
	}


	void SymConfset::sym2exp()
	{
		if(conv)
			econf = rel->conv_exp();
	}

	void SymConfset::conv2sym(){
      if(!conv)
        {
            exp2sym();
            conv=true;
        }
	}


	unsigned SymConfset::remove(Conf *c)
	{
		BddConfset cs(rel->get_quants());
		vector<unsigned int> v;
		for(unsigned int i = 0; i<c->size(); ++i)
			v.push_back(((SymValue*)c->get(i))->get_index());
		cs.insert(v);
		rel = new BddConfset((*rel) - cs);
		return 1;
	}

	void SymConfset::hide(unsigned int k)
	{
		hidden[k] = true;
	}

	void SymConfset::hide(unsigned int k, unsigned int l)
	{
		for (unsigned int i = k; i < l; i++)
			hidden[i] = true;
	}	

	void SymConfset::unhide(unsigned int k)
	{
		hidden[k] = false;
	}

	void SymConfset::unhide(unsigned int k, unsigned int l)
	{
		for (unsigned int i = k; i < l; i++)
			hidden[i] = false;
	}

	unsigned int SymConfset::size(void) const
	{
		return rel->size();
	}

	unsigned int SymConfset::width(void) const
	{
		return rel->arity();
	}

	string SymConfset::str(void) const
	{
		if(conv)
			return rel->str();
		else
			return econf->str();
	}

	Iter *SymConfset::iter(void)
	{
		if(conv)
			sym2exp();
		ExplConfset *C = (ExplConfset*) econf;
		return new ExplIter(*C);
	}

	void SymConfset::clear(void)
	{
		delete rel;
		sig.clear();
	}

	void SymConfset::addAndMin(Conf *c)
	{
		if(!conv)
		{
			econf->addAndMin(c);
		}
		else
		{
			char ex[128];
			sprintf(ex, "Dynamic addition in symbolic version is not supported. %s",
					c->str().data());
			throw Exception(ex);
		}
		
	}

	Confset *SymCalculator::newConfset(void)
	{
		SymConfset *cs = new SymConfset(*this);
		csList.push_back(cs);
		return cs;
	}
		
		
	Confset *SymCalculator::newConfset(const Confset *C) 
	{
		SymConfset *cs = (SymConfset*) C->copy();
		csList.push_back(cs);
		return cs;	
	}

	Confset *SymCalculator::minimise(Confset *C)
	{
		if(!((SymConfset*)C)->conv)
		{
			((SymConfset*)C)->econf = ((SymConfset*)C)->calce.minimise(((SymConfset *)C)->econf);
			return C;
		}
		else
		{
			BddConfset *C0 = new BddConfset(*((SymConfset*) C)->get_rel());

			((SymConfset*)C)->set_rel(new BddConfset((C0->minimise(((SymConfset*) C)->hidden))));
			delete C0;
			return C;
		}
	}

	Confset *SymCalculator::advanced_minimise(const Confset *C)
	{
		((SymConfset*)C)->conv2sym();
		
		BddConfset *C0 = new BddConfset(*((SymConfset*) C)->get_rel());
		SymConfset *Cmin = (SymConfset*) newConfset();

		Cmin->set_rel(new BddConfset((C0->advanced_minimise(((SymConfset*) C)->hidden))));
		delete C0;
		Cmin->conv = true;
		return Cmin;
	}

	Confset *SymCalculator::multiply(const Confset *C, const Confset *D)
	{
		((SymConfset*)C)->conv2sym();
		((SymConfset*)D)->conv2sym();

		BddConfset *C0 = new BddConfset(*((SymConfset*) C)->get_rel());
		BddConfset *D0 = new BddConfset(*(((SymConfset*) D)->get_rel()));
		SymConfset *Cp = (SymConfset*) newConfset();
		Cp->set_rel(new BddConfset(*C0 * *D0));
		delete C0;
		delete D0;
		Cp->conv = true;
		return Cp;
	}

	Confset *SymCalculator::abstract(Confset *C, unsigned int k)
	{
		((SymConfset*)C)->conv2sym();
		BddConfset *C0 = new BddConfset(*(((SymConfset*) C)->get_rel()));
		((SymConfset*)C)->set_rel(new BddConfset(C0->my_abstract(k)));
		delete C0;		
		return C;
	}

	Confset *SymCalculator::abstract(Confset *C, unsigned int k, unsigned int l)
	{
		((SymConfset*)C)->conv2sym();
		BddConfset *C0 = new BddConfset(*(((SymConfset*) C)->get_rel()));
		((SymConfset*)C)->set_rel(new BddConfset(C0->my_abstract(k,l)));
		delete C0;		
		return C;
	}

	Confset *SymCalculator::constrain(Confset *C, const Confset *D)
	{
		((SymConfset*)C)->conv2sym();
		((SymConfset*)D)->conv2sym();

		BddConfset *C0 = new BddConfset(*(((SymConfset*) C)->get_rel()));
		BddConfset *D0 = new BddConfset(*(((SymConfset*) D)->get_rel()));
		((SymConfset*)C)->set_rel(new BddConfset(*C0 & *D0));
		delete C0;
		delete D0;
		return C;
	}

	Confset *SymCalculator::unite(const Confset *C, const Confset *D)
	{
		((SymConfset*)C)->conv2sym();
		((SymConfset*)D)->conv2sym();


		BddConfset *C0 = new BddConfset(*(((SymConfset*) C)->get_rel()));
		BddConfset *D0 = new BddConfset(*(((SymConfset*) D)->get_rel()));
		SymConfset *Cu = (SymConfset*) newConfset();
		Cu->set_rel(new BddConfset(*C0 | *D0));
		delete C0;
		delete D0;
		Cu->conv = true;
		return Cu;
	}

	Confset *SymCalculator::join(const Confset *C, unsigned k, const Confset *D,
								 unsigned l, bool abstr)
	{
		((SymConfset*)C)->conv2sym();
		((SymConfset*)D)->conv2sym();


		BddConfset *C0 = new BddConfset(*(((SymConfset*) C)->get_rel()));
		BddConfset *D0 = new BddConfset(*(((SymConfset*) D)->get_rel()));
		SymConfset *Cu = (SymConfset*) newConfset();
		Cu->set_rel(new BddConfset(C0->join(*D0,k,l,abstr)));
		delete C0;
		delete D0;
		Cu->conv = true;
		return Cu;
	}

	Iter *SymCalculator::prodgen(vector<Confset*> Clist)
	{
		SymConfset *C = (SymConfset*) Clist[0];
		C->conv2sym();
		for(unsigned int i = 1; i<Clist.size(); ++i)
		{
			SymConfset *D = (SymConfset*) Clist[i];
			D->conv2sym();
			Confset *Cprod = multiply(C, D);
			disposeConfset(C);
			C = (SymConfset*) Cprod;
		}
		
		C->sym2exp();
		ExplConfset *Ce = (ExplConfset*) C->econf;
		return new ExplIter(*Ce);
	}

	Iter *SymCalculator::prodgen(Confset *C0, Confset *C1)
	{
		((SymConfset*)C0)->conv2sym();
		((SymConfset*)C1)->conv2sym();
		SymConfset *Cprod = (SymConfset *)multiply(C0, C1);
		Cprod->sym2exp();
		ExplConfset *C = (ExplConfset*) Cprod->econf;
		return new ExplIter(*C);
	}
	
	
	Confset *SymCalculator::addDerived(Confset *C, Value *(*f)(Conf*, const void*), const void *x)
	{	
		((SymConfset*)C)->sym2exp();
		((SymConfset*)C)->calce.addDerived(((SymConfset*)C)->econf,f);
		if(((SymConfset*)C)->conv)
			delete ((SymConfset*)C)->rel;
		((SymConfset*)C)->conv = false;

		return C;
	}
	
	Confset *SymCalculator::addDerivedVector(Confset *C, Conf *(*F)(Conf*, const void*), const void *x)
	{
		((SymConfset*)C)->sym2exp();	
		((SymConfset*)C)->calce.addDerivedVector(((SymConfset*)C)->econf,F);
		if(((SymConfset*)C)->conv)
			delete ((SymConfset*)C)->rel;
		((SymConfset*)C)->conv = false;		
		return C;
	}
		
	Confset *SymCalculator::prodcons(const Confset *C, unsigned pq, const Confset * D, unsigned cq,
									  Value *(*f)(const Value*))
	{
		((SymConfset*)C)->conv2sym();
		((SymConfset*)D)->conv2sym();

		BddConfset *C0 = new BddConfset(*(((SymConfset*) C)->get_rel()));
		BddConfset *D0 = new BddConfset(*(((SymConfset*) D)->get_rel()));
	
		SymConfset *Cu = (SymConfset*) newConfset();
		Cu->set_rel(new BddConfset(C0->ProdCons(*D0, pq, cq,f)));
		delete C0;
		delete D0;
		Cu->conv = true;
		return Cu;
	}

	Confset *SymCalculator::constrain(Confset *C, bool (*check)(Conf*))
	{
		((SymConfset*)C)->sym2exp();
		((SymConfset*)C)->calce.constrain(((SymConfset*)C)->econf,check);
		if(((SymConfset*)C)->conv)
			delete ((SymConfset*)C)->rel;
		((SymConfset*)C)->conv = false;
		return C;
	}
	
	Confset *SymCalculator::constrain(Confset *C, const Conf *d)
	{
		((SymConfset*)C)->sym2exp();	
		((SymConfset*)C)->calce.constrain(((SymConfset*)C)->econf,d);
		((SymConfset*)C)->conv = false;		
		return C;
	}

	void SymCalculator::identicalQuantities(const Confset *C, unsigned k, const Confset *D, unsigned l)
	{

		RealQuant *rq1;
		RealQuant *rq2;
		SymConfset *Cs = (SymConfset*)C;
		SymConfset *Ds = (SymConfset*)D;
		if(Cs->setQuants[k])
		{
			rq1 = Cs->rquants[k];
		}
		else
		{
			Cs->rquants[k] = new RealQuant();
			rq1 = new RealQuant();
			if(C->signature()[k+1] == QFLOAT )
				rq1->IsReal = true;
			else
				rq1->IsUnordered = true;

			Iterator i1(Cs->econf);
			while (!i1.done()) 
			{
				if(C->signature()[k+1] == QFLOAT )
					rq1->FValues.insert(((FloatConf*)(*i1))->getFloat(k));
				else
					rq1->FValues.insert(( (FloatValue*)((GenConf*)(*i1))->get(k))->getFloat());
				i1++;
			}
		}

		if(Ds->setQuants[l])
		{
			rq2 = Cs->rquants[l];
		}
		else
		{
			Ds->rquants[l] = new RealQuant();
			rq2 = new RealQuant();
			rq2->IsReal = true;
			Iterator i1(Ds->econf);
			while (!i1.done()) 
			{
				if(C->signature()[k+1] == QFLOAT )
					rq1->FValues.insert(((FloatConf*)(*i1))->getFloat(k));
				else
					rq1->FValues.insert(( (FloatValue*)((GenConf*)(*i1))->get(k))->getFloat());
				i1++;
			}
		}

		RealQuant *res = new RealQuant();
		if(C->signature()[k+1] == QFLOAT )
				res->IsReal = true;
			else
				res->IsUnordered = true;
		set<double>::const_iterator ir1 = rq1->FValues.begin();
		set<double>::const_iterator ir2 = rq2->FValues.begin();
		while(ir1 != rq1->FValues.end())
		{
			res->FValues.insert(*ir1);
			++ir1;
		}
		while(ir2 != rq2->FValues.end())
		{
			res->FValues.insert(*ir2);
			++ir2;
		}
		
		Cs->setQuants[k] = true;
		Cs->rquants[k] = res;
		Ds->rquants[l] = res;
		Ds->setQuants[l] = true;
			
	}
		
	
}

