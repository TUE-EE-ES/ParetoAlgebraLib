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


#include "bddconfset.h"
#include "gbdd.h"
#include <typeinfo>
#include <iostream>
#include <fstream>

using namespace gbdd;

namespace Pareto
{
	typedef std::pair<Bdd*, unsigned int> BDDnc;
	enum{ //andOrder
	  smallestFirst = 0,
	  largestFirst = 1,
	  smallestFirstRandom = 2,
	  largestFirstRandom = 3
	};


	struct BDDPredicate {
	  const int andOrder;
	  BDDPredicate(int andOrder_ = smallestFirst) : andOrder(andOrder_) {}
	  bool operator()(const BDDnc& a, const BDDnc& b){
		switch(andOrder){
		  //smallest BDDs first: faster but needs more memory
		  case smallestFirst: return (a.second > b.second);

		  //largest BDDs first: less memory consumption but slower
		  case largestFirst: return (a.second < b.second);

		  //smallest BDDs first and randomly if node counts equal: usually faster but needs more memory
		  case smallestFirstRandom: return (a.second == b.second) ? (rand() < 0.5) : (a.second > b.second);

		  //largest BDDs first and randomly if node counts equal: usually less memory consumption but slower
		  case largestFirstRandom: return (a.second == b.second) ? (rand() < 0.5) : (a.second < b.second);
		}
		return false;
	  };
	};


	std::priority_queue<BDDnc, std::vector<BDDnc>, BDDPredicate> prQueue;

	Bdd* andBDDs(){

	  while(prQueue.size() > 1){
		cerr<<prQueue.size()<<" BDDs remaining: ";

		Bdd* add1 = prQueue.top().first;
		prQueue.pop();

		unsigned int nodeCount = add1->size();
		cerr<<"("<<nodeCount;

		Bdd* add2 = prQueue.top().first;
		prQueue.pop();

		nodeCount = add2->size();
		cerr<<") & ("<<nodeCount;

		cerr<<") ==> ";

		Bdd* addResult = (new Bdd((*add1) & (*add2)));

		nodeCount = addResult->size();
		cerr<<"("<<nodeCount<<endl;

		delete add1;
		delete add2;

		prQueue.push(BDDnc(addResult, nodeCount));
	  }

	  Bdd* resultBDD = prQueue.top().first;
	  prQueue.pop();

	  return resultBDD;
	}

	BddConfset *BddConfset::Conv_exp(Confset *econ)
	{

		Iterator i0(econ);
		Domain::Var var = 0;
		quantities qs;

		for(unsigned int i=0; i<(*i0)->size(); ++i)
		{
			RealQuant RQ1;
			RQ1.IsReal = true;
			Iterator i1(econ);
			while (!i1.done())
			{
				RQ1.FValues.insert(((FloatConf*)(*i1))->getFloat(i));
				i1++;
			}
			quantity Q1(get_space(),RQ1.FValues.size());
			Domain new_dom1 = Q1.get_domain();
			new_dom1 = new_dom1 + var +1;
			var = new_dom1.highest();
			new_dom1.RealVal = RQ1;
			Q1 = quantity(new_dom1, Q1.get_bdd());
			qs = qs * Q1;
		}
		BddConfset *rel = new BddConfset(qs);
		while (!i0.done())
		{
			vector<unsigned int> v;
			for(unsigned int i=0; i<(*i0)->size(); ++i)
			{
				set<double> Fval = qs[i].get_domain().RealVal.FValues;
				set<double>::const_iterator it = Fval.find(((FloatConf*)(*i0))->getFloat(i));
				unsigned int n = 0;
				for(set<double>::const_iterator i2 = Fval.begin(); it != i2; ++i2)
					++n;
				v.push_back(n);
			}
			rel->insert(v);
			i0++;
		}
		return rel;
	}


	Bdd BddConfset::get_add(const Domain& d0, const Domain& d1, const Domain& d2)
	{

//		unsigned int n = d0.size();
		Domain vd1 = d0;
		Domain vd2 = d1;
		Domain vs=d2;
		Bdd ci(get_space(),false);
		Bdd s(get_space(),true);
		Domain::const_iterator i2 = vd2.begin();
		Domain::const_iterator si = vs.begin();
		int count = 0;

		for(Domain::const_iterator i1 = vd1.begin(); i1 != vd1.end(); ++i1)
		{


			Bdd s0 = !Bdd::var_equal(get_space(), *i1, *i2);
			++count;
			Bdd t=Bdd::bdd_product((s0 ^ ci), Bdd::var_true(get_space(),*si), StructureConstraint::fn_iff);

			s &= Bdd::bdd_product((s0 ^ ci), Bdd::var_true(get_space(),*si), StructureConstraint::fn_iff);

			ci = (s0&ci) | (Bdd::var_true(get_space(), (*i1)) & Bdd::var_true(get_space(),(*i2)));


			++i2;
			++si;
		}
		return s;
	}

