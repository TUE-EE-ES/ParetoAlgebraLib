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


#ifndef PARETOBASE_H
#define PARETOBASE_H

#include <iostream>
#include <vector>
#include <list>
#include <string>
#include <exception>
#include <set>

using namespace std;

/*
 * \mainpage The Pareto Calculator Library
 * 
 * \section Compilation
 * \subsection Windows
 * ...
 *
 * \subsection Mac OS X and Linux
 *
 * Compiling the Pareto library and examples for Mac OS X, Linux, and other *nix
 * with a GCC compiler can be done with the included Makefile. The Pareto
 * calculator depends on the CUDD library (for the symbolic implementation),
 * which needs to be installed separately. It can be downloaded from
 * http://vlsi.colorado.edu/~fabio, and compiled as described on the site. After
 * that, the path to the CUDD folder should be inserted into the Makefile
 * (CUDD = ...), and also the path to the Pareto library folder itself needs to
 * be include (ROOT = ...). Then, run `make lib' to compile the core Pareto
 * library files, followed by `make' to compile the three examples in the example
 * folder.
 */

/// Namespace including all objects related to the Pareto library
namespace Pareto {

// forward references
class Value;
class Conf;
class Confset;
class Calculator;
class Iter;

/// The types of quantities and configurations
typedef enum signature_ {
	QUNDEFINED,
	QFLOAT,
	QUFLOAT,
	QINT,
	QUINT,
	CGEN,
	CFLOAT
} Signature;

typedef set<unsigned int> VisibleList;
typedef std::vector<bool> Hidevec;	// obsolete! changed to VisibleList
	
/// Abstract class for a value (with default implementations)
/**
 * Represents a value in a configuration.
 * All functions should be overridden by a derived implementation class.
 */
class Value {
public:
	/// Destructor
	virtual ~Value() {}

	/// Virtual "copy constructor"
	virtual Value *copy(void) const {return new Value(*this);}

	/// The signature representing the quantity type
	virtual Signature signature(void) {return QUNDEFINED;}

	/// Checks whether this value equals the given one
	virtual bool equals(const Value *v) const {return false;}

	/// Checks whether this value dominates the given one
	virtual bool dominates(const Value *v) const {return false;}

	// Return a string representation of the value
	virtual string str(void) const {return (string) "x";}
	
	/// Compare with value \a v in an (artificial) total order
	virtual bool totalCompare(const Value *v) const {return false;}
};

/// Abstract class for a configuration
/**
 * This class represents a configuration as defined in Pareto algebra.
 * All functions should be overridden by a derived implemetation class.
 */
class Conf {
public:
	Conf() {}
	virtual ~Conf() {}

	/// Virtual "copy constructor"
	virtual Conf *copy(void) const = 0;

	/// The signature representing the type of the configuration and its space
	virtual std::vector<Signature> signature(void) const = 0;

	/// Append value \a v
	/**
	 * After appending, the \a v is owned by the configuration. The
	 * original pointer to \a v should no longer be used. If \a v is still
	 * needed, a copy should be made.
	 * \param v value to be added
	 */
	virtual void append(Value *v) = 0;

	/// Concatenate configuration \a c
	/**
	 * After appending, the \a c is owned by the configuration. The
	 * original pointer to \a c should no longer be used. If \a c is still
	 * needed, a copy should be made.
	 * \param c configuration to be added
	 */
	virtual void append(Conf *c) = 0;

	/// Get value of quantity \a k
	/**
	 * Returns a pointer to a Value. The Value remains to be owned by the 
	 * configuration.
	 */
	virtual Value *get(unsigned k) const = 0;

	/// Retrieve the number of quantities
	virtual unsigned size(void) const = 0;

	/// Checks whether this configurations and \a c are equal
	virtual bool equals(const Conf *c) const = 0;
	
	/// Checks whether this configuration and \a c are equal in given quantities
	/** 
	 * \param c configuration to compare
	 * \param visible list of quantities (indices) used in comparison
	 */
	virtual bool equals(const Conf *c, const VisibleList &visible) const = 0;

	/// Checks whether this configuration dominates \a c
	virtual bool dominates(const Conf *c) const = 0;
	
	/// Checks whether this configuration dominates \a c in given quantities
	/** 
	 * \param c configuration to compare
	 * \param visible list of quantities (indices) used in comparison
	 */
	virtual bool dominates(const Conf *c, const VisibleList &visible) const = 0;

	/// Remove quantity \a k
	virtual void abstract(unsigned k) = 0;
	
	/// Remove quantities in range [\a k, \a l)
	virtual void abstract(unsigned k, unsigned l) = 0;

