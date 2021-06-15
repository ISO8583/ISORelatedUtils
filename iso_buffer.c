/*
	Creative Commons Attribution-NonCommercial-NoDerivs (CC-BY-NC-ND)
	https://creativecommons.org/licenses/by-nc-nd/4.0/
	The most restrictive creative commons license.
	This only allows people to download and share your work for no commercial gain and for no other purposes.
*/


#include <stdio.h>
#include <string.h>

#include "iso_buffer.h"
#include "util.h"

const iso_def_t iso_hypercom = {
		0,
		0,
		{
		//  i       field
		/* 000 */ /* 000 */ {LEN_FIX, TYPE_NONE, 4, TYPE_BCD},
		/* 001 */ /* BIT */ {LEN_FIX, TYPE_NONE, 8, TYPE_BYTE},
		/* 002 */ /* 001 */ {LEN_FIX, TYPE_NONE, 8, TYPE_BYTE},
		/* 003 */ /* 002 */ {LEN_2L, TYPE_BCD, 20, TYPE_BCD},
		/* 004 */ /* 003 */ {LEN_FIX, TYPE_NONE, 6, TYPE_BCD},
		/* 005 */ /* 004 */ {LEN_FIX, TYPE_NONE, 12, TYPE_BCD},
		/* 006 */ /* 005 */ {LEN_FIX, TYPE_NONE, 12, TYPE_BCD},
		/* 007 */ /* 006 */ {LEN_FIX, TYPE_NONE, 12, TYPE_BCD},
		/* 008 */ /* 007 */ {LEN_FIX, TYPE_NONE, 10, TYPE_BCD},
		/* 009 */ /* 008 */ {LEN_FIX, TYPE_NONE, 8, TYPE_BCD},
		/* 010 */ /* 009 */ {LEN_FIX, TYPE_NONE, 8, TYPE_BYTE},
		/* 011 */ /* 010 */ {LEN_FIX, TYPE_NONE, 8, TYPE_BYTE},
		/* 012 */ /* 011 */ {LEN_FIX, TYPE_NONE, 6, TYPE_BCD},
		/* 013 */ /* 012 */ {LEN_FIX, TYPE_NONE, 6, TYPE_BCD},
		/* 014 */ /* 013 */ {LEN_FIX, TYPE_NONE, 4, TYPE_BCD},
		/* 015 */ /* 014 */ {LEN_FIX, TYPE_NONE, 4, TYPE_BCD},
		/* 016 */ /* 015 */ {LEN_FIX, TYPE_NONE, 4, TYPE_BCD},
		/* 017 */ /* 016 */ {LEN_FIX, TYPE_NONE, 4, TYPE_BCD},
		/* 018 */ /* 017 */ {LEN_FIX, TYPE_NONE, 4, TYPE_BCD},
		/* 019 */ /* 018 */ {LEN_FIX, TYPE_NONE, 4, TYPE_BCD},
		/* 020 */ /* 019 */ {LEN_FIX, TYPE_NONE, 3, TYPE_BCD},
		/* 021 */ /* 020 */ {LEN_FIX, TYPE_NONE, 3, TYPE_BCD},
		/* 022 */ /* 021 */ {LEN_FIX, TYPE_NONE, 3, TYPE_BCD},
		/* 023 */ /* 022 */ {LEN_FIX, TYPE_NONE, 3, TYPE_BCD},
		/* 024 */ /* 023 */ {LEN_FIX, TYPE_NONE, 3, TYPE_BCD},
		/* 025 */ /* 024 */ {LEN_FIX, TYPE_NONE, 3, TYPE_BCD},
		/* 026 */ /* 025 */ {LEN_FIX, TYPE_NONE, 2, TYPE_BCD},
		/* 027 */ /* 026 */ {LEN_FIX, TYPE_NONE, 2, TYPE_BCD},
		/* 028 */ /* 027 */ {LEN_FIX, TYPE_NONE, 1, TYPE_BCD},
		/* 029 */ /* 028 */ {LEN_FIX, TYPE_NONE, 9, TYPE_BCD},
		/* 030 */ /* 029 */ {LEN_FIX, TYPE_NONE, 9, TYPE_BCD},
		/* 031 */ /* 030 */ {LEN_FIX, TYPE_NONE, 9, TYPE_BCD},
		/* 032 */ /* 031 */ {LEN_FIX, TYPE_NONE, 9, TYPE_BCD},
		/* 033 */ /* 032 */ {LEN_2L, TYPE_BCD, 11, TYPE_BCD},
		/* 034 */ /* 033 */ {LEN_2L, TYPE_BCD, 11, TYPE_BCD},
		/* 035 */ /* 034 */ {LEN_2L, TYPE_BCD, 28, TYPE_BYTE},
		/* 036 */ /* 035 */ {LEN_2L, TYPE_BCD, 40, TYPE_BCD},
		/* 037 */ /* 036 */ {LEN_3L, TYPE_BCD, 104, TYPE_BYTE},
		/* 038 */ /* 037 */ {LEN_FIX, TYPE_NONE, 12, TYPE_BYTE},
		/* 039 */ /* 038 */ {LEN_FIX, TYPE_NONE, 6, TYPE_BYTE},
		/* 040 */ /* 039 */ {LEN_FIX, TYPE_NONE, 2, TYPE_BYTE},
		/* 041 */ /* 040 */ {LEN_FIX, TYPE_NONE, 3, TYPE_BYTE},
		/* 042 */ /* 041 */ {LEN_FIX, TYPE_NONE, 8, TYPE_BYTE},
		/* 043 */ /* 042 */ {LEN_FIX, TYPE_NONE, 15, TYPE_BYTE},
		/* 044 */ /* 043 */ {LEN_FIX, TYPE_NONE, 40, TYPE_BYTE},
		/* 045 */ /* 044 */ {LEN_2L, TYPE_BCD, 99, TYPE_BYTE},
		/* 046 */ /* 045 */ {LEN_2L, TYPE_BCD, 76, TYPE_BYTE},
		/* 047 */ /* 046 */ {LEN_3L, TYPE_BCD, 999, TYPE_BYTE},
		/* 048 */ /* 047 */ {LEN_3L, TYPE_BCD, 999, TYPE_BYTE},
		/* 049 */ /* 048 */ {LEN_3L, TYPE_BCD, 999, TYPE_BYTE},
		/* 050 */ /* 049 */ {LEN_FIX, TYPE_NONE, 3, TYPE_BYTE},
		/* 051 */ /* 050 */ {LEN_FIX, TYPE_NONE, 3, TYPE_BYTE},
		/* 052 */ /* 051 */ {LEN_FIX, TYPE_NONE, 3, TYPE_BYTE},
		/* 053 */ /* 052 */ {LEN_FIX, TYPE_NONE, 8, TYPE_BYTE},
		/* 054 */ /* 053 */ {LEN_FIX, TYPE_NONE, 16, TYPE_BCD},
		/* 055 */ /* 054 */ {LEN_3L, TYPE_BCD, 120, TYPE_BYTE},
		/* 056 */ /* 055 */ {LEN_3L, TYPE_BCD, 999, TYPE_BYTE},
		/* 057 */ /* 056 */ {LEN_3L, TYPE_BCD, 999, TYPE_BYTE},
		/* 058 */ /* 057 */ {LEN_3L, TYPE_BCD, 999, TYPE_BYTE},
		/* 059 */ /* 058 */ {LEN_3L, TYPE_BCD, 999, TYPE_BYTE},
		/* 060 */ /* 059 */ {LEN_3L, TYPE_BCD, 999, TYPE_BYTE},
		/* 061 */ /* 060 */ {LEN_3L, TYPE_BCD, 999, TYPE_BYTE},
		/* 062 */ /* 061 */ {LEN_3L, TYPE_BCD, 999, TYPE_BYTE},
		/* 063 */ /* 062 */ {LEN_3L, TYPE_BCD, 999, TYPE_BYTE},
		/* 064 */ /* 063 */ {LEN_3L, TYPE_BCD, 999, TYPE_BYTE},
		/* 065 */ /* 064 */ {LEN_FIX, TYPE_NONE, 8, TYPE_BYTE},
		/* 066 */ /* 065 */ {LEN_FIX, TYPE_NONE, 1, TYPE_BYTE},
		/* 067 */ /* 066 */ {LEN_FIX, TYPE_NONE, 1, TYPE_BCD},
		/* 068 */ /* 067 */ {LEN_FIX, TYPE_NONE, 2, TYPE_BCD},
		/* 069 */ /* 068 */ {LEN_FIX, TYPE_NONE, 3, TYPE_BCD},
		/* 070 */ /* 069 */ {LEN_FIX, TYPE_NONE, 3, TYPE_BCD},
		/* 071 */ /* 070 */ {LEN_FIX, TYPE_NONE, 3, TYPE_BCD},
		/* 072 */ /* 071 */ {LEN_FIX, TYPE_NONE, 4, TYPE_BCD},
		/* 073 */ /* 072 */ {LEN_FIX, TYPE_NONE, 4, TYPE_BCD},
		/* 074 */ /* 073 */ {LEN_FIX, TYPE_NONE, 6, TYPE_BCD},
		/* 075 */ /* 074 */ {LEN_FIX, TYPE_NONE, 10, TYPE_BCD},
		/* 076 */ /* 075 */ {LEN_FIX, TYPE_NONE, 10, TYPE_BCD},
		/* 077 */ /* 076 */ {LEN_FIX, TYPE_NONE, 10, TYPE_BCD},
		/* 078 */ /* 077 */ {LEN_FIX, TYPE_NONE, 10, TYPE_BCD},
		/* 079 */ /* 078 */ {LEN_FIX, TYPE_NONE, 10, TYPE_BCD},
		/* 080 */ /* 079 */ {LEN_FIX, TYPE_NONE, 10, TYPE_BCD},
		/* 081 */ /* 080 */ {LEN_FIX, TYPE_NONE, 10, TYPE_BCD},
		/* 082 */ /* 081 */ {LEN_FIX, TYPE_NONE, 10, TYPE_BCD},
		/* 083 */ /* 082 */ {LEN_FIX, TYPE_NONE, 12, TYPE_BCD},
		/* 084 */ /* 083 */ {LEN_FIX, TYPE_NONE, 12, TYPE_BCD},
		/* 085 */ /* 084 */ {LEN_FIX, TYPE_NONE, 12, TYPE_BCD},
		/* 086 */ /* 085 */ {LEN_FIX, TYPE_NONE, 12, TYPE_BCD},
		/* 087 */ /* 086 */ {LEN_FIX, TYPE_NONE, 16, TYPE_BCD},
		/* 088 */ /* 087 */ {LEN_FIX, TYPE_NONE, 16, TYPE_BCD},
		/* 089 */ /* 088 */ {LEN_FIX, TYPE_NONE, 16, TYPE_BCD},
		/* 090 */ /* 089 */ {LEN_FIX, TYPE_NONE, 16, TYPE_BCD},
		/* 091 */ /* 090 */ {LEN_FIX, TYPE_NONE, 42, TYPE_BCD},
		/* 092 */ /* 091 */ {LEN_FIX, TYPE_NONE, 1, TYPE_BYTE},
		/* 093 */ /* 092 */ {LEN_FIX, TYPE_NONE, 2, TYPE_BYTE},
		/* 094 */ /* 093 */ {LEN_FIX, TYPE_NONE, 6, TYPE_BYTE},
		/* 095 */ /* 094 */ {LEN_FIX, TYPE_NONE, 7, TYPE_BYTE},
		/* 096 */ /* 095 */ {LEN_FIX, TYPE_NONE, 42, TYPE_BYTE},
		/* 097 */ /* 096 */ {LEN_FIX, TYPE_NONE, 16, TYPE_BYTE},
		/* 098 */ /* 097 */ {LEN_FIX, TYPE_NONE, 17, TYPE_BCD},
		/* 099 */ /* 098 */ {LEN_FIX, TYPE_NONE, 25, TYPE_BYTE},
		/* 100 */ /* 099 */ {LEN_2L, TYPE_BCD, 11, TYPE_BCD},
		/* 101 */ /* 100 */ {LEN_2L, TYPE_BCD, 11, TYPE_BCD},
		/* 102 */ /* 101 */ {LEN_2L, TYPE_BCD, 17, TYPE_BYTE},
		/* 103 */ /* 102 */ {LEN_2L, TYPE_BCD, 28, TYPE_BYTE},
		/* 104 */ /* 103 */ {LEN_2L, TYPE_BCD, 99, TYPE_BCD},
		/* 105 */ /* 104 */ {LEN_3L, TYPE_BCD, 100, TYPE_BYTE},
		/* 106 */ /* 105 */ {LEN_3L, TYPE_BCD, 999, TYPE_BYTE},
		/* 107 */ /* 106 */ {LEN_3L, TYPE_BCD, 999, TYPE_BYTE},
		/* 108 */ /* 107 */ {LEN_3L, TYPE_BCD, 999, TYPE_BYTE},
		/* 109 */ /* 108 */ {LEN_3L, TYPE_BCD, 999, TYPE_BYTE},
		/* 110 */ /* 109 */ {LEN_3L, TYPE_BCD, 999, TYPE_BYTE},
		/* 111 */ /* 110 */ {LEN_3L, TYPE_BCD, 999, TYPE_BYTE},
		/* 112 */ /* 111 */ {LEN_3L, TYPE_BCD, 999, TYPE_BYTE},
		/* 113 */ /* 112 */ {LEN_3L, TYPE_BCD, 999, TYPE_BYTE},
		/* 114 */ /* 113 */ {LEN_3L, TYPE_BCD, 999, TYPE_BYTE},
		/* 115 */ /* 114 */ {LEN_3L, TYPE_BCD, 999, TYPE_BYTE},
		/* 116 */ /* 115 */ {LEN_3L, TYPE_BCD, 999, TYPE_BYTE},
		/* 117 */ /* 116 */ {LEN_3L, TYPE_BCD, 999, TYPE_BYTE},
		/* 118 */ /* 117 */ {LEN_3L, TYPE_BCD, 999, TYPE_BYTE},
		/* 119 */ /* 118 */ {LEN_3L, TYPE_BCD, 999, TYPE_BYTE},
		/* 120 */ /* 119 */ {LEN_3L, TYPE_BCD, 999, TYPE_BYTE},
		/* 121 */ /* 120 */ {LEN_3L, TYPE_BCD, 999, TYPE_BYTE},
		/* 122 */ /* 121 */ {LEN_3L, TYPE_BCD, 999, TYPE_BYTE},
		/* 123 */ /* 122 */ {LEN_3L, TYPE_BCD, 999, TYPE_BYTE},
		/* 124 */ /* 123 */ {LEN_3L, TYPE_BCD, 999, TYPE_BYTE},
		/* 125 */ /* 124 */ {LEN_3L, TYPE_BCD, 999, TYPE_BYTE},
		/* 126 */ /* 125 */ {LEN_3L, TYPE_BCD, 999, TYPE_BYTE},
		/* 127 */ /* 126 */ {LEN_3L, TYPE_BCD, 999, TYPE_BYTE},
		/* 128 */ /* 127 */ {LEN_3L, TYPE_BCD, 999, TYPE_BYTE},
		/* 129 */ /* 128 */ {LEN_FIX, TYPE_NONE, 8, TYPE_BYTE},
		}
};

