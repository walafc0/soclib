/*
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
 * Copyright (c) UPMC, Lip6
 *         Nicolas Pouillon <nipo@ssji.net>, 2009
 * Alexandre Becoulet <alexandre.becoulet@free.fr>, 2010-2013
 *
 * Maintainers: nipo becoulet
 */

#include <cstring>

#include <cassert>

#include <stdint.h>
#include <signal.h>

#include "iss_memchecker_registers.h"
#include "iss_memchecker.h"
#include "exception.h"

#include "soclib_endian.h"
#include "loader.h"

#define MEMCHK_COLOR_WARN(str) "\x1b[93;1m" << str << "\x1b[m"
#define MEMCHK_COLOR_ERR(str) "\x1b[91;1m" << str << "\x1b[m"
#define MEMCHK_COLOR_INFOC(str) "\x1b[96;1m" << str << "\x1b[m"
#define MEMCHK_COLOR_INFOS(str) "\x1b[94;1m" << str << "\x1b[m"
#define MEMCHK_COLOR_INFOR(str) "\x1b[92;1m" << str << "\x1b[m"
#define MEMCHK_COLOR_INFOL(str) "\x1b[95;1m" << str << "\x1b[m"
#define MEMCHK_BOLD(str) "\x1b[1m" << str << "\x1b[m"

namespace soclib { namespace common {

namespace __iss_memchecker {

typedef uint32_t error_level_t;

namespace {
MemoryState * s_memory_state = NULL;
}

enum {
    ERROR_NONE = 0,
    ERROR_UNINITIALIZED_WORD            = 0x00000001,
    ERROR_INVALID_REGION                = 0x00000002,
    ERROR_SP_OUTOFBOUNDS                = 0x00000004,
    ERROR_DATA_ACCESS_BELOW_SP          = 0x00000010,
    ERROR_CREATING_STACK_NOT_ALLOC      = 0x00000020,
    ERROR_BAD_REGION_REALLOCATION       = 0x00000040,
    ERROR_CONTEXT_ON_TWO_CPUS           = 0x00000080,
    ERROR_BAD_CONTEXT_DEL               = 0x00000100,
    ERROR_BAD_CONTEXT_CREATE            = 0x00000200,
    ERROR_BAD_CONTEXT_INVALIDATE        = 0x00000400,
    ERROR_BAD_CONTEXT_SWITCH            = 0x00000800,
    ERROR_REGION_OVERLAP                = 0x00001000,
    ERROR_IRQ_ENABLED_MAGIC             = 0x00002000,
    ERROR_IRQ_ENABLED_TMP               = 0x00004000,
    ERROR_IRQ_ENABLED_LOCK              = 0x00008000,
    ERROR_LOCK_DEAD_LOCK                = 0x00010000,
    ERROR_NULL_POINTER_ACCESS           = 0x00020000,
    ERROR_BAD_MAGIC_OP                  = 0x00040000,
    ERROR_IRQ_DISABLED_USER             = 0x00080000,
    ERROR_INVALID_ACCESS                = 0x00100000,
};

// errors which use a repeat mask to avoid flooding output
static const uint32_t repeat_filter = ( ERROR_IRQ_ENABLED_LOCK |
                                        ERROR_SP_OUTOFBOUNDS |
                                        ERROR_IRQ_ENABLED_TMP |
                                        ERROR_IRQ_DISABLED_USER |
                                        ERROR_LOCK_DEAD_LOCK);

class ContextState
{
    const uint32_t m_id;
    uint32_t m_running_on;
    static const uint32_t s_not_running = (uint32_t)-1;
    uint32_t m_refcount;
    bool m_valid;
    bool m_tmp;

public:
    const uint64_t m_stack_lower;
    const uint64_t m_stack_upper;
    uint32_t m_last_sp;

    ContextState( uint32_t id, uint32_t stack_low, uint32_t stack_up, bool tmp = false )
        : m_id(id),
          m_running_on(s_not_running),
          m_refcount(0),
          m_valid(true),
          m_tmp(tmp),
          m_stack_lower(stack_low),
          m_stack_upper(stack_up ? stack_up : ((uint64_t)1<<32)),
          m_last_sp(0)
    {
        assert(m_stack_lower <= m_stack_upper && "Stack upside down");
#if defined(SOCLIB_MODULE_DEBUG)
        std::cout << "Creating new context " << *this << std::endl;
#endif
    }

    ~ContextState()
    {
#if defined(SOCLIB_MODULE_DEBUG)
        std::cout << "Deleting context " << *this << std::endl;
#endif
    }

    inline bool valid() const
    {
        return m_valid;
    }

    inline bool temporary() const
    {
        return m_tmp;
    }

    inline void invalidate()
    {
        m_valid = false;
    }

    inline bool stack_contains( uint64_t sp ) const
    {
        return (m_stack_lower < sp && sp <= m_stack_upper);
    }

    inline bool is( uint32_t id ) const
    {
        return m_id == id;
    }

    inline uint32_t id() const
    {
        return m_id;
    }

    inline error_level_t schedule( uint32_t cpu )
    {
        error_level_t r = 0;
        if ( m_running_on != s_not_running )
            r |= ERROR_CONTEXT_ON_TWO_CPUS;
        m_running_on = cpu;
        return r;
    }

    inline void unschedule()
    {
        m_running_on = s_not_running;
    }

    bool overlaps( ContextState &other ) const
    {
        return overlaps(
            other.m_stack_lower,
            other.m_stack_upper );
    }

    bool overlaps( uint64_t base, uint64_t end ) const
    {
        if ( end <= m_stack_lower )
            return false;
        if ( m_stack_upper <= base )
            return false;
        return true;
    }

    void print( std::ostream &o ) const
    {
        o << "Context #" << std::hex << std::showbase << m_id
          << ", SP=" << m_last_sp << " in " << m_stack_lower << "-" << m_stack_upper << " stack range";
    }

    friend std::ostream &operator << (std::ostream &o, const ContextState &cs)
    {
        cs.print(o);
        return o;
    }

    void ref()
    {
        ++m_refcount;
    }
    void unref()
    {
        if ( --m_refcount == 0 ) {
#if defined(SOCLIB_MODULE_DEBUG)
            std::cout << *this << " has not more refs, del" << std::endl;
#endif
            delete this;
        }
    }
};

class RegionInfo
{
public:
    enum State {
        REGION_INVALID = 1,
        REGION_STATE_GLOBAL = 2,
        REGION_STATE_GLOBAL_READ_ONLY = 4,
        REGION_STATE_ALLOCATED = 8,
        REGION_STATE_FREE = 16,
        REGION_STATE_STACK = 32,
        REGION_STATE_WAS_STACK = 64,
        REGION_STATE_PERIPHERAL = 128,
        REGION_STATE_RAW = 256,
    };

private:
    enum State m_state;
    uint32_t m_at;
    uint32_t m_refcount;
    uint64_t m_base_addr;
    uint64_t m_end_addr;
    RegionInfo *m_previous_state;

public:
    RegionInfo *get_updated_region( enum State state, uint32_t at, uint64_t base_addr, uint64_t end_addr )
    {
        RegionInfo *n = new RegionInfo( state, at, base_addr, end_addr, this );
        return n;
    }

