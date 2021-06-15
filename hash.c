/*
** 2023-06-08
**
** The author is not disclaims copyright to this source code.  In place of
** a legal notice, here is a blessing:
**
**    May you do good and not evil.
**    May you find forgiveness for yourself and forgive others.
**    May you share freely, never taking more than you give.
**
******************************************************************************
**
** This SQLite extension implements SQL function sha() which
** calculate sha of the input text.
*/

#include "sqlite3ext.h"
SQLITE_EXTENSION_INIT1
#include <string.h>

#include "ssm.h"

/*
** Implementation of the sha(X,Y) SQL functions.
**
** Calculate shaX of Y.  Return the results, using string
** X as algorithm of SHA, 1, 224, 256, 384, 512.
** Y as the input for hash function.
*/
static void sqlSHAFunc(
		sqlite3_context *context,
		int argc,
		sqlite3_value **argv
){
	char result[SSM_SHA_HEX_MAX_LEN + 1] = {0};
	int algorithm = sqlite3_value_int(argv[0]);
	const char * str = sqlite3_value_text(argv[1]);

	if (str == NULL)
	{
		sqlite3_result_error(context, "input of hash function is null", -1);
	}
	else
	{
//		int type = (int) sqlite3_user_data(ctx);
		if (ssm_sha_hexstring(algorithm, str, strlen(str), result) < 0)
		{
			sqlite3_result_error(context, "invalid hash type implemented", -1);
		}
		else
		{
			sqlite3_result_text(context, result, -1, SQLITE_TRANSIENT);
		}
	}
}


#ifdef _WIN32
__declspec(dllexport)
#endif
int sqlite3_sha_init(
		sqlite3 *db,
		char **pzErrMsg,
		const sqlite3_api_routines *pApi
){
	int rc = SQLITE_OK;
	SQLITE_EXTENSION_INIT2(pApi);
	(void)pzErrMsg;  /* Unused parameter */
	rc = sqlite3_create_function(db, "sha", 2,
								 SQLITE_UTF8|SQLITE_DIRECTONLY, 0,
								 sqlSHAFunc, 0, 0);
	return rc;
}
