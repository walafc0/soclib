/* -*- c++ -*- */
/** thumb instruction format union */
typedef union {

	PACKED_BITFIELD(
		uint16_t decod0:10,
		uint16_t rm:3,
		uint16_t rd:3,
		) reg2;

	PACKED_BITFIELD(
		uint16_t decod0:7,
		uint16_t imm:3,
		uint16_t rn:3,
		uint16_t rd:3,
		) imm3;

	PACKED_BITFIELD(
		uint16_t decod0:7,
		uint16_t rm:3,
		uint16_t rn:3,
		uint16_t rd:3,
		) reg3;

	PACKED_BITFIELD(
		uint16_t decod0:4,
		uint16_t mode:3,
		uint16_t rm:3,
		uint16_t rn:3,
		uint16_t rd:3,
		) ldst;

	PACKED_BITFIELD(
		uint16_t decod0:8,
		uint16_t h1:1,
		uint16_t h2:1,
		uint16_t rm:3,
		uint16_t rd:3,
		) hilo;

	PACKED_BITFIELD(
		uint16_t decod0:5,
		uint16_t rd:3,
		uint16_t imm:8,
		) imm8;

	PACKED_BITFIELD(
		uint16_t decod0:5,
		uint16_t imm:5,
		uint16_t rn:3,
		uint16_t rd:3,
		) imm5;

	PACKED_BITFIELD(
		uint16_t decod0:9,
		uint16_t imm:7,
		) imm7;

	PACKED_BITFIELD(
		uint16_t decod0:5,
		uint16_t imm:11,
		) imm11;

	PACKED_BITFIELD(
		uint16_t decod0:4,
		uint16_t cond:4,
		uint16_t imm:8,
		) cond;

	PACKED_BITFIELD(
		uint16_t decod0:10,
		uint16_t is_cps:1,
		uint16_t imod:1,
		uint16_t end:1,
		uint16_t a:1,
		uint16_t i:1,
		uint16_t f:1,
		) cps;

	PACKED_BITFIELD(
		uint16_t decod0:5,
		uint16_t rn:3,
        uint16_t regs:8,
		) ldstm;

	PACKED_BITFIELD(
		uint16_t decod0:7,
		uint16_t r:1,
        uint16_t regs:8,
		) push_pop;

	PACKED_BITFIELD(
		uint16_t decod0:12,
		uint16_t end:1,
        uint16_t zero:3,
		) setend;

	uint16_t ins;
}  thumb_ins_t;
