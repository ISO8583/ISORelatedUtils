/*
	Creative Commons Attribution-NonCommercial-NoDerivs (CC-BY-NC-ND)
	https://creativecommons.org/licenses/by-nc-nd/4.0/
	The most restrictive creative commons license.
	This only allows people to download and share your work for no commercial gain and for no other purposes.
*/

#include "models.h"


int config_load(config_item_t * cf)
{
	if (cf->type == CONFIG_INTEGER)
	{
		return db_get_setting_int(cf->db_key, cf->integer_init, &cf->integer);
	}
	else if (cf->type == CONFIG_STRING)
	{
		return db_get_setting_str(cf->db_key, cf->str_init, cf->str);
	}
	else
	{
		return -1;
	}
}

int config_save(const config_item_t * cf)
{
	if (cf->type == CONFIG_INTEGER)
	{
		return db_insert_setting_int(cf->db_key, cf->integer);
	}
	else if (cf->type == CONFIG_STRING)
	{
		return db_insert_setting_str(cf->db_key, cf->str);
	}
	else
	{
		return -1;
	}
}