    RegionInfo( enum State state, uint32_t at, uint64_t base_addr, uint64_t end_addr, RegionInfo *previous_state = 0 )
        : m_state(state),
          m_at(at),
          m_refcount(0),
          m_base_addr(base_addr),
          m_end_addr(end_addr),
          m_previous_state(previous_state)
    {
        if ( m_previous_state )
            m_previous_state->ref();
#if defined(SOCLIB_MODULE_DEBUG)
        std::cout << "Creating a new region info " << *this
                  << std::endl;
#endif
    }

    ~RegionInfo()
    {
        if ( m_previous_state )
            m_previous_state->unref();
    }

    bool new_state_valid( enum State new_state )
    {
        int valid_new_states = new_state;
        switch(m_state) {
        case REGION_INVALID:
            break;
        case REGION_STATE_GLOBAL:
            valid_new_states |=
                REGION_STATE_FREE |
                REGION_STATE_STACK;
            break;
        case REGION_STATE_GLOBAL_READ_ONLY:
            break;
        case REGION_STATE_ALLOCATED:
            valid_new_states |=
                REGION_STATE_FREE |
                REGION_STATE_STACK;
            break;
        case REGION_STATE_FREE:
            valid_new_states |=
                REGION_STATE_FREE |
                REGION_STATE_ALLOCATED;
            break;
        case REGION_STATE_PERIPHERAL:
            break;
        case REGION_STATE_STACK:
            valid_new_states |=
                REGION_STATE_WAS_STACK;
            break;
        case REGION_STATE_WAS_STACK:
            valid_new_states |=
                REGION_STATE_FREE |
                REGION_STATE_STACK;
            break;
        case REGION_STATE_RAW:
            valid_new_states |=
                REGION_STATE_PERIPHERAL |
                REGION_STATE_GLOBAL |
                REGION_STATE_FREE;
            break;
        }
        return valid_new_states & new_state;
    }

    bool contains( uint64_t addr ) const
    {
        return m_base_addr <= addr && addr < m_end_addr;
    }

    State state() const
    {
        return m_state;
    }

    void ref()
    {
        ++m_refcount;
    }
    void unref()
    {
        if ( --m_refcount == 0 )
            delete this;
    }

    error_level_t do_write() const
    {
        if ( m_state & ( REGION_STATE_GLOBAL_READ_ONLY 
                         | REGION_STATE_WAS_STACK ))
            return ERROR_INVALID_REGION;
        return do_read();
    }

    error_level_t do_read() const
    {
        if ( m_state & 
             ( REGION_STATE_FREE
               | REGION_INVALID
               | REGION_STATE_WAS_STACK )
            )
            return ERROR_INVALID_REGION;
        return ERROR_NONE;
    }

    static const char *state_str(State state)
    {
        switch (state) {
        case REGION_INVALID: return "invalid";
        case REGION_STATE_GLOBAL: return "global";
        case REGION_STATE_GLOBAL_READ_ONLY: return "global read only";
        case REGION_STATE_ALLOCATED: return "allocated";
        case REGION_STATE_FREE: return "free";
        case REGION_STATE_PERIPHERAL: return "devices";
        case REGION_STATE_STACK: return "stack";
        case REGION_STATE_WAS_STACK: return "former stack";
        case REGION_STATE_RAW: return "raw";
        default: return "unknown";
        }
    }

    const char *state_str() const
    {
        return state_str(m_state);
    }

    void print( std::ostream &o ) const
    {
        o << "Region " << std::hex
            //          << "at "  << m_at << " "
          << m_base_addr << "-"  << m_end_addr << " : " << state_str();
        o << " memory";
    }

    RegionInfo *prev_state() const
    {
        return m_previous_state;
    }

    friend std::ostream &operator << (std::ostream &o, const RegionInfo &ri)
    {
        ri.print(o);
        return o;
    }
};

class AddressInfo
{
    RegionInfo *m_info;
    static const uintptr_t s_initialized_bit = 1;
    static const uintptr_t s_invalid_bit = 2;
    static const uintptr_t s_islock_bit = 4;
    static const uintptr_t s_addr_mask = ~(uintptr_t)7;

    AddressInfo & operator=( const AddressInfo &ref );

public:

    bool is_initialized() const
    {
        return (uintptr_t)m_info & s_initialized_bit;
    }

    bool is_invalid() const
    {
        return (uintptr_t)m_info & s_invalid_bit;
    }

    bool is_spinlock() const
    {
        return (uintptr_t)m_info & s_islock_bit;
    }

    void set_initialized( bool initialized )
    {
        if ( initialized )
            m_info = (RegionInfo*)((uintptr_t)m_info | s_initialized_bit);
        else
            m_info = (RegionInfo*)((uintptr_t)m_info & ~s_initialized_bit);
    }

    void set_invalid( bool invalid )
    {
        if ( invalid )
            m_info = (RegionInfo*)((uintptr_t)m_info | s_invalid_bit);
        else
            m_info = (RegionInfo*)((uintptr_t)m_info & ~s_invalid_bit);
    }

    void set_spinlock( bool islock )
    {
        if ( islock )
            m_info = (RegionInfo*)((uintptr_t)m_info | s_islock_bit);
        else
            m_info = (RegionInfo*)((uintptr_t)m_info & ~s_islock_bit);
    }

    RegionInfo *region() const
    {
        return (RegionInfo*)((uintptr_t)m_info & s_addr_mask);
    }

    error_level_t region_set( RegionInfo *ptr )
    {
        bool r = false;
        assert(ptr && ((uintptr_t)ptr & ~s_addr_mask) == 0);
        ptr->ref();

        if ( region() ) {
            if ( region()->new_state_valid(ptr->state()) )
                region()->unref();
            else
                r = true;
        }
        m_info = (RegionInfo*)(
            ((uintptr_t)ptr & s_addr_mask) |
            ((uintptr_t)m_info & ~s_addr_mask));
        return r;
    }

    AddressInfo( RegionInfo *ri, bool initialized = false, bool invalid = false )
        : m_info(0)
    {
        region_set(ri);
        set_initialized(initialized);
        set_invalid(invalid);
    }

    AddressInfo()
        : m_info(0)
    {}
    
    ~AddressInfo()
    {
        if ( region() )
            region()->unref();
    }

    AddressInfo( const AddressInfo &ref )
        : m_info(0)
    {
        if ( ref.region() )
            region_set(ref.region());
    }

    error_level_t do_write()
    {
        set_initialized(true);
        if ( is_invalid() )
            return ERROR_INVALID_ACCESS;
        return ERROR_NONE;
    }

    error_level_t do_read()
    {
        if ( is_invalid() )
            return ERROR_INVALID_ACCESS;
        if (! is_initialized() &&
            ! (m_info->state() & RegionInfo::REGION_STATE_PERIPHERAL) )
            return ERROR_UNINITIALIZED_WORD;
//        set_initialized(true);
        return ERROR_NONE;
    }

    void print( std::ostream &o ) const
    {
        o << "<AddressInfo " << std::dec
          << (is_initialized() ? " initialized" : " uninitialized");
        if ( region() )
            o << ", " << *region();
        else
            o << ", no region";
        o << ">";
    }