	/// Return a string representation of the configuration
	virtual string str(void) const = 0;
	
	/// Compare with configuration \a c in an (artificial) total order
	/** 
	 * \param c configuration to compare
	 */
	virtual bool totalCompare(const Conf *c) const = 0;
	
public:
	// Operators
	inline const Value *operator[](unsigned k) const {return get(k);}
	inline bool operator==(Conf *c) const {return equals(c);}
	inline bool operator<=(Conf *c) const {return dominates(c);}
	inline bool operator<(Conf *c) const {return dominates(c) && !equals(c);}
	inline void operator+=(Value *v) {append(v);}
	inline void operator+=(Conf *c) {append(c);}
};


/// Abstract class for a set of configurations
/**
 * This class represents a set of configurations as defined in Pareto algebra.
 * A Confset instance should always be obtained from a Calculator object, and
 * never be created on its own. The Calculator ensures a Confset of the right
 * type is created and does memory management.
 * All functions should be overridden by a derived implementation class.
 */
class Confset {
public:
	/// Constructor
	/**
	 * \param c calculator that owns to the new confset
	 */
	Confset(Calculator &c) : calc(c) {}

	/// Destructor
	virtual ~Confset(void) {}

	/// Virtual "copy constructor"
	/**
	 * Creates a copy of the Confset
	 */
	virtual Confset *copy(void) const = 0;

	/// Add configuration \a c
	/**
	 * Adding a configuration to an empty set fixes the configuration space
	 * of the set. Further added configurations need to have the same
	 * signature as the first one. Adding a configuration \a c means that the
	 * Confset now owns \a c. This means that the Confset will dispose of
	 * the configuration when it is disposed of itself. Furthermore, \a c
	 * should not be altered anymore, or added to another Confset. Moreover,
	 * the Confset may alter or remove the configuration, so the original
	 * pointer to \a c is no longer safe and should be handled with care.
	 * If \a c is still needed a copy should be made.
	 */
	virtual void add(Conf *c) = 0;
	
	/// Add configuration \a c to a minimal set and keep it minimal
	/**
	 * The same conditions as for the \a add function apply. Configuration \a c
	 * is either added to the Confset, which then owns \a c, or deleted.
	 * \sa add
	 * \note Does not make sense if the set is not minimal!
	 */
	virtual void addAndMin(Conf *c) = 0;

	/// Remove configuration \a c
	virtual unsigned remove(Conf *c) = 0;

	/// Ignore quantity \a k in dominance comparisons
	virtual void hide(unsigned k) = 0;
	
	/// Ignore quantities in range [\a k, \a l) in dominance comparisons
	virtual void hide(unsigned k, unsigned l) = 0;

	/// Unhide quantity \a k
	virtual void unhide(unsigned k) = 0;
	
	/// Unhide quantities in range [\a k, \a l)
	virtual void unhide(unsigned k, unsigned l) = 0;

	/// Return an iterator that points to the first configuration in the set
	/**
	 * \sa Iter
	 */
	virtual Iter *iter(void) = 0;

	/// Get number of configurations in the set
	virtual unsigned size(void) const = 0;

	/// Get the width (number of quantities) of the configurations in the set
	virtual unsigned width(void) const = 0;
	
	/// Return a string representation of the configuration set
	virtual string str(void) const = 0;
	
	/// Removes all configurations from the set and resets the signature
	virtual void clear(void) = 0;

	/// Return the signature of the configuration set
	virtual inline std::vector<Signature> signature(void) const {return sig;}
	
	/// A reference to the Calculator that owns the confset
	Calculator &calc;

protected:
	/// The signature of the configurations in the confset
	std::vector<Signature> sig;
};


/// Abstract class for a Pareto calculator
/**
 * A Calculator contains all operations of Pareto algebra. Operations are
 * always on Confset objects that are created by and linked to a Calculator
 * object. The Calculator maintains a list of created Confsets and takes care
 * of memory management.
 * All functions should be overridden by a derived implementation class.
 */
class Calculator
{
public:
	/// Destructor
	virtual ~Calculator() {disposeAll();}

	/// Create a confset
	virtual Confset *newConfset(void) = 0;
	
	/// Create a copy of confset \a C
	virtual Confset *newConfset(const Confset *C) = 0;

	/// Dispose confset \a C
	virtual void disposeConfset(Confset *C)
	{
		csList.remove(C);
		delete C;
	}

	/// Trash all configuration sets linked to this calculator
	virtual void disposeAll(void)
	{
		for (list<Confset*>::const_iterator iter = csList.begin();
			 iter != csList.end(); iter++)
			delete *iter;
		csList.clear();
	}
	
	// elementary operations

