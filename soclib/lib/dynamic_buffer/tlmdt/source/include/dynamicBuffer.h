#ifndef __DYNAMIC_BUFFER_H__
#define __DYNAMIC_BUFFER_H__

#include <assert.h>
#include <iostream>
#include <stdio.h>

template<class T>
class DynamicBufferElement{
 private:
  T element;
 public:
  DynamicBufferElement<T> *phys_prev;
  DynamicBufferElement<T> *phys_next;
  DynamicBufferElement<T> *virt_prev;
  DynamicBufferElement<T> *virt_next;
  size_t _freemark;
  bool valid;
 public:
  DynamicBufferElement(){
    phys_prev = NULL;
    phys_next = NULL;
    virt_prev = NULL;
    virt_next = NULL;
    _freemark = 0;
    valid = false;
  }
  DynamicBufferElement(T e){
    element = e;
    phys_prev = NULL;
    phys_next = NULL;
    virt_prev = NULL;
    virt_next = NULL;
    _freemark = 0;
    valid = false;
  }
  DynamicBufferElement<T> *getThis(){return this;}
  T* getElem(){return &element;}
  void setValid(){valid = true;}
  void setInvalid(){valid = false;}
  bool getValidity(){return valid;}
  bool operator< (DynamicBufferElement<T> &c){return element < c.element;}
};

template<class T>
class DynamicBuffer{
 private:
  //Class Vars
  bool   m_ordered;
  size_t m_delta, m_delta_size;
  DynamicBufferElement<T> m_fake;

  //Productor Private
  size_t p_gen_var, p_size;
  DynamicBufferElement<T> *p_gen_pt1, *p_gen_pt2, *p_begin, *p_end;

  //Customer Private
  size_t c_size, c_real_size;
  DynamicBufferElement<T> *c_gen_pt1, *c_gen_pt2, *c_begin, *c_end;

 public:
  //ITERATOR
  class reverse_iterator;
  class iterator{
  public:
    T* e;
    DynamicBuffer* db;
  public:
    iterator(){e = NULL; db = NULL;}
    iterator(const iterator &it){e = it.e; db = it.db;}
    iterator(T* el, DynamicBuffer *d){e = el;db = d;}
    iterator(const reverse_iterator &it){
      db = it.db;
      e = (((DynamicBufferElement<T>*)(it.e))->virt_next)->getElem();
    }
    inline iterator operator++(){
      e = ((DynamicBufferElement<T>*)e)->virt_next->getElem(); 
      return *this;
    }
    inline iterator operator++(int){iterator it = *this; ++*this; return it;}
    inline iterator operator--(){
      e = ((DynamicBufferElement<T>*)e)->virt_prev->getElem();
      return *this;
    }
    inline iterator operator--(int){iterator it = *this; --*this; return it;}
    inline bool     operator==(const iterator &it){return e == it.e;}
    inline bool     operator!=(const iterator &it){return e != it.e;}
    inline iterator advance(int n){for(int i=0;i<n;i++)++*this; return *this;}
    inline T operator*(){return *e;}
    inline T* operator&(){return e;}
    inline operator T(){return *e;} //For Casting Issues
  };
  friend std::ostream &operator<<(std::ostream &os,const iterator &it){
    return os << it.e;
  }
  iterator begin(){return iterator(m_fake.virt_next->getElem(),this);}
  iterator end()  {return iterator(m_fake.getElem()        ,this);}

  //REVERSE ITERATOR
  class reverse_iterator{
  public:
    T* e;
    DynamicBuffer* db;
  public:
    reverse_iterator(){e = NULL; db = NULL;}
    reverse_iterator(const reverse_iterator &it){e = it.e; db = it.db;}
    reverse_iterator(T* el,DynamicBuffer* d){e = el;db = d;}
    reverse_iterator(const iterator &it){
      db = it.db;
      e = (((DynamicBufferElement<T>*)(it.e))->virt_prev)->getElem(); 
    }
    inline reverse_iterator operator++(){
      e = ((DynamicBufferElement<T>*)e)->virt_prev->getElem();
      return *this;
    }
    inline reverse_iterator operator++(int){reverse_iterator it = *this; ++*this; return it;}
    inline reverse_iterator operator--(){
      e = ((DynamicBufferElement<T>*)e)->virt_next->getElem();
      return *this;
    }
    inline reverse_iterator operator--(int){reverse_iterator it = *this; --*this; return it;}
    inline reverse_iterator operator==(iterator &it){return e == it.e;}
    inline reverse_iterator operator!=(iterator &it){return e != it.e;}
    inline reverse_iterator advance(int n){for(int i=0;i<n;i++)++*this; return *this;}
    inline T operator*(){return *e;}
    inline T* operator&(){return e;}
    inline operator T(){return *e;} //For Casting Issues
  };
  friend std::ostream &operator<<(std::ostream &os,const reverse_iterator &it){
    return os << it.e;
  }
  reverse_iterator rbegin(){return reverse_iterator(m_fake.virt_prev.getElem(),this);}
  reverse_iterator rend()  {return reverse_iterator(m_fake.getElem()       ,this);}
  