	Bdd BddConfset::get_add(const Domain& d0, const Domain& d1, const Domain& d2, int a)
	{
//		unsigned int n = d0.size();
		Domain vd1 = d0;
		Domain vd2 = d1;
		Domain vs=d2;
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
		Domain d3 = Domain(1,d0.size()) + (d1|d0|d2).highest();
		BddSet r1(d3,Bdd(get_space(),false));
		r1.insert(a);

		BddConfset rel = BddConfset((Domains) d0 * (Domains) d1 * (Domains) d2 * (Domains) d3, s&(*get_dominance(d2,d3,false,false) & r1.get_bdd())).Abstract_Domain(d3);

		return rel.get_bdd();
	}

	//Add two Bdd
	Bdd BddConfset::get_add(const BddSet& s0, const BddSet& s1, int a)
	{

//		unsigned int n = s0.get_domain().size();
		Domain vd1 = s0.get_domain();
		Domain vd2 = s1.get_domain();
		Domain vs=s0.get_domain()+(s0.get_domain()|s1.get_domain()).highest()+1;
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
		Domain d3 = Domain(1,s0.get_domain().size()) + (s1.get_domain()|s0.get_domain()|vs).highest();
		BddSet r1(d3,Bdd(get_space(),false));
		r1.insert(a);

		BddConfset rel = BddConfset((Domains)s0.get_domain()*(Domains)s1.get_domain()*(Domains)vs*(Domains)d3, (*get_dominance(vs,d3,false,false) & r1.get_bdd()) & (s & s0.get_bdd() & s1.get_bdd())).Abstract_Domain(d3);
		Bdd rb = rel.get_bdd();
		rb = rb.exists(s0.get_domain());
		rb = rb.exists(s1.get_domain());
		rb.rename(vs, s0.get_domain());

		return rb;
	}

	//Add two Bdd
	Bdd BddConfset::get_add(const BddSet& s0, const BddSet& s1)
	{

//		unsigned int n = s0.get_domain().size();
		Domain vd1 = s0.get_domain();
		Domain vd2 = s1.get_domain();
		Domain vs=s0.get_domain()+(s0.get_domain()|s1.get_domain()).highest()+1;
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

		BddConfset rel = BddConfset((Domains)s0.get_domain()*(Domains)s1.get_domain()*(Domains)vs, (s0.get_bdd() & s1.get_bdd()& s));
		Bdd rb = rel.get_bdd();
		rb = rb.exists(s0.get_domain());
		rb = rb.exists(s1.get_domain());
		rb.rename(vs, s0.get_domain());
		return rb;
	}

	unique_ptr<Bdd>  BddConfset::get_dominance(const Domain& d0, const Domain& d1, bool gl, bool disjoined)
	{
//		unsigned int n = d0.size();
		Domain vs1 = d0;
		Domain vs2 = d1;
		unique_ptr<Bdd> r(new Bdd(Bdd::vars_equal(get_space(),vs1,vs2)));;
		if(d0.RealVal.IsUnordered)
			return r;
		vector<Space::Var> vvs1;
		vector<Space::Var> vvs2;
		for(Domain::const_iterator i1 = vs1.begin(); i1 != vs1.end(); ++i1)
			vvs1.push_back(*i1);
		for(Domain::const_iterator i1 = vs2.begin(); i1 != vs2.end(); ++i1)
			vvs2.push_back(*i1);


		unique_ptr<Bdd> p(new Bdd(get_space(), false));

		for(int i=(vvs1.size()-1); 0<=i; --i)
		{
			unique_ptr<Bdd> q(new Bdd(get_space(), true));
			for(int j=(vvs1.size()-1); i<j; --j)
			{
				unique_ptr<Bdd> qt(new Bdd(Bdd::var_equal(get_space(), vvs1[j], vvs2[j])));
				unique_ptr<Bdd> q1(q->ptr_product(*qt, StructureConstraint::fn_and));
				q = std::move(q1);
			}
			if(gl)
			{
				unique_ptr<Bdd> q1(q->ptr_product(Bdd::var_true(get_space(), vvs1[i]) & Bdd::var_false(get_space(), vvs2[i]),
					StructureConstraint::fn_and));
				q = std::move(q1);

			}
			else
			{
				unique_ptr<Bdd> q1(q->ptr_product(Bdd::var_false(get_space(), vvs1[i]) & Bdd::var_true(get_space(), vvs2[i]),
					StructureConstraint::fn_and));
				q = std::move(q1);
			}
			unique_ptr<Bdd> q1(p->ptr_product(*q, StructureConstraint::fn_or));
			p = std::move(q1);

		}

		if(disjoined)
			return p;
		else
		{
			unique_ptr<Bdd> q1(p->ptr_product(*r, StructureConstraint::fn_or));
			return q1;
		}
	}

	//Constructor
	BddConfset::BddConfset(quantities qs)
	{
		Domains dm;
		for(unsigned int i = 0; i < qs.size(); ++i)
		{
			dm = dm * qs[i].get_domain();
		}
		*this = BddConfset(dm, Bdd(qs[0].get_space(),false));
		quants = qs;
		return;
	}

