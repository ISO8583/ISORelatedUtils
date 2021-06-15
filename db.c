/*
	Creative Commons Attribution-NonCommercial-NoDerivs (CC-BY-NC-ND)
	https://creativecommons.org/licenses/by-nc-nd/4.0/
	The most restrictive creative commons license.
	This only allows people to download and share your work for no commercial gain and for no other purposes.
*/


#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <signal.h>

#include <sqlite3.h>

#include "db.h"
#include "util.h"
#include <log.h>

/**
 * Recommended Compile Options:
 * SQLITE_CORE
 * SQLITE_ENABLE_UPDATE_DELETE_LIMIT
 * SQLITE_ENABLE_STMT_SCANSTATUS
 * SQLITE_ENABLE_EXPLAIN_COMMENTS
 * SQLITE_ENABLE_COLUMN_METADATA
 *
 * SQLITE_ENABLE_STAT4
 */

// ---------- SQLITE Extension Definition --------------

int sqlite3_eval_init(
		sqlite3 *db,
		char **pzErrMsg,
		const sqlite3_api_routines *pApi
);

int sqlite3_sha_init(
		sqlite3 *db,
		char **pzErrMsg,
		const sqlite3_api_routines *pApi
);

typedef void(*xEntryPoint)(void);

static const xEntryPoint sqlite_extension_list[] = {
		(xEntryPoint) sqlite3_eval_init,
		(xEntryPoint) sqlite3_sha_init,
};

// ---------------- General SQL ----------------------

static const char sql_get_version[] =
		"PRAGMA user_version;";

static const char sql_set_version[] =
		"PRAGMA user_version = %d;";

static const char sql_integrity_check[] =
		"PRAGMA integrity_check;";

static const char sql_sqlite_flags[] =
		"PRAGMA compile_options;";

static const char sql_vacuum[] =
		"VACUUM ;";

static const char sql_file_size[] =
		"SELECT page_count * page_size as size FROM pragma_page_count(), pragma_page_size();";

// ---------------- Setting Table ----------------------

static const char sql_table_setting[] =
		"CREATE TABLE IF NOT EXISTS `settings` ("
		"`key` TEXT NOT NULL UNIQUE,"
		"`value` TEXT);";

static const char sql_insert_setting[] =
		"REPLACE INTO `settings` (`key`, `value`) VALUES (?, ?);";

static const char sql_get_setting[] =
		"SELECT `value` FROM `settings` WHERE `key` = ? LIMIT 1;";

static const char sql_remove_setting[] =
		"DELETE FROM `settings` WHERE `key` LIKE ?;";

static const char sql_clear_setting[] =
		"DELETE FROM `settings` ;";

static const char sql_get_all_settings[] =
		"SELECT JSON_GROUP_ARRAY(JSON_OBJECT('rowid', rowid, 'key', key, 'value', value)) FROM `settings` ;";

// ---------------- Transaction Table ----------------------

static const char sql_table_transaction[] =
		"CREATE TABLE IF NOT EXISTS `transactions` ("
		"`id` INTEGER NOT NULL,"
		"`type` INTEGER,"
		"`stan` INTEGER,"
		"`amount` INTEGER,"
		"`datetime` TEXT,"
		"`datetime_fa` TEXT,"
		"`track2` TEXT,"
		"`track2_hash` TEXT,"
		"`terminal` TEXT,"
		"`merchant` TEXT,"
		"`shift` INTEGER DEFAULT 0,"
		"`bill_id` TEXT,"
		"`pay_id` TEXT,"
		"`phone` TEXT,"
		"`logo` INTEGER,"
		"`server` INTEGER,"
		"`additional_flag` INTEGER,"
		"`voucher_operator` INTEGER,"
		"`voucher_code` INTEGER,"
		"`voucher_type` INTEGER,"
		"`need_verify` INTEGER,"
		"`need_reverse` INTEGER,"
		"`rrn` TEXT,"
		"`rrn_verify` TEXT,"
		"`rrn_reverse` TEXT,"
		"`resp` TEXT,"
		"`resp_verify` TEXT,"
		"`resp_reverse` TEXT,"
		"`resp_datetime` TEXT,"
		"`resp_datetime_fa` TEXT,"
		"`resp_f48` TEXT,"
		"PRIMARY KEY (`id` AUTOINCREMENT));";

