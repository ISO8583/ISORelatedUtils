/*
	Creative Commons Attribution-NonCommercial-NoDerivs (CC-BY-NC-ND)
	https://creativecommons.org/licenses/by-nc-nd/4.0/
	The most restrictive creative commons license.
	This only allows people to download and share your work for no commercial gain and for no other purposes.
*/

#ifndef SSM_H
#define SSM_H 1

#include <stdlib.h>
#include <stdint.h>

#define SSM_SHA1_LEN					20
#define SSM_SHA1_HEX_LEN				(SSM_SHA1_LEN << 1)
#define SSM_SHA224_LEN					28
#define SSM_SHA224_HEX_LEN				(SSM_SHA224_LEN << 1)
#define SSM_SHA256_LEN					32
#define SSM_SHA256_HEX_LEN				(SSM_SHA256_LEN << 1)
#define SSM_SHA384_LEN					48
#define SSM_SHA384_HEX_LEN				(SSM_SHA384_LEN << 1)
#define SSM_SHA512_LEN					64
#define SSM_SHA512_HEX_LEN				(SSM_SHA512_LEN << 1)
#define SSM_SHA_MAX_LEN					SSM_SHA512_LEN
#define SSM_SHA_HEX_MAX_LEN				SSM_SHA512_HEX_LEN

enum
{
	SSM_MODE_DECRYPT,
	SSM_MODE_ENCRYPT,
};

int ssm_des(int mode, const char * in, int in_len, const char * key, char * out);
int ssm_3des_ecb(int mode, const char * in, int in_len, const char * key, int key_len, char * out);
int ssm_3des_cbc(int mode, const char * in, int in_len, const char * key, int key_len, char * out);
int ssm_calc_pin_block(char * out, const char * track2, const char * pin, int pin_len, const char * key, int key_len);
int ssm_mac_des_3des(const char * in, int in_len, const char * key, int key_len, char * out);
int ssm_mac_des(const char * in, int in_len, const char * key, int key_len, char * out);
int ssm_aes_cbc(int mode, const char * in, size_t in_len, const char * key, size_t key_len, char * out);
int ssm_sha_hexstring(int algorithm, const char * in, size_t len, char * out);
uint32_t ssm_hotp_generate(const uint8_t * key, size_t key_len, uint64_t interval, size_t digits);
int ssm_opt_mkurl(const char * issue, const char * name, const char * alg,
			   const uint8_t * key, size_t key_len, int digit, int window, char * out);

#endif //SSM_H