int iso_get_buffer(const iso_field_t * restrict def, const char * buffer, int len)
{
	int field_len = 0;
	int len_bytes = 0;

	if (def->len_var == LEN_FIX)
	{
		if (def->field_type == TYPE_BCD)
		{
			field_len = (def->max_len + 1) >> 1;
		}
		else if (def->field_type == TYPE_BYTE)
		{
			field_len = def->max_len;
		}
		else
		{
			return -1;
		}
	}
	else
	{
		len_bytes = (def->len_var + 1) >> 1;
		if (len_bytes > len)
		{
			return -1;
		}

		if (def->len_type == TYPE_BCD)
		{
			for (int i = 0; i < len_bytes; ++i)
			{
				field_len *= 10;
				field_len += (buffer[i] & 0xF0) >> 4;
				field_len *= 10;
				field_len += (buffer[i] & 0x0F);
			}
		}
		else if (def->len_type == TYPE_BYTE)
		{
			for (int i = 0; i < len_bytes; ++i)
			{
				field_len <<= 8;
				field_len += (unsigned char) buffer[i];
			}
		}
		else
		{
			return -1;
		}

		if (def->field_type == TYPE_BCD)
		{
			field_len += 1;
			field_len >>= 1;
		}
	}

	field_len += len_bytes;
	return field_len > len ? -1 : field_len;
}

