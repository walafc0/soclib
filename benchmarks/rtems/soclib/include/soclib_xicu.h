/*
 * Soclib timer registers
 *
 * Alexandre Becoulet for the SoCLib BSP
 * Based on MutekH xicu driver by Nicolas Pouillon
 */

#ifndef SOCLIB_TIMER_H_
#define SOCLIB_TIMER_H_

#define XICU_WTI_REG 0
#define XICU_PTI_PER 1
#define XICU_PTI_VAL 2
#define XICU_PTI_ACK 3
#define XICU_MSK_PTI 4
#define XICU_MSK_PTI_ENABLE 5
#define XICU_MSK_PTI_DISABLE 6
#define XICU_PTI_ACTIVE 6
#define XICU_MSK_HWI 8
#define XICU_MSK_HWI_ENABLE 9
#define XICU_MSK_HWI_DISABLE 10
#define XICU_HWI_ACTIVE 10
#define XICU_MSK_WTI 12
#define XICU_MSK_WTI_ENABLE 13
#define XICU_MSK_WTI_DISABLE 14
#define XICU_WTI_ACTIVE 14
#define XICU_PRIO 15

#define XICU_PRIO_WTI(val) (((val) >> 24) & 0x1f)
#define XICU_PRIO_HWI(val) (((val) >> 16) & 0x1f)
#define XICU_PRIO_PTI(val) (((val) >> 8) & 0x1f)
#define XICU_PRIO_HAS_WTI(val) ((val) & 0x4)
#define XICU_PRIO_HAS_HWI(val) ((val) & 0x2)
#define XICU_PRIO_HAS_PTI(val) ((val) & 0x1)

#define XICU_MAX_HWI 32
#define XICU_MAX_PTI 32

#define SOCLIB_XICU_ADDR( _base, _op, _id ) \
  ( (volatile uint32_t*)(_base) + ((_op)<<5) + (_id))

#define SOCLIB_XICU_READ( _base, _op, _id )	\
  ld_le32(SOCLIB_XICU_ADDR( _base, _op, _id ))

#define SOCLIB_XICU_WRITE( _base, _op, _id, _value )	\
  st_le32(SOCLIB_XICU_ADDR( _base, _op, _id ), (_value))

#endif

