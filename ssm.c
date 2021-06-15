/*
	Creative Commons Attribution-NonCommercial-NoDerivs (CC-BY-NC-ND)
	https://creativecommons.org/licenses/by-nc-nd/4.0/
	The most restrictive creative commons license.
	This only allows people to download and share your work for no commercial gain and for no other purposes.
*/


#include "ssm.h"
#include "util.h"

#include <stdio.h>
#include <string.h>

#include <base32.h>
#include <mbedtls/md.h>
#include <mbedtls/des.h>
#include <mbedtls/aes.h>
#include <mbedtls/sha1.h>
#include <mbedtls/sha256.h>
#include <mbedtls/sha512.h>

int ssm_des(int mode, const char * in, int in_len, const char * key, char * out)
{
	unsigned char padding[8] = {0};
	mbedtls_des_context ctx;
	mbedtls_des_init(&ctx);

	if (mode == SSM_MODE_ENCRYPT)
	{
		mbedtls_des_setkey_enc(&ctx, key);
	}
	else if (mode == SSM_MODE_DECRYPT)
	{
		mbedtls_des_setkey_dec(&ctx, key);
	}
	else
	{
		mbedtls_des_free(&ctx);
		return -1;
	}

	for (int i = 0; i < in_len; i += 8)
	{
		mbedtls_des_crypt_ecb(&ctx, in + i, out + i);
	}

	int remain = in_len & 0x07;
	if (remain)
	{
		memcpy(padding, in + in_len - remain, remain);
		mbedtls_des_crypt_ecb(&ctx, padding, out + in_len - remain);
	}

	mbedtls_des_free(&ctx);

	return 0;
}

int ssm_3des_ecb(int mode, const char * in, int in_len, const char * key, int key_len, char * out)
{
	unsigned char padding[8] = {0};
	mbedtls_des3_context ctx3;

	if (key_len != 16 && key_len != 24)
	{
		return -1;
	}

	mbedtls_des3_init(&ctx3);

	if (mode == SSM_MODE_ENCRYPT)
	{
		if (key_len == 16)
		{
			mbedtls_des3_set2key_enc(&ctx3, key);
		}
		else
		{
			mbedtls_des3_set3key_enc(&ctx3, key);
		}
	}
	else if (mode == SSM_MODE_DECRYPT)
	{
		if (key_len == 16)
		{
			mbedtls_des3_set2key_dec(&ctx3, key);
		}
		else
		{
			mbedtls_des3_set3key_dec(&ctx3, key);
		}
	}
	else
	{
		mbedtls_des3_free(&ctx3);
		return -1;
	}

	if (in_len >= 8)
	{
		for (int i = 0; i < in_len; i += 8)
		{
			mbedtls_des3_crypt_ecb(&ctx3, in + i, out + i);
		}
	}

	int remain = in_len & 0x07;
	if (remain)
	{
		memcpy(padding, in + in_len - remain, remain);
		mbedtls_des3_crypt_ecb(&ctx3, padding, out + in_len - remain);
	}

	mbedtls_des3_free(&ctx3);

	return 0;
}

int ssm_3des_cbc(int mode, const char * in, int in_len, const char * key, int key_len, char * out)
{
	unsigned char iv[8] = {0};
	unsigned char padding[8] = {0};

	if (key_len != 16 && key_len != 24)
	{
		return -1;
	}

	mbedtls_des3_context ctx3;
	mbedtls_des3_init(&ctx3);

	if (mode == SSM_MODE_ENCRYPT)
	{
		if (key_len == 16)
		{
			mbedtls_des3_set2key_enc(&ctx3, key);
		}
		else
		{
			mbedtls_des3_set3key_enc(&ctx3, key);
		}
	}
	else if (mode == SSM_MODE_DECRYPT)
	{
		if (key_len == 16)
		{
			mbedtls_des3_set2key_dec(&ctx3, key);
		}
		else
		{
			mbedtls_des3_set3key_dec(&ctx3, key);
		}
	}
	else
	{
		mbedtls_des3_free(&ctx3);
		return -1;
	}

	int mmode = mode == SSM_MODE_ENCRYPT ? MBEDTLS_DES_ENCRYPT : MBEDTLS_DES_DECRYPT;

	if (in_len >= 8)
	{
		mbedtls_des3_crypt_cbc(&ctx3, mmode, in_len - (in_len & 0x07), iv, in, out);
	}

	int remain = in_len & 0x07;
	if (remain)
	{
		memcpy(padding, in + in_len - remain, remain);
		mbedtls_des3_crypt_cbc(&ctx3, mmode, 8, iv, padding, out + in_len - remain);
	}

	mbedtls_des3_free(&ctx3);

	return 0;
}