	const quantities BddConfset::get_quants() const
	{
		return quants;
	}

	void BddConfset::set_quants( const quantities &qs)
	{
		quants = qs;
		return;
	}

	unsigned int BddConfset::size()
	{
		BddConfset r=*this;
		int n=0;
		vector<BddSet> projected;
		{
			unsigned int i;
			for (i = 0;i < r.arity();++i)
			{
				projected.push_back(r.project_on(i));
			}
		}
		vector<BddSet::const_iterator> is;

		{
			unsigned int i;

			for(i = 0;i < r.arity();++i)
			{
				is.push_back(projected[i].begin());
			}
		}

		while(is.size() > 0 && is[0] != projected[0].end())
		{
			{
				vector<StructureSet> element;
				unsigned int i;

				for(i = 0; i < is.size();++i)
				{
					element.push_back(BddSet(projected[i], *(is[i])));
				}

				BddConfset prod = StructureRelation::cross_product(r.get_domains(), element);

				if (!(prod & r).is_false())
				{
					++n;

				}
			}

			// Increase to next

			unsigned int i = r.arity() - 1;

			++(is[i]);

			while (is[i] == projected[i].end() && i > 0)
			{
				is.pop_back();
					--i;
				++(is[i]);
			}

			if(is[i] != projected[i].end())
			{
				++i;
				while (i < projected.size())
				{
					is.push_back(projected[i].begin());
					++i;
				}
			}

		}
		return n;
	}


	BddConfset BddConfset::Abstract(const quantity& Q)
	{
		unsigned int i;
		Domains doms1;
		Domains doms2 = get_domains();
		for ( i = 0;i < doms2.size()  ;++i)
			if(doms2[i]!= Q.get_domain())
				doms1 = doms1 * doms2[i];

		unique_ptr<StructureConstraint> product(get_bdd_based().ptr_project(Q.get_domain()));

		return StructureRelation(doms1, *product);
	}


	BddConfset BddConfset::Abstract_Domain(const Domain& d)
	{
		unsigned int i;
		Domains doms1;
		Domains doms2 = get_domains();
		for ( i = 0;i < doms2.size()  ;++i)
			if(doms2[i]!= d)
				doms1 = doms1 * doms2[i];
		unique_ptr<StructureConstraint> product(get_bdd_based().ptr_project(d));

		return StructureRelation(doms1, *product);
	}

	Confset *BddConfset::conv_exp(void) const
	{
		BddConfset r = *this;
		ExplCalculator calce;
		Confset *CT = new ExplConfset(calce);
		GenConf *c;
		FloatValue *v;
		UnorderedFloatValue *uv;

		while(!r.is_false())
		{

			vector<set<unsigned int> > projected;
			{
				unsigned int i;
				for (i = 0;i < r.arity();++i)
				{
					projected.push_back(r.project_on(i).get_bdd().assignments_value(r.get_domain(i)));
				}
			}
			vector<set<unsigned int>::const_iterator> is;

			{
				unsigned int i;

				for(i = 0;i < r.arity();++i)
				{
					is.push_back(projected[i].begin());
				}
			}
			bool test = true;

		while(is.size() > 0 && is[0] != projected[0].end()&& test)
		{
			{
				vector<StructureSet> element;
				unsigned int i;

				for(i = 0; i < is.size();++i)
				{
					BddSet tmp(r.get_space(), *(is[i]));
					element.push_back(BddSet(r.get_domain(i), tmp));
				}

				BddConfset prod = StructureRelation::cross_product(r.get_domains(), element);

				if (!(prod & r).is_false())
				{
					c = new GenConf();
					r = r - prod;
					test = false;
					if(r.get_domains()[0].RealVal.IsReal){
						set<double>::const_iterator ir = r.get_domains()[0].RealVal.FValues.begin();
						for(unsigned int i =0 ; i<(*is[0]);++i)
							++ir;

						v = new FloatValue(*ir);
						c->append(v);}
						
					else{
						set<double>::const_iterator ir = r.get_domains()[0].RealVal.FValues.begin();
						for(unsigned int i =0 ; i<(*is[0]);++i)
							++ir;
						uv = new UnorderedFloatValue(*ir);
						c->append(uv);}
						
					vector<set<unsigned int>::const_iterator>::iterator j = is.begin();

					++j;
					int count=1;
					while(j != is.end())
					{
						if(r.get_domains()[count].RealVal.IsReal){
						set<double>::const_iterator ir = r.get_domains()[count].RealVal.FValues.begin();
						for(unsigned int i =0 ; i<(**j);++i)
							++ir;
						v = new FloatValue(*ir);
						c->append(v);}

						else{
						set<double>::const_iterator ir = r.get_domains()[count].RealVal.FValues.begin();
						for(unsigned int i =0 ; i<(**j);++i)
							++ir;
						uv = new UnorderedFloatValue(*ir);
						c->append(uv);}
						++j;
						++count;
					}

					CT->add(c);
				}
		}


		// Increase to next

			unsigned int i = r.arity() - 1;

			++(is[i]);

			while (is[i] == projected[i].end() && i > 0)
			{
				is.pop_back();
					--i;
				++(is[i]);
			}

			if(is[i] != projected[i].end())
			{
				++i;
				while (i < projected.size())
				{
					is.push_back(projected[i].begin());
					++i;
				}
			}
		}
		}
		return CT;
	}
	
