#include <stdio.h>

//just c&p from network.cpp
static int m_packetLen[256] =
{
// 0..15
	0x0068,	0x0005,	0x0007,	0x0000,	0x0002,	0x0005,	0x0005,	0x0007,	0x000E,	0x0005,	0x000B,	0x010A,	0x0000,	0x0003,	0x0000,	0x003D,
//16..31
	0x00D7,	0x0000,	0x0000,	0x000A,	0x0006,	0x0009,	0x0001,	0x0000,	0x0000,	0x0000,	0x0000,	0x0025,	0x0000,	0x0005,	0x0004,	0x0008,
//32..47
	0x0013,	0x0008,	0x0003,	0x001A,	0x0007,	0x0014,	0x0005,	0x0002,	0x0005,	0x0001,	0x0005,	0x0002,	0x0002,	0x0011,	0x000F,	0x000A,
//48..63
	0x0005,	0x0001,	0x0002,	0x0002,	0x000A,	0x028D,	0x0000,	0x0008,	0x0007,	0x0009,	0x0000,	0x0000,	0x0000,	0x0002,	0x0025,	0x0000,
//64..79
	0x00C9,	0x0000,	0x0000,	0x0229,	0x02C9,	0x0005,	0x0000,	0x000B,	0x0049,	0x005D, 0x0005,	0x0009,	0x0000,	0x0000,	0x0006,	0x0002,
//80..95
	0x0000,	0x0000,	0x0000,	0x0002,	0x000C,	0x0001,	0x000B,	0x006E,	0x006A,	0x0000,	0x0000,	0x0004,	0x0002,	0x0049,	0x0000,	0x0031,
//96..111
	0x0005,	0x0009,	0x000F,	0x000D,	0x0001,	0x0004,	0x0000,	0x0015,	0x0000,	0x0000,	0x0003,	0x0009,	0x0013,	0x0003,	0x000E,	0x0000,
//112..127
	0x001C,	0x0000,	0x0005,	0x0002,	0x0000,	0x0023,	0x0010,	0x0011,	0x0000,	0x0009,	0x0000,	0x0002,	0x0000,	0x000D,	0x0002,	0x0000,
//128..143
	0x003E,	0x0000,	0x0002,	0x0027,	0x0045,	0x0002,	0x0000,	0x0000,	0x0042,	0x0000,	0x0000,	0x0000,	0x000B,	0x0000,	0x0000,	0x0000,
//144..159
	0x0013,	0x0041,	0x0000,	0x0063,	0x0000,	0x0009,	0x0000,	0x0002,	0x0000,	0x001A,	0x0000,	0x0102,	0x0135,	0x0033,	0x0000,	0x0000,
//160..175
	0x0003,	0x0009,	0x0009,	0x0009,	0x0095,	0x0000,	0x0000,	0x0004,	0x0000,	0x0000,	0x0005,	0x0000,	0x0000,	0x0000,	0x0000,	0x000D,
//176..193
	0x0000,	0x0000,	0x0000,	0x0000,	0x0000,	0x0040,	0x0009,	0x0000,	0x0000,	0x0003,	0x0006,	0x0009,	0x0003,	0x0000,	0x0000,	0x0000,
//192..207
	0x0024,	0x0000,	0x0000,	0x0000,	0x0006,	0x00CB,	0x0001,	0x0031,	0x0002,	0x0006,	0x0006,	0x0007,	0x0000,	0x0001,	0x0000,	0x004E,
//208..223
	0x0000,	0x0002,	0x0019,	0x0000,	0x0000,	0xFFFF,	0xFFFF,	0xFFFF,	0xFFFF,	0xFFFF,	0xFFFF,	0xFFFF,	0xFFFF,	0xFFFF,	0xFFFF,	0xFFFF,
//224..239
	0xFFFF,	0xFFFF,	0xFFFF,	0xFFFF,	0xFFFF,	0xFFFF,	0xFFFF,	0xFFFF,	0xFFFF,	0xFFFF,	0xFFFF,	0xFFFF,	0xFFFF,	0xFFFF,	0xFFFF,	0xFFFF,
//240..255
	0x0000,	0xFFFF,	0xFFFF,	0xFFFF,	0xFFFF,	0xFFFF,	0xFFFF,	0xFFFF,	0xFFFF,	0xFFFF,	0xFFFF,	0xFFFF,	0xFFFF,	0xFFFF,	0xFFFF,	0xFFFF
};

void name(unsigned char index)
{
	switch(index)
	{
		case 0x00: printf("CreateChar"); break;
		case 0x01: printf("DisconnectNotify"); break;
		case 0x02: printf("MoveRequest"); break;
		case 0x03: printf("TalkRequest"); break;
		case 0x05: printf("AttackRequest"); break;
		case 0x06: printf("Doubleclick"); break;
		case 0x07: printf("PickUp"); break;
		case 0x08: printf("DropItem"); break;
		case 0x09: printf("Singleclick"); break;
		case 0x12: printf("ActionRequest"); break;
		case 0x13: printf("WearItem"); break;
		case 0x22: printf("MoveACK_ResyncReq"); break;
		case 0x2c: printf("RessChoice"); break;
		case 0x34: printf("StatusRequest"); break;
		case 0x3a: printf("SetSkillLock"); break;
		case 0x3b: printf("BuyItems"); break;
		case 0x56: printf("MapPlotCourse"); break;
		case 0x5d: printf("LoginChar"); break;
		case 0x66: printf("BookPage"); break;
		case 0x6c: printf("TargetSelected"); break;
		case 0x6f: printf("SecureTrade"); break;
		case 0x71: printf("BBoardMessage"); break;
		case 0x72: printf("WarModeChange"); break;
		case 0x73: printf("Ping"); break;
		case 0x75: printf("RenameCharacter"); break;
		case 0x7d: printf("DialogResponse"); break;
		case 0x80: printf("LoginRequest"); break;
		case 0x83: printf("DeleteCharacter"); break;
		case 0x91: printf("GameServerLogin"); break;
		case 0x93: printf("BookUpdateTitle"); break;
		case 0x95: printf("DyeItem"); break;
		case 0x9b: printf("RequestHelp"); break;
		case 0x9f: printf("SellItems"); break;
		case 0xa0: printf("SelectServer"); break;
		case 0xa7: printf("TipsRequest"); break;
		case 0xac: printf("GumpTextDialogReply"); break;
		case 0xad: printf("UnicodeSpeechReq"); break;
		case 0xb1: printf("GumpResponse"); break;
		case 0xb2: printf("ChatMessage"); break;
		case 0xb5: printf("ChatWindowOpen"); break;
		case 0xb6: printf("PopupHelpRequest"); break;
		case 0xb8: printf("CharProfileRequest"); break;
		case 0xbd: printf("ClientVersion"); break;
		case 0xbe: printf("AssistVersion"); break;
		case 0xbf: printf("MiscCommand"); break;
		case 0xc2: printf("TextEntryUnicode"); break;
		case 0xc8: printf("ClientViewRange"); break;
		case 0xd1: printf("LogoutStatus"); break;
		case 0xd4: printf("NewBookHeader"); break;
		case 0xd7: printf("FightBookSelection"); break;
	}
}

int main()
{
	for(int i = 0; i < 256; i++)
	{
		printf("%02x %d\t", i, m_packetLen[i]);
		name(i);
		printf("\n");
	}
}