int iso_test_buffer(const iso_def_t * restrict def, const char * buffer, int len)
{
	int index = 0;
	int temp = 0;
	char bitmap[16] = {0};

	// check len
	index += def->iso_len;
	if (index >= len)
	{
		return -1;
	}

	// check tdpu
	index += def->iso_tpdu;
	if (index > len)
	{
		return -1;
	}

	for (int i = 0, bitmap_len = 0; i < (128 + 1); ++i)
	{
		if (i <= 1 || ISO_HAS_FIELD(bitmap, i - 1))
		{
			temp = iso_get_buffer(&def->defs[i], buffer + index, len - index);
			if (temp < 0)
			{
				return -1;
			}

			index += temp;
			if (index > len)
			{
				return -1;
			}

			if (i == 1)
			{
				memcpy(bitmap, buffer + index - temp, temp);
				bitmap_len = temp;
			}
			else if (i == 2)
			{
				memcpy(bitmap + bitmap_len, buffer + index - temp, temp);
			}
		}
	}

	return index;
}

void iso_init(iso_buffer_t * iso)
{
	memset(iso, 0, sizeof(iso_buffer_t));
}

void iso_pack_byte(iso_buffer_t * iso, int value)
{
	if (iso->index <= (ISO_BUFFER_LENGTH - 1))
	{
		iso->buffer[iso->index] = (char) (value & 0xFF);
		++iso->index;
	}
}