	string BddConfset::str(void) const
	{
		BddConfset r = *this;
		string s = "\0";

		while(!r.is_false())
		{

			vector<set<unsigned int> > projected;
			{
				unsigned int i;
				for (i = 0;i < r.arity();++i)
				{
					projected.push_back(r.project_on(i).get_bdd().assignments_value(r.get_domain(i)));
				}
			}
			vector<set<unsigned int>::const_iterator> is;

			{
				unsigned int i;

				for(i = 0;i < r.arity();++i)
				{
					is.push_back(projected[i].begin());
				}
			}
		bool test = true;
		while(is.size() > 0 && is[0] != projected[0].end() && test)
		{
			{

				vector<StructureSet> element;
				unsigned int i;

				for(i = 0; i < is.size();++i)
				{
					BddSet tmp(r.get_space(), *(is[i]));
					element.push_back(BddSet(r.get_domain(i), tmp));
				}

				BddConfset prod = StructureRelation::cross_product(r.get_domains(), element);

				if (!(prod & r).is_false())
				{
					r = r - prod;
					test = false;
					if(r.get_domains()[0].RealVal.IsReal){
						set<double>::const_iterator ir = r.get_domains()[0].RealVal.FValues.begin();
						for(unsigned int i =0 ; i<(*is[0]);++i)
							++ir;

						char buf[16];
						sprintf(buf, "%.3f", *ir);
						s += "(" + (string)buf;}

					if(r.get_domains()[0].RealVal.IsInt){
						set<int>::const_iterator ir = r.get_domains()[0].RealVal.IValues.begin();
						for(unsigned int i =0 ; i<(*is[0]);++i)
							++ir;
						char buf[16];
						sprintf(buf, "%.3f", (double)(*ir));
						s += "(" + (string)buf;}

					if(r.get_domains()[0].RealVal.IsUnordered){
						set<double>::const_iterator ir = r.get_domains()[0].RealVal.FValues.begin();
						for(unsigned int i =0 ; i<(*is[0]);++i)
							++ir;

						char buf[16];

						sprintf(buf, "%.3f", (*ir));

						s += "(" + (string)buf;}

					vector<set<unsigned int>::const_iterator>::iterator j = is.begin();

					++j;
					int count=1;
					while(j != is.end())
					{
						if(r.get_domains()[count].RealVal.IsReal){
						set<double>::const_iterator ir = r.get_domains()[count].RealVal.FValues.begin();
						for(unsigned int i =0 ; i<(**j);++i)
							++ir;
						char buf[16];
						sprintf(buf, "%.3f", *ir);
						s += "," + (string)buf;}

					if(r.get_domains()[count].RealVal.IsInt){
						set<int>::const_iterator ir = r.get_domains()[count].RealVal.IValues.begin();
						for(unsigned int i =0 ; i<(**j);++i)
							++ir;
						char buf[16];
						sprintf(buf, "%.3f", (double)(*ir));
						s += "," + (string)buf;}

					if(r.get_domains()[count].RealVal.IsUnordered){
						set<double>::const_iterator ir = r.get_domains()[count].RealVal.FValues.begin();
						for(unsigned int i =0 ; i<(**j);++i)
							++ir;
						char buf[16];

						sprintf(buf,"%.3f", (*ir));

						s += "," + (string)buf;}
						++j;
						++count;
					}

					s += ")\n";
				}
		}


		// Increase to next

			unsigned int i = r.arity() - 1;

			++(is[i]);

			while (is[i] == projected[i].end() && i > 0)
			{
				is.pop_back();
					--i;
				++(is[i]);
			}

			if(is[i] != projected[i].end())
			{
				++i;
				while (i < projected.size())
				{
					is.push_back(projected[i].begin());
					++i;
				}
			}
		}
		}



		return s;
	}

	BddConfset operator*(const BddConfset& rel1, const BddConfset& rel2)
	{
		Domain doms = rel1.get_domains().union_all();

		Domains new_doms;
		for(unsigned int i = 0; i < rel2.get_domains().size(); ++i)
		{
			new_doms = new_doms * (rel2.get_domain(i) + doms.highest() + 1);
			new_doms[i].RealVal = rel2.get_domains()[i].RealVal;
		}

		unique_ptr<BddConfset> r2(new BddConfset(new_doms,rel2));
		unique_ptr<Bdd> b1(rel1.get_bdd().ptr_clone());
		unique_ptr<Bdd> b2 (r2->get_bdd().ptr_clone());
		unique_ptr<Bdd> b3(b1->ptr_product(*b2, StructureConstraint::fn_and));

		return StructureRelation(rel1.get_domains()*new_doms, *b3);

	}

