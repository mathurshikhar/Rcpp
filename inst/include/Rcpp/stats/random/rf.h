// -*- mode: C++; c-indent-level: 4; c-basic-offset: 4; tab-width: 4 -*-
//
// rf.h: Rcpp R/C++ interface class library -- 
//
// Copyright (C) 2010 Douglas Bates, Dirk Eddelbuettel and Romain Francois
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

#ifndef Rcpp__stats__random_rf_h
#define Rcpp__stats__random_rf_h

namespace Rcpp {
namespace stats {


class FGenerator_Finite_Finite : public ::Rcpp::Generator<false,double> {
public:
	
	FGenerator_Finite_Finite( double n1_, double n2_ ) : 
		n1(n1_), n2(n2_), n1__2(n1_ / 2.0 ), n2__2(n2_ / 2.0 ), ratio(n2_/n1_) {}
	
	inline double operator()() const {
		// here we know that both n1 and n2 are finite
		// return ( ::rchisq( n1 ) / n1 ) / ( ::rchisq( n2 ) / n2 );
		return ratio * ::rgamma( n1__2, 2.0 ) / ::rgamma( n2__2, 2.0 ) ;
	}
	
private:
	double n1, n2, n1__2, n2__2, ratio ;
} ;


class FGenerator_NotFinite_Finite : public ::Rcpp::Generator<false,double> {
public:
	
	FGenerator_NotFinite_Finite( double n2_ ) : n2( n2_), n2__2(n2_ / 2.0 ) {}
	
	inline double operator()() const {
		// return n2  / ::rchisq( n2 ) ;
		return n2 / ::rgamma( n2__2, 2.0 ) ;
	}
	
private:
	double n2, n2__2 ;
} ;


class FGenerator_Finite_NotFinite : public ::Rcpp::Generator<false,double> {
public:
	
	FGenerator_Finite_NotFinite( double n1_ ) : n1(n1_), n1__2(n1_ / 2.0 ) {}
	
	inline double operator()() const {
		// return ::rchisq( n1 ) / n1 ;
		return ::rgamma( n1__2, 2.0 ) / n1 ;
	}
	
private:
	double n1, n1__2 ;
} ;

} // stats 

inline NumericVector rf( int n, double n1, double n2 ){
	if (ISNAN(n1) || ISNAN(n2) || n1 <= 0. || n2 <= 0.)
		return NumericVector( n, R_NaN ) ;
	if( R_FINITE( n1 ) && R_FINITE( n2 ) ){
		return NumericVector( n, stats::FGenerator_Finite_Finite( n1, n2 ) ) ;
	} else if( ! R_FINITE( n1 ) && ! R_FINITE( n2 ) ){
		return NumericVector( n, 1.0 ) ;
	} else if( ! R_FINITE( n1 ) ) {
		return NumericVector( n, stats::FGenerator_NotFinite_Finite( n2 ) ) ;
	} else {
		return NumericVector( n, stats::FGenerator_Finite_NotFinite( n1 ) ) ;	
	}
}

} // Rcpp

#endif