	/// Minimise configuration set \a C
	/**
	 * This function operator directly on \a C. If the original \a C is still
	 * needed, a copy should be made before applying the operation.
	 */
	virtual Confset *minimise(Confset *C) = 0;

	/// Take the free-product of configuration sets \a C and \a D
	/**
	 * The function creates a new configuration set and retains \a C and \a D.
	 */
	virtual Confset *multiply(const Confset *C, const Confset *D) = 0;

	/// Abstract from quantity \a k in configuration set \a C
	/**
	 * This function operator directly on \a C. If the original \a C is still
	 * needed, a copy should be made before applying the operation.
	 */
	virtual Confset *abstract(Confset *C, unsigned k) = 0;
	
	/// Abstract from quantities in the range [\a k, \a l) in configuration set \a C
	/**
	 * This function operator directly on \a C. If the original \a C is still
	 * needed, a copy should be made before applying the operation.
	 */
	virtual Confset *abstract(Confset *C, unsigned k, unsigned l) = 0;

	/// Apply constraint set \a D to confset \a C
	/**
	 * This function operator directly on \a C. If the original \a C is still
	 * needed, a copy should be made before applying the operation.
	 */
	virtual Confset *constrain(Confset *C, const Confset *D) = 0;
	
	/// Apply constraints to \a C based on predicate function \a check
	/**
	 * This function operator directly on \a C. If the original \a C is still
	 * needed, a copy should be made before applying the operation.
	 */
	virtual Confset *constrain(Confset *C, bool (*check)(Conf*)) = 0;
	
	/// Remove all configuration from \a C that do _not_ dominate \a d
	/**
	 * This function operator directly on \a C. If the original \a C is still
	 * needed, a copy should be made before applying the operation.
	 */
	virtual Confset *constrain(Confset *C, const Conf *d) = 0;

	/// Take the union of configuration sets \a C and \a D
	/**
	 * The function creates a new configuration set and retains \a C and \a D.
	 */
	virtual Confset *unite(const Confset *C, const Confset *D) = 0;

	/// Return a Product generator for all sets in \a Clist
	virtual Iter *prodgen(std::vector<Confset*> Clist) = 0;
	
	/// Return a Product generator for \a C and \a D
	virtual Iter *prodgen(Confset *C, Confset *D) = 0;

	/// Add derived quantity to \a C using mapping function \a f
	/**
	 * This function operator directly on \a C. If the original \a C is still
	 * needed, a copy should be made before applying the operation.
	 */
	virtual Confset *addDerived(Confset *C, Value *(*f)(Conf*, const void*), const void* = NULL) = 0;
	
	/// Add multiple derived quantities to \a C using mapping \a F
	/**
	 * This function operator directly on \a C. If the original \a C is still
	 * needed, a copy should be made before applying the operation.
	 */
	virtual Confset *addDerivedVector(Confset *C, Conf *(*F)(Conf*, const void*), const void* = NULL) = 0;

	// compound operations

	/// Producer-consumer constraint
	/**
	 * The function creates a new configuration set and retains \a C and \a D.
	 */
	virtual Confset *prodcons(const Confset *P, unsigned k,
							  const Confset *C, unsigned l,
							  Value *(*f)(const Value*)) = 0;

	/// Join two configuration sets on a quantity
	/**
	 * The default version leaves both joined quantities in the resulting set,
	 * i.e. it takes the product of \a C and \a D and applies the join constraint.
	 * The function creates a new configuration set and retains \a C and \a D.
	 * \sa joinAndAbstract
	 */
	virtual Confset *join(const Confset *C, unsigned k,
						  const Confset *D, unsigned l,
						  bool abstr=false) = 0;
	
	/// Join two configuration sets and abstract from the second joined quantity
	/**
	 * \sa join
	 */
	virtual inline Confset *joinAndAbstract(const Confset *C, unsigned k,
											const Confset *D, unsigned l)
	{
		return join(C, k, D, l, true);
	}
	
	/// Declares two quantities as identical
	/**
	 * \note Only needed for the join operation of the symbolic calculator:
	 * two quantities that are joined should have been declared identical
	 * before any symbolic operation is done.
	 */
	virtual void identicalQuantities(const Confset *C, unsigned k,
									 const Confset *D, unsigned l) {}
	
protected:
	list<Confset*> csList;
};


/// Abstract iterator base class
/**
 * An iterator is used to walk through the configurations in a Confset. An Iter
 * object can be obtained from a Confset by calling its iter member function.
 * Upon creation, the iterator points at the first configuration in the set. It
 * is advanced by the increment (\a ++) operator. The current configuration is
 * retrieved by the dereference (\a *) operator. When the iterator is advanced
 * past the last configuration in the set, the \a done function will return true. 
 * The order in which the iterator returns the configurations in the set
 * is not specified. An iterator returns pointers to configuration; the
 * configurations remain to be owned by the Confset it belongs to.
 * \note This is an abstract class that needs to be sub-classed for a specific
 * Confset class.
 */
class Iter {
public:
	/// Destructor
	virtual ~Iter() {}
	
