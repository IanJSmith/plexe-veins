/* Crankshaft, TDH

   TDH has one problem: sequentially numbered nodes will follow the same, but
   staggered schedule. This means that there will be periods where nodes all
   wake up, and times where all nodes are asleep. Therefore we first hash the
   node ID, and use the hashed version and the slot number to assign slots.
*/

#include "crankshaftt1.h"

Define_Module_Like( CrankshaftT1, EyesMacLayer );

static unsigned int hashLower[256] = {
0x0000,0xBB43,0xA529,0x1E6A,0xC13C,0x7A7F,0x6415,0xDF56,0x6575,0xDE36,
0xC05C,0x7B1F,0xA449,0x1F0A,0x0160,0xBA23,0x15BE,0xAEFD,0xB097,0x0BD4,
0xD482,0x6FC1,0x71AB,0xCAE8,0x70CB,0xCB88,0xD5E2,0x6EA1,0xB1F7,0x0AB4,
0x14DE,0xAF9D,0x26BB,0x9DF8,0x8392,0x38D1,0xE787,0x5CC4,0x42AE,0xF9ED,
0x43CE,0xF88D,0xE6E7,0x5DA4,0x82F2,0x39B1,0x27DB,0x9C98,0x3305,0x8846,
0x962C,0x2D6F,0xF239,0x497A,0x5710,0xEC53,0x5670,0xED33,0xF359,0x481A,
0x974C,0x2C0F,0x3265,0x8926,0x57B1,0xECF2,0xF298,0x49DB,0x968D,0x2DCE,
0x33A4,0x88E7,0x32C4,0x8987,0x97ED,0x2CAE,0xF3F8,0x48BB,0x56D1,0xED92,
0x420F,0xF94C,0xE726,0x5C65,0x8333,0x3870,0x261A,0x9D59,0x277A,0x9C39,
0x8253,0x3910,0xE646,0x5D05,0x436F,0xF82C,0x710A,0xCA49,0xD423,0x6F60,
0xB036,0x0B75,0x151F,0xAE5C,0x147F,0xAF3C,0xB156,0x0A15,0xD543,0x6E00,
0x706A,0xCB29,0x64B4,0xDFF7,0xC19D,0x7ADE,0xA588,0x1ECB,0x00A1,0xBBE2,
0x01C1,0xBA82,0xA4E8,0x1FAB,0xC0FD,0x7BBE,0x65D4,0xDE97,0x84E9,0x3FAA,
0x21C0,0x9A83,0x45D5,0xFE96,0xE0FC,0x5BBF,0xE19C,0x5ADF,0x44B5,0xFFF6,
0x20A0,0x9BE3,0x8589,0x3ECA,0x9157,0x2A14,0x347E,0x8F3D,0x506B,0xEB28,
0xF542,0x4E01,0xF422,0x4F61,0x510B,0xEA48,0x351E,0x8E5D,0x9037,0x2B74,
0xA252,0x1911,0x077B,0xBC38,0x636E,0xD82D,0xC647,0x7D04,0xC727,0x7C64,
0x620E,0xD94D,0x061B,0xBD58,0xA332,0x1871,0xB7EC,0x0CAF,0x12C5,0xA986,
0x76D0,0xCD93,0xD3F9,0x68BA,0xD299,0x69DA,0x77B0,0xCCF3,0x13A5,0xA8E6,
0xB68C,0x0DCF,0xD358,0x681B,0x7671,0xCD32,0x1264,0xA927,0xB74D,0x0C0E,
0xB62D,0x0D6E,0x1304,0xA847,0x7711,0xCC52,0xD238,0x697B,0xC6E6,0x7DA5,
0x63CF,0xD88C,0x07DA,0xBC99,0xA2F3,0x19B0,0xA393,0x18D0,0x06BA,0xBDF9,
0x62AF,0xD9EC,0xC786,0x7CC5,0xF5E3,0x4EA0,0x50CA,0xEB89,0x34DF,0x8F9C,
0x91F6,0x2AB5,0x9096,0x2BD5,0x35BF,0x8EFC,0x51AA,0xEAE9,0xF483,0x4FC0,
0xE05D,0x5B1E,0x4574,0xFE37,0x2161,0x9A22,0x8448,0x3F0B,0x8528,0x3E6B,
0x2001,0x9B42,0x4414,0xFF57,0xE13D,0x5A7E,
};

