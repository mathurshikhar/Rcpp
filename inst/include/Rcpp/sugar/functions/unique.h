// -*- mode: C++; c-indent-level: 4; c-basic-offset: 4; tab-width: 8 -*-
//
// unique.h: Rcpp R/C++ interface class library -- unique
//
// Copyright (C) 2012   Dirk Eddelbuettel and Romain Francois
//
// This file is part of Rcpp.
//
// Rcpp is free software: you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 2 of the License, or
// (at your option) any later version.
//
// Rcpp is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with Rcpp.  If not, see <http://www.gnu.org/licenses/>.

#ifndef Rcpp__sugar__unique_h
#define Rcpp__sugar__unique_h
          
namespace Rcpp{
namespace sugar{

template <int RTYPE, typename T>
class Unique {
public:
    typedef typename Rcpp::traits::storage_type<RTYPE>::type STORAGE ;
    
    Unique( const T& vec ) : set( vec.begin(), vec.end() ) {}
    
    Vector<RTYPE> get( ) {
        return Vector<RTYPE>( set.begin(), set.end() ) ;
    }
    Vector<RTYPE> get_sorted( ) {
        Vector<RTYPE> out( set.begin(), set.end() ) ;
        std::sort( out.begin(), out.end() ) ;
        return out ;
    }
    
private:
    
    RCPP_UNORDERED_SET<STORAGE> set ;
    
} ;
   
// for a character expression
template <typename T>
class Unique<STRSXP,T> {
public:
    Unique( const T& vec ) : set() {
        std::string buffer ;
        int n = vec.size() ;
        for( int i=0; i<n; i++){
            buffer = vec[i] ;
            set.insert( buffer ) ;
        }
    }
    
    CharacterVector get( ) {
        return CharacterVector( set.begin(), set.end() ) ;
    }
    CharacterVector get_sorted( ) {
        CharacterVector out( set.begin(), set.end() ) ;
        SEXP* p_out = get_string_ptr(out) ;
        std::sort( p_out, p_out + set.size(), StringCompare() );
        return out ;
    }
    
private:
    
    RCPP_UNORDERED_SET<std::string> set ;
   
} ;

// for a character vector
template <>
class Unique<STRSXP,CharacterVector> {
public:
    Unique( const CharacterVector& vec ) : 
        set( get_string_ptr(vec), get_string_ptr(vec) + vec.size() )
    {
    }
    
    CharacterVector get( ) {
        CharacterVector out(set.size()) ;
        std::copy( set.begin(), set.end(), get_string_ptr(out) ) ;
        return out ;
    }
    
    CharacterVector get_sorted( ) {
        int n = set.size() ;
        CharacterVector out(n) ;
        SEXP* p_out = get_string_ptr(out) ;
        std::copy( set.begin(), set.end(), p_out ) ;
        std::sort( p_out, p_out+n, StringCompare() ) ;
        
        return out ;
    }
private:
    
    RCPP_UNORDERED_SET<SEXP> set ;
   
} ;

template <typename SET>
class InSet {
public:
    InSet( const SET& hash_ ) : hash(hash_), end(hash_.end()){}
    
    template <typename T>
    inline int operator()(T value){
        return hash.find(value) != end ;    
    }
    
private:
    const SET& hash ;
    typename SET::const_iterator end ;
} ;

template <int RTYPE, typename TABLE_T>
class In {
public:
    In( const TABLE_T& table) : hash( get_const_begin(table), get_const_end(table) ){}
    
    template <typename T>
    LogicalVector get( const T& x) const {
        int n = x.size() ;
        LogicalVector out = no_init(n) ;
        std::transform( 
            get_const_begin(x), get_const_end(x),
            out.begin(),
            InSet<SET>(hash)
        ) ;
        return out ;
    }
    
private:
    typedef typename RCPP_UNORDERED_SET< typename traits::storage_type<RTYPE>::type > SET ;
    SET hash ;
    
} ;


} // sugar

template <int RTYPE, bool NA, typename T>
inline Vector<RTYPE> unique( const VectorBase<RTYPE,NA,T>& t ){
	return sugar::Unique<RTYPE,T>( t.get_ref() ).get() ;
}
template <int RTYPE, bool NA, typename T>
inline Vector<RTYPE> sort_unique( const VectorBase<RTYPE,NA,T>& t ){
	return sugar::Unique<RTYPE,T>( t.get_ref() ).get_sorted() ;
}

template <int RTYPE, bool NA, typename T, bool RHS_NA, typename RHS_T>
inline LogicalVector in( const VectorBase<RTYPE,NA,T>& x, const VectorBase<RTYPE,RHS_NA,RHS_T>& table ){
    sugar::In<RTYPE,RHS_T> obj(table.get_ref()) ;
    return obj.get( x.get_ref() );
}


} // Rcpp
#endif