	BddConfset operator|(const BddConfset& rel1, const BddConfset& rel2)
	{
		Domains doms;
		for(unsigned int i=0; i<rel1.arity(); ++i)
			doms = doms * Domain::sup(rel1.get_domain(i),rel2.get_domain(i));

		BddConfset r1(doms, rel1);
		BddConfset r2(doms, rel2);

		return BddConfset(r1.get_domains(),r1.product(r2, StructureConstraint::fn_or));
	}

	//Constrain rel1 & rel2
	BddConfset operator&(const BddConfset& rel1, const BddConfset& rel2)
	{
		Domains doms;
		for(unsigned int i=0; i<rel1.arity(); ++i)
			doms = doms * Domain::sup(rel1.get_domain(i),rel2.get_domain(i));

		BddConfset r1(doms, rel1);
		BddConfset r2(doms, rel2);

		return BddConfset(r1.get_domains(),r1.product(r2, StructureConstraint::fn_and));
	}

	//Mnimization.
	BddConfset BddConfset::minimise(vector<bool> h)
	{
		BddConfset *C = this;
		unsigned int n = C->get_domains().size();
		unique_ptr<BddConfset> Cm(new BddConfset( *C * *C));
		unique_ptr<BddConfset> dm;
		unique_ptr<BddConfset> iden;

		if(!h[0])
		{
			unique_ptr<Bdd> tmp = get_dominance((Cm->get_domains()[0]), (Cm->get_domains()[n]), false,false);
			unique_ptr<Bdd> b1(new Bdd(C->project_on(0).get_bdd() & *tmp));
			unique_ptr<BddConfset> dm1( new BddConfset((Domains) (Cm->get_domains()[0]) * (Domains) (Cm->get_domains()[n]), *b1));
			unique_ptr<Bdd> b2(b1->ptr_product(Bdd::vars_equal(get_space(),(Cm->get_domains()[0]) , (Cm->get_domains()[n])),StructureConstraint::fn_and));
			unique_ptr<BddConfset> iden1(new BddConfset((Domains) (Cm->get_domains()[0]) * (Domains) (Cm->get_domains()[n]), *b2));
			dm = std::move(dm1);
			iden = std::move(iden1);
		}
		else
		{
			unique_ptr<BddConfset> dm1(new BddConfset((Domains) (Cm->get_domains()[0]) * (Domains) (Cm->get_domains()[n]), Bdd(get_space(),true)));
			unique_ptr<BddConfset> iden1(new BddConfset((Domains) (Cm->get_domains()[0]) * (Domains) (Cm->get_domains()[n]), Bdd(get_space(),true)));
			dm = std::move(dm1);
			iden = std::move(iden1);
		}


		unique_ptr<Bdd> new_rel(dm->get_bdd_based().ptr_clone());

		unique_ptr<Bdd> new_rel2(iden->get_bdd_based().ptr_clone());

		unique_ptr<Bdd> new_rel3(C->project_on(0).get_bdd_based().ptr_clone());


		for(unsigned int i=1; i<n; ++i)
		{
			if(!h[i])
			{
				unique_ptr<Bdd> tmp = get_dominance((Cm->get_domains()[i]), (Cm->get_domains()[i+n]), false, false);
				unique_ptr<Bdd> b1(new Bdd(C->project_on(i).get_bdd() & *tmp));
				unique_ptr<BddConfset> dm1(new BddConfset((Domains) (Cm->get_domains()[i]) * (Domains) (Cm->get_domains()[i+n]), *b1));
				unique_ptr<Bdd> b2(b1->ptr_product(Bdd::vars_equal(get_space(),(Cm->get_domains()[i]) , (Cm->get_domains()[i+n])), StructureConstraint::fn_and));
				unique_ptr<BddConfset> iden1(new BddConfset((Domains) (Cm->get_domains()[i]) * (Domains) (Cm->get_domains()[i+n]), *b2));
				dm = std::move(dm1);
				iden = std::move(iden1);
			}
			else
			{
				unique_ptr<BddConfset> dm1(new BddConfset((Domains) (Cm->get_domains()[i]) * (Domains) (Cm->get_domains()[i+n]),  Bdd(get_space(),true)));
				unique_ptr<BddConfset> iden1(new BddConfset((Domains) (Cm->get_domains()[i]) * (Domains) (Cm->get_domains()[i+n]), Bdd(get_space(),true)));
				dm = std::move(dm1);
				iden = std::move(iden1);
			}

			unique_ptr<Bdd> product1(new_rel->ptr_product(dm->get_bdd_based(),
									StructureConstraint::fn_and));

			unique_ptr<Bdd> product(product1->ptr_product(Cm->get_bdd_based(),
									StructureConstraint::fn_and));

			unique_ptr<Bdd> product4(new_rel2->ptr_product(iden->get_bdd_based(),
									StructureConstraint::fn_and));

			unique_ptr<Bdd> product2(product4->ptr_product(Cm->get_bdd_based(),
									StructureConstraint::fn_and));

			new_rel = std::move(product);
			new_rel2 = std::move(product2);
		}

		unique_ptr<Bdd> product(new_rel->ptr_product(*new_rel2,
									StructureConstraint::fn_minus));

		for(unsigned int i = 0; i < n; ++i)
		{
			unique_ptr<Bdd> product1(product->ptr_project(C->get_domain(i)));

			product = std::move(product1);
		}

		for(unsigned int i = 0; i < n; ++i)
		{
			unique_ptr<Bdd> product1(product->ptr_rename(Domain::map_vars(Cm->get_domain(i+n), C->get_domain(i))));

			product = std::move(product1);
		}

		unique_ptr<Bdd> product4(C->get_bdd_based().ptr_product(*product,
									StructureConstraint::fn_minus));

		return StructureRelation(C->get_domains(), *product4);;
	}