static unsigned int hashUpper[256] = {
0x0000,0x3148,0x4764,0x762C,0x471C,0x7654,0x0078,0x3130,0xC564,0xF42C,
0x8200,0xB348,0x8278,0xB330,0xC51C,0xF454,0x3E5A,0x0F12,0x793E,0x4876,
0x7946,0x480E,0x3E22,0x0F6A,0xFB3E,0xCA76,0xBC5A,0x8D12,0xBC22,0x8D6A,
0xFB46,0xCA0E,0xB15E,0x8016,0xF63A,0xC772,0xF642,0xC70A,0xB126,0x806E,
0x743A,0x4572,0x335E,0x0216,0x3326,0x026E,0x7442,0x450A,0x8F04,0xBE4C,
0xC860,0xF928,0xC818,0xF950,0x8F7C,0xBE34,0x4A60,0x7B28,0x0D04,0x3C4C,
0x0D7C,0x3C34,0x4A18,0x7B50,0x234B,0x1203,0x642F,0x5567,0x6457,0x551F,
0x2333,0x127B,0xE62F,0xD767,0xA14B,0x9003,0xA133,0x907B,0xE657,0xD71F,
0x1D11,0x2C59,0x5A75,0x6B3D,0x5A0D,0x6B45,0x1D69,0x2C21,0xD875,0xE93D,
0x9F11,0xAE59,0x9F69,0xAE21,0xD80D,0xE945,0x9215,0xA35D,0xD571,0xE439,
0xD509,0xE441,0x926D,0xA325,0x5771,0x6639,0x1015,0x215D,0x106D,0x2125,
0x5709,0x6641,0xAC4F,0x9D07,0xEB2B,0xDA63,0xEB53,0xDA1B,0xAC37,0x9D7F,
0x692B,0x5863,0x2E4F,0x1F07,0x2E37,0x1F7F,0x6953,0x581B,0x813B,0xB073,
0xC65F,0xF717,0xC627,0xF76F,0x8143,0xB00B,0x445F,0x7517,0x033B,0x3273,
0x0343,0x320B,0x4427,0x756F,0xBF61,0x8E29,0xF805,0xC94D,0xF87D,0xC935,
0xBF19,0x8E51,0x7A05,0x4B4D,0x3D61,0x0C29,0x3D19,0x0C51,0x7A7D,0x4B35,
0x3065,0x012D,0x7701,0x4649,0x7779,0x4631,0x301D,0x0155,0xF501,0xC449,
0xB265,0x832D,0xB21D,0x8355,0xF579,0xC431,0x0E3F,0x3F77,0x495B,0x7813,
0x4923,0x786B,0x0E47,0x3F0F,0xCB5B,0xFA13,0x8C3F,0xBD77,0x8C47,0xBD0F,
0xCB23,0xFA6B,0xA270,0x9338,0xE514,0xD45C,0xE56C,0xD424,0xA208,0x9340,
0x6714,0x565C,0x2070,0x1138,0x2008,0x1140,0x676C,0x5624,0x9C2A,0xAD62,
0xDB4E,0xEA06,0xDB36,0xEA7E,0x9C52,0xAD1A,0x594E,0x6806,0x1E2A,0x2F62,
0x1E52,0x2F1A,0x5936,0x687E,0x132E,0x2266,0x544A,0x6502,0x5432,0x657A,
0x1356,0x221E,0xD64A,0xE702,0x912E,0xA066,0x9156,0xA01E,0xD632,0xE77A,
0x2D74,0x1C3C,0x6A10,0x5B58,0x6A68,0x5B20,0x2D0C,0x1C44,0xE810,0xD958,
0xAF74,0x9E3C,0xAF0C,0x9E44,0xE868,0xD920,
};

#define HASH(x) (hashUpper[((x) >> 8) & 0xff] ^ hashLower[(x) & 0xff])

unsigned int CrankshaftT1::broadcastThreshold, CrankshaftT1::threshold;
bool CrankshaftT1::parametersInitialised = false;

void CrankshaftT1::initialize() {
	/* Max header contains from, type, to, and clock data */
	max_header_length = ADDRESS_BYTES + TYPE_BYTES + ADDRESS_BYTES + CLOCK_BYTES;
	/* Min header contains from, type, and clock data */
	min_header_length = ADDRESS_BYTES + TYPE_BYTES + CLOCK_BYTES;
	
	CrankshaftBase::initialize();

	if (!parametersInitialised) {
		parametersInitialised = true;
		threshold = (int) (getDoubleParameter("threshold", 0.94117647058824) * 65536); // (1/17)
		broadcastThreshold = (unsigned int) (getDoubleParameter("broadcastThreshold", 0.94117647058824) * 65536); // (1/17)
	}
}

void CrankshaftT1::finish() {
	CrankshaftBase::finish();
}

CrankshaftT1::~CrankshaftT1() {
	parametersInitialised = false;
}

void CrankshaftT1::wrapSlotCounter() {
	current_slot &= 0xffff;
}

CrankshaftBase::SlotState CrankshaftT1::getCurrentSlotState() {
	/* Check for broadcast slot. */
	if (HASH(current_slot + 0xffff) > broadcastThreshold)
		return tx_msg && tx_msg->local_to == BROADCAST ? SSTATE_SEND_RECEIVE : SSTATE_RECEIVE;

	/* Check if it is one of my slots. */
	if (HASH(current_slot + HASH(macid())) > threshold) {
		/* Listening in this slot. */
		printf("listening");
		return SSTATE_RECEIVE;
	}

	/* Check if we have a message for a node that is awake in this slot. */
	if (tx_msg && tx_msg->local_to != BROADCAST && HASH(current_slot + HASH(tx_msg->local_to)) > threshold)
		return SSTATE_SEND;

#ifdef SINK_ALWAYS_ON // Assumes that node 0 is sink!
	if (tx_msg && tx_msg->local_to == 0)
		return SSTATE_SEND;

	if (macid() == 0)
		return SSTATE_RECEIVE;
#endif

	return SSTATE_SLEEP;
}

int CrankshaftT1::slotsUntilWake(int destination) {
	int myHash = HASH(macid());
	int destinationHash = HASH(destination);
	int i;

	for (i = 0; i < 0xffff; i++) {
		if (HASH(((current_slot + i + 1) & 0xffff) + 0xffff) > broadcastThreshold)
			continue;
		if (HASH(((current_slot + i + 1) & 0xffff) + myHash) > threshold)
			continue;
		if (HASH(((current_slot + i + 1) & 0xffff) + destinationHash) > threshold)
			break;
	}
	if (i == 0xffff) {
		assert(0);
		return INT_MAX;
	}
	return i + 1;
}