int ssm_aes_cbc(int mode, const char * in, size_t in_len, const char * key, size_t key_len, char * out)
{
	unsigned char iv[16] = {0};
	unsigned char padding[16] = {0};
	mbedtls_aes_context ctx;

	if (key_len != 16 && key_len != 24 && key_len != 32)
	{
		return -1;
	}

	mbedtls_aes_init(&ctx);

	if (mode == SSM_MODE_ENCRYPT)
	{
		if (mbedtls_aes_setkey_enc(&ctx, key, key_len << 3) != 0)
		{
			mbedtls_aes_free(&ctx);
			return -1;
		}
	}
	else if (mode == SSM_MODE_DECRYPT)
	{
		if (mbedtls_aes_setkey_dec(&ctx, key, key_len << 3) != 0)
		{
			mbedtls_aes_free(&ctx);
			return -1;
		}
	}
	else
	{
		mbedtls_aes_free(&ctx);
		return -1;
	}

	int mmode = mode == SSM_MODE_ENCRYPT ? MBEDTLS_AES_ENCRYPT : MBEDTLS_AES_DECRYPT;

	if (in_len >= 16)
	{
		mbedtls_aes_crypt_cbc(&ctx, mmode, in_len - (in_len & 0x0F), iv, in, out);
	}

	int remain = in_len & 0x0F;
	if (remain)
	{
		memcpy(padding, in + in_len - remain, remain);
		mbedtls_aes_crypt_cbc(&ctx, mmode, 16, iv, padding, out + in_len - remain);
	}

	mbedtls_aes_free(&ctx);

	return 0;
}

int ssm_calc_pin_block(char * out, const char * track2, const char * pin, int pin_len, const char * key, int key_len)
{
	char res[8];
	char temp[16];
	char key_1[24];

	if (key_len == 24)
	{
		memcpy(key_1, key, 24);
	}
	else if (key_len == 16)
	{
		memcpy(key_1, key, 16);
		memcpy(key_1 + 16, key, 8);
	}
	else
	{
		return -1;
	}

	// pin buffer
	temp[0] = '0';
	temp[1] = '0' + pin_len;
	memset(temp + 2, 'f', 14);
	memcpy(temp + 2, pin, pin_len);
	unhexify(res, temp, 16);
	// track buffer
	temp[0] = 0;
	temp[1] = 0;
	unhexify(temp + 2, track2 + 3, 12);
	// xor part
	res[0] ^= temp[0];
	res[1] ^= temp[1];
	res[2] ^= temp[2];
	res[3] ^= temp[3];
	res[4] ^= temp[4];
	res[5] ^= temp[5];
	res[6] ^= temp[6];
	res[7] ^= temp[7];
	// encrypt
	mbedtls_des3_context ctx3;
	mbedtls_des3_init(&ctx3);
	mbedtls_des3_set3key_enc(&ctx3, key_1);
	mbedtls_des3_crypt_ecb(&ctx3, res, out);

	return 0;
}

