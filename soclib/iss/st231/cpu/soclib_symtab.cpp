/*
 *  Copyright (c) 2008,
 *  INRIA
 *  All rights reserved.
 *
 *  Redistribution and use in source and binary forms, with or without 
 *  modification, are permitted provided that the following conditions are met:
 *
 *   - Redistributions of source code must retain the above copyright notice, 
 *     this list of conditions and the following disclaimer.
 *
 *   - Redistributions in binary form must reproduce the above copyright notice,
 *     this list of conditions and the following disclaimer in the documentation
 *     and/or other materials provided with the distribution.
 *
 *   - Neither the name of INRIA nor the names of its contributors may be used to
 *     endorse or promote products derived from this software without specific 
 *     prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" 
 *  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE 
 *  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 *  ARE DISCLAIMED.
 *  IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY
 *  DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES 
 *  (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR 
 *  SERVICES;LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 *  CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT 
 *  LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 *  OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF 
 *  SUCH DAMAGE.
 *
 * Authors: Sylvain Girbal
 */

#include <soclib_symtab.hh>

namespace st231 {

  /**
   * \brief Unitialized symbol table
   */ 
  SymbolTable::SymbolTable()
  {
  }

  /**
   * \brief Build a new symbol table
   */
  SymbolTable::SymbolTable(const std::string &filename)
  { std::ifstream mapfile(filename.c_str());
    std::string type, name;
    uint32_t addr;
    while(!mapfile.eof())
    { mapfile >> std::hex >> addr >> type >> name;
//      cerr << std::hex << addr << " " << type << " " << name << std::dec << endl;
      table[name] = addr;
    }
    mapfile.close();
  }

   uint32_t SymbolTable::operator[](const std::string &name)
   { return table[name];
   }


  /**
   * \brief Pretty printer
   */
  std::ostream & operator<<(std::ostream &os, const SymbolTable &st)
  { std::map<std::string,uint32_t>::const_iterator it;
    os << std::hex;
    for(it = st.table.begin(); it!=st.table.end(); it++)
    { os << it->first << " " << it->second << std::endl;
    }
    os << std::dec;
    return os;
  }

  std::string mapfile;

} // namespace st231
