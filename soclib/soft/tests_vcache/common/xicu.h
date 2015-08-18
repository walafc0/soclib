#define XICU_BASE 0xd8200000
#define __XICU_FUNC_SHIFT 7
#define __XICU_IDX_SHIFT 2
#define __XICU_RIDX(func, idx) \
    ((((func) & 0x1f) << __XICU_FUNC_SHIFT) + \
     (((idx) & 0x1f) << __XICU_IDX_SHIFT))

/* triggers the wtiidx interrupt. R/W */
#define XICU_WTI_REG_FUNC 0x00
#define XICU_WTI_REG(wtiidx) __XICU_RIDX(XICU_WTI_REG_FUNC, wtiidx)

/* Timer period R/W */
#define XICU_PTI_PER_FUNC 0x01
#define XICU_PTI_PER(ptiidx) __XICU_RIDX(XICU_PTI_PER_FUNC, ptiidx)

/* timer value R/W */
#define XICU_PTI_VAL_FUNC 0x02
#define XICU_PTI_VAL(ptiidx) __XICU_RIDX(XICU_PTI_VAL_FUNC, ptiidx)

/* Timer interrupt acknowledge. R */
#define XICU_PTI_ACK_FUNC 0x03
#define XICU_PTI_ACK(ptiidx) __XICU_RIDX(XICU_PTI_ACK_FUNC, ptiidx)

/*
 * remaining registers define the way interrupt sources are mutiplexed to
 * the output lines. Indexed by output line number.
 */
/*
 * Multiplex timers to output lines:
 * XICU_MSK_PTI mask register for outidx line (bit to 1 enable interrupt)
 * XICU_MSK_PTI_E atomically add a set of bits to XICU_MSK_PTI
 * XICU_MSK_PTI_D atomically clear a set of bits from XICU_MSK_PTI
 * XICU_PTI_ACT get active PTI lines for this output line
 */
#define XICU_MSK_PTI_FUNC 0x04 /* R/W */
#define XICU_MSK_PTI(outidx) __XICU_RIDX(XICU_MSK_PTI_FUNC, outidx)

#define XICU_MSK_PTI_E_FUNC 0x05 /* W */
#define XICU_MSK_PTI_E(outidx) __XICU_RIDX(XICU_MSK_PTI_E_FUNC, outidx)

#define XICU_MSK_PTI_D_FUNC 0x06 /* W */
#define XICU_MSK_PTI_D(outidx) __XICU_RIDX(XICU_MSK_PTI_D_FUNC, outidx)

#define XICU_PTI_ACT_FUNC 0x06 /* R */
#define XICU_PTI_ACT(outidx) __XICU_RIDX(XICU_PTI_ACT_FUNC, outidx)

/*
 * Multiplex hardware input lines to output lines:
 * XICU_MSK_HWI mask register for outidx line (bit to 1 enable interrupt)
 * XICU_MSK_HWI_E atomically add a set of bits to XICU_MSK_HWI
 * XICU_MSK_HWI_D atomically clear a set of bits from XICU_MSK_HWI
 * XICU_HWI_ACT get active HWI lines for this output line
 */
#define XICU_MSK_HWI_FUNC 0x08 /* R/W */
#define XICU_MSK_HWI(outidx) __XICU_RIDX(XICU_MSK_HWI_FUNC, outidx)

#define XICU_MSK_HWI_E_FUNC 0x09 /* W */
#define XICU_MSK_HWI_E(outidx) __XICU_RIDX(XICU_MSK_HWI_E_FUNC, outidx)

#define XICU_MSK_HWI_D_FUNC 0x0a /* W */
#define XICU_MSK_HWI_D(outidx) __XICU_RIDX(XICU_MSK_HWI_D_FUNC, outidx)

#define XICU_HWI_ACT_FUNC 0x0a /* R */
#define XICU_HWI_ACT(outidx) __XICU_RIDX(XICU_HWI_ACT_FUNC, outidx)

/*
 * Multiplex hardware input lines to output lines:
 * XICU_MSK_WTI mask register for outidx line (bit to 1 enable interrupt)
 * XICU_MSK_WTI_E atomically add a set of bits to XICU_MSK_WTI
 * XICU_MSK_WTI_D atomically clear a set of bits from XICU_MSK_WTI
 * XICU_WTI_ACT get active WTI lines for this output line
 */
#define XICU_MSK_WTI_FUNC 0x0c /* R/W */
#define XICU_MSK_WTI(outidx) __XICU_RIDX(XICU_MSK_WTI_FUNC, outidx)

#define XICU_MSK_WTI_E_FUNC 0x0d /* W */
#define XICU_MSK_WTI_E(outidx) __XICU_RIDX(XICU_MSK_WTI_E_FUNC, outidx)

#define XICU_MSK_WTI_D_FUNC 0x0e /* W */
#define XICU_MSK_WTI_D(outidx) __XICU_RIDX(XICU_MSK_WTI_D_FUNC, outidx)

#define XICU_WTI_ACT_FUNC 0x0e /* R */
#define XICU_WTI_ACT(outidx) __XICU_RIDX(XICU_WTI_ACT_FUNC, outidx)

/* source priority encoder for outpout lines */
#define XICU_PRIO_FUNC 0x0f
#define XICU_PRIO(outidx) __XICU_RIDX(XICU_PRIO_FUNC, outidx)

#define XICU_PRIO_PTI 0x00000001 /* Timer interrrupt pending */
#define XICU_PRIO_PTII(val) ((val) >> 8 & 0x1f) /* first PTI pending */
#define XICU_PRIO_HWI 0x00000002 /* Hardware interrrupt pending */
#define XICU_PRIO_HWII(val) ((val) >> 16 & 0x1f) /* first HWI pending */
#define XICU_PRIO_WTI 0x00000004 /* write-triggered interrrupt pending */
#define XICU_PRIO_WTII(val) ((val) >> 24 & 0x1f) /* first WTI pending */

#define XICU_PRIO_PENDING (XICU_PRIO_PTI|XICU_PRIO_HWI|XICU_PRIO_WTI)