    friend std::ostream &operator << (std::ostream &o, const AddressInfo &ai)
    {
        ai.print(o);
        return o;
    }
};

class MemoryState
{
    Loader m_binary;

    typedef std::map<uint64_t, ContextState *> context_map_t;
    typedef std::map<uint64_t, std::vector<AddressInfo> *> region_map_t;
    context_map_t m_contexts;
    region_map_t m_regions;
    AddressInfo m_default_address;
    uintptr_t m_comm_address;

public:
    ContextState * const unknown_context;

    MemoryState( const soclib::common::MappingTable &mt,
                 const soclib::common::Loader &loader,
                 const std::string &exclusions )
        : m_binary(loader),
          m_contexts(),
          m_regions(),
          m_default_address(new RegionInfo(RegionInfo::REGION_INVALID, 0, 0, 0), true),
          m_comm_address(0x400),
          unknown_context(new ContextState(ISS_MEMCHECKER_ID_UNKNOWN, 0, 0 )) //(uint32_t)-1 ))
    {
        unknown_context->ref();

        const std::list<Segment> &segments = mt.getAllSegmentList();
        Loader::section_list_t sections = loader.sections();

        std::string exclusion_list = ",";
        exclusion_list += exclusions + ",";

        for ( std::list<Segment>::const_iterator i = segments.begin();
              i != segments.end();
              ++i ) {

            RegionInfo *ri = new RegionInfo(
                RegionInfo::REGION_STATE_RAW, 0,
                i->baseAddress(), (uint64_t)i->baseAddress() + i->size() );
            std::vector<AddressInfo> *rm = new std::vector<AddressInfo>( i->size() / 4 );

            for ( size_t j=0; j<i->size()/4; ++j )
                (*rm)[j].region_set(ri);

            m_regions[i->baseAddress()] = rm;
        }

        for ( Loader::section_list_t::const_iterator i = sections.begin();
              i != sections.end();
              ++i ) {

#if defined( SOCLIB_MODULE_DEBUG )
              std::cout << "Creating a region info for"
                        << " " << i->name()
                        << " @" << std::hex << i->lma()
                        << ", " << std::dec << i->size() << " bytes long"
                        << " flags: " << (i->flag_read_only() ? "RO" : "")
                        << std::endl;
#endif

            RegionInfo::State state = RegionInfo::REGION_STATE_GLOBAL;
            if ( i->flag_read_only() )
                state = RegionInfo::REGION_STATE_GLOBAL_READ_ONLY;

            region_new_state( state, 0, i->lma(), i->size() );
//            if ( i->has_data() )
            for ( size_t j=0; j<i->size(); j+=4 )
                info_for_address( (uint64_t)i->lma()+j )->do_write();
        }

        for ( std::list<Segment>::const_iterator i = segments.begin();
              i != segments.end();
              ++i ) {
            std::string vname = std::string(",")+i->name()+",";

            if ( exclusion_list.find(vname) == std::string::npos )
                continue;

            region_update_state( RegionInfo::REGION_STATE_PERIPHERAL, 0, i->baseAddress(), i->size() );
        }

        const BinaryFileSymbol *sym = loader.get_symbol_by_name( "soclib_iss_memchecker_addr" );
        if ( sym ) {
            m_comm_address = sym->address();
            std::cout << "Binary file defined IssMemchecker communication address to "
                      << m_comm_address << std::endl;
        }
    }

    uintptr_t comm_address() const
    {
        return m_comm_address;
    }

    ContextState *context_get( uint32_t id ) const
    {
        context_map_t::const_iterator i = m_contexts.find(id);
        if ( i != m_contexts.end() )
            return i->second;
        else
            return unknown_context;
    }

    bool context_create( uint32_t id, ContextState *context )
    {
        context_map_t::const_iterator i = m_contexts.find(id);

        if ( i != m_contexts.end() ) {
            return false;
        }

        for ( context_map_t::const_iterator i = m_contexts.begin();
              i != m_contexts.end();
              ++i ) {
#if defined(SOCLIB_MODULE_DEBUG)
            std::cout << "Checking " << *context << " and " << *i->second << std::endl;
#endif
            if ( context->overlaps( *i->second ) ) {
                std::cout << "Context " << *context << " overlaps " << *i->second << std::endl;
                abort();
            }
        }
        m_contexts[id] = context;
        context->ref();
        return true;
    }

    bool context_delete( uint32_t id )
    {
        if ( id == ISS_MEMCHECKER_ID_UNKNOWN ) {
            return false;
        }

        context_map_t::iterator i = m_contexts.find(id);

        if (i == m_contexts.end()) {
            return false;
        }

        i->second->unref();
        m_contexts.erase(i);
        return true;
    }

    bool context_invalidate( uint32_t id )
    {
        context_map_t::iterator i = m_contexts.find(id);

        if (i == m_contexts.end()) {
            return false;
        }

        for ( uint64_t addr = i->second->m_stack_lower; addr < i->second->m_stack_upper; addr+= 4 ) {
            AddressInfo *ai = info_for_address(addr);
            ai->set_initialized(false);
        }

        i->second->invalidate();

        return true;
    }

    AddressInfo *info_for_address(uint64_t address)
    {
        region_map_t::iterator i = m_regions.upper_bound(address);

#if defined(SOCLIB_MODULE_DEBUG)
        std::cout
            << "info_for_address(" << std::hex << address << "): " << i->first << " is_end: " << (i == m_regions.end()) << std::endl;
#endif

        if ( ! ( i == m_regions.end() && (--i)->first <= address ) )
            ++i;

        while ( i != m_regions.begin()
                && i->first > address )
            --i;

        if ( i == m_regions.end() ) {
#if defined(SOCLIB_MODULE_DEBUG)
             std::cout
                 << "Address " << std::hex << address << " in no region." << std::endl
                 << "Regions: " << std::endl;
             for ( region_map_t::iterator i = m_regions.begin();
                   i != m_regions.end();
                   ++i )
                 std::cout << " " << i->first << " size: " << i->second->size() << " words" << std::endl;
#endif
            
            //abort();
            return &m_default_address;
        }
        uint64_t region_base = i->first;
        uint64_t word_no = (address-region_base)/4;
        std::vector<AddressInfo> &r = *(i->second);
        if ( region_base <= address && word_no < r.size() )
            return &r[word_no];
#if defined(SOCLIB_MODULE_DEBUG)
         std::cout << "Warning: address " << std::hex << address
                   << " " << std::dec << (r.size()-word_no) << " words beyond "
                   << r[r.size()-1] << std::endl;
#endif

        return &m_default_address;
    }

