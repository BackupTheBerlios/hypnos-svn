#include <stdio.h>

static unsigned int bit_table[257][2] =
{
{0x0002, 0x0000}, {0x0005, 0x001F}, {0x0006, 0x0022}, {0x0007, 0x0034}, {0x0007, 0x0075}, {0x0006, 0x0028}, {0x0006, 0x003B}, {0x0007, 0x0032},
{0x0008, 0x00E0}, {0x0008, 0x0062}, {0x0007, 0x0056}, {0x0008, 0x0079}, {0x0009, 0x019D}, {0x0008, 0x0097}, {0x0006, 0x002A}, {0x0007, 0x0057},
{0x0008, 0x0071}, {0x0008, 0x005B}, {0x0009, 0x01CC}, {0x0008, 0x00A7}, {0x0007, 0x0025}, {0x0007, 0x004F}, {0x0008, 0x0066}, {0x0008, 0x007D},
{0x0009, 0x0191}, {0x0009, 0x01CE}, {0x0007, 0x003F}, {0x0009, 0x0090}, {0x0008, 0x0059}, {0x0008, 0x007B}, {0x0008, 0x0091}, {0x0008, 0x00C6},
{0x0006, 0x002D}, {0x0009, 0x0186}, {0x0008, 0x006F}, {0x0009, 0x0093}, {0x000A, 0x01CC}, {0x0008, 0x005A}, {0x000A, 0x01AE}, {0x000A, 0x01C0},
{0x0009, 0x0148}, {0x0009, 0x014A}, {0x0009, 0x0082}, {0x000A, 0x019F}, {0x0009, 0x0171}, {0x0009, 0x0120}, {0x0009, 0x00E7}, {0x000A, 0x01F3},
{0x0009, 0x014B}, {0x0009, 0x0100}, {0x0009, 0x0190}, {0x0006, 0x0013}, {0x0009, 0x0161}, {0x0009, 0x0125}, {0x0009, 0x0133}, {0x0009, 0x0195},
{0x0009, 0x0173}, {0x0009, 0x01CA}, {0x0009, 0x0086}, {0x0009, 0x01E9}, {0x0009, 0x00DB}, {0x0009, 0x01EC}, {0x0009, 0x008B}, {0x0009, 0x0085},
{0x0005, 0x000A}, {0x0008, 0x0096}, {0x0008, 0x009C}, {0x0009, 0x01C3}, {0x0009, 0x019C}, {0x0009, 0x008F}, {0x0009, 0x018F}, {0x0009, 0x0091},
{0x0009, 0x0087}, {0x0009, 0x00C6}, {0x0009, 0x0177}, {0x0009, 0x0089}, {0x0009, 0x00D6}, {0x0009, 0x008C}, {0x0009, 0x01EE}, {0x0009, 0x01EB},
{0x0009, 0x0084}, {0x0009, 0x0164}, {0x0009, 0x0175}, {0x0009, 0x01CD}, {0x0008, 0x005E}, {0x0009, 0x0088}, {0x0009, 0x012B}, {0x0009, 0x0172},
{0x0009, 0x010A}, {0x0009, 0x008D}, {0x0009, 0x013A}, {0x0009, 0x011C}, {0x000A, 0x01E1}, {0x000A, 0x01E0}, {0x0009, 0x0187}, {0x000A, 0x01DC},
{0x000A, 0x01DF}, {0x0007, 0x0074}, {0x0009, 0x019F}, {0x0008, 0x008D}, {0x0008, 0x00E4}, {0x0007, 0x0079}, {0x0009, 0x00EA}, {0x0009, 0x00E1},
{0x0008, 0x0040}, {0x0007, 0x0041}, {0x0009, 0x010B}, {0x0009, 0x00B0}, {0x0008, 0x006A}, {0x0008, 0x00C1}, {0x0007, 0x0071}, {0x0007, 0x0078},
{0x0008, 0x00B1}, {0x0009, 0x014C}, {0x0007, 0x0043}, {0x0008, 0x0076}, {0x0007, 0x0066}, {0x0007, 0x004D}, {0x0009, 0x008A}, {0x0006, 0x002F},
{0x0008, 0x00C9}, {0x0009, 0x00CE}, {0x0009, 0x0149}, {0x0009, 0x0160}, {0x000A, 0x01BA}, {0x000A, 0x019E}, {0x000A, 0x039F}, {0x0009, 0x00E5},
{0x0009, 0x0194}, {0x0009, 0x0184}, {0x0009, 0x0126}, {0x0007, 0x0030}, {0x0008, 0x006C}, {0x0009, 0x0121}, {0x0009, 0x01E8}, {0x000A, 0x01C1},
{0x000A, 0x011D}, {0x000A, 0x0163}, {0x000A, 0x0385}, {0x000A, 0x03DB}, {0x000A, 0x017D}, {0x000A, 0x0106}, {0x000A, 0x0397}, {0x000A, 0x024E},
{0x0007, 0x002E}, {0x0008, 0x0098}, {0x000A, 0x033C}, {0x000A, 0x032E}, {0x000A, 0x01E9}, {0x0009, 0x00BF}, {0x000A, 0x03DF}, {0x000A, 0x01DD},
{0x000A, 0x032D}, {0x000A, 0x02ED}, {0x000A, 0x030B}, {0x000A, 0x0107}, {0x000A, 0x02E8}, {0x000A, 0x03DE}, {0x000A, 0x0125}, {0x000A, 0x01E8},
{0x0009, 0x00E9}, {0x000A, 0x01CD}, {0x000A, 0x01B5}, {0x0009, 0x0165}, {0x000A, 0x0232}, {0x000A, 0x02E1}, {0x000B, 0x03AE}, {0x000B, 0x03C6},
{0x000B, 0x03E2}, {0x000A, 0x0205}, {0x000A, 0x029A}, {0x000A, 0x0248}, {0x000A, 0x02CD}, {0x000A, 0x023B}, {0x000B, 0x03C5}, {0x000A, 0x0251},
{0x000A, 0x02E9}, {0x000A, 0x0252}, {0x0009, 0x01EA}, {0x000B, 0x03A0}, {0x000B, 0x0391}, {0x000A, 0x023C}, {0x000B, 0x0392}, {0x000B, 0x03D5},
{0x000A, 0x0233}, {0x000A, 0x02CC}, {0x000B, 0x0390}, {0x000A, 0x01BB}, {0x000B, 0x03A1}, {0x000B, 0x03C4}, {0x000A, 0x0211}, {0x000A, 0x0203},
{0x0009, 0x012A}, {0x000A, 0x0231}, {0x000B, 0x03E0}, {0x000A, 0x029B}, {0x000B, 0x03D7}, {0x000A, 0x0202}, {0x000B, 0x03AD}, {0x000A, 0x0213},
{0x000A, 0x0253}, {0x000A, 0x032C}, {0x000A, 0x023D}, {0x000A, 0x023F}, {0x000A, 0x032F}, {0x000A, 0x011C}, {0x000A, 0x0384}, {0x000A, 0x031C},
{0x000A, 0x017C}, {0x000A, 0x030A}, {0x000A, 0x02E0}, {0x000A, 0x0276}, {0x000A, 0x0250}, {0x000B, 0x03E3}, {0x000A, 0x0396}, {0x000A, 0x018F},
{0x000A, 0x0204}, {0x000A, 0x0206}, {0x000A, 0x0230}, {0x000A, 0x0265}, {0x000A, 0x0212}, {0x000A, 0x023E}, {0x000B, 0x03AC}, {0x000B, 0x0393},
{0x000B, 0x03E1}, {0x000A, 0x01DE}, {0x000B, 0x03D6}, {0x000A, 0x031D}, {0x000B, 0x03E5}, {0x000B, 0x03E4}, {0x000A, 0x0207}, {0x000B, 0x03C7},
{0x000A, 0x0277}, {0x000B, 0x03D4}, {0x0008, 0x00C0}, {0x000A, 0x0162}, {0x000A, 0x03DA}, {0x000A, 0x0124}, {0x000A, 0x01B4}, {0x000A, 0x0264},
{0x000A, 0x033D}, {0x000A, 0x01D1}, {0x000A, 0x01AF}, {0x000A, 0x039E}, {0x000A, 0x024F}, {0x000B, 0x0373}, {0x000A, 0x0249}, {0x000B, 0x0372},
{0x0009, 0x0167}, {0x000A, 0x0210}, {0x000A, 0x023A}, {0x000A, 0x01B8}, {0x000B, 0x03AF}, {0x000A, 0x018E}, {0x000A, 0x02EC}, {0x0007, 0x0062},
{0x0004, 0x000D}
};

