/** instruction format union */
typedef union {

	// in use

	/* run() decoder */
	PACKED_BITFIELD(
		uint32_t cond:4,
		uint32_t decod0:4,
		uint32_t unused0:3,
		uint32_t decod1:1,
		uint32_t unused1:12,
		uint32_t decod2:4,
		uint32_t unused2:4,
		) decod;


	/* data processing */
	PACKED_BITFIELD(
		uint32_t cond:4,
		uint32_t is00:2,
		uint32_t i:1,
		uint32_t opcode:4,
		uint32_t setcond:1,
		uint32_t rn:4,
		uint32_t rd:4,
		uint32_t shift_code:8,
		uint32_t rm:4,
		) dp;

	/* rotate */
	PACKED_BITFIELD(
		uint32_t dummy:20,
		uint32_t rotate:4,
		uint32_t immval:8,	/* unsigned 8 bits immediate */
		) rot;

	/* mrs msr */
	PACKED_BITFIELD(
		uint32_t cond:4,
		uint32_t is00:2,
		uint32_t i:1,
		uint32_t is10:2,
		uint32_t p:1,
		uint32_t is10_:2,
		uint32_t f:1,
		uint32_t s:1,
		uint32_t x:1,
		uint32_t c:1,
		uint32_t rd:4,
		uint32_t dummy2:8,
		uint32_t rm:4,
		) ms;

	/* multiply */
	PACKED_BITFIELD(
		uint32_t cond:4,
		uint32_t is0000:4,
		uint32_t op:3,
		uint32_t setcond:1,
		uint32_t rd:4,
		uint32_t rn:4,
		uint32_t rs:4,
		uint32_t is1:1,
		uint32_t y:1,
		uint32_t x:1,
		uint32_t is1_2:1,
		uint32_t rm:4,
		) mul;

	/* branch and link */
	PACKED_BITFIELD(
		uint32_t cond:4,
		uint32_t is101:3,
		uint32_t link:1,
		uint32_t offset:24,
		) brl;

	/* branch exchange */
	PACKED_BITFIELD(
		uint32_t cond:4,
		uint32_t dummy:24,
		uint32_t rn:4,
		) brx;

	/* block data transfert */
	PACKED_BITFIELD(
		uint32_t cond:4,
		uint32_t is100:3,
		uint32_t pre_post:1,
		uint32_t up_down:1,
		uint32_t force_user:1,
		uint32_t write_back:1,
		uint32_t load_store:1,
		uint32_t rn:4,
		uint32_t reg_list:16,
		) bdt;

	/* single data transfert immediate */
	PACKED_BITFIELD(
		uint32_t cond:4,
		uint32_t is010:3,
		uint32_t pre_post:1,
		uint32_t up_down:1,
		uint32_t byte_word:1,
		uint32_t write_back:1,
		uint32_t load_store:1,
		uint32_t rn:4,
		uint32_t rd:4,
		uint32_t offset:12,
		) sdt;

	/* swap */
	PACKED_BITFIELD(
		uint32_t cond:4,
		uint32_t is00010:5,
		uint32_t b:1,
		uint32_t is00:2,
		uint32_t rn:4,
		uint32_t rd:4,
		uint32_t is00001001:8,
		uint32_t rm:4,
		) swp;

	/* atomic */
	PACKED_BITFIELD(
		uint32_t cond:4,
		uint32_t mode:8,
		uint32_t rn:4,
		uint32_t rd:4,
		uint32_t foo:4,
		uint32_t is1001:4,
		uint32_t rm:4,
		) atomic;

	/* coproc data operation */
	PACKED_BITFIELD(
		uint32_t cond:4,
		uint32_t is1110:4,
		uint32_t opcode1:3,
		uint32_t l:1,
		uint32_t crn:4,
		uint32_t rd:4,
		uint32_t cp_no:4,
		uint32_t opcode2:3,
		uint32_t move:1,
		uint32_t crm:4,
		) coproc;

	/* half-word and signed data transfert */
	PACKED_BITFIELD(
		uint32_t cond:4,
		uint32_t is000:3,
		uint32_t pre_post:1,
		uint32_t up_down:1,
		uint32_t immediate:1,
		uint32_t write_back:1,
		uint32_t load_store:1,
		uint32_t rn:4,
		uint32_t rd:4,
		uint32_t im_up:4,
		uint32_t is_1_0:1,
		uint32_t s:1,
		uint32_t h:1,
		uint32_t is_1_1:1,
		uint32_t rm:4,
		) sdth;

	// not used yet

	/* single data swap */
	PACKED_BITFIELD(
		uint32_t cond:4,
		uint32_t is00010:5,
		uint32_t byte_word:1,
		uint32_t is00:1,
		uint32_t rn:4,
		uint32_t rd:4,
		uint32_t is00001001:8,
		uint32_t rm:4,
		) sds;
	/* undefined */
	PACKED_BITFIELD(
		uint32_t cond:4,
		uint32_t is011:3,
		uint32_t dummy1:20,
		uint32_t is1:1,
		uint32_t dummy2:4,
		) und;
	/* coproc data transfert */
	PACKED_BITFIELD(
		uint32_t cond:4,
		uint32_t is110:3,
		uint32_t pre_post:1,
		uint32_t up_down:1,
		uint32_t length:1,
		uint32_t write_back:1,
		uint32_t load_store:1,
		uint32_t rn:4,
		uint32_t crd:4,
		uint32_t cp_:4,
		uint32_t offset:8,
		) cdt;
	/* software interrupt */
	PACKED_BITFIELD(
		uint32_t cond:4,
		uint32_t opcode:4,
		uint32_t dummy:24,
		) si;
	/* setend */
	PACKED_BITFIELD(
		uint32_t is111100010000:12,
		uint32_t dummy1:3,
		uint32_t is_setend:1,
		uint32_t dummy2:6,
		uint32_t be:1,
		uint32_t dummy3:9,
		) setend;

	uint32_t ins;
}  arm_ins_t;
