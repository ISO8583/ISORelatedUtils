/*
	Creative Commons Attribution-NonCommercial-NoDerivs (CC-BY-NC-ND)
	https://creativecommons.org/licenses/by-nc-nd/4.0/
	The most restrictive creative commons license.
	This only allows people to download and share your work for no commercial gain and for no other purposes.
*/

#ifndef PAX_ISO_BUFFER_H
#define PAX_ISO_BUFFER_H 1

#define ISO_BUFFER_LENGTH					0xFFFF

#define ISO_HAS_FIELD(bitmap, f)			((bitmap)[((f) - 1) >> 3] & (1 << (7 - (((f) - 1) & 0x07))))

typedef struct {
	char buffer[ISO_BUFFER_LENGTH];
	int index;
	int size;
} iso_buffer_t;

enum
{
	LEN_FIX = 0,
	LEN_1L,
	LEN_2L,
	LEN_3L,
	LEN_4L,
};

enum
{
	TYPE_NONE,
	TYPE_BYTE,
	TYPE_BCD,
};

typedef struct
{
	int len_var;
	int len_type;
	int max_len;
	int field_type;
} iso_field_t;

typedef struct
{
	int iso_len;
	int iso_tpdu;
	iso_field_t defs[];
} iso_def_t;

extern const iso_def_t iso_hypercom;
int iso_test_buffer(const iso_def_t * restrict def, const char * buffer, int len);

void iso_init(iso_buffer_t * iso);
void iso_pack_byte(iso_buffer_t * iso, int value);
void iso_pack_word(iso_buffer_t * iso, int value);
void iso_pack_mem(iso_buffer_t * iso, const char * data, int len);
void iso_pack_strbcd(iso_buffer_t * iso, const char * data, int len);
int iso_get_waste(iso_buffer_t * iso, int len);
int iso_get_byte(iso_buffer_t * iso);
int iso_get_word(iso_buffer_t * iso);
long iso_get_int_bytes(iso_buffer_t * iso, int n);
void iso_get_mem(iso_buffer_t * iso, char * out, int len);
void iso_get_bcdstr(iso_buffer_t * iso, char * out, int len);
void iso_print(const char * label, iso_buffer_t * iso);

#endif //PAX_ISO_BUFFER_H