void iso_pack_word(iso_buffer_t * iso, int value)
{
	if (iso->index <= (ISO_BUFFER_LENGTH - 2))
	{
		iso->buffer[iso->index] = (char) ((value & 0xFF00) >> 8);
		++iso->index;

		iso->buffer[iso->index] = (char) (value & 0xFF);
		++iso->index;
	}
}

void iso_pack_mem(iso_buffer_t * iso, const char * data, int len)
{
	if (len && iso->index <= (ISO_BUFFER_LENGTH - len))
	{
		memcpy(iso->buffer + iso->index, data, len);
		iso->index += len;
	}
}

void iso_pack_strbcd(iso_buffer_t * iso, const char * data, int len)
{
	if (len && iso->index >= (ISO_BUFFER_LENGTH - ((len + 1) / 2)))
	{
		return;
	}

	if ((len % 2) == 0)
	{
		for (int i = 0; i < len; i += 2)
		{
			iso->buffer[iso->index] = (char) (ascii2bcd(data[i]) << 4) | ascii2bcd(data[i + 1]);
			++iso->index;
		}
	}
	else
	{
		iso->buffer[iso->index] = (char) ascii2bcd(data[0]);
		++iso->index;

		for (int i = 1; i < len; i += 2)
		{
			iso->buffer[iso->index] = (char) (ascii2bcd(data[i]) << 4) | ascii2bcd(data[i + 1]);
			++iso->index;
		}
	}
}