	//Mnimization.
	BddConfset BddConfset::advanced_minimise(vector<bool> h)
	{
		BddConfset *C = this;
		unsigned int n = C->get_domains().size();
		unique_ptr<BddConfset> Cm(new BddConfset( *C * *C));
		unique_ptr<BddConfset> dm(new BddConfset(*C));

		for(unsigned int i=0; i<n; ++i)
		{
			if(!h[i])
			{
				unique_ptr<Bdd> tmp = get_dominance((C->get_domains()[i]), (Cm->get_domains()[i+n]), false, true);
				unique_ptr<Bdd> b1(new Bdd(C->project_on(i).get_bdd() & *tmp));
				unique_ptr<BddConfset> dm1(new BddConfset(dm->compose(i, BddRelation((Domains) Cm->get_domain(i) * (Domains) Cm->get_domain(i+n), *b1))));
				dm = std::move(dm1);
			}
		}

		unique_ptr<BddConfset> dmtm(new BddConfset(C->get_domains(), *dm));
		unique_ptr<BddConfset> dm2(new BddConfset(*C & *dmtm));
		return (*C - *dm2);
	}


	BddConfset* BddConfset::add_values(const BddConfset C0, const BddConfset D0)
	{

		Domains dom0;
		Domain sd = (C0.get_domains() * D0.get_domains()).union_all();
		Domains new_doms, old_doms;


		for(unsigned int i=0 ; i<(C0.get_domains().size()-1); ++i)
		{
			new_doms = new_doms * C0.get_domain(i);
			old_doms = old_doms * C0.get_domain(i);
		}

		for(unsigned int i=0; i<(D0.get_domains().size()-1); ++i)
		{
			new_doms = new_doms * (D0.get_domain(i) - D0.get_domain(i).lowest() + sd.highest()+1);
			old_doms = old_doms * D0.get_domain(i);
			dom0 = dom0 * (D0.get_domain(i) - sd.lowest() + sd.highest()+1);
		}
		old_doms = old_doms * D0.get_domain(D0.arity()-1);
		dom0 = dom0 * (D0.get_domain(D0.arity()-1) - sd.lowest() + sd.highest() + 1);
		new_doms = new_doms * (dom0[dom0.size()-1] + (dom0[dom0.size()-1].highest()-dom0[dom0.size()-1].lowest() +1));

		unique_ptr<BddConfset> drename(new BddConfset(dom0, D0));
		unique_ptr<BddConfset> bc(new BddConfset(C0.get_domains()*dom0, C0.get_bdd() & drename->get_bdd()));
		unique_ptr<Bdd> btest(new Bdd(get_add(C0.get_domain(C0.get_domains().size()-1), dom0[dom0.size()-1], new_doms[new_doms.size()-1])));
		unique_ptr<BddConfset> badd(new BddConfset((Domains) C0.get_domain(C0.get_domains().size()-1) * (Domains) dom0[dom0.size()-1] * (Domains) new_doms[new_doms.size()-1], *btest));
		unique_ptr<BddConfset> bv1(new BddConfset((Domains) C0.get_domains() * (Domains) dom0 * (Domains) new_doms[new_doms.size()-1], badd->get_bdd() & bc->get_bdd() ));
		unique_ptr<BddConfset> bv2(new BddConfset(bv1->my_abstract(C0.arity()-1)));

		return  new BddConfset(bv2->my_abstract(bv2->arity()-2));
	}

