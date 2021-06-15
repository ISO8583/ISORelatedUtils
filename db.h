/*
	Creative Commons Attribution-NonCommercial-NoDerivs (CC-BY-NC-ND)
	https://creativecommons.org/licenses/by-nc-nd/4.0/
	The most restrictive creative commons license.
	This only allows people to download and share your work for no commercial gain and for no other purposes.
*/


#ifndef _DB_H
#define _DB_H 1

#define DB_MAX_LOG_LEN				1000

typedef int (*db_simple_callback) (const char * str);
typedef int (*db_column_log_callback) (long long int row, const char * time, const char * text, void * arg);
typedef int (*db_sqlite_callback) (void * arg, int column_count, char ** column_value, char ** column_name);

int db_dump_callback(void * arg, int c, char ** value, char ** col);

int db_get_version(int * ver);
int db_set_version(int ver);

int db_init();
int db_shutdown();
int db_is_threadsafe();
int db_open(const char * path);
int db_migrate();
int db_setup_connection();
int db_has_connection();
int db_prepare();
int db_close();
int db_vacuum();
int db_repair(const char * path);
int db_backup(const char * path);
int db_get_file_size();
int db_perform_exec(db_sqlite_callback c, const char * query);
int db_raw_query(db_sqlite_callback c, const char * fmt, ...);

int db_integrity_check(db_simple_callback c);
int db_sqlite_flags(db_simple_callback c);
void db_set_error_callback(db_simple_callback c);

// Setting
int db_insert_setting_str(const char * key, const char * value);
int db_insert_setting_int(const char * key, int value);
int db_remove_setting(const char * key);
int db_get_setting(const char * key, char * out);
int db_get_setting_str(const char * key, const char * def, char * out);
int db_get_setting_int(const char * key, int def, int * out);
int db_clear_setting();
int db_get_all_settings(db_sqlite_callback c, void * arg);

// Log
int db_insert_log(const char * fmt, ...);
int db_rotate_log(int remaining);
int db_get_log(int page, int count_per_page, db_column_log_callback callback, void * arg);

#endif // _DB_H