 private:
  void expand(){
    p_gen_pt1 = NULL;
    p_gen_pt1 = new DynamicBufferElement<T>[m_delta];
    p_gen_pt2 = p_begin;
    while(p_gen_pt2->phys_next->_freemark){
      p_gen_pt2=p_gen_pt2->phys_next;
    }
    for(p_gen_var=1; p_gen_var < m_delta-1; p_gen_var++){
      p_gen_pt1[p_gen_var].phys_prev = p_gen_pt1[p_gen_var-1].getThis();
      p_gen_pt1[p_gen_var].phys_next = p_gen_pt1[p_gen_var+1].getThis();
      p_gen_pt1[p_gen_var]._freemark = p_gen_var;
    }
    p_gen_pt1->phys_prev = p_gen_pt2;
    p_gen_pt1->phys_next = p_gen_pt1[1].getThis();
    p_gen_pt1->_freemark = 0;
    p_gen_pt1[m_delta-1].phys_prev = p_gen_pt1[m_delta-2].getThis();
    p_gen_pt1[m_delta-1].phys_next = p_gen_pt2->phys_next;
    p_gen_pt1[m_delta-1]._freemark = m_delta-1;
    p_gen_pt2->phys_next->phys_prev = p_gen_pt1[m_delta-1].getThis();
    p_gen_pt2->phys_next = p_gen_pt1;
    ++m_delta_size;
    p_size += m_delta;
  }
  void reduce(){
    p_gen_pt1 = p_begin->phys_next;
    p_gen_var = 0;
    while(p_gen_pt1->_freemark){
      p_gen_pt1 = p_gen_pt1->phys_next;
    }
    p_gen_pt1->phys_prev->phys_next = p_gen_pt1[m_delta-1].phys_next;
    p_gen_pt1[m_delta-1].phys_next->phys_prev = p_gen_pt1->phys_prev;
    p_size -= m_delta;
    --m_delta_size;
    delete [] p_gen_pt1;
  }
  void clear(){
    c_begin = c_begin->phys_next;
    --c_size;
  }