	BddConfset* BddConfset::constrain(const BddConfset C0, const BddConfset D0, vector<unsigned int> R)
	{
		Domains dom0;
		Domain sd = (C0.get_domains() * D0.get_domains()).union_all();

		Domains new_doms, old_doms;

		for(unsigned int i=0; i<R.size(); ++i)
		{
			new_doms = new_doms * (C0.get_domain(i) - C0.get_domain(i).lowest() + sd.highest()+1);
			old_doms = old_doms * C0.get_domain(i);
			dom0 = dom0 * D0.get_domain(i);
		}

		for(unsigned int i=R.size(); i<(C0.get_domains().size()); ++i)
		{
			new_doms = new_doms * C0.get_domain(i);
			old_doms = old_doms * C0.get_domain(i);
		}

		for(unsigned int i=R.size(); i<(D0.get_domains().size()); ++i)
		{
			new_doms = new_doms * (D0.get_domain(i) - sd.lowest() + sd.highest()+1);
			old_doms = old_doms * D0.get_domain(i);
			dom0 = dom0 * (D0.get_domain(i) - sd.lowest() + sd.highest()+1);
		}

		Bdd *bc0 = C0.get_bdd().ptr_clone();

		Bdd *bd0 = D0.get_bdd().ptr_clone();

		Bdd *brd0 = bd0->ptr_rename(Domain::map_vars(D0.get_domain(D0.arity()-1), dom0[dom0.size()-1]));
		delete bd0;

		Bdd *bcm = bc0->ptr_product((*brd0),StructureConstraint::fn_and);

		delete bc0;
		delete brd0;

		for(unsigned int i=0; i<R.size(); ++i)
		{
			Domain d = new_doms[i];
			Bdd *badd=new Bdd(get_add(C0.get_domains()[i], D0.get_domains()[i],d,R[i]));
			Bdd *be1 = bcm->ptr_product((*badd),StructureConstraint::fn_and);
			delete badd;
			delete bcm;
			Bdd *be2 = be1->ptr_project(C0.get_domain(i));
			delete be1;
			bcm = be2->ptr_project(D0.get_domain(i));
			delete be2;
		}
		return new BddConfset(new_doms, *bcm);;
	}


	BddConfset BddConfset::mmkp(vector<BddConfset> S,vector<BddConfset> V, vector<unsigned int> R)
	{

		unique_ptr<BddConfset> Vm(new BddConfset(V[0]));
		vector<unsigned int> k;
		vector<bool> h;
		for(unsigned int i=1; i<S.size(); ++i)
		{
			unique_ptr<BddConfset> vtmp(add_values(*Vm,V[i]));
			Vm = std::move(vtmp);
		}

		unique_ptr<BddConfset> values(new BddConfset(Vm->my_abstract(Vm->arity()-1)));
		for(unsigned int k=0; k<R.size(); ++k)
		{
			vector<unsigned int> rtmp;
			rtmp.push_back(R[k]);
			unique_ptr<BddConfset> BCtmp(new BddConfset(S[0]));
			for(unsigned int j=0; j<k; ++j)
			{
				unique_ptr<BddConfset> BCtmp2(new BddConfset(BCtmp->my_abstract(0)));
				BCtmp = std::move(BCtmp2);
			}
			for(unsigned int j=k+1; j<R.size(); ++j)
			{
				unique_ptr<BddConfset> BCtmp2(new BddConfset(BCtmp->my_abstract(1)));
				BCtmp = std::move(BCtmp2);
			}

			unique_ptr<BddConfset> Cm = std::move(BCtmp);


			for(unsigned int i=1; i<S.size(); ++i)
			{

				unique_ptr<BddConfset> BCtmp(new BddConfset(S[i]));

				for(unsigned int j=0; j<k; ++j)
				{
					unique_ptr<BddConfset> BCtmp2(new BddConfset(BCtmp->my_abstract(0)));
					BCtmp = std::move(BCtmp2);
				}
				for(unsigned int j=k+1; j<R.size(); ++j)
				{
					unique_ptr<BddConfset> BCtmp2(new BddConfset(BCtmp->my_abstract(1)));
					BCtmp = std::move(BCtmp2);
				}

				unique_ptr<BddConfset> ctmp(constrain(*Cm,*BCtmp, rtmp));
				Cm = std::move(ctmp);
			}
			unique_ptr<BddConfset> ctmp1(new BddConfset(values->get_domains(),Cm->my_abstract(0)));
			Bdd* btmp = (new Bdd(ctmp1->get_bdd()));
			const unsigned int nodeCount = btmp->nodes().size();
			prQueue.push(BDDnc(btmp, nodeCount));
		}

		Bdd* bres = andBDDs();

		Vm->reset(Vm->get_domains(), Vm->get_bdd() & *bres);
		delete bres;
		BddConfset sum(Vm->get_domain(Vm->arity()-1), Vm->project_on(Vm->arity()-1));

		h.push_back(false);
		BddConfset res(Vm->get_domains(), Vm->get_bdd() & sum.minimise(h).get_bdd());

		Domains new_doms = res.get_domains();
		for(unsigned int i=0; i<V.size(); ++i)
			new_doms[i].RealVal = V[i].get_domain(0).RealVal;

		res.reset(new_doms,res.get_bdd());

		return res;
	}