int ssm_mac_des_3des(const char * in, int in_len, const char * key, int key_len, char * out)
{
	char extra[8];
	char temp[8];
	char k1[8];
	char k2[8];
	char k3[8];

	if (key_len == 24)
	{
		memcpy(k1, key, 8);
		memcpy(k2, key + 8, 8);
		memcpy(k3, key + 16, 8);
	}
	else if (key_len == 16)
	{
		memcpy(k1, key, 8);
		memcpy(k2, key + 8, 8);
		memcpy(k3, k1, 8);
	}
	else
	{
		return -1;
	}

	int remain = in_len & 0x07;
	int step = in_len >> 3;

	if (remain)
	{
		memset(extra, 0, sizeof(extra));
		memcpy(extra, in + in_len - remain, remain);
	}

	memset(out, 0, 8);

	mbedtls_des_context ctx;
	mbedtls_des_init(&ctx);
	mbedtls_des_setkey_enc(&ctx, k1);
	const char * tldata = in;

	for (int i = 0; i < step; ++i)
	{
		temp[0] = out[0] ^ tldata[0];
		temp[1] = out[1] ^ tldata[1];
		temp[2] = out[2] ^ tldata[2];
		temp[3] = out[3] ^ tldata[3];
		temp[4] = out[4] ^ tldata[4];
		temp[5] = out[5] ^ tldata[5];
		temp[6] = out[6] ^ tldata[6];
		temp[7] = out[7] ^ tldata[7];

		mbedtls_des_crypt_ecb(&ctx, temp, out);

		tldata += 8;
	}

	if (remain)
	{
		temp[0] = out[0] ^ extra[0];
		temp[1] = out[1] ^ extra[1];
		temp[2] = out[2] ^ extra[2];
		temp[3] = out[3] ^ extra[3];
		temp[4] = out[4] ^ extra[4];
		temp[5] = out[5] ^ extra[5];
		temp[6] = out[6] ^ extra[6];
		temp[7] = out[7] ^ extra[7];

		mbedtls_des_crypt_ecb(&ctx, temp, out);
	}

	mbedtls_des_setkey_dec(&ctx, k2);
	mbedtls_des_crypt_ecb(&ctx, out, temp);

	mbedtls_des_setkey_enc(&ctx, k3);
	mbedtls_des_crypt_ecb(&ctx, temp, out);

	return 0;
}

int ssm_mac_des(const char * in, int in_len, const char * key, int key_len, char * out)
{
	char extra[8];
	char temp[8];

	if (key_len != 8)
	{
		return -1;
	}

	int remain = in_len & 0x07;
	int step = in_len >> 3;

	if (remain)
	{
		memset(extra, 0, sizeof(extra));
		memcpy(extra, in + in_len - remain, remain);
	}

	memset(out, 0, 8);

	mbedtls_des_context ctx;
	mbedtls_des_init(&ctx);
	mbedtls_des_setkey_enc(&ctx, key);
	const char * tldata = in;

	for (int i = 0; i < step; ++i)
	{
		temp[0] = out[0] ^ tldata[0];
		temp[1] = out[1] ^ tldata[1];
		temp[2] = out[2] ^ tldata[2];
		temp[3] = out[3] ^ tldata[3];
		temp[4] = out[4] ^ tldata[4];
		temp[5] = out[5] ^ tldata[5];
		temp[6] = out[6] ^ tldata[6];
		temp[7] = out[7] ^ tldata[7];

		mbedtls_des_crypt_ecb(&ctx, temp, out);

		tldata += 8;
	}

	if (remain)
	{
		temp[0] = out[0] ^ extra[0];
		temp[1] = out[1] ^ extra[1];
		temp[2] = out[2] ^ extra[2];
		temp[3] = out[3] ^ extra[3];
		temp[4] = out[4] ^ extra[4];
		temp[5] = out[5] ^ extra[5];
		temp[6] = out[6] ^ extra[6];
		temp[7] = out[7] ^ extra[7];

		mbedtls_des_crypt_ecb(&ctx, temp, out);
	}

	return 0;
}

