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



#ifndef VALUE_H
#define VALUE_H

#include "paretobase.h"
#include "quantity.h"

namespace Pareto {
	
	/// Smaller-is-better real value
	class FloatValue : public Value {	
	public:
		FloatValue(double val) : val_(val) {}
		virtual Value *copy() const {return new FloatValue(val_);}
		virtual Signature signature(void) {return QFLOAT;}
		virtual bool equals(const Value *v) const {
			return val_ == ((FloatValue*) v)->val_;
		}
		virtual bool dominates(const Value *v) const {
			return val_ <= ((FloatValue*) v)->val_;
		}
		virtual string str(void) const {
			char buf[16];
			sprintf(buf, "%.3f", val_);
			return (string) buf;
		}		
		virtual bool totalCompare(const Value *v) const {
			return val_ <= ((FloatValue*) v)->val_;
		}
		
		double getFloat(void) const {return val_;}
		
	protected:
		FloatValue() {}
		double val_;
	};

	/// Unordered real value
	class UnorderedFloatValue : public FloatValue {	
	public:
		UnorderedFloatValue(double val) {val_ = val;}
		virtual Value *copy() const {return new UnorderedFloatValue(val_);}
		virtual Signature signature(void) {return QUFLOAT;}
		virtual bool equals(const Value *v) const {
			return val_ == ((UnorderedFloatValue*) v)->val_;
		}
		virtual bool dominates(const Value *v) const {
			return val_ == ((UnorderedFloatValue*) v)->val_ ? true : false;
		}
		virtual bool totalCompare(const Value *v) const {
			return val_ <= ((UnorderedFloatValue*) v)->val_;
		}
	};
		
	/// Smaller-is-better integer value
	class IntegerValue : public Value {	
	public:
		IntegerValue(int val) : val_(val) {}
		virtual Value *copy() const {return new IntegerValue(val_);}
		virtual Signature signature(void) {return QINT;}
		virtual bool equals(const Value *val) const {
			return val_ == ((IntegerValue*) val)->val_;
		}
		virtual bool dominates(const Value *val) const {
			return val_ <= ((IntegerValue*) val)->val_;
		}
		virtual string str(void) const
		{
			char buf[16];
			sprintf(buf, "%d", val_);
			return (string) buf;
		}		
		virtual bool totalCompare(const Value *v) const {
			return val_ <= ((IntegerValue*) v)->val_;
		}
		
		int getInt(void) const {return val_;}
		
	protected:
		IntegerValue() {}
		int val_;
	};
		
	/// Unordered integer value
	class UnorderedIntegerValue : public IntegerValue {	
	public:
		UnorderedIntegerValue(int val) {val_ = val;}
		virtual Value *copy() const {return new UnorderedIntegerValue(val_);}
		virtual Signature signature(void) {return QUINT;}
		virtual bool equals(const Value *v) const {
			return val_ == ((UnorderedIntegerValue*) v)->val_;
		}
		virtual bool dominates(const Value *v) const {
			return val_ == ((UnorderedIntegerValue *) v)->val_ ? true : false;
		}		
		virtual bool totalCompare(const Value *v) const {
			return val_ <= ((UnorderedIntegerValue*) v)->val_;
		}
	};

	/// Symbolic value
	/**
	 * \todo Should be removed eventually
	 */
	class SymValue : public Value {	
	public:
		SymValue(quantity qu, double val) : v_(val), q(qu) {}
		virtual Value *copy() const {return new SymValue(q, v_);}
		virtual Signature signature(void) {return QFLOAT;}
		virtual bool equals(const SymValue *val) const {return (v_ == val->v_ );}
		virtual bool dominates(const SymValue *val) const {return v_ <= val->v_;}
		
		virtual string str(void) const
		{
			char buf[16];
			sprintf(buf, "%.3f", v_);
			return (string) buf;
		}
		
		double getReal(void) const {return v_;}
		quantity get_quant(void) const {return q;}

		unsigned int get_index(void) const
		{
			set<double> Fval = q.get_domain().RealVal.FValues;
			set<double>::const_iterator it = Fval.find(v_);
			unsigned int n = 0;
			for(set<double>::const_iterator i = Fval.begin(); it != i; ++i)
				++n;
			return n;
		}
			
		protected:
			double v_;
			quantity q;
	};
	
} // namespace Pareto

#endif // VALUE_H
