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


#include "reduce.h"
#include "combinations.h"

#define ABS(x) (x < 0 ? -(x) : x)

using namespace Pareto;

/**
 * The distance between two confs
 */

double dist(FloatConf *a, FloatConf *b, vector<double> &N,
			const VisibleList *visible)
{
	double sd = 0;
	
	if (a->size() != N.size()) {
		cout << "ERROR: conf and normalisation vector of different sizes!\n";
		return -1;
	}
	
	for (VisibleList::const_iterator i = visible->begin();
		 i != visible->end(); i++) {
		double di = a->getFloat(*i) - b->getFloat(*i);
		sd += ABS(di) / N[*i];
	}
			
/*		for (unsigned i = d; i < a->size(); i++) {
		double di = a->getFloat(i) - b->getFloat(i);
		sd += ABS(di) / N[i];
	}*/
	
	return sd / a->size();
}

/**
 * Reduction of a Pareto set: helper functions
 */

typedef struct {
	double d;
	FloatConf *c0, *c1;
} distpair;

bool compDistpair (distpair *a, distpair *b)
{
	if (a->d < b->d)
		return true;
	else
		return false;
}

class distPerPair
{
public:
	explicit distPerPair(vector<double> N, const VisibleList *visible)
		: N_(N), visible_(visible) {}
	
	template <class It>
	void operator()(It first, It last)
	{
		distpair *p = new distpair;
		It iter = first;
		p->c0 = *iter++;
		p->c1 = *iter;	
		p->d = dist(p->c0, p->c1, N_, visible_);
		l.push_back(p);
	}
	
	list<distpair*> l;
	
private:
	vector<double> N_;
	const VisibleList *visible_;
};

/**
 * Reduces the confset to k confs by removing confs by the spread/distribution
 * heuristic
 */

Confset *reduce(Confset *C, unsigned k)
{
	ExplConfset *C0 = (ExplConfset*) C;
	int i;
	int confWidth = C->width();
	vector<double> N(confWidth), M(confWidth);
	unsigned n = C->size();
	Calculator &calc = C->calc;
	
	if (k == 0)
		return NULL;
	else if (k >= n || n == 0)
		return calc.newConfset(C);
		
	Iterator iter(C);
	for (i = 0; i < confWidth; i++) {
		FloatConf *c = (FloatConf*) *iter;
		N[i] = M[i] = c->getFloat(i);
	}
	iter++;
	while (!iter.done()) {
		for (i = 0; i < confWidth; i++) {
			FloatConf *c = (FloatConf*) *iter;
			N[i] = max(N[i], c->getFloat(i));
			M[i] = min(M[i], c->getFloat(i));
		}
		iter++;
	}
	for (i = 0; i < confWidth; i++)
		N[i] -= M[i];
	
	// select well-distributed points, if needed
	if (k > 0) {
		// construct list of distance pairs
		list<FloatConf*> confsetL;
		for (Iterator i(C); !i.done(); i++)
			confsetL.push_back((FloatConf *) *i);
		distPerPair x = for_each_combination(confsetL.begin(), confsetL.end(),
											 2, distPerPair(N, &C0->visible));
		
		// sort the list from small to large distances
		x.l.sort(compDistpair);
		
		// remove n-k confs
		list<distpair*>::iterator it, it1;
		while (n > k) {
			// randomly choose a conf from the closest pair
			FloatConf *r = (rand() % 2 ? x.l.front()->c0 : x.l.front()->c1);
			x.l.pop_front();
			
			// remove all entries from the list that contain r
			it = x.l.begin();
			while (it != x.l.end()) {
				if ((*it)->c0 == r || (*it)->c1 == r) {
					it1 = it;
					it++;
					x.l.erase(it1);
				}
				else
					it++;
			}
			
			// remove r
			for (Iterator i(C); !i.done(); i++) {
				if ((*i)->equals(r)) {
					C->remove(*i);
					break;
				}
			}
			
			n--;
		}
	}
	
	return C;
}