typedef struct uopnode {
	unsigned char code; /* Coded byte */
	unsigned char set;
	struct uopnode *zero;
	struct uopnode *one;
} uopnode_t;

uopnode_t *root;

void check_node(uopnode_t *n, int level)
{
	if ( ! n ) return;
	
	if ( !( (n->zero && n->one) || (!n->zero && !n->one) ) )
	{
		int i;
		for(i = 0; i < level; i++)
			fprintf(stderr, "\t");
	
		fprintf(stderr, "Error in node %x (%x) [%08x %08x]\n", n->code, n->set, bit_table[n->code][0], bit_table[n->code][1]);
	}
	
	check_node(n->zero, level+1); check_node(n->one, level+1);
}

int main()
{
	int i, j;
	root = (uopnode_t*)malloc(sizeof(uopnode_t));
	root->one = NULL;
	root->zero = NULL;
	root->set = 0;
	for(i = 0; i < 256; i++)
	{
		//unsigned char shift = 32 - bit_table[i][0];
		unsigned char shift = 0;
		
		uopnode_t *curr = root;
		fprintf(stderr, "%02x %04x %d ", i, bit_table[i][1], bit_table[i][0]);
		for(j = bit_table[i][0]-1; j >= 0 ; j--)
		{
			fprintf(stderr, "%d", (bit_table[i][1] >> (shift+j)) & 0x1);
			if ( (bit_table[i][1] >> (shift+j)) & 0x1 )
			{
				if ( ! curr->one )
				{
					curr->one = (uopnode_t*)malloc(sizeof(uopnode_t));
					curr->one->one = NULL;
					curr->one->zero = NULL;
					curr->one->set = 0;
				}
				
				curr = curr->one;
			} else {
				if ( ! curr->zero )
				{
					curr->zero = (uopnode_t*)malloc(sizeof(uopnode_t));
					curr->zero->one = NULL;
					curr->zero->zero = NULL;
					curr->zero->set = 0;
				}
				
				curr = curr->zero;
			}
		}
		
		fprintf(stderr, "\n");
		
		curr->code = i;
		curr->set = 1;
	}
	
	//check_node(root,0);
	
	FILE *f = fopen("huffpacket", "r");
	unsigned char buffer[2048];
	
	int read = fread(buffer, 1, 2048, f);
		
	uopnode_t *curr = root;
	for(i = 0; i < read; i++)
	{
		fprintf(stderr, "[%02x]", buffer[i]);
		for(j = 7; j >= 0; j--)
		{
			unsigned char prevcode = curr->code;
			if ( (buffer[i] >> j)&0x1 )
			{
				curr = curr->one;
			} else {
				curr = curr->zero;
			}
			
			if ( ! curr )
			{
				if ( (buffer[i] >> j)&0x1 )
				{
					curr = root->one;
				} else {
					curr = root->zero;
				}
				
				if ( ! curr )
				{
					fprintf(stderr, "%d %d", i, j);
					return;
				}
				
				fprintf(stderr, "\n%d", (buffer[i] >> j)&0x1);
				fprintf(stdout, "%c", prevcode);
			} else
				fprintf(stderr, "%d", (buffer[i] >> j)&0x1); 
		}
		
	}
	
	fclose(f);
}