	/// Retrieve current configuration
	virtual Conf *operator*(void) = 0;
	
	/// Move to the next configuration
	virtual void operator++(int) = 0;
	
	/// Indication whether the end of the sequence has been reached or not
	virtual bool done(void) = 0;
};


/// Helper class that handles creation and destruction of an iterator
class Iterator
{
public:
	/// Constructor, creates and attaches to Iter object for Confset \a C
	Iterator(Confset *C) : iter(C->iter()) {}
	
	/// Destructor, removes attached Iter object
	virtual ~Iterator() {delete iter;}
	
	/// Retrieve current configuration
	inline Conf *operator*(void) {return **iter;}
	
	/// Move to the next configuration
	inline void operator++(int) {(*iter)++;}
	
	/// Indication whether the end of the sequence has been reached or not
	inline bool done(void) {return iter->done();}

protected:
	Iter *iter;
};


/// Helper class that handles creation and destruction of a product generator
/**
 * The product generator can be used in place of the normal multiply operator.
 * It behaves like an iterator, and can be used in a loop to retrieve the 
 * configurations in the product one-by-one. The advantage is that the
 * configurations in the product set can already be processed without having
 * to first construct the whole product. It can for example be used together
 * with the addAndMin function to efficiently create a minimised product set,
 * without reserving memory for the whole product.
 * The configurations that are returned by the generator are owned by the caller.
 */
class ProdGen
{
public:
	/// Constructor, creates and attaches to product generator for sets in \a Clist
	ProdGen(std::vector<Confset*> Clist) :
		prodgen(Clist[0]->calc.prodgen(Clist)) {}
	
	/// Constructor, creates and attaches to product generator for sets \a C and \a D
	ProdGen(Confset *C, Confset *D) :
		prodgen(C->calc.prodgen(C, D)) {}
	
	/// Destructor, removes attached product generator
	virtual ~ProdGen() {delete prodgen;}
	
	/// Retrieve current configuration
	inline Conf *operator*(void) {return **prodgen;}
	
	/// Move to the next configuration
	inline void operator++(int) {(*prodgen)++;}
	
	/// Indication whether the end of the sequence has been reached or not
	inline bool done(void) {return prodgen->done();}

protected:
	Iter *prodgen;
};


/// Exception class
class Exception : public exception
{
public:
	/// Constructor
	Exception(const char *txt) {strncpy(txt_, txt, 256);}
	
	/// Returns a string description of the exception
	virtual const char* what() const throw() {return txt_;}

protected:
	char txt_[256];
};

// stream operators
inline ostream &operator<<(ostream &os, const Value &v)
{
	return os << v.str();
}

inline ostream &operator<<(ostream &os, const Value *v)
{
	return os << v->str();
}

inline ostream &operator<<(ostream &os, const Conf &c)
{
	return os << c.str();
}

inline ostream &operator<<(ostream &os, const Conf *c)
{
	return os << c->str();
}

inline ostream &operator<<(ostream &os, const Confset &C)
{
	return os << C.str();
}

inline ostream &operator<<(ostream &os, const Confset *C)
{
	return os << C->str();
}

// convenience functions
inline Confset &min(Confset &C)
{
	return *C.calc.minimise(&C);
}

inline Confset &abstr(Confset &C, int k)
{
	return *C.calc.abstract(&C, k);
}

inline Confset &operator*(const Confset &C0, const Confset &C1)
{
	return *C0.calc.multiply(&C0, &C1);
}

inline Confset &operator&(Confset &C0, const Confset &C1)
{
	return *C0.calc.constrain(&C0, &C1);
}

inline Confset &constr(Confset &C, bool (*check)(Conf*))
{
	return *C.calc.constrain(&C, check);
}

inline Confset &operator|(const Confset &C0, const Confset &C1)
{
	return *C0.calc.unite(&C0, &C1);
}

inline Confset &derive(Confset &C, Value *(*f)(Conf*, const void*), const void *x=NULL)
{
	return *C.calc.addDerived(&C, f, x);
}

inline Confset &deriveVec(Confset &C, Conf *(*F)(Conf*, const void*), const void *x=NULL)
{
	return *C.calc.addDerivedVector(&C, F, x);
}

}; // namespace Pareto

#endif /* PARETOBASE_H */
