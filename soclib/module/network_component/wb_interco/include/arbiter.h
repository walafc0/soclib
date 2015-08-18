/*
 * =============================================================================
 *
 *       Filename:  arbiter.h
 *
 *    Description:  Simple round robin arbiter for wishbone interconnect
 *
 *        Version:  1.0
 *        Created:  19.03.2010 16:03:53
 *
 *         Author:  Tarik Graba (), tarik.graba@telecom-paristech.fr
 *        Company:  Telecom Paris TECH
 *
 * =============================================================================
 */


#ifndef _ARBITER_H_
#define _ARBITER_H_

namespace soclib { namespace caba {
    namespace _WbI_ { // wishbone interconnect

        class Arbiter {
            public:
                Arbiter (size_t nb_m);
                
                // reset the arbiter
                void reset();
                // retrns the granted master
                unsigned int run(unsigned int & reqs);

            private:
                // which master is granted
                // works like a shift register
                size_t grant;

                // number of masters
                const size_t m_masters_n;

        };

        // integer log2
        // from http://graphics.stanford.edu/~seander/bithacks.html#IntegerLog
        inline unsigned int _log2( unsigned int v )
        {
            register unsigned int r; // result of log2(v) will go here
            register unsigned int shift;

            r =     (v > 0xFFFF) << 4; v >>= r;
            shift = (v > 0xFF  ) << 3; v >>= shift; r |= shift;
            shift = (v > 0xF   ) << 2; v >>= shift; r |= shift;
            shift = (v > 0x3   ) << 1; v >>= shift; r |= shift;
            r |= (v >> 1);

            return r;
        }
    }
}}
#endif