    error_level_t region_update_state( RegionInfo::State new_state, uint32_t at, uint64_t addr,
                                       uint64_t size, RegionInfo ** lrt = 0, uint32_t *lat = 0)
    {
        error_level_t r = 0;
        RegionInfo *lri = info_for_address(addr)->region();
        RegionInfo *nri = lri->get_updated_region( new_state, at, addr, addr+size );

#if defined(SOCLIB_MODULE_DEBUG)
        std::cout << "Updating " << *lri << " to " << *nri << std::endl;
#endif

        for ( context_map_t::const_iterator i = m_contexts.begin();
              i != m_contexts.end();
              ++i ) {
            if ( i->second->overlaps( addr, addr+size ) ) {
                std::cout
                    << "Region " << *nri
                    << " overlaps " << *(i->second)
                    << std::endl;

                r = ERROR_REGION_OVERLAP;
            }
        }

        for ( uint64_t a = addr; a < addr+size; a+=4 ) {
            AddressInfo *ai = info_for_address(a);
            RegionInfo *ri = ai->region();
            if ( ai->region_set(nri) && !r ) {
                r = ERROR_BAD_REGION_REALLOCATION;
                if ( lat )
                    *lat = a;
                if ( lrt )
                    *lrt = ri;
            }
        }
        return r;
    }

    void region_new_state( RegionInfo::State new_state, uint32_t at, uint64_t addr, uint64_t size )
    {
        RegionInfo *nri = new RegionInfo( new_state, at, addr, addr+size );
        for ( uint64_t a = addr; a < addr+size; a+=4 )
            info_for_address(a)->region_set(nri);
    }
 
