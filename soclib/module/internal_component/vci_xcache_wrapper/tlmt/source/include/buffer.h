#ifndef BUFFER_H
#define BUFFER_H

namespace soclib { namespace tlmt {

template<typename iss_t,typename vci_param>
class buffer 
{

public:

typename vci_param::addr_t     *m_addr;
typename vci_param::data_t     *m_data;
typename iss_t::DataAccessType *m_type;
tlmt_core::tlmt_time           *m_time;
int                            m_depth;
int                            m_caba_depth;
int                            m_ptr;
int                            m_ptw;
int                            m_items;

//////// constructor ////////////
buffer(int depth)
{
    m_data = new typename vci_param::data_t[depth];
    m_addr = new typename vci_param::addr_t[depth];
    m_type = new typename iss_t::DataAccessType[depth];
    m_time = new tlmt_core::tlmt_time[depth];
    m_depth = depth;
    m_caba_depth = 9; //caba depth + 1
    m_items = 0;
    m_ptw = 0;
    m_ptr = 0;
}

/////////////////////
inline void reset( )
{
    m_items = 0;
    m_ptw = 0;
    m_ptr = 0;
}

///////////////////////////////////////////////////
 inline void push(typename vci_param::addr_t ad, typename iss_t::DataAccessType ty, typename vci_param::data_t dt, tlmt_core::tlmt_time time)
{
    assert( m_items < m_depth ) ;
    m_data[m_ptw] = dt ;
    m_addr[m_ptw] = ad ;
    m_type[m_ptw] = ty ;
    m_time[m_ptw] = time ;
    m_items++ ;
    m_ptw++ ;
    if ( m_ptw == m_depth ) m_ptw = 0 ;
}

/////////////////////////
inline typename vci_param::data_t popData( )
{
    assert( m_items > 0 ) ;
    typename vci_param::data_t dt = m_data[m_ptr] ;
    m_items-- ;
    m_ptr++ ;
    if ( m_ptr == m_depth) m_ptr = 0 ;
    return dt ;
}

///////////////////////////
inline typename vci_param::addr_t getAddress( )
{
    return m_addr[m_ptr] ;
}

///////////////////////////
inline typename iss_t::DataAccessType getType( )
{
    return m_type[m_ptr] ;
}

///////////////////////////
 inline tlmt_core::tlmt_time getTime( )
{
    return m_time[m_ptr] ;
}

///////////////////////////
inline int getNItems( )
{
    return m_items;
}

////////////////////
inline bool empty( )
{
    return (m_items == 0) ;
}

////////////////////
inline bool full( )
{
    return (m_items == m_caba_depth) ;
}

//////////////////////////////////
inline bool endBurst( typename vci_param::addr_t ad )
{
    int ptlast = m_ptr + m_items - 1 ;
    if ( ptlast >= m_items ) ptlast = ptlast - m_items ;
    return ( ( ad & ~0X3 )  + 4 != m_addr[ptlast] ) ;
}

///////////////////////
inline bool notlastWrite()
{
  int ptnext = m_ptr + 1 ;
  if ( ptnext == m_depth ) ptnext = 0 ;
  bool notlast = ( m_addr[ptnext] == ( m_addr[m_ptr] + 4 ) ) && ( m_items > 1 );
  return notlast ;
}

}; // end class Buffer

}}

#endif