// ---------------- Log Queries ----------------------

static const char sql_table_log[] =
		"CREATE TABLE IF NOT EXISTS `logs` ("
		"`time` TEXT NOT NULL,"
		"`text` TEXT NOT NULL);";

//static const char sql_table_log_drop[] =
//		"DROP TABLE IF EXISTS `logs`;";

static const char sql_insert_log[] =
		"INSERT INTO `logs` (`time`, `text`) VALUES (STRFTIME('%Y-%m-%d %H:%M:%f', 'NOW', 'LOCALTIME'), ?) ;";

// SQLITE_ENABLE_UPDATE_DELETE_LIMIT
static const char sql_rotate_log[] =
		"DELETE FROM `logs` ORDER BY `ROWID` DESC LIMIT -1 OFFSET ? ;";
//		"DELETE FROM `logs` WHERE `ROWID` NOT IN (SELECT `ROWID` FROM `logs` ORDER BY `ROWID` DESC LIMIT ?) ;";

static const char sql_select_log[] =
		"SELECT `ROWID`, `time`, `text` from `logs` ORDER BY ROWID DESC LIMIT ? OFFSET ? ;";

// ---------------- Migration Queries ----------------------

static const char sql_table_migration[] =
		"CREATE TABLE IF NOT EXISTS `migration` ("
		"`version` INTEGER NOT NULL,"
		"`step` INTEGER NOT NULL,"
		"`forward` TEXT,"
		"`rollback` TEXT,"
		"PRIMARY KEY (`version`, `step`)) WITHOUT ROWID;";

//static const char sql_table_migration_drop[] =
//		"DROP TABLE IF EXISTS `migration`;";

static const char sql_migration_insert[] =
		"REPLACE INTO `migration` (`version`, `step`, `forward`, `rollback`) VALUES (?, ?, ?, ?);";

//static const char sql_migration_get_rollback[] =
//		"SELECT `version`, `step`, `rollback` FROM `migration` WHERE `version` >= ? AND `rollback` ORDER BY `version` DESC, `step` DESC;";

static const char sql_migration_get_rollback_inplace[] =
		"SELECT EVAL(`rollback`) FROM `migration` WHERE `version` >= ? AND `rollback` NOT NULL ORDER BY `version` DESC, `step` DESC;";

// ---------------- Macros ----------------------

#define MIGRATION_ROW(x)						{ARRAY_SIZE(x), (x)}

#define SQLITE_REQUIRED_OPEN(db) \
	do { \
		if ((db) == NULL) \
		{ \
			LOG(LOG_ERR, "sqlite3 object is null"); \
			return -1; \
		} \
	} while (0)

#define SQLITE_BIND_STRING(stm, index, std_string) \
	if ((std_string) != NULL && (std_string)[0] != 0) \
		sqlite3_bind_text(stm, index, std_string, strlen(std_string), SQLITE_STATIC); \
	else \
		sqlite3_bind_null(stm, index)

#define SQLITE_CHECK_PREPARE(...) \
	do { \
		int _ = sqlite3_prepare_v2(__VA_ARGS__); \
		if (_ != SQLITE_OK) \
		{ \
			get_error((_)); \
			return -1; \
		} \
		else \
		{ \
			++db_connections; \
		} \
	} while (0)

#define SQLITE_CHECK_FINALIZE(stm) \
	do { \
		sqlite3_finalize(stm); \
		--db_connections; \
	} while (0)

// ---------------- Static Variables ----------------------

static sqlite3 * db = NULL;
static sig_atomic_t db_connections = 0;
static db_simple_callback call_back = NULL;

