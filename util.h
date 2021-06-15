/*
	Creative Commons Attribution-NonCommercial-NoDerivs (CC-BY-NC-ND)
	https://creativecommons.org/licenses/by-nc-nd/4.0/
	The most restrictive creative commons license.
	This only allows people to download and share your work for no commercial gain and for no other purposes.
*/


#ifndef UTIL_H
#define UTIL_H 1

#include <time.h>

#define CHECK_FLAG(flags, bit)		(((flags) & (bit)) == (bit))
#define CHECK_NFLAG(flags, bit)		(((flags) & (bit)) != (bit))
#define ARRAY_CLEAR(x)				memset(x, 0, sizeof(x))
#define OBJECT_CLEAR(x)				memset(&(x), 0, sizeof(x))
#define ARRAY_SIZE(x)				(sizeof(x) / sizeof((x)[0]))
#define TO_STR(x)					(#x)

// Utility
void reverse_array(char * arr, int start, int end);
int bin2bcd(int value);
int bcd2bin(int value);
char ascii2bcd(char i);
void three_digit_separator(char * dst, const char * src);
void unhexify(char * dst, const char * src, int len);
void hexify(char * dst, const char * src, int len);
int valid_hexstring(const char * in);
void left_pad(char * out, const char * in, int len, char pad);
void remove_zero_padding(const char * start, int length, char * out);
void remove_zero_padding2(const char * src, char * dst);
int read_int(const char * start, int len);
int findChar(const char * buffer, char delim, int len);
void random_string(char * out, int len);

// Date Converter
size_t convert_datetime_shamsi(const char * i_date, const char * i_fmt, const char * out_fmt, char * out, int len);
size_t convert_datetime(const char * i_date, const char * i_fmt, const char * out_fmt, char * out, int len);
// t->tm_year -= 1900, t->tm_mon -= 1
void change_date_to_persian(struct tm * t);
void change_date_to_gregorian(struct tm * t);
size_t get_miladi_date(const char * fmt, char * out, int len);
size_t get_shamsi_date(const char * fmt, char * out, int len);

// Text Encoder/Decoder
int iransystem_to_utf8_index(int iransystem_index);
int windows1256_to_utf8(char * dst, char * src, int len);
int iransystem_to_utf_8(unsigned char * dst, unsigned char * src, int len);
int convert_iransystem_to_utf8(char * out, const char * in_hex_str, int reverse);

// Passwords
void passwd_supervisor(const char * terminal, char * out);
void passwd_reset(const char * terminal, char * out);
void passwd_debug(const char * terminal, int menu, char * out);
int passwd_debug_check(const char * terminal, const char * pin, int * menu);

int validate_ip_string(const char * ip, int subnet);
char * rgba_to_bitmap(const unsigned char * rgba, int w, int h, int ref_value);
char get_next_keypad_char(char in);

#endif //UTIL_H