    BinaryFileSymbolOffset get_symbol( uintptr_t addr ) const
    {
        return m_binary.get_symbol_by_addr(addr);
    }
};

} // namespace __iss_memchecker

using namespace __iss_memchecker;


template<typename iss_t>
uint32_t IssMemchecker<iss_t>::get_cpu_sp() const
{
    return iss_t::debugGetRegisterValue(m_sp_reg_id);
}

template<typename iss_t>
uint32_t IssMemchecker<iss_t>::get_cpu_pc() const
{
    return iss_t::debugGetRegisterValue(iss_t::s_pc_register_no);
}


template<typename iss_t>
void IssMemchecker<iss_t>::init( const soclib::common::MappingTable &mt,
                                 const soclib::common::Loader &loader,
                                 const std::string &exclusions )
{
    if ( s_memory_state == NULL )
        s_memory_state = new MemoryState( mt, loader, exclusions );
}

template<typename iss_t>
IssMemchecker<iss_t>::IssMemchecker(const std::string &name, uint32_t ident)
    : iss_t(name, ident),
      m_last_region_touched(0),
      m_has_data_answer(false),
      m_cpuid(ident),
      m_enabled_checks(0),
      m_r1(0),
      m_r2(0),
      m_opt_dump_iss(false),
      m_opt_dump_access(false),
      m_opt_show_enable(false),
      m_opt_show_ctx(false),
      m_opt_show_ctxsw(false),
      m_opt_show_region(false),
      m_opt_show_lockops(false),
      m_opt_exit_on_error(false),
      m_trap_mask(0),
      m_report_mask(-1),
      m_no_repeat_mask(0),
      m_magic_state(MAGIC_NONE)
{
    struct iss_t::DataRequest init = ISS_DREQ_INITIALIZER;
    m_last_data_access = init;

    if ( !s_memory_state ) {
        std::cerr
            << std::endl
            << "You must call the static initialized with:" << std::endl
            << "soclib::common::IssMemchecker<...>::init( mapping_table, loader );" << std::endl
            << "Prior to any IssMemchecker constructor." << std::endl
            << std::endl;
        abort();
    }

    m_comm_address = s_memory_state->comm_address();
    m_sp_reg_id = iss_t::s_sp_register_no;

    m_current_context = s_memory_state->unknown_context;
    m_last_context = s_memory_state->unknown_context;
    m_current_context->ref();
    m_last_context->ref();

    if ( const char *env = getenv( "SOCLIB_MEMCHK" ) ) {
        m_opt_dump_iss = strchr( env, 'I' );
        m_opt_dump_access = strchr( env, 'A' );
        m_opt_show_enable = strchr( env, 'E' );
        m_opt_show_ctx = strchr( env, 'C' );
        m_opt_show_ctxsw = strchr( env, 'S' );
        m_opt_show_region = strchr( env, 'R' );
        m_trap_mask = strchr( env, 'T' ) ? -1 : 0;
        m_opt_show_lockops = strchr( env, 'L' );
        m_opt_exit_on_error = strchr( env, 'X' );
    }

        if ( ident == 0 )
            std::cerr << "[MemChecker] SOCLIB_MEMCHK env variable may contain the following flag letters: " << std::endl
                      << "  R (show region changes),     C (show context ops),  S (show context switch), " << std::endl
                      << "  T (raise gdb except on err), I (show iss dump),     A (show access details), " << std::endl
                      << "  L (show locks accesses),     E (show checks enable) X (exit simulation on err)" << std::endl
                      << "  => See http://www.soclib.fr/trac/dev/wiki/Tools/MemoryChecker" << std::endl;

    if ( const char *env = getenv( "SOCLIB_MEMCHK_TRAPON" ) ) {
        m_trap_mask = strtoul(env, NULL, 0);
    }

    if ( const char *env = getenv( "SOCLIB_MEMCHK_REPORT" ) ) {
        m_report_mask = strtoul(env, NULL, 0);
    }
}

template<typename iss_t>
uint32_t IssMemchecker<iss_t>::register_get(uint32_t reg_no) const
{
    assert( reg_no < ISS_MEMCHECKER_REGISTER_MAX && "Undefined regsiter" );

    switch ((enum SoclibIssMemcheckerRegisters)reg_no) {
    case ISS_MEMCHECKER_CONTEXT_SWITCH:
        return m_current_context->id();
    case ISS_MEMCHECKER_R1:
        return m_current_context->m_stack_lower;
    case ISS_MEMCHECKER_R2:
        return m_current_context->m_stack_upper;
    default:
        assert(!"This register is write only");
        return 0;
    }
}

template<typename iss_t>
void IssMemchecker<iss_t>::register_set(uint32_t reg_no, uint32_t value)
{
    assert( reg_no < ISS_MEMCHECKER_REGISTER_MAX && "Undefined regsiter" );

#if defined(SOCLIB_MODULE_DEBUG)
    std::cout
        << "memchecker register set " << std::dec << reg_no
        << " val: " << std::hex << value
        << std::endl;
#endif

    enum SoclibIssMemcheckerRegisters reg_id = (enum SoclibIssMemcheckerRegisters)reg_no;

#if 1     // Irq get enabled before last write occurs, need a write barrier in software
    if ( (m_enabled_checks & ISS_MEMCHECKER_CHECK_IRQ) &&
         iss_t::debugGetRegisterValue(iss_t::ISS_DEBUG_REG_IS_INTERRUPTIBLE) ) {
        report_error( ERROR_IRQ_ENABLED_MAGIC );
    }
#endif

    if (reg_id != ISS_MEMCHECKER_MAGIC && m_magic_state == MAGIC_NONE)
        return;

    switch (reg_id) {
    case ISS_MEMCHECKER_MAGIC:
        switch (m_magic_state) {
        case MAGIC_NONE:
            switch (value) {
            case ISS_MEMCHECKER_MAGIC_VAL:
                m_magic_state = MAGIC_LE;
                break;
            case ISS_MEMCHECKER_MAGIC_VAL_SWAPPED:
                m_magic_state = MAGIC_BE;
                break;
            default:
                report_error(ERROR_BAD_MAGIC_OP, value);
                return;
            }
            break;
        case MAGIC_DELAYED:
            // Cant access memchecker when in delayed magic
            report_error(ERROR_BAD_MAGIC_OP, value);
            break;
        default:
            if (value != 0)
                report_error(ERROR_BAD_MAGIC_OP, value);
            m_magic_state = MAGIC_NONE;
            break;
        }
        break;
	case ISS_MEMCHECKER_R1:
        m_r1 = value;
        break;
	case ISS_MEMCHECKER_R2:
        m_r2 = value;
        break;

	case ISS_MEMCHECKER_CONTEXT_ID_CREATE_TMP:
	case ISS_MEMCHECKER_CONTEXT_ID_CREATE:
    {
        if ( m_opt_show_ctx ) {
            std::cout << " -----------------"
                      << MEMCHK_COLOR_INFOC(" New execution context #" << std::hex << value)
                      << " by " << iss_t::m_name << " cpu" << std::endl << std::endl;
            report_current_ctx();
            std::cout << " New stack         " << std::hex << m_r1 << "-" << (uint64_t)m_r1+m_r2
                      << std::dec << " (" << m_r2 << " bytes)" << std::endl << std::endl;
        }

        ContextState *c = new ContextState( value, m_r1, (uint64_t)m_r1+m_r2,
                                            reg_id == ISS_MEMCHECKER_CONTEXT_ID_CREATE_TMP );

        if ( ! s_memory_state->context_create(value, c) )
            report_error(ERROR_BAD_CONTEXT_CREATE, value);

        uint32_t sp = get_cpu_sp();
        if (!c->stack_contains(sp))
            sp = c->m_stack_upper - 4;

        c->m_last_sp = sp - iss_t::debugGetRegisterValue(iss_t::ISS_DEBUG_REG_STACK_REDZONE_SIZE);

        bool err = false;

        for ( uint64_t addr = c->m_stack_lower; addr < c->m_last_sp; addr+= 4 ) {
            AddressInfo *ai = s_memory_state->info_for_address(addr);
            if ( ! ( ai->region()->state() & (
                         __iss_memchecker::RegionInfo::REGION_STATE_ALLOCATED
                         | __iss_memchecker::RegionInfo::REGION_STATE_GLOBAL
                         | __iss_memchecker::RegionInfo::REGION_STATE_STACK
                                              ) ) ) {
                err = true;
                m_last_region_touched = ai->region();
            }
            ai->set_initialized(false);
            ai->set_invalid(true);
        }

        for ( uint64_t addr = c->m_last_sp; addr < c->m_stack_upper; addr+= 4 ) {
            AddressInfo *ai = s_memory_state->info_for_address(addr);
            if ( ! ( ai->region()->state() & (
                         __iss_memchecker::RegionInfo::REGION_STATE_ALLOCATED
                         | __iss_memchecker::RegionInfo::REGION_STATE_GLOBAL
                         | __iss_memchecker::RegionInfo::REGION_STATE_STACK
                                              ) ) ) {
                err = true;
                m_last_region_touched = ai->region();
            }
            ai->set_invalid(false);
        }

        if ( (m_enabled_checks & ISS_MEMCHECKER_CHECK_REGION) && err)
            report_error(ERROR_CREATING_STACK_NOT_ALLOC);
        break;
    }

    case ISS_MEMCHECKER_INITIALIZED:
    {
        if ( value ) {    // single address
            AddressInfo *ai = s_memory_state->info_for_address( value );
            ai->set_initialized(true);

        } else {          // address range
            for ( uint32_t addr = m_r1; addr < m_r1 + m_r2; addr += 4 ) {
                AddressInfo *ai = s_memory_state->info_for_address( addr );
                ai->set_initialized(true);
            }
        }

        break;
    }

    case ISS_MEMCHECKER_LOCK_DECLARE:
    {
        AddressInfo *ai = s_memory_state->info_for_address( m_r1 );
        ai->set_spinlock(value != 0);
        break;
    }

    case ISS_MEMCHECKER_DELAYED_MAGIC:
    {
        m_delayed_pc_min = get_cpu_pc();
        m_delayed_pc_max = value;
        m_magic_state = MAGIC_DELAYED;
        break;
    }

    case ISS_MEMCHECKER_BYPASS_SP_CHECK:
    {
        assert( m_r1 <= m_r2 );
        if (value) {
            assert( m_r2 - m_r1 < 8192 ); // ensure we do not declare large areas
            m_bypass_pc |= address_set_t(m_r1, m_r2);
        }
        else
            m_bypass_pc &= ~address_set_t(m_r1, m_r2);
        break;
    }

	case ISS_MEMCHECKER_CONTEXT_ID_CHANGE:
    {
        if ( m_opt_show_ctx ) {
            std::cout << " -----------------"
                      << MEMCHK_COLOR_INFOC(" Rename context #" << std::hex << m_r1 << " to " << value)
                      << " by " << iss_t::m_name << " cpu" << std::endl << std::endl;
            report_current_ctx();
        }

        ContextState *ref = s_memory_state->context_get( m_r1 );
        ContextState *n = new ContextState(
            value, ref->m_stack_lower, ref->m_stack_upper );

        n->m_last_sp = ref->m_last_sp;

        if ( ! s_memory_state->context_delete(m_r1) )
            report_error(ERROR_BAD_CONTEXT_DEL, m_r1);

        if ( ! s_memory_state->context_create( value, n ) )
            report_error(ERROR_BAD_CONTEXT_CREATE, value);

        if ( m_current_context->is( m_r1 ) ) {
            update_context(n);
        }

        break;
    }

	case ISS_MEMCHECKER_CONTEXT_INVALIDATE: {

        if (value == ISS_MEMCHECKER_ID_CURRENT)
            value = m_current_context->id();

        if ( m_opt_show_ctx ) {
            std::cout << " -----------------"
                      << MEMCHK_COLOR_INFOC(" Invalidate context #" << std::hex << value)
                      << " by " << iss_t::m_name << " cpu" << std::endl << std::endl;
            report_current_ctx();
            std::cout << std::endl;
        }

        ContextState *ref = s_memory_state->context_get( value );

        for ( uint64_t addr = ref->m_stack_lower; addr < ref->m_stack_upper; addr+= 4 ) {
            AddressInfo *ai = s_memory_state->info_for_address(addr);
            ai->set_initialized(false);
            ai->set_invalid(true);
        }

        if ( !s_memory_state->context_invalidate( value ) ) {
            report_error(ERROR_BAD_CONTEXT_INVALIDATE, value);
        }

        break;
    }

	case ISS_MEMCHECKER_CONTEXT_ID_DELETE: {

        if (value == ISS_MEMCHECKER_ID_CURRENT)
            value = m_current_context->id();

        if ( m_opt_show_ctx ) {
            std::cout << " -----------------"
                      << MEMCHK_COLOR_INFOC(" Delete context #" << std::hex << value)
                      << " by " << iss_t::m_name << " cpu" << std::endl << std::endl;
            report_current_ctx();
            std::cout << std::endl;
        }

        ContextState *ref = s_memory_state->context_get( value );

        for ( uint64_t addr = ref->m_stack_lower; addr < ref->m_stack_upper; addr+= 4 ) {
            AddressInfo *ai = s_memory_state->info_for_address(addr);
            ai->set_initialized(false);
            ai->set_invalid(false);
        }

        if ( m_current_context->is( value ) ) {
            update_context(s_memory_state->unknown_context);
        }

        if ( !s_memory_state->context_delete(value) )
            report_error(ERROR_BAD_CONTEXT_DEL, value);

        break;
    }

	case ISS_MEMCHECKER_CONTEXT_SWITCH:
    {
        m_no_repeat_mask &= ~ERROR_IRQ_ENABLED_TMP;

        ContextState *cs = s_memory_state->context_get( value );

        if ( m_opt_show_ctxsw ) {
            std::cout << " -----------------"
                      << MEMCHK_COLOR_INFOS(" Context switch to #" << std::hex << value)
                      << " by " << iss_t::m_name << " cpu" << std::endl << std::endl;
            report_current_ctx();
            std::cout << " Switch to" << std::endl
                      << "                   " << *cs << std::endl << std::endl;
        }

        if ( m_current_context->temporary() ) {
            uint32_t tid = m_current_context->id();
            s_memory_state->context_delete( tid );

            if ( m_opt_show_ctxsw )
                std::cout << "                   Deleted temporary context #"
                          << std::hex << tid << std::endl << std::endl;
        }

        update_context( cs );

        if ( cs == s_memory_state->unknown_context || !cs->valid() )
            report_error(ERROR_BAD_CONTEXT_SWITCH, value);

        break;
    }

	case ISS_MEMCHECKER_MEMORY_REGION_UPDATE:
    {
        __iss_memchecker::RegionInfo::State state;
        switch (value) {
        case ISS_MEMCHECKER_REGION_FREE:
            state = __iss_memchecker::RegionInfo::REGION_STATE_FREE;
            break;
        case ISS_MEMCHECKER_REGION_ALLOC:
            state = __iss_memchecker::RegionInfo::REGION_STATE_ALLOCATED;
            break;
        case ISS_MEMCHECKER_REGION_NONALLOC_STACK:
            state = __iss_memchecker::RegionInfo::REGION_STATE_STACK;
            break;
        case ISS_MEMCHECKER_REGION_GLOBAL:
            state = __iss_memchecker::RegionInfo::REGION_STATE_GLOBAL;
            break;
        default:
            assert(!"Invalid region state");
        }

        if ( m_opt_show_region ) {
            std::cout << " -----------------"
                      << MEMCHK_COLOR_INFOR(" Region " << std::hex << m_r1 << "-" << (uint64_t)m_r1+m_r2
                                            << " now " << RegionInfo::state_str(state))
                      << " by " << iss_t::m_name << " cpu" << std::endl << std::endl;
            report_current_ctx();
            std::cout << std::endl;
        }

        uint32_t lta;
        error_level_t e = s_memory_state->region_update_state(
          state, get_cpu_pc(), m_r1, m_r2, &m_last_region_touched, &lta );

        report_error( e, lta );
        break;
    }
	case ISS_MEMCHECKER_ENABLE_CHECKS:

        m_enabled_checks |= value;

        if ( m_opt_show_enable ) {
            std::cout << " -----------------"
                      << MEMCHK_COLOR_INFOR(" Enabled checks " << std::hex << value
                                            << " (new value is " << m_enabled_checks << ")" )
                      << " by " << iss_t::m_name << " cpu" << std::endl << std::endl;
            report_current_ctx();
            std::cout << std::endl;
        }

        break;
	case ISS_MEMCHECKER_DISABLE_CHECKS:

        m_enabled_checks &= ~value;

        if ( m_opt_show_enable ) {
            std::cout << " -----------------"
                      << MEMCHK_COLOR_INFOR(" Disabled checks " << std::hex << value
                                            << " (new value is " << m_enabled_checks << ")" )
                      << " by " << iss_t::m_name << " cpu" << std::endl << std::endl;
            report_current_ctx();
            std::cout << std::endl;
        }

        break;

	case ISS_MEMCHECKER_SET_SP_REG:
        m_sp_reg_id = value;
        break;

    default:
        assert(!"Unknown register");
        break;
    }
}

template<typename iss_t>
void IssMemchecker<iss_t>::update_context( ContextState *state )
{
#if defined(SOCLIB_MODULE_DEBUG)
     std::cout << iss_t::m_name
               << " switching from " << *m_current_context
               << " to " << *state << std::endl;
#endif

#if 1
    m_last_context->unref();
    m_last_context = m_current_context;
#else
    m_current_context->unref();
#endif

    m_current_context->unschedule();
    m_current_context = state;
    m_current_context->schedule(m_cpuid);
    m_current_context->ref();
}

template<typename iss_t>
void IssMemchecker<iss_t>::handle_comm( const struct iss_t::DataRequest &dreq )
{
    uint32_t reg_no = (dreq.addr-m_comm_address)/4;
    assert( dreq.be == 0xf && "Only read/write word are allowed in memchecker area" );

    switch ( dreq.type ) {
    case iss_t::DATA_READ:
        m_data_answer_value = register_get(reg_no);
        if ( m_magic_state == MAGIC_BE )
            m_data_answer_value = soclib::endian::uint32_swap(m_data_answer_value);
        break;
    case iss_t::DATA_WRITE: {
        uint32_t data = dreq.wdata;
        m_data_answer_value = 0;
        if ( m_magic_state == MAGIC_BE )
            data = soclib::endian::uint32_swap(data);
        register_set(reg_no, data);
        break;
    }
    case iss_t::XTN_WRITE:
    case iss_t::XTN_READ:
    case iss_t::DATA_LL:
    case iss_t::DATA_SC:
        assert(!"Only read & write allowed in memchecker area");
        break;
    }
    m_has_data_answer = true;
}

template<typename iss_t>
void IssMemchecker<iss_t>::check_data_access( const struct iss_t::DataRequest &dreq,
                                              const struct iss_t::DataResponse &drsp )
{
    bool new_req = !m_req_checked && dreq.valid;
    m_req_checked = dreq.valid && !drsp.valid;

    error_level_t err = ERROR_NONE;
    AddressInfo *ai = s_memory_state->info_for_address(dreq.addr);
    const char *op = NULL;

    if (new_req && dreq.addr == 0 && (m_enabled_checks & ISS_MEMCHECKER_CHECK_INIT))
        err |= ERROR_NULL_POINTER_ACCESS;

    switch ( dreq.type ) {
    case iss_t::DATA_LL:
        if ( new_req && ai->is_spinlock() ) {
            if ( m_held_locks.count(dreq.addr) )
                err |= ERROR_LOCK_DEAD_LOCK;

            if ( m_opt_show_lockops &&
                 m_last_data_access == dreq && !(m_blast_data_access == m_last_data_access) ) {
                op = "Spinning on";
            }
        }
    case iss_t::DATA_READ:
        if ( new_req && m_enabled_checks & ISS_MEMCHECKER_CHECK_INIT )
            err |= ai->do_read();
        break;

    case iss_t::DATA_SC:
        if (drsp.valid) {
            err |= ai->do_write();

            if ( drsp.rdata ) // sc failed
                break;
        }

    case iss_t::DATA_WRITE:
        if (drsp.valid) {

            err |= ai->do_write();

            // record held spin-locks states
            if ( ai->is_spinlock() ) {
                if ( dreq.type == iss_t::DATA_WRITE || drsp.rdata == Iss2::SC_ATOMIC ) {
                    m_no_repeat_mask &= ~(ERROR_IRQ_ENABLED_LOCK | ERROR_LOCK_DEAD_LOCK);

                    if (dreq.wdata) {
                        m_held_locks[dreq.addr] = true;

                        if ( m_opt_show_lockops )
                            op = "Lock";
                    } else {
                        m_held_locks.erase(dreq.addr);
                        if ( m_opt_show_lockops )
                            op = "Unlock";
                    }
                }
            }
        }

        break;
    case iss_t::XTN_WRITE:
    case iss_t::XTN_READ:
        return;
    }

    if (op) {
        std::cout << " -----------------"
                  << MEMCHK_COLOR_INFOL(" " << op << " " << std::hex << dreq.addr)
                  << " by " << iss_t::m_name << " cpu" << std::endl << std::endl;
        report_current_ctx();
        std::cout << std::endl;
    }

    m_blast_data_access = m_last_data_access;
    m_last_data_access = dreq;

    if (new_req) {
        uint32_t sp_bound = get_cpu_sp() - iss_t::debugGetRegisterValue(iss_t::ISS_DEBUG_REG_STACK_REDZONE_SIZE);

        if ( m_current_context->stack_contains(dreq.addr) ) {
            if ( ( m_enabled_checks & ISS_MEMCHECKER_CHECK_SP )
                 && dreq.addr < sp_bound ) {
                err |= ERROR_DATA_ACCESS_BELOW_SP;
            }
        } else {
            if ( m_enabled_checks & ISS_MEMCHECKER_CHECK_REGION ) {
                RegionInfo *ri = ai->region();
                switch ( dreq.type ) {
                case iss_t::DATA_READ:
                case iss_t::DATA_LL:
                    err |= ri->do_read();
                    break;
                case iss_t::DATA_SC:
                case iss_t::DATA_WRITE:
                    err |= ri->do_write();
                    break;
                case iss_t::XTN_WRITE:
                case iss_t::XTN_READ:
                    return;
                }
            }
        }
    }

    report_error(err, dreq.addr);
}

template<typename iss_t>
void IssMemchecker<iss_t>::report_current_ctx()
{
    std::cout << " Executed from     PC=" << s_memory_state->get_symbol(get_cpu_pc())
              << ", SP=" << get_cpu_sp() << std::endl;
    std::cout << "                   " << *m_current_context << std::endl;
}

template<typename iss_t>
void IssMemchecker<iss_t>::report_error(error_level_t errors_, uint32_t extra)
{
    errors_ &= m_report_mask;
    errors_ &= ~m_no_repeat_mask;
    m_no_repeat_mask |= (errors_ & repeat_filter);
    error_level_t old_errors = errors_;

    while ( errors_ ) {

        static const char *acc;

        switch (m_last_data_access.type) {
        case iss_t::DATA_READ:
            acc = "read";
            break;
        case iss_t::DATA_WRITE:
            acc = "write";
            break;
        case iss_t::DATA_LL:
            acc = "linked load";
            break;
        case iss_t::DATA_SC:
            acc = "conditional store";
            break;
        default:
            acc = "special";
        }

        // process one error at once
        error_level_t error = errors_ & ~(errors_ - 1);

        uint32_t sp = get_cpu_sp();
        uint32_t oob = 0;
        bool show_access = false;
        bool show_locks = false;

        RegionInfo *ri = 0, *ro = 0;

        // Signal to GDB

        std::cout << " -----------------";

        AddressInfo *ai = s_memory_state->info_for_address(m_last_data_access.addr);

        switch ( error ) {

        case ERROR_UNINITIALIZED_WORD:
            std::cout << MEMCHK_COLOR_WARN(" Memory " << acc << " in non-initialized word at "
                                           << std::hex << m_last_data_access.addr);
            ri = ai->region();
            show_access = true;
            break;

        case ERROR_REGION_OVERLAP:
            std::cout << MEMCHK_COLOR_ERR(" Region overlap ");
            break;

        case ERROR_INVALID_REGION:
            ri = ai->region();
            std::cout << MEMCHK_COLOR_ERR(" Memory " << acc <<
                                          " in " << ri->state_str() << " region at "
                                          << std::hex << m_last_data_access.addr);
            show_access = true;
            break;

        case ERROR_INVALID_ACCESS:
            ri = ai->region();
            std::cout << MEMCHK_COLOR_ERR(" Memory " << acc << " to invalid stack word "
                                          << std::hex << m_last_data_access.addr);
            show_access = true;
            break;

        case ERROR_NULL_POINTER_ACCESS:
            std::cout << MEMCHK_COLOR_ERR(" Null pointer " << acc << " access" );
            break;

        case ERROR_CREATING_STACK_NOT_ALLOC:
            std::cout << MEMCHK_COLOR_ERR(" Stack creation in non-allocated memory");
            ri = m_last_region_touched;
            break;

        case ERROR_BAD_REGION_REALLOCATION:
            std::cout << MEMCHK_COLOR_ERR(" Bad memory region state change at: "
                                          << std::hex << extra);
            ro = m_last_region_touched;
            ri = s_memory_state->info_for_address(extra)->region();
            break;

        case ERROR_CONTEXT_ON_TWO_CPUS:
            std::cout << MEMCHK_COLOR_ERR(" Context running on two processors");
            break;

        case ERROR_SP_OUTOFBOUNDS:
            std::cout << MEMCHK_COLOR_WARN(" Stack pointer out of bounds: ") << std::hex << sp;
            oob = sp;
            break;

        case ERROR_DATA_ACCESS_BELOW_SP:
            std::cout << MEMCHK_COLOR_ERR(" Memory " << acc << " below stack pointer at "
                                           << std::hex << m_last_data_access.addr);
            show_access = true;
            break;

        case ERROR_BAD_CONTEXT_DEL:
            std::cout << MEMCHK_COLOR_ERR(" Trying to delete non-existing context: "
                                          << std::hex << extra);
            show_access = true;
            break;

        case ERROR_BAD_CONTEXT_CREATE:
            std::cout << MEMCHK_COLOR_ERR(" Trying to create context with existing id: "
                                          << std::hex << extra);
            show_access = true;
            break;

        case ERROR_BAD_CONTEXT_INVALIDATE:
            std::cout << MEMCHK_COLOR_ERR(" Trying to invalidate context with non-existing id: "
                                          << std::hex << extra);
            show_access = true;
            break;

        case ERROR_BAD_CONTEXT_SWITCH:
            std::cout << MEMCHK_COLOR_ERR(" Trying to switch to an invalid context "
                                          << std::hex << extra);
            show_access = true;
            break;

        case ERROR_IRQ_ENABLED_MAGIC:
            std::cout << MEMCHK_COLOR_ERR(" Processor IRQs enabled while in memchecker magic mode ");
            break;

        case ERROR_IRQ_ENABLED_TMP:
            std::cout << MEMCHK_COLOR_ERR(" Processor IRQs enabled during temporary context execution ");
            break;

        case ERROR_IRQ_ENABLED_LOCK:
            std::cout << MEMCHK_COLOR_WARN(" Processor IRQs enabled while some spinlocks are held ");
            show_locks = true;
            break;

        case ERROR_IRQ_DISABLED_USER:
            std::cout << MEMCHK_COLOR_WARN(" Processor IRQs disabled in user mode ");
            break;

        case ERROR_LOCK_DEAD_LOCK:
            std::cout << MEMCHK_COLOR_ERR(" Spinlock dead lock ");
            show_locks = true;
            break;

        case ERROR_BAD_MAGIC_OP:
            std::cout << MEMCHK_COLOR_ERR(" Bad magic register operation: " << std::hex << extra);
            break;
        }

        std::cout << " by " << iss_t::m_name << " cpu" << std::endl << std::endl;

        report_current_ctx();

        if ( error & ERROR_SP_OUTOFBOUNDS ) {
            if ( oob < m_current_context->m_stack_lower )
                std::cout << " Out of bounds     " << (m_current_context->m_stack_lower - oob)
                          << " bytes below" << std::endl;
            else if ( oob > m_current_context->m_stack_upper )
                std::cout << "                   " << (oob - m_current_context->m_stack_upper)
                          << " bytes above" << std::endl;
        }

        if ( ri ) {
            std::cout << " Region            Current    " << *ri << std::endl;

            if ( RegionInfo *p = ri->prev_state() )
                std::cout << "                   Previous   " << *p << std::endl;
        }

        if ( ro ) {
            std::cout << " Offending         Current    " << *ri << std::endl;

            if ( RegionInfo *p = ri->prev_state() )
                std::cout << "                   Previous   " << *p << std::endl;
        }

        if ( show_access ) {
            if ( m_opt_dump_access )
                std::cout << " Memory access     " << m_last_data_access << std::endl;

            BinaryFileSymbolOffset s = s_memory_state->get_symbol(m_last_data_access.addr);

            if (s.symbol().name() != "Unknown" )
                std::cout << " Memory access to     " << s << std::endl;
        }

        if ( show_locks ) {
            std::cout << std::endl;
            for ( held_locks_map_t::iterator i = m_held_locks.begin(); i != m_held_locks.end(); i++ )
                std::cout << " Spin-lock held    " << MEMCHK_BOLD(i->first) << std::endl;
        }

        std::cout << std::endl;

        if ( m_opt_dump_iss ) {
            iss_t::dump();
            std::cout << std::endl;
        }

        if ( ( m_trap_mask & error ) && m_bypass && this->debugExceptionBypassed( iss_t::EXCL_TRAP ) )
            m_bypass = false;

        errors_ ^= error;
    }

    if (old_errors && m_opt_exit_on_error)
        abort();
}

template<typename iss_t>
uint32_t IssMemchecker<iss_t>::executeNCycles(
    uint32_t ncycle, struct iss_t::InstructionResponse irsp,
    struct iss_t::DataResponse drsp, uint32_t irq_bit_field )
{
    struct iss_t::InstructionRequest ireq = ISS_IREQ_INITIALIZER;
    struct iss_t::DataRequest dreq = ISS_DREQ_INITIALIZER;
    iss_t::getRequests(ireq, dreq);
    m_bypass = true;

