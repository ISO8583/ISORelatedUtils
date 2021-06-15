/*
	Creative Commons Attribution-NonCommercial-NoDerivs (CC-BY-NC-ND)
	https://creativecommons.org/licenses/by-nc-nd/4.0/
	The most restrictive creative commons license.
	This only allows people to download and share your work for no commercial gain and for no other purposes.
*/

#define COUNT_FIELD_LENGTH					10
#define SUM_FIELD_LENGTH					30
#define DATE_TIME_FIELD_LENGTH				14 // YYYYmmddHHMMSS
#define IP_FIELD_LENGTH						15
#define TEL_FIELD_LENGTH					20
#define NII_FIELD_LENGTH					10
#define TERMINAL_FIELD_LENGTH				8
#define MERCHANT_FIELD_LENGTH				15
#define AMOUNT_FIELD_LENGTH					12
#define BILL_ID_FIELD_LENGTH				30
#define PAY_ID_FIELD_LENGTH					200
#define TRACK2_FIELD_LENGTH					40
#define RRN_FIELD_LENGTH					12
#define PROCESS_CODE_FIELD_LENGTH			6
#define PINBLOCK_FIELD_LENGTH				16
#define KEY_BIN_FIELD_LENGTH				24
#define KEY_HEX_FIELD_LENGTH				(KEY_BIN_FIELD_LENGTH << 1)
#define KEY_KCV_LENGTH						3
#define KEY_KCV_HEX_LENGTH					(KEY_KCV_LENGTH << 1)

#define ENV_LOYALTY_COUNT					20
#define ENV_LOYALTY_BIN						16
#define ENV_LOYALTY_NAME_UTF8				60
#define ENV_LOYALTY_SEGMENT_BUF				150

#define ENV_NTP_MAX_COUNT					4
#define ENV_NTP_MAX_LENGTH					50
#define ENV_PAYMENT_HEADER_MAX_COUNT		10

#define ENV_SERIAL_LENGTH					30
#define ENV_SMALL_FIELD_LENGTH				60
#define ENV_MEDIUM_FIELD_LENGTH				150
#define ENV_LARGE_FIELD_LENGTH				300
#define ENV_XLARGE_FIELD_LENGTH				1000
#define ENV_XXLARGE_FIELD_LENGTH			2000

#define TRANS_SMALL_FIELD_LENGTH			50
#define TRANS_MEDIUM_FIELD_LENGTH			200
#define TRANS_LL_FIELD_LENGTH				100
#define TRANS_LLL_FIELD_LENGTH				1000
#define TRANS_LLLL_FIELD_LENGTH				5000

#define CONFIG_STR_LENGTH					ENV_XXLARGE_FIELD_LENGTH

typedef enum
{
	CONFIG_INTEGER,
	CONFIG_STRING,
} config_type_t;

typedef struct
{
	const char * db_key;
	const config_type_t type;
	int integer;
	int integer_init;
	char str[CONFIG_STR_LENGTH + 1];
	const char * str_init;
} config_item_t;

#define CONFIG_ITEM(x) config_item_t x
#define CONFIG_INIT_INT(x, v) .x.integer = (v), .x.integer_init = (v), .x.db_key = #x, .x.type = CONFIG_INTEGER,
#define CONFIG_INIT_STR(x, v) .x.str = {0}, .x.str_init = (v), .x.db_key = #x, .x.type = CONFIG_STRING,

typedef struct
{
	int enable;
	int selected;
	int rtl;
	const char * text;
} menu_item_t;

typedef struct
{
	const char * header;
	int count;
	int page;
	int on_enter_index;
	menu_item_t items[];
} menu_t;

typedef struct
{
	const int interface_id;
	const char * if_name;
	int is_up;
	int dhcp_status;
	char mac[30];
	int dhcp_enable;
	char ip[30];
	char subnet[30];
	char gw[30];
	char dns[30];
} interface_stat_t;

typedef struct
{
	int shift;
	int time_2_time;
	char start[50];
	char end[50];

	char purchase_count[COUNT_FIELD_LENGTH];
	char purchase_sum[SUM_FIELD_LENGTH];

	char bill_count[COUNT_FIELD_LENGTH];
	char bill_sum[SUM_FIELD_LENGTH];

	char voucher_count[COUNT_FIELD_LENGTH];
	char voucher_sum[SUM_FIELD_LENGTH];

	char topup_count[COUNT_FIELD_LENGTH];
	char topup_sum[SUM_FIELD_LENGTH];
} totally_report_result_t;

typedef struct
{
	int row_id;
	int stan;
	int shift;
	int voucher_operator;
	int voucher_code;
	int voucher_type;
	int need_verify;
	int need_reverse;
	int type;
	int logo;
	int server;
	int additional_flag;

	char amount[AMOUNT_FIELD_LENGTH + 1];
	char terminal[TERMINAL_FIELD_LENGTH + 1];
	char merchant[MERCHANT_FIELD_LENGTH + 1];
	char bill_id[BILL_ID_FIELD_LENGTH + 1];
	char pay_id[PAY_ID_FIELD_LENGTH + 1];
	char track2[TRACK2_FIELD_LENGTH + 1];
	char track2_hash[SSM_SHA1_HEX_LEN + 1];
	char pin_block[PINBLOCK_FIELD_LENGTH + 1];
	char cell_no[TRANS_SMALL_FIELD_LENGTH];
	char datetime[DATE_TIME_FIELD_LENGTH + 1];
	char datetime_fa[DATE_TIME_FIELD_LENGTH + 1];
	char rrn[RRN_FIELD_LENGTH + 1];
	char rrn_verify[RRN_FIELD_LENGTH + 1];
	char rrn_reverse[RRN_FIELD_LENGTH + 1];
	char resp[TRANS_SMALL_FIELD_LENGTH];
	char resp_verify[TRANS_SMALL_FIELD_LENGTH];
	char resp_reverse[TRANS_SMALL_FIELD_LENGTH];
	char resp_datetime[DATE_TIME_FIELD_LENGTH + 1];
	char resp_datetime_fa[DATE_TIME_FIELD_LENGTH + 1];
	int mac_wrong;
	int retry;
	int return_money;
	char resp_38[PROCESS_CODE_FIELD_LENGTH + 1];
	char resp_xx[10];
	int resp_xx_len;
} transaction_t;


typedef struct
{
	int shift;
	time_t start;
	time_t end;
	char start_human_readable[50];
	char end_human_readable[50];
	char start_db_format[50];
	char end_db_format[50];
} report_time_2_time_info_t;

typedef struct
{
	const report_time_2_time_info_t * report;

	int eod;
	int count;
	int flags;
	int error_exit;
	int header_print;

	char current_date_str[100];
	unsigned long long temp_long;

	unsigned long long day_purchase;
	unsigned long long day_bill;
	unsigned long long day_voucher;
	unsigned long long day_topup;

	unsigned long long total_purchase;
	unsigned long long total_bill;
	unsigned long long total_voucher;
	unsigned long long total_topup;

	unsigned int total_purchase_count;
	unsigned int total_bill_count;
	unsigned int total_voucher_count;
	unsigned int total_topup_count;
} report_state_t;

int config_load(config_item_t * cf);
int config_save(const config_item_t * cf);
