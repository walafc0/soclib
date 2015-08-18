/* -*- mode: c++; coding: utf-8 -*-
 *
 * SOCLIB_LGPL_HEADER_BEGIN
 * 
 * This file is part of SoCLib, GNU LGPLv2.1.
 * 
 * SoCLib is free software; you can redistribute it and/or modify it
 * under the terms of the GNU Lesser General Public License as published
 * by the Free Software Foundation; version 2.1 of the License.
 * 
 * SoCLib is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public
 * License along with SoCLib; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
 * 02110-1301 USA
 * 
 * SOCLIB_LGPL_HEADER_END
 *
 * Copyright (c) UPMC / Lip6, 20010, Nicolas Pouillon <nipo@ssji.net>
 */
#ifndef TLMDT_CACHE_STATS
#define TLMDT_CACHE_STATS

#include <pthread.h>
#include <systemc>
#include <iostream>

namespace soclib {
namespace tlmdt {

template<typename cache_t>
class CacheStatdumper
{
  static void *stat(void *__this)
  {
    CacheStatdumper *_this = static_cast<CacheStatdumper*>(__this);
    _this->work();
    return NULL;
  }

  void work()
  {
    uint64_t last_time = m_cache->get_time().value();
    while (!m_quit) {
      sleep(10);
      uint64_t new_time = m_cache->get_time().value();
      uint64_t d = new_time-last_time;
      std::cout << std::dec << d/10 << " Hz" << std::endl;
      last_time = new_time;
    }
  }

  bool m_quit;
  cache_t *m_cache;
  pthread_t m_thread;

public:
  CacheStatdumper(cache_t *cache)
    : m_quit(false),
      m_cache(cache)
  {
    pthread_create(&m_thread, NULL,
		   &CacheStatdumper::stat, (void*)this);
  }

  ~CacheStatdumper()
  {
    m_quit = true;
    pthread_join(m_thread, NULL);
  }
};

}}

#endif
