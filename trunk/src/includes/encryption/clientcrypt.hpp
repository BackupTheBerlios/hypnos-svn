/*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*
| Hypnos UO Server Emulator                                                |
|                                                                          |
| This software is free software released under GPL2 license.              |
| You can find detailed license information in hypnos.cpp file.            |
|                                                                          |
*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*/

#ifndef __CLIENTCRYPT_H__
#define __CLIENTCRYPT_H__
#include "../common_libs.hpp"
#include "../constants.hpp"
#include "crypt.hpp"

enum { CRYPT_NONE, CRYPT_LOGIN, CRYPT_GAME } ;

const int loginKeys [10] [2]  = {
	{ 0x32750719, 0x0a2d100b } // crypt 1.26.4
	, {0x2d13a5fd, 0xa39d527f } // crypt 2.0.0
	, {0x2dbbb7cd, 0xa3c95e7f } // crypt 2.0.3
	, {0x2d93a5fd, 0xa3dd527f } // crypt 3.0.0c
	, {0x2daba7ed, 0xa3c17e7f } // crypt 3.0.1
	, {0x2de3addd, 0xa3e5227f } // crypt 3.0.2
	, {0x2d3bb7cd, 0xa3895e7f } // crypt 3.0.3a
	, {0x2d7385bd, 0xa3ad127f } // crypt 3.0.4p
	, {0x2c8b97ad, 0xa350de7f } // crypt 3.0.5
	, {0x2cc3ed9d, 0xa374227f } // crypt 3.0.6j
};

enum {
	  CRYPT_UNENCRYPTED
	, CRYPT_1_26_4
	, CRYPT_2_0_0
	, CRYPT_2_0_3
	, CRYPT_3_0_0c
	, CRYPT_3_0_1
	, CRYPT_3_0_2
	, CRYPT_3_0_3a
	, CRYPT_3_0_4p
	, CRYPT_3_0_5
	, CRYPT_3_0_6j
};


class ClientCrypt
{
private:
	LoginCrypt loginCrypter;
	GameCrypt *gameCrypter;
	GameCrypt *crypt203;
	uint8_t crypt_mode;
	unsigned char clientSeed[4];
	uint32_t loginKey1, loginKey2;
	int cryptVersion;
//    NormalCopier m_copier;
//    CompressingCopier m_compressor;
//    DecompressingCopier m_decompressor;
    bool compressed, first_send;
	bool entering;
	unsigned char cryptPacket [MAXBUFFER];
	unsigned char decryptPacket [MAXBUFFER];

public:
	void decrypt(unsigned char *in, unsigned char *out, int length);
	void encrypt(unsigned char *in, unsigned char *out, int length);
	void setLoginCryptKeys(uint32_t key1, uint32_t key2);
	void setGameEncryption(int version);
	int getCryptVersion() { return cryptVersion; };
	void setCryptMode(uint8_t mode);
	uint8_t getCryptMode() { return crypt_mode; };
	virtual ~ClientCrypt();
	ClientCrypt();
	void init (unsigned char seed[4]);
	void setCryptSeed(uint8_t pseed[4]);
	void setCryptSeed(uint32_t pseed);
	void setEntering(bool state);
	bool getEntering();
};

#endif