 public:
  DynamicBuffer(bool i, size_t d=10):m_ordered(i),m_delta(d){
    assert(m_delta >= 2);
    p_begin = new DynamicBufferElement<T>[m_delta];
    p_end = p_begin;
    c_begin = p_begin;
    c_end = c_begin;
    p_size = m_delta;
    c_size = 0;
    c_real_size = 0;
    m_delta_size = 1;
    for(p_gen_var=1;p_gen_var < m_delta-1; p_gen_var++){
      p_end[p_gen_var].phys_prev = p_end[p_gen_var-1].getThis();
      p_end[p_gen_var].phys_next = p_end[p_gen_var+1].getThis();
      p_end[p_gen_var]._freemark = p_gen_var;
    }
    p_end[0].phys_prev = p_end[m_delta-1].getThis();
    p_end[0].phys_next = p_end[1].getThis();
    p_end[m_delta-1].phys_prev = p_end[m_delta-2].getThis();
    p_end[m_delta-1].phys_next = p_end[0].getThis();
    p_end[0]._freemark = 0;
    p_end[m_delta-1]._freemark = m_delta-1;
    expand();
    p_gen_pt1 = c_begin;
    for(int i=0;i<=10;i++){
      p_gen_pt1 = p_gen_pt1->phys_next;
    }
    m_fake.virt_next   = &m_fake;
    m_fake.virt_prev   = &m_fake;
  }
  ~DynamicBuffer(){
    p_gen_pt1 = p_begin;
    while(p_gen_pt1->_freemark)
      p_gen_pt1 = p_gen_pt1->phys_next;
    while(--m_delta_size){
      p_gen_pt2 = p_gen_pt1[m_delta-1].phys_next;
      delete [] p_gen_pt1;
      p_gen_pt1 = p_gen_pt2;
    }
    delete [] p_gen_pt1;
  }
  //reserveSlot / commitSlot allows to directly use the elements of the dynamicBuffer
  //Faster because there is no use of the copy constructor
  //Need more control from the user thread (reserve then commit)
  T* reserveSlot(){
    while(p_end != c_begin){
      p_end = p_end->phys_next;
      ++p_size;
    }
    if(p_size == m_delta)
      expand();
    while(p_size >= 3*m_delta){
      reduce();
    }
    return p_begin->getElem();
  }
  void commitSlot(){
    p_begin->setValid();
    --p_size;
    p_begin = p_begin->phys_next;
  }
  //This push is a standard, one call push but requires the copy constructor
  void push(T& e){
    *p_begin->getElem() = e;
    p_begin->setValid();
    --p_size;
    while(p_end != c_begin){
      p_end = p_end->phys_next;
      ++p_size;
    }
    if(p_size == m_delta)
      expand();
    p_begin = p_begin->phys_next;
    while(p_size >= 3*m_delta){
      reduce();
    }
  }
  void pop(){pop(begin());}
  void pop(iterator it){
    c_gen_pt1 = (DynamicBufferElement<T>*)it.e; 
    c_gen_pt1->setInvalid();
    c_gen_pt1->virt_next->virt_prev = c_gen_pt1->virt_prev;
    c_gen_pt1->virt_prev->virt_next = c_gen_pt1->virt_next;
    --c_real_size;
    while(!c_begin->getValidity() && c_size)
      clear();
  }
  void desynch(iterator it){
    c_gen_pt1 = (DynamicBufferElement<T>*)it.e;
    c_gen_pt1->virt_next->virt_prev = c_gen_pt1->virt_prev;
    c_gen_pt1->virt_prev->virt_next = c_gen_pt1->virt_next;
    --c_real_size;
  }
  void pop_desynched(iterator it){
    c_gen_pt1 = (DynamicBufferElement<T>*)it.e; 
    c_gen_pt1->setInvalid();
    while(!c_begin->getValidity() && c_size)
      clear();
  }
  T *front(){
    while(c_end != p_begin){
      c_gen_pt1 = m_fake.virt_prev;
      if(m_ordered)
	while(*c_end < *c_gen_pt1 && c_gen_pt1 != &m_fake)
	  c_gen_pt1 = c_gen_pt1->virt_prev;

      c_end->virt_prev = c_gen_pt1;
      c_end->virt_next = c_gen_pt1->virt_next;
      c_gen_pt1->virt_next->virt_prev = c_end;
      c_gen_pt1->virt_next = c_end;	  
      
      c_end = c_end->phys_next;
      ++c_size;
      ++c_real_size;
    }
    if(c_real_size)
      return m_fake.virt_next->getElem();
    else
      return NULL;
  }
  void reorder(iterator it){
    c_gen_pt1 = (DynamicBufferElement<T>*)it.e;
    c_gen_pt2 = c_gen_pt1->virt_next;
    //Find the new place
    while(c_gen_pt2 < c_gen_pt1 && c_gen_pt2 != &m_fake)
      c_gen_pt2 = c_gen_pt2->virt_next;
    //Disconnect moved elem
    c_gen_pt1->virt_next->virt_prev = c_gen_pt1->virt_prev;
    c_gen_pt1->virt_prev->virt_next = c_gen_pt1->virt_next;
    //Reconnect
    c_gen_pt1->virt_next = c_gen_pt2;
    c_gen_pt1->virt_prev = c_gen_pt2->virt_prev;
    c_gen_pt2->virt_prev->virt_next = c_gen_pt1;
    c_gen_pt2->virt_prev = c_gen_pt1;    
  }
  bool empty(){return !c_real_size;}
  size_t size(){return c_real_size;}
  size_t capacity(){return m_delta*m_delta_size;}
};

#endif /* __DYNAMIC_BUFFER_H__ */