long iso_get_int_bytes(iso_buffer_t * iso, int n)
{
	if (iso->index >= ISO_BUFFER_LENGTH - n)
	{
		return 0;
	}

	long res = 0;
	for (int i = 0; i < n; ++i)
	{
		res <<= 8;
		res += (unsigned char) iso->buffer[iso->index++];
	}

	return res;
}

int iso_get_waste(iso_buffer_t * iso, int len)
{
	if (len && iso->index >= ISO_BUFFER_LENGTH - len)
	{
		return -1;
	}

	iso->index += len;
	return 0;
}

int iso_get_byte(iso_buffer_t * iso)
{
	return (unsigned char) (iso->index >= ISO_BUFFER_LENGTH ? 0 : iso->buffer[iso->index++]);
}

int iso_get_word(iso_buffer_t * iso)
{
	if (iso->index >= ISO_BUFFER_LENGTH - 2)
	{
		return 0;
	}

	int res = (unsigned char) iso->buffer[iso->index++];
	res <<= 8;
	res += (unsigned char) iso->buffer[iso->index++];
	return res;
}

void iso_get_mem(iso_buffer_t * iso, char * out, int len)
{
	if (len && iso->index <= (ISO_BUFFER_LENGTH - len))
	{
		memcpy(out, iso->buffer + iso->index, len);
		iso->index += len;
	}
}

void iso_get_bcdstr(iso_buffer_t * iso, char * out, int len)
{
	if (len && iso->index <= (ISO_BUFFER_LENGTH - len))
	{
		hexify(out, iso->buffer + iso->index, len);
		iso->index += len;
	}
}

void iso_print(const char * label, iso_buffer_t * iso)
{
	printf("%s: ", label);
	for (int i = 0; i < iso->index; ++i)
	{
		printf("%02X", (unsigned char) iso->buffer[i]);
	}
	puts("\n");
}