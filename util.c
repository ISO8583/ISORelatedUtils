/*
	Creative Commons Attribution-NonCommercial-NoDerivs (CC-BY-NC-ND)
	https://creativecommons.org/licenses/by-nc-nd/4.0/
	The most restrictive creative commons license.
	This only allows people to download and share your work for no commercial gain and for no other purposes.
*/

#include "util.h"

#include <base.h>

#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <stdint.h>
#include <string.h>

static const int sumDayMiladiMonth[] = {0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334};
static const int sumDayMiladiMonthLeap[]= {0, 31, 60, 91, 121, 152, 182, 213, 244, 274, 305, 335};
static const int miladiMonthDays[] = {0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
static const char alphabet[] = "~!@#$%^&*()_+-=[]{};':\",./<>?`0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";
static const char bin_map[] = "0123456789ABCDEF";
static const char * keypad_map[] = {
		"0,*#_-+=!@$%^&();:.", "1qzQZ", "2abcABC",
		"3defDEF", "4ghiGHI", "5jklJKL", "6mnoMNO",
		"7prsPRS", "8tuvTUV", "9wxyWXY"};

static const unsigned char iransystem_arabic_table[][2] = {
		{0xdb, 0xb0}, {0xdb, 0xb1}, {0xdb, 0xb2}, {0xdb, 0xb3}, {0xdb, 0xb4}, // 80 - 84
		{0xdb, 0xb5}, {0xdb, 0xb6}, {0xdb, 0xb7}, {0xdb, 0xb8}, {0xdb, 0xb9}, // 85 - 89
		{0xd8, 0x8c}, {'-' , 0x00}, {0xd8, 0x9f}, {0xd8, 0xa2}, {0xd8, 0xa6}, // 8A - 8E
		{0xd8, 0xa1}, {0xd8, 0xa7}, {0xd8, 0xa7}, {0xd8, 0xa8}, {0xd8, 0xa8}, // 8F - 93
		{0xd9, 0xbe}, {0xd9, 0xbe}, {0xd8, 0xaa}, {0xd8, 0xaa}, {0xd8, 0xab}, // 94 - 98
		{0xd8, 0xab}, {0xd8, 0xac}, {0xd8, 0xac}, {0xda, 0x86}, {0xda, 0x86}, // 99 - 9D
		{0xd8, 0xad}, {0xd8, 0xad}, {0xd8, 0xae}, {0xd8, 0xae}, {0xd8, 0xaf}, // 9E - A2
		{0xd8, 0xb0}, {0xd8, 0xb1}, {0xd8, 0xb2}, {0xda, 0x98}, {0xd8, 0xb3}, // A3 - A7
		{0xd8, 0xb3}, {0xd8, 0xb4}, {0xd8, 0xb4}, {0xd8, 0xb5}, {0xd8, 0xb5}, // A8 - AC
		{0xd8, 0xb6}, {0xd8, 0xb6}, {0xd8, 0xb7}, // AD - AF
		{0xd8, 0xb8}, {0xd8, 0xb9}, {0xd8, 0xb9}, {0xd8, 0xb9}, {0xd8, 0xb9}, // E0 - E4
		{0xd8, 0xba}, {0xd8, 0xba}, {0xd8, 0xba}, {0xd8, 0xba}, {0xd9, 0x81}, // E5 - E9
		{0xd9, 0x81}, {0xd9, 0x82}, {0xd9, 0x82}, {0xda, 0xa9}, {0xda, 0xa9}, // EA - EE
		{0xda, 0xaf}, {0xda, 0xaf}, {0xd9, 0x84}, {0x00, 0x00}, {0xd9, 0x84}, // EF - F3
		{0xd9, 0x85}, {0xd9, 0x85}, {0xd9, 0x86}, {0xd9, 0x86}, {0xd9, 0x88}, // F4 - F8
		{0xd9, 0x87}, {0xd9, 0x87}, {0xd9, 0x87}, {0xd9, 0x8a}, {0xd9, 0x8a}, // F9 - FD
		{0xd9, 0x8a}, {' ' , 0x00}, // FE - FF
};

void reverse_array(char * arr, int start, int end)
{
	char t;
	while (start < end)
	{
		t = arr[start];
		arr[start] = arr[end];
		arr[end] = t;
		++start;
		--end;
	}
}

int bin2bcd(int value)
{
	char temp[20];
	sprintf(temp, "%d", value);
	int res;
	sscanf(temp, "%x", &res);
	return res;
}

int bcd2bin(int value)
{
	char temp[20];
	sprintf(temp, "%x", value);
	int res;
	sscanf(temp, "%d", &res);
	return res;
}

char ascii2bcd(char i)
{
	if (i >= '0' && i <= '9')
	{
		return i - '0';
	}
	else if (i >= 'A' && i <= 'F')
	{
		return i - 'A' + 10;
	}
	else if (i >= 'a' && i <= 'f')
	{
		return i - 'a' + 10;
	}
	else
	{
		return 0;
	}
}

void three_digit_separator(char * dst, const char * src)
{
	int nIndex = strlen(src) % 3;
	dst[0] = 0;

	while (1)
	{
		if (nIndex)
		{
			strncat(dst, src, nIndex);
			src += nIndex;

			if (*src == 0)
			{
				break;
			}

			strcat(dst, ",");
			dst += nIndex + 1;
		}
		nIndex = 3;
	}
}

void unhexify(char * dst, const char * src, int len)
{
	for (int i_src = 0, i_dst = 0; i_src < len; i_src += 2, ++i_dst)
	{
		dst[i_dst] = (char) (((ascii2bcd(src[i_src]) << 4) | ascii2bcd(src[i_src + 1])));
	}
}

void hexify(char * dst, const char * src, int len)
{
	for (int i_src = 0, i_dst = 0; i_src < len; ++i_src, i_dst += 2)
	{
		dst[i_dst] = bin_map[(src[i_src] & 0xF0) >> 4];
		dst[i_dst + 1] = bin_map[(src[i_src] & 0x0F)];
	}
}

int valid_hexstring(const char * in)
{
	char c;
	while ((c = *in))
	{
		if ((c >= '0' && c <= '9') || (c >= 'a' && c <= 'f') || (c >= 'A' && c <= 'F'))
		{
			++in;
		}
		else
		{
			return -1;
		}
	}

	return 0;
}

void left_pad(char * out, const char * in, int len, char pad)
{
	int in_len = strlen(in);
	if (len > in_len)
	{
		memset(out, pad, len - in_len);
		memcpy(out + (len - in_len), in, in_len);
	}
	else
	{
		memcpy(out, in, len);
	}
}

void remove_zero_padding(const char * start, int length, char * out)
{
	int index = 0;
	while (index < length)
	{
		if (*start != '0')
		{
			break;
		}

		++start;
		++index;
	}

	index = length - index;
	memcpy(out, start, index);
	out[index] = 0;
}

void remove_zero_padding2(const char * src, char * dst)
{
	while (*src != 0 && *src == '0')
	{
		++src;
	}

	strcpy(dst, src);
}

int read_int(const char * start, int len)
{
	int res = 0;
	for (int i = 0; i < len; ++i)
	{
		if (isdigit(start[i]))
		{
			res *= 10;
			res += start[i] - '0';
		}
	}
	return res;
}

int findChar(const char * buffer, char delim, int len)
{
	int index = 0;
	while (index < len)
	{
		if (buffer[index] == delim)
		{
			return index;
		}
		else
		{
			++index;
		}
	}
	return -1;
}

void random_string(char * out, int len)
{
	for (int i = 0; i < len; ++i)
	{
		out[i] = alphabet[rand() % (sizeof(alphabet) - 1)];
	}

	out[len] = 0;
}

size_t convert_datetime_shamsi(const char * i_date, const char * i_fmt, const char * out_fmt, char * out, int len)
{
	struct tm t;
	sscanf(i_date, i_fmt, &t.tm_year, &t.tm_mon, &t.tm_mday, &t.tm_hour, &t.tm_min, &t.tm_sec);
	t.tm_year -= 1900;
	t.tm_mon -= 1;
	mktime(&t);
	change_date_to_persian(&t);
	return strftime(out, len, out_fmt, &t);
}

size_t convert_datetime(const char * i_date, const char * i_fmt, const char * out_fmt, char * out, int len)
{
	struct tm t;
	sscanf(i_date, i_fmt, &t.tm_year, &t.tm_mon, &t.tm_mday, &t.tm_hour, &t.tm_min, &t.tm_sec);
	t.tm_year -= 1900;
	t.tm_mon -= 1;
	mktime(&t);
	return strftime(out, len, out_fmt, &t);
}

int miladi_is_leap(int year)
{
	return ((year % 100) != 0 && (year % 4) == 0) || ((year % 100) == 0 && (year % 400) == 0);
}

void change_date_to_persian(struct tm * t)
{
	int dayCount, deyDayDiff;
	t->tm_year += 1900;
	++t->tm_mon;

	if (miladi_is_leap(t->tm_year))
	{
		dayCount = sumDayMiladiMonthLeap[t->tm_mon - 1];
	}
	else
	{
		dayCount = sumDayMiladiMonth[t->tm_mon - 1];
	}

	dayCount += t->tm_mday;

	if (miladi_is_leap(t->tm_year - 1))
	{
		deyDayDiff = 11;
	}
	else
	{
		deyDayDiff = 10;
	}

	if(dayCount > 79)
	{
		dayCount -= 79;

		if(dayCount <= 186)
		{
			t->tm_mon = dayCount / 31;

			if ((dayCount % 31) == 0)
			{
				t->tm_mday = 31;
			}
			else
			{
				++t->tm_mon;
				t->tm_mday = (dayCount % 31);
			}
		}
		else
		{
			dayCount -= 186;
			t->tm_mon = (dayCount / 30);

			if ((dayCount % 30) == 0)
			{
				t->tm_mon += 6;
				t->tm_mday = 30;
			}
			else
			{
				t->tm_mon += 7;
				t->tm_mday = (dayCount % 30);
			}
		}

		t->tm_year -= 621;
	}
	else
	{
		dayCount += deyDayDiff;
		t->tm_mon = (dayCount / 30);

		if ((dayCount % 30) == 0)
		{
			t->tm_mon += 9;
			t->tm_mday = 30;
		}
		else
		{
			t->tm_mon += 10;
			t->tm_mday = (dayCount % 30);
		}

		t->tm_year -= 622;
	}

	t->tm_year -= 1900;
	--t->tm_mon;
}

void change_date_to_gregorian(struct tm * t)
{
	t->tm_year += 1900;
	++t->tm_mon;

	int gy, gd, gm;
	int doy_j = (t->tm_mon < 7) ? (((t->tm_mon - 1) * 31) + t->tm_mday) : (((t->tm_mon - 7) * 30) + 186 + t->tm_mday);
	int d_4 = (t->tm_year + 1) & 0x03;
	int d_33 = (((t->tm_year - 55) % 132) * 10000) / 305;
	int a = (d_33 != 3 && d_4 <= d_33) ? 287 : 286;
	int b = ((d_33 == 1 || d_33 == 2) && (d_33 == d_4 || d_4 == 1)) ? 78 : ((d_33 == 3 && d_4 == 0) ? 80 : 79);

	if (((t->tm_year - 19) / 63) == 20)
	{
		--a;
		++b;
	}

	if (doy_j <= a)
	{
		gy = t->tm_year + 621;
		gd = doy_j + b;
	}
	else
	{
		gy = t->tm_year + 622;
		gd = doy_j - a;
	}

	for (gm = 0; gm < ARRAY_SIZE(miladiMonthDays); ++gm)
	{
		int delta = miladiMonthDays[gm] + ((gm == 2 && ((gy & 0x03) == 0)) ? 1 : 0);
		if (gd <= delta)
		{
			break;
		}

		gd -= delta;
	}

	t->tm_year = gy;
	t->tm_mon = gm;
	t->tm_mday = gd;

	t->tm_year -= 1900;
	--t->tm_mon;
}

size_t get_miladi_date(const char * fmt, char * out, int len)
{
	struct tm now;
	time_t t = time(NULL);
	localtime_r(&t, &now);
	return strftime(out, len, fmt, &now);
}

size_t get_shamsi_date(const char * fmt, char * out, int len)
{
	struct tm now;
	time_t t = time(NULL);
	localtime_r(&t, &now);
	change_date_to_persian(&now);
	return strftime(out, len, fmt, &now);
}

int iransystem_to_utf8_index(int iransystem_index)
{
	if (iransystem_index <= 0x7F)
	{
		return -1;
	}
	else if (iransystem_index <= 0xAF)
	{
		return iransystem_index - 0x80;
	}
	else if (iransystem_index >= 0xE0 && iransystem_index <= 0xFF)
	{
		return iransystem_index - 0xB0;
	}
	else
	{
		return 79;
	}
}

int windows1256_to_utf8(char * dst, char * src, int len)
{
	int iransystem_index = 0;
	int i_dst = 0;

	for (int i_src = 0; i_src < len; ++i_src)
	{
		unsigned char c = src[i_src];

		if (c <= 0x7F)
		{
			dst[i_dst++] = c;
			continue;
		}
		else if (c == 0x81) c = 0x94;
		else if (c == 0x8D) c = 0x9C;
		else if (c == 0x8E) c = 0xA6;
		else if (c == 0x90) c = 0xEF;
		else if (c == 0x98 || c == 0xDF) c = 0xED;
		else if (c == 0xA1) c = 0x8A;
		else if (c == 0xAA) c = 0xFB;
		else if (c == 0xBF) c = 0x8C;
		else if (c == 0xC0 || c == 0xC9 || c == 0xE5) c = 0xF9;
		else if (c == 0xC1) c = 0x8F;
		else if (c == 0xC2 || c == 0xC3 || c == 0xC5 || c == 0xC7) c = 0x91;
		else if (c == 0xC4 || c == 0xE6) c = 0xF8;
		else if (c == 0xC6 || c == 0xEC || c == 0xED) c = 0xFD;
		else if (c == 0xC8) c = 0x92;
		else if (c == 0xCA) c = 0x96;
		else if (c == 0xCB) c = 0x98;
		else if (c == 0xCC) c = 0x9A;
		else if (c == 0xCD) c = 0x9E;
		else if (c == 0xCE) c = 0xA0;
		else if (c == 0xCF) c = 0xA2;
		else if (c == 0xD0) c = 0xA3;
		else if (c == 0xD1 || c == 0x9A) c = 0xA4;
		else if (c == 0xD2) c = 0xA5;
		else if (c == 0xD3) c = 0xA7;
		else if (c == 0xD4) c = 0xA9;
		else if (c == 0xD5) c = 0xAB;
		else if (c == 0xD6) c = 0xAD;
		else if (c == 0xD8) c = 0xAF;
		else if (c == 0xD9) c = 0xE0;
		else if (c == 0xDA) c = 0xE1;
		else if (c == 0xDB) c = 0xE5;
		else if (c == 0xDC) c = 0x8B;
		else if (c == 0xDD) c = 0xE9;
		else if (c == 0xDE) c = 0xEB;
		else if (c == 0xE1) c = 0xF1;
		else if (c == 0xE3) c = 0xF4;
		else if (c == 0xE4) c = 0xF6;
		else c = 0XFF;

		iransystem_index = iransystem_to_utf8_index(c);
		dst[i_dst++] = iransystem_arabic_table[iransystem_index][0];
		if (iransystem_arabic_table[iransystem_index][1])
		{
			dst[i_dst++] = iransystem_arabic_table[iransystem_index][1];
		}
	}

	dst[i_dst] = 0;
	return i_dst;
}

int iransystem_to_utf_8(unsigned char * dst, unsigned char * src, int len)
{
	int i_dst = 0;
	for (int i_src = 0; i_src < len; ++i_src)
	{
		unsigned char c = src[i_src];

		if (c <= 0x7F)
		{
			dst[i_dst++] = c;
		}
		else if (c >= 0x80 && c <= 0xAF)
		{
			c -= 0x80;
			dst[i_dst++] = iransystem_arabic_table[c][0];
			if (iransystem_arabic_table[c][1])
			{
				dst[i_dst++] = iransystem_arabic_table[c][1];
			}
		}
		else if (c == 0xF2)
		{
			dst[i_dst++] = iransystem_arabic_table[0xF1 - 0xB0][0];
			dst[i_dst++] = iransystem_arabic_table[0xF1 - 0xB0][1];

			dst[i_dst++] = iransystem_arabic_table[0x91 - 0x80][0];
			dst[i_dst++] = iransystem_arabic_table[0x91 - 0x80][1];
		}
		else if (c >= 0xE0 && c <= 0xFE)
		{
			c -= 0xB0;
			dst[i_dst++] = iransystem_arabic_table[c][0];
			if (iransystem_arabic_table[c][1])
			{
				dst[i_dst++] = iransystem_arabic_table[c][1];
			}
		}
		else
		{
			dst[i_dst++] = ' ';
		}
	}

	dst[i_dst] = 0;
	return i_dst;
}

int convert_iransystem_to_utf8(char * out, const char * in_hex_str, int reverse)
{
	char temp[200];
	int len = strlen(in_hex_str);
	unhexify(temp, in_hex_str, len);
	len >>= 1;

	if (reverse)
	{
		reverse_array(temp, 0, len - 1);
	}

	return iransystem_to_utf_8(out, temp, len);
}

void passwd_supervisor(const char * terminal, char * out)
{
	char current_password[50] = {0};
	char time[50] = {0};
	int year = 0, month = 0, day = 0;

	strcpy(current_password, terminal);
	int start = 0, end = strlen(current_password) - 1;
	get_shamsi_date("%Y/%m/%d", time, sizeof(time));
	sscanf(time, "%d/%d/%d", &year, &month, &day);

	while (start < end)
	{
		char t = current_password[start];
		current_password[start] = current_password[end];
		current_password[end] = t;
		++start;
		--end;
	}

	int result = atoi(current_password) + 56297;
	result += year + month + day;
	sprintf(out, "%d", result);
}

void passwd_reset(const char * terminal, char * out)
{
	char time[50] = {0};
	int year = 0, month = 0, day = 0;
	unsigned long long b = atoll(terminal);

	get_shamsi_date("%Y/%m/%d", time, sizeof(time));
	sscanf(time, "%d/%d/%d", &year, &month, &day);
	b *= (year + month + day);
	b %= year;
	sprintf(out, "%llu", b);
}

void passwd_debug(const char * terminal, int menu, char * out)
{
	char time[30] = {0};
	char temp[30] = {0};

	get_shamsi_date("%Y%m%d", time, sizeof(time));
	sprintf(temp, "3%d3%d", menu / 10, menu % 10);
	get_shamsi_date("%H%M", temp + 4, sizeof(temp) - 4);
	sprintf(out, "%08d", atoi(time) ^ atoi(temp) ^ atoi(terminal));
}

int passwd_debug_check(const char * terminal, const char * pin, int * menu)
{
	char temp[30] = {0};
	char temp1[30] = {0};

	get_shamsi_date("%Y%m%d", temp, sizeof(temp));
	sprintf(temp, "%08d", atoi(pin) ^ atoi(terminal) ^ atoi(temp));
	get_miladi_date("%H%M", temp1, sizeof(temp1));

	if (strcmp(temp1, temp + 4) == 0)
	{
		*menu = ((temp[1] - '0') * 10) + (temp[3] - '0');
		return 0;
	}
	else
	{
		return -1;
	}
}

// todo, skip for dns
int validate_ip_string(const char * ip, int subnet)
{
	const char * s = ip;
	int dots = 0, a, b, c, d;

	while (*s)
	{
		if (*s == '.')
		{
			++dots;
		}

		++s;
	}

	if (dots != 3)
	{
		return -1;
	}

	if (sscanf(ip, "%d.%d.%d.%d", &a, &b, &c, &d) != 4)
	{
		return -1;
	}

	if (a > 255 || b > 255 || c > 255 || d > 255)
	{
		return -1;
	}

	if (a == 0 && b == 0 && c == 0 && d == 0)
	{
		return -1;
	}

	if (a == 255 && b == 255 && c == 255 && d == 255)
	{
		return -1;
	}

	if (a == 127)
	{
		return -1;
	}

	if (subnet)
	{
		uint32_t mask = (a << 24) | (b << 16) | (c << 8) | d;
		if (mask == 0)
		{
			return -1;
		}

		if (mask & (~mask >> 1))
		{
			return -1;
		}
	}

	return 0;
}

char * rgba_to_bitmap(const unsigned char * rgba, int w, int h, int ref_value)
{
	int index = 0;
	int rgba_ptr = 0;
	int row_bytes = (w / 8) + ((w % 8) ? 1 : 0);
	int print_buffer_size = 1 + (h * (2 + row_bytes));

	char * bitmap = malloc(print_buffer_size);
	if (bitmap == NULL)
	{
		return NULL;
	}

	memset(bitmap, 0, print_buffer_size);
	bitmap[index++] = h & 0xFF;

	for (int y = 0; y < h; ++y)
	{
		bitmap[index++] = (row_bytes >> 8) & 0xFF;
		bitmap[index++] = row_bytes & 0xFF;

		for (int x = 0; x < w; ++x, rgba_ptr += 4)
		{
			if (rgba[rgba_ptr] <= ref_value &&
				rgba[rgba_ptr + 1] <= ref_value &&
				rgba[rgba_ptr + 2] <= ref_value)
			{
				bitmap[index + (x >> 3)] |= (1 << (7 - (x & 0x07)));
			}
		}

		index += row_bytes;
	}

	return bitmap;
}

char get_next_keypad_char(char in)
{
	int found = 0;
	for (int key = 0; key < 10; ++key)
	{
		const char * s = keypad_map[key];

		while (*s)
		{
			if (*s == in)
			{
				found = 1;
			}
			else if (found)
			{
				return *s;
			}

			++s;
		}

		if (found)
		{
			return keypad_map[key][0];
		}
	}

	return '-';
}