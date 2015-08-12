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


#ifndef CONF_H
#define CONF_H

#include "paretobase.h"
#include "value.h"

namespace Pareto {
	
/// Generic Conf
class GenConf : public Conf {	
public:
	GenConf() : Conf() {}
	virtual ~GenConf(void);
	virtual Conf *copy(void) const;
	virtual vector<Signature> signature(void) const;
	virtual void append(Value*);
	virtual void append(Conf*);
	virtual Value *get(unsigned) const;
	virtual unsigned size(void) const;
	virtual bool equals(const Conf*) const;
	virtual bool equals(const Conf*, const VisibleList&) const;
	virtual bool dominates(const Conf*) const;
	virtual bool dominates(const Conf*, const VisibleList&) const;
	virtual void abstract(unsigned k);
	virtual void abstract(unsigned k, unsigned l);
	virtual string str(void) const;
	virtual bool totalCompare(const Conf *c) const;
	
protected:
	typedef vector<Value*> tgenconf;
	tgenconf conf;
};


/// Conf of only real numbers
/**
 * This class provides an efficient implementation for configurations that
 * consist of only real values (i.e. FloatValue objects). Using this class
 * will greatly speed up the execution of operations. However, it should not be
 * mixed with other Conf sub-classes in configuration sets and operations.
 */
class FloatConf : public Conf {	
public:
	FloatConf() : Conf() {}
	virtual ~FloatConf(void);
	virtual Conf *copy(void) const;
	virtual vector<Signature> signature(void) const;
	virtual void append(Value*);
	virtual void append(double);
	virtual void append(Conf*);
	virtual void append(const FloatConf*);
	virtual Value *get(unsigned) const;
	virtual double getFloat(unsigned) const;
	virtual unsigned size(void) const;
	virtual bool equals(const Conf*) const;
	virtual bool equals(const Conf*, const VisibleList&) const;
	virtual bool dominates(const Conf*) const;
	virtual bool dominates(const Conf*, const VisibleList&) const;
	virtual void abstract(unsigned k);
	virtual void abstract(unsigned k, unsigned l);
	virtual string str(void) const;
	virtual bool totalCompare(const Conf *c) const;
	
protected:
	typedef vector<double> tFloatConf;
	tFloatConf conf;
};

} // namespace Pareto

#endif // CONF_H
