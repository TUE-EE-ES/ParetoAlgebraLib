/*
 * sgi_ext.h: 
 *
 * Copyright (C) 2003 Marcus Nilsson (marcusn@it.uu.se)
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 * 
 * Authors:
 *    Marcus Nilsson (marcusn@it.uu.se)
 */

#ifndef GBDD_SGI_EXT_H
#define GBDD_SGI_EXT_H

// The following are taken from the GCC FAQ. Hash structures are
// supposed to make it to a future standard, until then this is the
// way to use the SGI extentions.

#ifdef __GNUC__
#	if __GNUC__ < 3
#		include <unordered_map.h>
#		include <unordered_set.h>
		// inherit globals
		namespace Sgi {using ::hash_map; using ::unordered_set; using ::hash;};
#		define DECL_NAMESPACE_SGI namespace std
#	else
#		include <ext/unordered_map>
#		include <ext/unordered_set>
#		if __GNUC__ == 3 && __GNUC_MINOR__ == 0	// GCC 3.0
			namespace Sgi = std;
#			define DECL_NAMESPACE_SGI namespace std
#		else									// GCC 3.1 and later
			namespace Sgi = ::__gnu_cxx;
#			define DECL_NAMESPACE_SGI namespace __gnu_cxx
#		endif
#	endif
#else      // ...  there are other compilers, right?
#include <unordered_map>
#include <unordered_set>
	namespace Sgi = stdext;
#	define DECL_NAMESPACE_SGI namespace stdext

#endif

using Sgi::unordered_map;
using Sgi::unordered_set;
//using Sgi::hash;

#endif /* GBDD_SGI_EXT_H */
