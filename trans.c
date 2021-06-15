/*
	Creative Commons Attribution-NonCommercial-NoDerivs (CC-BY-NC-ND)
	https://creativecommons.org/licenses/by-nc-nd/4.0/
	The most restrictive creative commons license.
	This only allows people to download and share your work for no commercial gain and for no other purposes.
*/

enum
{
	LANG_PERSIAN,
	LANG_ENGLISH,
};

const char * get_bank_name_utf8(const char * pan, int lang)
{
	bin = read_int(pan, 6);
	if (bin >= 100600 && bin <= 100609)
	{
		return lang == LANG_PERSIAN ? "مرکزی" : "Markazi";
	}
	else if (bin == 622106 || bin == 627884 || bin == 639194)
	{
		return lang == LANG_PERSIAN ? "پارسیان" : "Parsian";
	}
	else if (bin == 585983 || bin == 627353)
	{
		return lang == LANG_PERSIAN ? "تجارت" : "Tejarat";
	}
	else if (bin == 603770 || bin == 639217)
	{
		return lang == LANG_PERSIAN ? "کشاورزی" : "Keshavarzi";
	}
	else if (bin == 610433 || bin == 991975)
	{
		return lang == LANG_PERSIAN ? "ملت" : "Mellat";
	}
	else if (bin == 627488 || bin == 502910)
	{
		return lang == LANG_PERSIAN ? "کارآفرین" : "Karafarin";
	}
	else if (bin == 627648 || bin == 207177)
	{
		return lang == LANG_PERSIAN ? "توسعه صادرات" : "Tose Saderat";
	}
	else if (bin == 636214 || bin == 186214)
	{
		return lang == LANG_PERSIAN ? "آینده" : "Ayande";
	}
	else if (bin == 589210 || bin == 604932)
	{
		return lang == LANG_PERSIAN ? "سپه" : "Sepah";
	}
	else if (bin == 502229 || bin == 639347)
	{
		return lang == LANG_PERSIAN ? "پاسارگاد" : "Pasargad";
	}
	else if (bin == 502806 || bin == 504706)
	{
		return lang == LANG_PERSIAN ? "شهر" : "Shahr";
	}
	else if (bin == 505809 || bin == 585947)
	{
		return lang == LANG_PERSIAN ? "خاورمیانه" : "Khavarmiane";
	}
	else if (bin == 502908)
	{
		return lang == LANG_PERSIAN ? "توسعه تعاون" : "Tose Tavaon";
	}
	else if (bin == 502938)
	{
		return lang == LANG_PERSIAN ? "دی" : "Dey";
	}
	else if (bin == 505416)
	{
		return lang == LANG_PERSIAN ? "گردشگری" : "Gardeshgari";
	}
	else if (bin == 505785)
	{
		return lang == LANG_PERSIAN ? "ایران زمین" : "Iranzamin";
	}
	else if (bin == 581672)
	{
		return lang == LANG_PERSIAN ? "شاپرک" : "Shaparak";
	}
	else if (bin == 581874)
	{
		return lang == LANG_PERSIAN ? "ایران ونزوئلا" : "Iran venezuela";
	}
	else if (bin == 603769)
	{
		return lang == LANG_PERSIAN ? "صادرات" : "Saderat";
	}
	else if (bin == 603799)
	{
		return lang == LANG_PERSIAN ? "ملی" : "Melli";
	}
	else if (bin == 621986)
	{
		return lang == LANG_PERSIAN ? "سامان" : "Saman";
	}
	else if (bin == 627412)
	{
		return lang == LANG_PERSIAN ? "اقتصاد نوین" : "Eghtesad Novin";
	}
	else if (bin == 627760)
	{
		return lang == LANG_PERSIAN ? "پست بانک" : "Post Bank";
	}
	else if (bin == 627961)
	{
		return lang == LANG_PERSIAN ? "صنعت و معدن" : "Sanat Madan";
	}
	else if (bin == 628023)
	{
		return lang == LANG_PERSIAN ? "مسکن" : "Maskan";
	}
	else if (bin == 639346)
	{
		return lang == LANG_PERSIAN ? "سینا" : "Sina";
	}
	else if (bin == 639607)
	{
		return lang == LANG_PERSIAN ? "سرمایه" : "Sarmaye";
	}
	else if (bin == 774433)
	{
		return lang == LANG_PERSIAN ? "سوخت" : "Sokht";
	}
	else if (bin == 639599)
	{
		return lang == LANG_PERSIAN ? "قوامین" : "Ghavamin";
	}
	else if (bin == 505801)
	{
		return lang == LANG_PERSIAN ? "کوثر" : "Kosar";
	}
	else if (bin == 636949)
	{
		return lang == LANG_PERSIAN ? "حکمت" : "Hekmat";
	}
	else if (bin == 627381)
	{
		return lang == LANG_PERSIAN ? "انصار" : "Ansar";
	}
	else if (bin == 639370)
	{
		return lang == LANG_PERSIAN ? "مهر اقتصاد" : "Mehr Eghtesad";
	}
	else if (bin == 504172)
	{
		return lang == LANG_PERSIAN ? "رسالت" : "Resalat";
	}
	else if (bin == 507677)
	{
		return lang == LANG_PERSIAN ? "نور" : "Noor";
	}
	else if (bin == 589463)
	{
		return lang == LANG_PERSIAN ? "رفاه" : "Refah";
	}
	else if (bin == 606256)
	{
		return lang == LANG_PERSIAN ? "ملل" : "Melal";
	}
	else if (bin == 606373)
	{
		return lang == LANG_PERSIAN ? "مهرایران" : "Mehr Iran";
	}
	else if (bin == 628157)
	{
		return lang == LANG_PERSIAN ? "توسعه" : "Tosee";
	}
	else if (bin == 636795)
	{
		return lang == LANG_PERSIAN ? "بانک مرکزی" : "Markazi Bank";
	}
	else
	{
		return "-";
	}
}

