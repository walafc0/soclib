/*
 * =============================================================================
 *
 *       Filename:  arbiter.cpp
 *
 *    Description:  Simple round robin arbiter for wishbone interconnect
 *
 *        Version:  1.0
 *        Created:  19.03.2010 16:58:52
 *
 *         Author:  Tarik Graba (), tarik.graba@telecom-paristech.fr
 *        Company:  Telecom Paris TECH
 *
 * =============================================================================
 */

#include <cassert>

#include "arbiter.h"

#ifdef NEXTMASTER
#undef NEXTMASTER
#endif

// shift left until last master
#define NEXTMASTER(X)   do { if ( X & (1<< (m_masters_n-1)) )\
                                 X = 1;\
                             else X = X << 1;} while(0)

namespace soclib { namespace caba {
    namespace _WbI_ { // wishbone interconnect

        Arbiter::Arbiter (size_t nb_m):m_masters_n(nb_m) {
            assert ( (m_masters_n <= 32) && "Arbiter does not support more than 32 msters");
            assert ( (m_masters_n != 0)  && "At least one master has to be declared");
            // To avoid segfaults when reset has not been done yet
            grant = 1;
        }


        // Reset the arbiter
        void Arbiter::reset() {
            // grant the master #0
            grant = 1;
        }

        // Do arbitration
        unsigned int Arbiter::run(unsigned int & reqs) {
            for (size_t i = 0; i< m_masters_n; i++) {

                // if the already granted master still 
                // has a request, then grant him again
                if ( grant & reqs ) return _log2( grant);

                // else try the next master
                NEXTMASTER(grant);
            }
            return _log2(grant);
        }

    } // _WbI_
}}
