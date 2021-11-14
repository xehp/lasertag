/*
cmd.h

Provide command line functions.

Copyright (C) 2021 Henrik Bjorkman www.eit.se/hb.

History
2021-05-01 Created. /Henrik
*/


#ifndef MSG_H
#define MSG_H


// First bits in all messages shall be one of these
// NOTE It is 4 bits, only numbers 0-15 are available, see also MSG_CATEGORY_SIZE_NOF_BITS
// Before this we should have had a protocol version field?
// Or just more bits for category so we can have more versions of each category.
enum
{
	msg_category_ignore = 0,
	msg_category_log = 2,
	msg_category_reply_nok = 6,
	msg_category_game = 8,
	msg_category_status = 10,
	msg_category_cmd = 12,
	msg_category_reply_ok = 14,
};

// One of these shall follow if "msg_category_cmd" or "msg_category_reply"
// NOTE It is 4 bits, only numbers 0-15 are available, see also MSG_SUBCMD_SIZE_NOF_BITS
enum
{
	cmd_set = 1,
	cmd_get,
	cmd_power_off,
	cmd_ping,
	cmd_save,
	cmd_reboot,
};

// One of these shall follow if "msg_category_status"
// see also MSG_SUBCMD_SIZE_NOF_BITS
enum
{
	status_hello = 1,
};


// See also translate_to_ir_code. If that function is
// changed this number may need to be adjusted.
#define NOF_AVAILABLE_PLAYER_CODES 22

// See also translate_to_ir_pairing_code.
#define NOF_AVAILABLE_PAIRING_CODES 64


#define PARING_CODE_MASK (NOF_AVAILABLE_PAIRING_CODES-1)

// Codes used for IR signaling
// See also translate_to_ir_code & translate_from_ir_code
enum
{
	red_team_safe_zone = NOF_AVAILABLE_PLAYER_CODES,
	blue_team_safe_zone,
	attention_to_radio_master, // Radio message from master will follow.
	spare_ir_code,
};

enum
{
	unknown_team = 0,
	red_team = 1,
	blue_team = 2,
};




// The RADIO_PAYLOAD_MAX_LEN is currently 16 bytes
// so the size of a message needs to be max 16 bytes.
// Sequence number only need 4-5 bits.
// Perhaps take some bits from sequence number for other things?
#define MSG_CATEGORY_SIZE_NOF_BITS 4
#define MSG_SUBCMD_SIZE_NOF_BITS (8-MSG_CATEGORY_SIZE_NOF_BITS)
#define SEQUENCE_NUMBER_NOF_BITS 8
#define PAR_ID_NOF_BITS 8
#define HDR_SPARE_NOF_BITS 8
#define ADDR_NR_NOF_BITS 32
#define VALUE_NOF_BITS 64
#define FREQUENCY_NOF_BITS 16
#define DURATION_NOF_BITS 32
#define VOLUME_NOF_BITS 8
#define STATUS_SUB_CAT_NOF_BITS 8
#define SERIAL64_NR_NOF_BITS 64
#define IR_CODE_NR_NOF_BITS 8
#define TIME_S_NOF_BITS 16
#define PAIR_NR_NOF_BITS 16

#endif