	BddConfset BddConfset::my_abstract(unsigned int k)
	{
			unsigned int i;
			Domains doms1;
			Domains doms2 = get_domains();
			for ( i = 0;i < doms2.size()  ;++i)
				if(i != k)
					doms1 = doms1 * doms2[i];
			unique_ptr<Bdd> b1(get_bdd().ptr_clone());
			unique_ptr<Bdd> b2(b1->ptr_project(get_domain(k)));

			return BddConfset(doms1, *b2);
	}

	BddConfset BddConfset::my_abstract(unsigned int k,unsigned int l)
	{
			unsigned int i;
			Domains doms1;
			Domains abs_doms;
			Domains doms2 = get_domains();
			for ( i = 0;i < doms2.size()  ;++i)
				if(k<=i && i<l)
					abs_doms = abs_doms * doms2[i];
				else
					doms1 = doms1 * doms2[i];
			Domain abs_dom = abs_doms.union_all();
			unique_ptr<Bdd> b1(get_bdd().ptr_clone());
			unique_ptr<Bdd> b2(b1->ptr_project(abs_dom));

			return BddConfset(doms1, *b2);
	}




	BddConfset BddConfset::join(const BddConfset& D, int k, int l, bool abstr)
	{
		Domain doms = get_domains().union_all();
		Domains new_doms;
		for(unsigned int i = 0; i < D.get_domains().size(); ++i)
		{
			if(abstr)
				if(i != (unsigned) l)
				{
					new_doms = new_doms * (D.get_domain(i) + doms.highest() + 1);
					new_doms[i].RealVal = D.get_domain(i).RealVal;
				}
				else
					new_doms  = new_doms * get_domains()[k];
			else
			{
				new_doms = new_doms * (D.get_domain(i) + doms.highest() + 1);
				new_doms[i].RealVal = D.get_domain(i).RealVal;

			}

		}
		unique_ptr<BddConfset> r2(new BddConfset(new_doms,D));
		Domains new_doms2;
		for(unsigned int i=0; i<new_doms.size(); ++i)
			if(abstr)
			{
				if(i != (unsigned) l)
					new_doms2 = new_doms2 * new_doms[i];
			}
			else
				new_doms2 = new_doms2 * new_doms[i];
		if(abstr)
			return BddConfset(get_domains() * new_doms2, get_bdd() & r2->get_bdd());
		else
			return BddConfset(get_domains() * new_doms2, get_bdd() & r2->get_bdd() & Bdd::vars_equal(get_space(), get_domain(k) , new_doms[l]));
	}

	// producer_consumer

		BddConfset BddConfset::ProdCons(const BddConfset& C2, int pq, int cq,
										  Value *(*f)(const Value*))
		{
			unique_ptr<BddConfset> rel2(new BddConfset(C2));

			Domain doms = get_domains().union_all();
			Domains new_doms;
			Domain dm;
			for(unsigned int i = 0; i < C2.get_domains().size(); ++i)
			{
					new_doms = new_doms * (C2.get_domain(i) + doms.highest() + 1);
					new_doms[i].RealVal = C2.get_domain(i).RealVal;
					if (i == (unsigned) cq)
					{
						dm = new_doms[i];
					}
			}

			unique_ptr<BddConfset> r2(new BddConfset(new_doms,C2));

			set<double> RQ1 = get_domain(pq).RealVal.FValues;


			set<double> RQ2 = C2.get_domain(cq).RealVal.FValues;

			ExplCalculator calctmp;

			Confset *Cp = calctmp.newConfset();
			Confset *Cc = calctmp.newConfset();
			FloatConf *c;

			set<double>::const_iterator i=RQ1.begin();
			while(i != RQ1.end())
			{
				c = new FloatConf();
				c->append(*i);
				Cp->add(c);
				++i;
			}

			set<double>::const_iterator j=RQ2.begin();
			while(j != RQ2.end())
			{
				c = new FloatConf();
				c->append(*j);
				Cc->add(c);
				++j;
			}
			Confset *Cpc = calctmp.prodcons(Cp, 0, Cc, 0, f);

			unique_ptr<BddConfset> PQuanty(new BddConfset((Domains) get_domain(pq)*(Domains) dm, Bdd(get_space(),false)));

			Iterator i0(Cpc);

			while (!i0.done())
			{
				set<double>::const_iterator it = RQ1.find(((FloatConf*)(*i0))->getFloat(0));
				unsigned int n1 = 0;
				for(set<double>::const_iterator i2 = RQ1.begin(); it != i2; ++i2)
					++n1;

				set<double>::const_iterator it2 = RQ2.find(((FloatConf*)(*i0))->getFloat(1));
				unsigned int n2 = 0;
				for(set<double>::const_iterator i2 = RQ2.begin(); it2 != i2; ++i2)
					++n2;

				PQuanty->insert(n1,n2);
				i0++;
			}

			return BddConfset(get_domains() * new_doms, (get_bdd() & PQuanty->get_bdd()) & (r2->get_bdd()& PQuanty->get_bdd()));

		}

	// printing
	ostream& operator<<(ostream &out, const BddConfset &r)
	{
		out << r.str();
		return out;
	}


}