static const char * sql_init_library[] = {
//		"PRAGMA journal_mode = OFF;", // DELETE, TRUNCATE, PERSIST, MEMORY, WAL, OFF
		"PRAGMA synchronous = NORMAL;", // OFF, NORMAL, FULL, EXTRA
//		"PRAGMA cache_size = 1000000;",
//		"PRAGMA locking_mode = EXCLUSIVE;",
//		"PRAGMA temp_store = MEMORY;",
};

// ------------------- Migrations -------------------------

typedef struct
{
	const char * const forward;
	const char * const rollback;
} db_migration_step_t;

// latest table schema of database
static const db_migration_step_t sql_migration_0_to_1[] = {
		{sql_table_log, NULL /*sql_table_log_drop*/},
		{sql_table_migration, NULL /*sql_table_migration_drop*/},
		{sql_table_setting, NULL /*"DROP TABLE IF EXISTS `settings`;"*/
};

static const struct {
	int count;
	const db_migration_step_t * const step;
} db_migrations[] = {
		MIGRATION_ROW(sql_migration_0_to_1),
};

// --------------------------------------------------------

void sqlite_get_string_column(sqlite3_stmt * stm, int i, char * dst)
{
	const char * tmp = sqlite3_column_text(stm, i);
	if (sqlite3_column_bytes(stm, i) == 0)
	{
		dst[0] = 0;
	}
	else
	{
		strcpy(dst, tmp);
	}
}

// ---------------- Custom SQL Functions ----------------------

int db_dump_callback(void * arg, int c, char ** value, char ** col)
{
	for (int i = 0; i < c; ++i)
	{
		printf("%s -> %s, ", col[i], value[i]);
	}
	printf("\n");
	return 0;
}

// ---------------- Functions ----------------------

void db_set_error_callback(db_simple_callback c)
{
	call_back = c;
}

void get_error(int err)
{
	char error[DB_MAX_LOG_LEN];

	if (db)
	{
		snprintf(error, sizeof(error), "Sqlite (%d) %s", sqlite3_errcode(db), sqlite3_errmsg(db));
	}
	else
	{
		snprintf(error, sizeof(error), "Sqlite (%d) %s", err, sqlite3_errstr(err));
	}

	LOG(LOG_ERR, error);

	if (call_back)
	{
		call_back(error);
	}
}

int db_init()
{
	return sqlite3_initialize() == SQLITE_OK;
}

int db_shutdown()
{
	return sqlite3_shutdown() == SQLITE_OK;
}

int db_is_threadsafe()
{
	return sqlite3_threadsafe() == 1;
}

int db_has_connection()
{
	return db_connections != 0;
}

int db_open(const char * path)
{
	int res = SQLITE_ERROR;
	if (db == NULL)
	{
		res = sqlite3_open_v2(path, &db, SQLITE_OPEN_FULLMUTEX | SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE, NULL);
	}
	else
	{
		LOG(LOG_ERR, "one sqlite instance is already open");
		return -1;
	}

	if (res == SQLITE_OK)
	{
		if (db_setup_connection() != 0)
		{
			db_close();
			return -1;
		}

		return 0;
	}
	else
	{
		get_error(res);
		return -1;
	}
}

int db_prepare()
{
	for (int i = 0; i < ARRAY_SIZE(sqlite_extension_list); ++i)
	{
		LOG(LOG_DEBUG, "sqlite auto load extension at %d", i);
		int rc = sqlite3_auto_extension(sqlite_extension_list[i]);
		if (rc != SQLITE_OK)
		{
			LOG(LOG_ERR, "can not register extension at %d. %s", i, sqlite3_errstr(rc));
			while (i > 0)
			{
				sqlite3_cancel_auto_extension(sqlite_extension_list[--i]);
			}

			return -1;
		}
	}

	return 0;
}

int db_setup_connection()
{
	int res = SQLITE_ERROR;
	SQLITE_REQUIRED_OPEN(db);

	LOG(LOG_DEBUG, "enable auto load extension for new db connection");
	//sqlite3_db_config(db, SQLITE_DBCONFIG_ENABLE_LOAD_EXTENSION, 1, NULL)
	sqlite3_enable_load_extension(db, 1);

	for (size_t i = 0; i < ARRAY_SIZE(sql_init_library); ++i)
	{
		if (db_perform_exec(NULL, sql_init_library[i]) < 0)
		{
			return -1;
		}
	}

	return 0;
}

int db_close()
{
	int res = 0;

	if (db != NULL)
	{
		sqlite3_db_cacheflush(db);
		res = sqlite3_close(db);
		if (res == SQLITE_OK)
		{
			db = NULL;
		}
		else
		{
			get_error(res);
			res = -1;
		}
	}

	return res;
}

void db_log_query(sqlite3_stmt * stm)
{
	if (log_get_level() == LOG_DEBUG)
	{
		char * sql = sqlite3_expanded_sql(stm);
		if (sql)
		{
			LOG(LOG_DEBUG, "%s", sql);
			sqlite3_free(sql);
		}
	}
}

int db_perform_single_step(sqlite3_stmt * stm)
{
	db_log_query(stm);
	int res = sqlite3_step(stm);
	if (res == SQLITE_DONE)
	{
		res = 0;
	}
	else
	{
		get_error(res);
		res = -1;
	}

	SQLITE_CHECK_FINALIZE(stm);
	return res;
}

int db_perform_exec(db_sqlite_callback c, const char * query)
{
	if (query == NULL)
	{
		return 0;
	}

	SQLITE_REQUIRED_OPEN(db);
	LOG(LOG_DEBUG, query);

	++db_connections;
	int res = sqlite3_exec(db, query, c, NULL, NULL);
	--db_connections;
	if (res != SQLITE_OK)
	{
		get_error(res);
		return -1;
	}

	return 0;
}

int db_raw_query(db_sqlite_callback c, const char * fmt, ...)
{
	if (fmt == NULL)
	{
		return -1;
	}

	SQLITE_REQUIRED_OPEN(db);

	char buffer[DB_MAX_LOG_LEN] = {0};
	va_list args;
	va_start(args, fmt);
	vsnprintf(buffer, DB_MAX_LOG_LEN, fmt, args);
	va_end (args);

	return db_perform_exec(c, buffer);
}

int db_check_result_done(sqlite3_stmt * stm, int res)
{
	if (res != SQLITE_DONE)
	{
		get_error(res);
		res = -1;
	}
	else
	{
		res = 0;
	}

	SQLITE_CHECK_FINALIZE(stm);
	return res;
}

int db_insert_migrations(int version, int step, const char * forward, const char * rollback)
{
	sqlite3_stmt * stm = NULL;
	SQLITE_REQUIRED_OPEN(db);
	SQLITE_CHECK_PREPARE(db, sql_migration_insert, -1, &stm, NULL);

	sqlite3_bind_int(stm, 1, version);
	sqlite3_bind_int(stm, 2, step);
	SQLITE_BIND_STRING(stm, 3, forward);
	SQLITE_BIND_STRING(stm, 4, rollback);

	db_log_query(stm);
	int res = sqlite3_step(stm);
	if (res == SQLITE_DONE)
	{
		res = 0;
	}
	else
	{
		get_error(res);
		res = -1;
	}

	SQLITE_CHECK_FINALIZE(stm);
	return res;
}

int db_perform_migration(int index)
{
	if (index < 0)
	{
		return -1;
	}

	SQLITE_REQUIRED_OPEN(db);

	for (int i = 0; i < db_migrations[index].count; ++i)
	{
		if (db_perform_exec(NULL, db_migrations[index].step[i].forward) < 0)
		{
			LOG(LOG_ERR, "forward failed on version %d, step %d", index, i);
			for (; i >= 0; --i)
			{
				if (db_perform_exec(NULL, db_migrations[index].step[i].rollback) < 0)
				{
					LOG(LOG_ERR, "rollback failed on version %d, step %d", index, i);
					return -1;
				}
			}

			return -1;
		}
	}

	return 0;
}

int db_perform_rollback(int to_version)
{
	if (to_version < 0)
	{
		return -1;
	}

	int res;
	sqlite3_stmt * stm = NULL;
	SQLITE_REQUIRED_OPEN(db);
	SQLITE_CHECK_PREPARE(db, sql_migration_get_rollback_inplace , -1, &stm, NULL);
	sqlite3_bind_int(stm, 1, to_version);

	db_log_query(stm);
	while ((res = sqlite3_step(stm)) == SQLITE_ROW)
	{
//		int version = sqlite3_column_int(stm, 0);
//		int step = sqlite3_column_int(stm, 1);
//		const char * query = (const char *) sqlite3_column_text(stm, 2);
//		LOG(LOG_INFO, "rollback from version(%d), step(%d)", version, step);
//
//		if (query != NULL)
//		{
//			if (db_perform_exec(NULL, query) < 0)
//			{
//				sqlite3_finalize(stm);
//				--db_connections;
//				return -1;
//			}
//		}
	}

	if (res != SQLITE_DONE)
	{
		get_error(res);
		res = -1;
	}

	SQLITE_CHECK_FINALIZE(stm);
	return res;
}

int db_migrate()
{
	int app_db_version = ARRAY_SIZE(db_migrations);
	int res = 0;
	int v = 0;
	const char * sql = NULL;

	res = db_get_version(&v);
	LOG(LOG_DEBUG, "db version: %d", v);

	int insert_migration = 0;
	if (res == 0)
	{
		if (v == 0)
		{
			LOG(LOG_INFO, "apply latest schema");
			if (db_perform_migration(0) < 0)
			{
				return -1;
			}

			insert_migration = 1;
		}
		else if (v < app_db_version)
		{
			LOG(LOG_INFO, "migrate from %d to %d", v, app_db_version);
			for (int i = v; i < app_db_version; ++i)
			{
				if (db_perform_migration(i) < 0)
				{
					return -1;
				}
			}

			insert_migration = 1;
		}
		else if (v != app_db_version)
		{
			LOG(LOG_INFO, "rollback from %d to %d", app_db_version, v);
			if (db_perform_rollback(app_db_version) < 0)
			{
				return -1;
			}
		}

		if (insert_migration)
		{
			for (int i = 0; i < app_db_version; ++i)
			{
				for (int j = 0; j < db_migrations[i].count; ++j)
				{
					LOG(LOG_DEBUG, "inserting migration %d, %d", i, j);
					if (db_insert_migrations(i, j, db_migrations[i].step[j].forward, db_migrations[i].step[j].rollback))
					{
						return -1;
					}
				}
			}
		}

		if (v != app_db_version)
		{
			LOG(LOG_DEBUG, "db version set to: %d", app_db_version);
			res = db_set_version(app_db_version);
		}
	}

	return res;
}

int db_get_version(int * ver)
{
	int res = 0;
	sqlite3_stmt * stm = NULL;
	SQLITE_REQUIRED_OPEN(db);
	SQLITE_CHECK_PREPARE(db, sql_get_version, -1, &stm, NULL);

	db_log_query(stm);
	res = sqlite3_step(stm);
	if (res == SQLITE_ROW)
	{
		res = 0;
		*ver = sqlite3_column_int(stm, 0);
	}
	else if (res != SQLITE_DONE)
	{
		get_error(res);
		res = -1;
	}

	SQLITE_CHECK_FINALIZE(stm);
	return res;
}

int db_get_file_size()
{
	int res = 0;
	sqlite3_stmt * stm = NULL;
	SQLITE_REQUIRED_OPEN(db);
	SQLITE_CHECK_PREPARE(db, sql_file_size, -1, &stm, NULL);

	db_log_query(stm);
	res = sqlite3_step(stm);
	if (res == SQLITE_ROW)
	{
		res = sqlite3_column_int(stm, 0);
	}
	else if (res != SQLITE_DONE)
	{
		get_error(res);
		res = -1;
	}

	SQLITE_CHECK_FINALIZE(stm);
	return res;
}

int db_set_version(int ver)
{
	sqlite3_stmt * stm = NULL;
	char sql_buffer[100];
	sprintf(sql_buffer, sql_set_version, ver);
	SQLITE_REQUIRED_OPEN(db);
	SQLITE_CHECK_PREPARE(db, sql_buffer, -1, &stm, NULL);
	sqlite3_bind_int(stm, 1, ver);
	return db_perform_single_step(stm);
}

int db_vacuum()
{
	sqlite3_stmt * stm = NULL;
	SQLITE_REQUIRED_OPEN(db);
	SQLITE_CHECK_PREPARE(db, sql_vacuum , -1, &stm, NULL);
	return db_perform_single_step(stm);
}

int db_integrity_check(db_simple_callback c)
{
	int res = -1;
	sqlite3_stmt * stm = NULL;
	SQLITE_REQUIRED_OPEN(db);
	SQLITE_CHECK_PREPARE(db, sql_integrity_check , -1, &stm, NULL);

	db_log_query(stm);
	while ((res = sqlite3_step(stm)) == SQLITE_ROW)
	{
		const char * row = (const char *) sqlite3_column_text(stm, 0);
		if (row != NULL)
		{
			res = strcmp(row, "ok") != 0 ? 1 : 0;
			printf("%s\n", row);
			if (c != NULL)
			{
				c(row);
			}
		}
	}

	if (res != SQLITE_DONE)
	{
		get_error(res);
		res = -1;
	}

	SQLITE_CHECK_FINALIZE(stm);
	return res;
}

int db_backup(const char * path)
{
	int res = 0;
	sqlite3 * db_b = NULL;

	SQLITE_REQUIRED_OPEN(db);
	LOG(LOG_INFO, "backup started at %s", path);

	res = sqlite3_open_v2(path, &db_b, SQLITE_OPEN_FULLMUTEX | SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE, NULL);
	if (res != SQLITE_OK)
	{
		LOG(LOG_ERR, "db at `%s` cannot be open (%d) %s", path, res, sqlite3_errstr(res));
		return -1;
	}

	sqlite3_backup * backup = sqlite3_backup_init(db_b, "main", db, "main");
	if (backup == NULL)
	{
		LOG(LOG_ERR, "db backup init failed (%d) %s", sqlite3_errcode(db_b), sqlite3_errmsg(db_b));
		sqlite3_close(db_b);
		return -1;
	}

	res = sqlite3_backup_step(backup, -1);
	if (res != SQLITE_OK && res != SQLITE_DONE)
	{
		LOG(LOG_ERR, "db backup step failed (%d) %s", sqlite3_errcode(db_b), sqlite3_errmsg(db_b));
		sqlite3_backup_finish(backup);
		sqlite3_close(db_b);
		return -1;
	}

	res = sqlite3_backup_finish(backup);
	sqlite3_close(db_b);
	if (res != SQLITE_OK && res != SQLITE_DONE)
	{
		LOG(LOG_ERR, "db backup step failed (%d) %s", sqlite3_errcode(db_b), sqlite3_errmsg(db_b));
		return -1;
	}

	LOG(LOG_INFO, "backup finished");
	return 0;
}

int db_repair(const char * path)
{
	SQLITE_REQUIRED_OPEN(db);

	sqlite3_recover * recovery = sqlite3_recover_init(db, NULL, path);
	if (recovery == NULL)
	{
		return -1;
	}

	int free_list = 1;
	int row_id_recover = 0;
	int slow_index = 0;

	sqlite3_recover_config(recovery, SQLITE_RECOVER_LOST_AND_FOUND, "lost-and-found");
	sqlite3_recover_config(recovery, SQLITE_RECOVER_FREELIST_CORRUPT, (void *) &free_list);
	sqlite3_recover_config(recovery, SQLITE_RECOVER_ROWIDS, (void *) &row_id_recover);
	sqlite3_recover_config(recovery, SQLITE_RECOVER_SLOWINDEXES, (void *) &slow_index);

	int res = sqlite3_recover_run(recovery);
	if (res != SQLITE_OK)
	{
		const char * err = sqlite3_recover_errmsg(recovery);
		if (err)
		{
			LOG(LOG_ERR, err);
			if (call_back)
			{
				call_back(err);
			}
		}
	}

	sqlite3_recover_finish(recovery);

	if (res != SQLITE_OK)
	{
		remove(path);
		return -1;
	}

	return 0;
}

int db_sqlite_flags(db_simple_callback c)
{
	int res = -1;
	sqlite3_stmt * stm = NULL;
	SQLITE_REQUIRED_OPEN(db);
	SQLITE_CHECK_PREPARE(db, sql_sqlite_flags, -1, &stm, NULL);

	db_log_query(stm);
	while ((res = sqlite3_step(stm)) == SQLITE_ROW)
	{
		const char * row = (const char *) sqlite3_column_text(stm, 0);
		if (row != NULL)
		{
			c(row);
		}
	}

	if (res == SQLITE_DONE)
	{
		res = 0;
	}
	else
	{
		get_error(res);
		res = -1;
	}

	SQLITE_CHECK_FINALIZE(stm);
	return res;
}

int db_insert_log(const char * fmt, ...)
{
	va_list args;
	sqlite3_stmt * stm = NULL;
	SQLITE_REQUIRED_OPEN(db);
	SQLITE_CHECK_PREPARE(db, sql_insert_log, -1, &stm, NULL);
	char buffer[DB_MAX_LOG_LEN] = {0};
	va_start(args, fmt);
	vsnprintf(buffer, DB_MAX_LOG_LEN, fmt, args);
	va_end (args);
	SQLITE_BIND_STRING(stm, 1, buffer);
	return db_perform_single_step(stm);
}

int db_insert_log_raw(const char * buffer)
{
	va_list args;
	sqlite3_stmt * stm = NULL;
	SQLITE_REQUIRED_OPEN(db);
	SQLITE_CHECK_PREPARE(db, sql_insert_log, -1, &stm, NULL);
	SQLITE_BIND_STRING(stm, 1, buffer);
	return db_perform_single_step(stm);
}

int db_rotate_log(int remaining)
{
	sqlite3_stmt * stm = NULL;
	SQLITE_REQUIRED_OPEN(db);
	SQLITE_CHECK_PREPARE(db, sql_rotate_log, -1, &stm, NULL);
	sqlite3_bind_int(stm, 1, remaining);
	return db_perform_single_step(stm);
}

int db_get_log(int page, int count_per_page, db_column_log_callback callback, void * arg)
{
	int res = 0;
	sqlite3_stmt * stm = NULL;
	SQLITE_REQUIRED_OPEN(db);
	SQLITE_CHECK_PREPARE(db, sql_select_log, -1, &stm, NULL);
	sqlite3_bind_int(stm, 1, count_per_page);
	sqlite3_bind_int(stm, 2, count_per_page * (page - 1));

	db_log_query(stm);
	while ((res = sqlite3_step(stm)) == SQLITE_ROW)
	{
		if (callback(sqlite3_column_int64(stm, 0), sqlite3_column_text(stm, 1),
					 sqlite3_column_text(stm, 2), arg) != 0)
		{
			res = SQLITE_DONE;
			break;
		}
	}

	if (res == SQLITE_DONE)
	{
		res = 0;
	}
	else
	{
		get_error(res);
		res = -1;
	}

	SQLITE_CHECK_FINALIZE(stm);
	return res;
}


int db_remove_setting(const char * key)
{
	sqlite3_stmt * stm = NULL;
	SQLITE_REQUIRED_OPEN(db);
	SQLITE_CHECK_PREPARE(db, sql_remove_setting, -1, &stm, NULL);
	SQLITE_BIND_STRING(stm, 1, key);
	return db_perform_single_step(stm);
}

int db_insert_setting_str(const char * key, const char * value)
{
	sqlite3_stmt * stm = NULL;
	SQLITE_REQUIRED_OPEN(db);
	SQLITE_CHECK_PREPARE(db, sql_insert_setting, -1, &stm, NULL);
	SQLITE_BIND_STRING(stm, 1, key);
	SQLITE_BIND_STRING(stm, 2, value);
	return db_perform_single_step(stm);
}

int db_insert_setting_int(const char * key, int value)
{
	sqlite3_stmt * stm = NULL;
	SQLITE_REQUIRED_OPEN(db);
	SQLITE_CHECK_PREPARE(db, sql_insert_setting, -1, &stm, NULL);
	SQLITE_BIND_STRING(stm, 1, key);
	sqlite3_bind_int(stm, 2, value);
	return db_perform_single_step(stm);
}

int db_get_setting_str(const char * key, const char * def, char * out)
{
	int res = 0;
	sqlite3_stmt * stm = NULL;
	SQLITE_REQUIRED_OPEN(db);
	SQLITE_CHECK_PREPARE(db, sql_get_setting, -1, &stm, NULL);
	SQLITE_BIND_STRING(stm, 1, key);

	db_log_query(stm);
	res = sqlite3_step(stm);
	if (res == SQLITE_ROW)
	{
		sqlite_get_string_column(stm, 0, out);
		res = 0;
	}
	else if (res == SQLITE_DONE)
	{
		strcpy(out, def);
		res = 0;
	}
	else
	{
		get_error(res);
		res = -1;
	}

	SQLITE_CHECK_FINALIZE(stm);
	return res;
}

int db_get_setting(const char * key, char * out)
{
	int res = 0;
	sqlite3_stmt * stm = NULL;
	SQLITE_REQUIRED_OPEN(db);
	SQLITE_CHECK_PREPARE(db, sql_get_setting, -1, &stm, NULL);
	SQLITE_BIND_STRING(stm, 1, key);

	db_log_query(stm);
	res = sqlite3_step(stm);
	if (res == SQLITE_ROW)
	{
		sqlite_get_string_column(stm, 0, out);
		res = 0;
	}
	else if (res != SQLITE_DONE)
	{
		get_error(res);
		res = -1;
	}

	SQLITE_CHECK_FINALIZE(stm);
	return res;
}

int db_get_setting_int(const char * key, int def, int * out)
{
	int res = 0;
	sqlite3_stmt * stm = NULL;
	SQLITE_REQUIRED_OPEN(db);
	SQLITE_CHECK_PREPARE(db, sql_get_setting, -1, &stm, NULL);
	SQLITE_BIND_STRING(stm, 1, key);

	db_log_query(stm);
	res = sqlite3_step(stm);
	if (res == SQLITE_ROW)
	{
		*out = sqlite3_column_int(stm, 0);
		res = 1;
	}
	else if (res == SQLITE_DONE)
	{
		*out = def;
		res = 0;
	}
	else
	{
		get_error(res);
		res = -1;
	}

	SQLITE_CHECK_FINALIZE(stm);
	return res;
}

int db_clear_setting()
{
	sqlite3_stmt * stm = NULL;
	SQLITE_REQUIRED_OPEN(db);
	SQLITE_CHECK_PREPARE(db, sql_clear_setting, -1, &stm, NULL);
	return db_perform_single_step(stm);
}

int db_get_all_settings(db_sqlite_callback c, void * arg)
{
	int res = 0;
	sqlite3_stmt * stm = NULL;
	SQLITE_REQUIRED_OPEN(db);

	LOG(LOG_DEBUG, sql_get_all_settings);
	++db_connections;
	res = sqlite3_exec(db, sql_get_all_settings, c, arg, NULL);
	--db_connections;
	if (res != SQLITE_OK && res != SQLITE_ABORT)
	{
		get_error(res);
		return -1;
	}

	return res;
}