    assert( !(drsp.valid && !dreq.valid) );

    if ( isMagicDreq(dreq) ) {
        if ( !ireq.valid || irsp.valid )
            handle_comm( dreq );
        dreq.valid = false;
    }

    if ( m_has_data_answer ) {
        assert( !drsp.valid && "Cache speaking while i'm answering ISS" );
        drsp.valid = true;
        drsp.error = false;
        drsp.rdata = m_data_answer_value;
        m_has_data_answer = false;

    } else {
        check_data_access( dreq, drsp );
    }

    error_level_t errl = ERROR_NONE;

    if (m_enabled_checks & ISS_MEMCHECKER_CHECK_IRQ) {
        if ( iss_t::debugGetRegisterValue(iss_t::ISS_DEBUG_REG_IS_INTERRUPTIBLE) ) {

            if ( m_current_context->temporary() ) {
                errl |=  ERROR_IRQ_ENABLED_TMP;
            }

            if ( !m_held_locks.empty() ) {
                errl |= ERROR_IRQ_ENABLED_LOCK;
            }
        } else {
            if (iss_t::debugGetRegisterValue(iss_t::ISS_DEBUG_REG_IS_USERMODE))
                errl |= ERROR_IRQ_DISABLED_USER;
        }
    }

    int cycles = 0;
    if ( m_bypass )
        cycles = iss_t::executeNCycles( ncycle, irsp, drsp, irq_bit_field );

