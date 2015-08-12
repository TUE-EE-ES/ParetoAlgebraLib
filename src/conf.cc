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


#include "conf.h"
#include "value.h"

namespace Pareto {
	// ** GenConf class definition **

	GenConf::~GenConf(void)
	{
		for (unsigned i = 0; i < size(); i++)
			delete conf[i];
	}
		
	Conf *GenConf::copy(void) const
	{
		GenConf *c = new GenConf();
		for (unsigned i = 0; i < size(); i++)
			c->append(conf[i]->copy());
		return c;
	}

	vector<Signature> GenConf::signature(void) const
	{
		vector<Signature> s;
		s.push_back(CGEN);
		for (unsigned i = 0; i < size(); i++)
			s.push_back(conf[i]->signature());
		return s;
	}
		
	void GenConf::append(Value *v)
	{
		conf.push_back(v);
	}

	void GenConf::append(Conf *c)
	{
		for (unsigned i = 0; i < c->size(); i++)
			conf.push_back(c->get(i));
	}

	Value *GenConf::get(unsigned k) const
	{
		return conf[k];
	}

	unsigned GenConf::size(void) const
	{
		return (unsigned) conf.size();
	}

	bool GenConf::equals(const Conf *c) const
	{
		for (unsigned i = 0; i < conf.size(); i++) {
			if (!conf[i]->equals(c->get(i)))
				return false;
		}
		return true;
	}

	bool GenConf::equals(const Conf *c, const VisibleList &visible) const
	{
		for (VisibleList::const_iterator i = visible.begin(); i != visible.end(); i++)
			if (!conf[*i]->equals(c->get(*i)))
				return false;
		return true;
	}

	bool GenConf::dominates(const Conf *c) const
	{
		for (unsigned i = 0; i < conf.size(); i++)
			if (!conf[i]->dominates(c->get(i)))
				return false;
		return true;
	}

	bool GenConf::dominates(const Conf *c, const VisibleList &visible) const
	{
		for (VisibleList::const_iterator i = visible.begin(); i != visible.end(); i++)
			if (!conf[*i]->dominates(c->get(*i)))
				return false;
		return true;
	}
				
	void GenConf::abstract(unsigned k)
	{
		tgenconf::iterator pos = conf.begin();
		advance(pos, k);
		delete *pos;
		conf.erase(pos);
	}
		
	void GenConf::abstract(unsigned k, unsigned l)
	{
		tgenconf::iterator pos = conf.begin();
		advance(pos, k);
		for (unsigned i = k; i < l; i++) {
			delete *pos;
			pos = conf.erase(pos);
		}
	}

	string GenConf::str(void) const
	{
		unsigned l = conf.size();
		string s = "(";

		for (unsigned i = 0; i < l; i++) {
			s += conf[i]->str();
			if (i < l - 1)
				s += ", ";
		}
		s += ")";
		
		return s;
	}

	bool GenConf::totalCompare(const Conf *c) const
	{
//		return str() < c->str();
		for (unsigned i = 0; i < conf.size(); i++) {
			if (!conf[i]->totalCompare(c->get(i)))
				return false;
			else if (!c->get(i)->totalCompare(conf[i]))
				return true;
		}
		return true;
	}
	

	// ** FloatConf class definition **

	FloatConf::~FloatConf(void)
	{
	}

	Conf *FloatConf::copy(void) const
	{
		FloatConf *c = new FloatConf();
		for (unsigned i = 0; i < size(); i++)
			c->append(conf[i]);
		return c;
	}

	vector<Signature> FloatConf::signature(void) const
	{
		vector<Signature> s;
		s.push_back(CFLOAT);
		for (unsigned i = 0; i < size(); i++)
			s.push_back(QFLOAT);
		return s;
	}

	void FloatConf::append(Value *v)
	{
		FloatValue *vr = dynamic_cast<FloatValue*>(v);
		if (vr && vr->signature() == QFLOAT) {
			conf.push_back(vr->getFloat());
			delete vr;
		}
		else
			throw Exception("Can only append a FloatValue to a FloatConf.");
	}

	void FloatConf::append(double v)
	{
		conf.push_back(v);
	}

	void FloatConf::append(Conf *c)
	{
		const FloatConf *cr = dynamic_cast<const FloatConf*>(c);
		if (cr && cr->signature()[0] == CFLOAT)
			append(cr);
		else
			throw Exception("Can only append a FloatConf to a FloatConf.");
	}

	void FloatConf::append(const FloatConf *c)
	{
		for (unsigned i = 0; i < c->size(); i++)
			conf.push_back(c->conf[i]);
		delete c;
	}

	/**
	 * \todo Here a new value object is created, while for GenConf this
	 * does not happen: source of potential memory leaks!
	 */
	Value *FloatConf::get(unsigned k) const
	{
		return new FloatValue(conf[k]);
	}

	double FloatConf::getFloat(unsigned k) const
	{
		return conf[k];
	}

	unsigned FloatConf::size(void) const
	{
		return (unsigned) conf.size();
	}

	/**
	 * \note You can only compare a FloatConf to another FloatConf!!
	 * This is not checked!
	 */
	bool FloatConf::equals(const Conf *c) const
	{
		const FloatConf *cr = (FloatConf*) c;
		for (unsigned i = 0; i < conf.size(); i++)
			if (conf[i] != cr->conf[i])
				return false;

		return true;
	}
		
	/**
	 * \note You can only compare a FloatConf to another FloatConf!!
	 * This is not checked!
	 */
	bool FloatConf::equals(const Conf *c, const VisibleList &visible) const
	{
		const FloatConf *cr = (FloatConf*) c;
		
		for (VisibleList::const_iterator i = visible.begin(); i != visible.end(); i++)
			if (conf[*i] != cr->conf[*i])
				return false;
		
		return true;
	}
		
	/**
	 * \note You can only compare a FloatConf to another FloatConf!!
	 * This is not checked!
	 */
	bool FloatConf::dominates(const Conf *c) const
	{
		const FloatConf *cr = (FloatConf*) c;
		for (unsigned i = 0; i < conf.size(); i++)
			if (conf[i] > cr->conf[i])
				return false;
		return true;
	}
		
	/**
	 * \note You can only compare a FloatConf to another FloatConf!!
	 * This is not checked!
	 */
	bool FloatConf::dominates(const Conf *c, const VisibleList &visible) const
	{
		const FloatConf *cr = (FloatConf*) c;
		
		for (VisibleList::const_iterator i = visible.begin(); i != visible.end(); i++)
			if (conf[*i] > cr->conf[*i])
				return false;
			
		return true;
	}

	void FloatConf::abstract(unsigned k)
	{
		tFloatConf::iterator pos = conf.begin();
		advance(pos, k);
		if (pos != conf.end())
			conf.erase(pos);
	}
		
	void FloatConf::abstract(unsigned k, unsigned l)
	{
		tFloatConf::iterator pos = conf.begin();
		advance(pos, k);
		tFloatConf::iterator pos1 = pos;
		advance(pos1, l-k);
		if (pos != conf.end())
			conf.erase(pos, pos1);
	}

	string FloatConf::str(void) const
	{
		char buf[32];
		unsigned l = conf.size();
		string s = "(";

		for (unsigned i = 0; i < l; i++) {
			sprintf(buf, "%.3f", conf[i]);
			s += buf;
			if (i < l - 1)
				s += ", ";
		}
		s += ")";
		
		return s;
	}

	bool FloatConf::totalCompare(const Conf *c) const
	{
		return conf < ((FloatConf*) c)->conf;
	}

} // namespace Pareto