int ssm_sha_hexstring(int algorithm, const char * in, size_t len, char * out)
{
	unsigned char result[SSM_SHA_MAX_LEN];

	if (algorithm == 1)
	{
		if (mbedtls_sha1(in, len, result) == 0)
		{
			hexify(out, result, SSM_SHA1_LEN);
			return SSM_SHA1_HEX_LEN;
		}
	}
	else if (algorithm == 224)
	{
		if (mbedtls_sha256(in, len, result, 1) == 0)
		{
			hexify(out, result, SSM_SHA224_LEN);
			return SSM_SHA224_HEX_LEN;
		}
	}
	else if (algorithm == 256)
	{
		if (mbedtls_sha256(in, len, result, 0) == 0)
		{
			hexify(out, result, SSM_SHA256_LEN);
			return SSM_SHA256_HEX_LEN;
		}
	}
	else if (algorithm == 384)
	{
		if (mbedtls_sha512(in, len, result, 1) == 0)
		{
			hexify(out, result, SSM_SHA384_LEN);
			return SSM_SHA384_HEX_LEN;
		}
	}
	else if (algorithm == 512)
	{
		if (mbedtls_sha512(in, len, result, 0) == 0)
		{
			hexify(out, result, SSM_SHA512_LEN);
			return SSM_SHA512_HEX_LEN;
		}
	}

	return -1;
}

int64_t pow1(int32_t base, int32_t n)
{
	int64_t result = 1;
	while (n > 0)
	{
		if (n & 1)
		{
			result *= base;
		}

		base *= base;
		n >>= 1;
	}

	return result;
}

uint32_t ssm_hotp_generate(const uint8_t * key, size_t key_len, uint64_t interval, size_t digits)
{
	// hotp_generate(key, sizeof(key), time(NULL) / 60, 6)
	uint8_t digest[100];
	uint32_t endianness = 0xdeadbeef;

	if ((*(const uint8_t *)&endianness) == 0xef)
	{
		interval = ((interval & 0x00000000ffffffffU) << 32u) | ((interval & 0xffffffff00000000U) >> 32u);
		interval = ((interval & 0x0000ffff0000ffffU) << 16u) | ((interval & 0xffff0000ffff0000U) >> 16u);
		interval = ((interval & 0x00ff00ff00ff00ffU) <<  8u) | ((interval & 0xff00ff00ff00ff00U) >>  8u);
	}

	// First Phase, get the digest of the message using the provided key ...
	mbedtls_md_hmac(mbedtls_md_info_from_type(MBEDTLS_MD_SHA1), key,
					key_len, &interval, sizeof(interval), digest);

	// Second Phase, get the dbc from the algorithm
	uint64_t offset = digest[19] & 0x0fU;
	uint32_t dbc = (digest[offset] & 0x7fU) << 24u | (digest[offset+1u] & 0xffU) << 16u |
					(digest[offset+2u] & 0xffU) << 8u | (digest[offset+3u] & 0xffU);

	// Third Phase: calculate the mod_k of the dbc to get the correct number
	return dbc % ((uint32_t) pow1(10, digits));
}

/*
 * OTP URI Schema
 * otpauth://(totp|hotp)/[issuer name:]name?
 * params:
 * secret -> base32 of key
 * algorithm -> MD5, SHA1, SHA224, SHA256, SHA384, SHA512
 * digits -> number of pin digits
 * lock -> require auth for show otp. true, false
 * image -> url encoded for icon
 * period -> totp time window (15s, 30s, 60s, ...)
 * counter -> h-otp init counter
*/

int ssm_opt_mkurl(const char * issue, const char * name, const char * alg,
					const uint8_t * key, size_t key_len, int digit, int window, char * out)
{
	char base32[500];
	base32_encode(key, key_len, base32, sizeof(base32));
	return sprintf(out, "otpauth://totp/%s:%s?secret=%s&algorithm=%s&digits=%d&period=%d&lock=false",
					issue, name, base32, alg, digit, window);
}