const char * get_resp_text_utf8(const char * resp, int lang)
{
	static char temp[50];
	int code = ((resp[0] - '0') * 10) + (resp[1] - '0');
	if (code == 0)
	{
		return lang == LANG_PERSIAN ? "عملیات موفق" : "Operation successful";
	}
	else if (code == 2)
	{
		return lang == LANG_PERSIAN ? "تاییده موفق 2" : "Settelment success 2";
	}
	else if (code == 3)
	{
		return lang == LANG_PERSIAN ? "پذیرنده نامعتبر 3" : "Invalid merchant 3";
	}
	else if (code == 13)
	{
		return lang == LANG_PERSIAN ? "مبلغ اصلاحیه نادرست 13" : "Invalid amount  13";
	}
	else if (code == 14)
	{
		return lang == LANG_PERSIAN ? "کارت نامعتبر 14" : "Invalid card number 14";
	}
	else if (code == 19)
	{
		return lang == LANG_PERSIAN ? "تراکنش تجدید شود 19" : "Re-enter transaction 19";
	}
	else if (code == 25)
	{
		return lang == LANG_PERSIAN ? "تراکنش یافت نشد 25" : "Transaction not found 25";
	}
	else if (code == 30)
	{
		return lang == LANG_PERSIAN ? "قالب پیام اشتباه است 30" : "Format error 30";
	}
	else if (code == 34)
	{
		return lang == LANG_PERSIAN ? "تراکنش اصلی ناموفق 34" : "Original transaction not successful 34";
	}
	else if (code == 45)
	{
		return lang == LANG_PERSIAN ? "قبض پرداخت شده است 45" : "Bill is paid 45";
	}
	else if (code == 51)
	{
		return lang == LANG_PERSIAN ? "موجودی کافی نیست 51" : "Balance not enough 51";
	}
	else if (code == 54)
	{
		return lang == LANG_PERSIAN ? "کارت منقضی شده 54" : "Card expired 54";
	}
	else if (code == 55)
	{
		return lang == LANG_PERSIAN ? "رمز اشتباه است 55" : "Wrong PIN 55";
	}
	else if (code == 64)
	{
		return lang == LANG_PERSIAN ? "مبلغ اصلی نادرست 64" : "Original amount incorrect 64";
	}
	else if (code == 75)
	{
		return lang == LANG_PERSIAN ? "کارت مسدود است 75" : "Card blocked 75";
	}
	else if (code == 84)
	{
		return lang == LANG_PERSIAN ? "اشکال در صادر کننده 84" : "Issuer error 84";
	}
	else if (code == 92)
	{
		return lang == LANG_PERSIAN ? "مقصد تراکنش نامعتبر 92" : "Destination unknown 92";
	}
	else if (code == 93)
	{
		return lang == LANG_PERSIAN ? "عدم تکمیل تراکنش 93" : " Transaction cannot be completed 93";
	}
	else if (code == 94)
	{
		return lang == LANG_PERSIAN ? "ارسال تکراری تراکنش 94" : "Duplicate transmission 94";
	}
	else if (code == 96)
	{
		return lang == LANG_PERSIAN ? "اشکال در سیستم 96" : "System fault 96";
	}
	else if (code == 99)
	{
		return lang == LANG_PERSIAN ? "خطای امنیتی 99" : "Security error 99";
	}
	else
	{
		if (lang == LANG_PERSIAN)
		{
			sprintf(temp, "تراکنش نامعتبر است %d", code);
		}
		else
		{
			sprintf(temp, "Invalid transaction %d", code);
		}
		return temp;
	}
}