    switch ( m_magic_state ) {
    case MAGIC_NONE: {
        if (iss_t::debugGetRegisterValue(iss_t::ISS_DEBUG_REG_IS_USERMODE))
            break;

        uint32_t sp = get_cpu_sp();
        uint32_t rsp = sp - iss_t::debugGetRegisterValue(iss_t::ISS_DEBUG_REG_STACK_REDZONE_SIZE);
        bool bypass_pc = m_bypass_pc[get_cpu_pc()];

        // keep track of non-initialized and invalid memory as the stack pointer changes
        if ( !bypass_pc && m_current_context->m_last_sp && m_current_context->stack_contains(rsp) ) {
            for (uint32_t i = rsp; i < m_current_context->m_last_sp; i += 4) {
                // std::cout << std::hex << "valid   " << i << " " << get_cpu_pc() << "\n";
                s_memory_state->info_for_address(i)->set_invalid(false);
            }
            for (uint32_t i = m_current_context->m_last_sp; i < rsp; i += 4) {
                // std::cout << std::hex << "invalid " << i << " " << get_cpu_pc() << "\n";
                s_memory_state->info_for_address(i)->set_initialized(false);
                s_memory_state->info_for_address(i)->set_invalid(true);
            }
            m_current_context->m_last_sp = rsp;
        }

        // check sp bounds
        if ( (m_enabled_checks & ISS_MEMCHECKER_CHECK_SP) && !bypass_pc &&
             (!m_current_context->stack_contains(sp) || !m_current_context->stack_contains(rsp)) )
            errl |= ERROR_SP_OUTOFBOUNDS;
        else
            m_no_repeat_mask &= ~ERROR_SP_OUTOFBOUNDS;
    }

    case MAGIC_DELAYED:
        if ( get_cpu_pc() < m_delayed_pc_min ||
             get_cpu_pc() >= m_delayed_pc_max )
            m_magic_state = MAGIC_NONE;
        break;

    default:
        break;
    }

    report_error(errl, 0);

    return cycles;
}

}}


// Local Variables:
// tab-width: 4
// c-basic-offset: 4
// c-file-offsets:((innamespace . 0)(inline-open . 0))
// indent-tabs-mode: nil
// End:

// vim: filetype=cpp:expandtab:shiftwidth=4:tabstop=4:softtabstop=4
