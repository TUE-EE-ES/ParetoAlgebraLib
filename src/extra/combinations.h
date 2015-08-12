//  (C) Copyright Howard Hinnant 2005.
//  Use, modification and distribution are subject to the Boost Software License,
//  Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt).
//
//  See http://www.boost.org/libs/type_traits for most recent version including documentation.

#include <iterator>
#include <algorithm>

namespace detail
{

template<class BidirectionalIterator, class Function, class Size>
Function
permute(BidirectionalIterator first, BidirectionalIterator mid,
          BidirectionalIterator active, BidirectionalIterator last,
          Size k, Function f, Size n)
{
    if (k == 0)
        f(first, mid);
    else
    {
        using std::swap;
        BidirectionalIterator activep1 = active;
        ++activep1;
        BidirectionalIterator p = active;
        ++p;
        for (Size i = 0; i < n; ++i, ++p)
        {
            if (k == 1)
                f(first, mid);
            else
                f = permute<BidirectionalIterator,Function,Size>(first, mid, activep1, last, k-1, f , n-1);
            if (i != n-1)
                swap(*active, *p);
        }
        std::rotate(active, activep1, last);
    }
    return f;
}

template<class BidirectionalIterator, class Function, class Size>
Function
combine(BidirectionalIterator first, BidirectionalIterator mid,
           BidirectionalIterator active, BidirectionalIterator last,
           Size k, Function f, Size n)
{
    if (k == 0)
        f(first, mid);
    else
    {
        Size e = n - k + 1;
        Size nn = n-1;
        BidirectionalIterator l2 = last;
        BidirectionalIterator ap1 = active;
        ++ap1;
        BidirectionalIterator apk = active;
        std::advance(apk, k);
        for (Size i = 0; i < e; ++i, --nn, --l2)
        {
            if (k == 1)
                f(first, mid);
            else
                f = combine<BidirectionalIterator,Function,Size>(first, mid, ap1, l2, k-1, f, nn);
            std::rotate(active, i < e-1 ? ap1 : apk, last);
        }
    }
    return f;
}

}  // detail

template<class BidirectionalIterator, class Function, class Size>
Function
for_each_permutation(BidirectionalIterator first, BidirectionalIterator last, Size k, Function f)
{
    typedef typename std::iterator_traits<BidirectionalIterator>::difference_type difference_type;
    Size n = static_cast<Size>(std::distance(first,last));
    if (k < 1 || n == 0)
        return f;
    if (k > n)
        k = n;
    BidirectionalIterator mid = first;
    std::advance(mid, k);
    return detail::permute<BidirectionalIterator,Function,Size>(first, mid, first, last, k, f, n);
}   

template<class BidirectionalIterator, class Function, class Size>
Function
for_each_circular_permutation(BidirectionalIterator first, BidirectionalIterator last, Size k, Function f)
{
    typedef typename std::iterator_traits<BidirectionalIterator>::difference_type difference_type;
    Size n = static_cast<Size>(std::distance(first,last));
    if (k < 1 || n == 0)
        return f;
    if (k > n)
        k = n;
    BidirectionalIterator mid = first;
    std::advance(mid, k);
    BidirectionalIterator f2 = first;
    ++f2;
    for (Size i = 0, e = n - k + 1; i < e; ++i, --n, ++first, ++f2, ++mid)
        f = detail::permute<BidirectionalIterator,Function,Size>(first, mid, f2, last, k-1, f, n-1);
    return f;
}   

template<class BidirectionalIterator, class Function, class Size>
Function
for_each_combination(BidirectionalIterator first, BidirectionalIterator last, Size k, Function f)
{
    typedef typename std::iterator_traits<BidirectionalIterator>::difference_type difference_type;
    Size n = static_cast<Size>(std::distance(first,last));
    if (k < 1 || n == 0)
        return f;
    if (k > n)
        k = n;
    BidirectionalIterator mid = first;
    std::advance(mid, k);
    return detail::combine<BidirectionalIterator,Function,Size>(first, mid, first, last, k, f, n);
}   
