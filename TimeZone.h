/**
 * @file TimeZone.h
 * @brief This file contains structures, which are needed for time zone and DST adjustment.
 *
 * @see TimeZones
 * @see TimeZone
 * @see DSTTransitionRule
 * @see DSTAdjustment
 *
 * # Credits
 * @author Matej Fitoš
 * @date Jun 4, 2022
 */

#ifndef TIME_ZONE_H
#define TIME_ZONE_H

#include "TimeSpan.h"

class DateTime;
template <class>
class DateTimeBase;

/**
* @struct TimeZones
* @brief Structure, that behaves as enumeration with all known time zones. Lasr revision was done at the May 19 2022.
* Source (long names): https://en.wikipedia.org/wiki/List_of_tz_database_time_zones
* Source (short names): https://gist.github.com/valo/c07f8db33d223f57a4cc9c670e1b6050
*/
struct TimeZones {

	enum TimeZoneName {
		BIT = -48, //Baker Island Time (GMT-12:00)
		NUT = -44, //Niue Time (GMT-11:00)
		SST = -44, //Samoa Standard Time (GMT-11:00)
		CKT = -40, //Cook Island Time (GMT-10:00)
		HAST = -40, //Hawaii-Aleutian Standard Time (GMT-10:00)
		TAHT = -40, //Tahiti Time (GMT-10:00)
		MART = -38, //Marquesas Islands Time (GMT-9:30)
		MIT = -38, //Marquesas Islands Time (GMT-9:30)
		AKST = -36, //Alaska Standard Time (GMT-9:00)
		GAMT = -36, //Gambier Islands (GMT-9:00)
		GIT = -36, //Gambier Island Time (GMT-9:00)
		//HADT = -36, //Hawaii-Aleutian Daylight Time (GMT-9:00)
		//AKDT = -32, //Alaska Daylight Time (GMT-8:00)
		CIST = -32, //Clipperton Island Standard Time (GMT-8:00)
		PST = -32, //Pacific Standard Time (North America) (GMT-8:00)
		MST = -28, //Mountain Standard Time (North America) (GMT-7:00)
		//PDT = -28, //Pacific Daylight Time (North America) (GMT-7:00)
		CST = -24, //Central Standard Time (North America) (GMT-6:00)
		EAST = -24, //Easter Island Standard Time (GMT-6:00)
		GALT = -24, //Galapagos Time (GMT-6:00)
		//MDT = -24, //Mountain Daylight Time (North America) (GMT-6:00)
		AcT = -20, //Acre Time (GMT-5:00)
		//CDT = -20, //Central Daylight Time (North America) (GMT-5:00)
		COT = -20, //Colombia Time (GMT-5:00)
		CuST = -20, //Cuba Standard Time (GMT-5:00)
		//EASST = -20, //Easter Island Summer Time (GMT-5:00)
		EcT = -20, //Ecuador Time (GMT-5:00)
		EST = -20, //Eastern Standard Time (North America) (GMT-5:00)
		PET = -20, //Peru Time (GMT-5:00)
		AMT = -16, //Amazon Time (Brazil)[2] (GMT-4:00)
		AtST = -16, //Atlantic Standard Time (GMT-4:00)
		BOT = -16, //Bolivia Time (GMT-4:00)
		//CDT = -16, //Cuba Daylight Time[4] (GMT-4:00)
		CLT = -16, //Chile Standard Time (GMT-4:00)
		//COST = -16, //Colombia Summer Time (GMT-4:00)
		ECT = -16, //Eastern Caribbean Time (does not recognise DST) (GMT-4:00)
		//EDT = -16, //Eastern Daylight Time (North America) (GMT-4:00)
		FKT = -16, //Falkland Islands Time (GMT-4:00)
		GYT = -16, //Guyana Time (GMT-4:00)
		PYT = -16, //Paraguay Time (South America)[8] (GMT-4:00)
		VET = -16, //Venezuelan Standard Time (GMT-4:00)
		NST = -14, //Newfoundland Standard Time (GMT-3:30)
		NT = -14, //Newfoundland Time (GMT-3:30)
		//ADT = -12, //Atlantic Daylight Time (GMT-3:00)
		//AMST = -12, //Amazon Summer Time (Brazil)[1] (GMT-3:00)
		ART = -12, //Argentina Time (GMT-3:00)
		BRT = -12, //Brasilia Time (GMT-3:00)
		//CLST = -12, //Chile Summer Time (GMT-3:00)
		//FKST = -12, //Falkland Islands Summer Time (GMT-3:00)
		GFT = -12, //French Guiana Time (GMT-3:00)
		PMST = -12, //Saint Pierre and Miquelon Standard Time (GMT-3:00)
		//PYST = -12, //Paraguay Summer Time (South America)[7] (GMT-3:00)
		ROTT = -12, //Rothera Research Station Time (GMT-3:00)
		SRT = -12, //Suriname Time (GMT-3:00)
		UYT = -12, //Uruguay Standard Time (GMT-3:00)
		//NDT = -10, //Newfoundland Daylight Time (GMT-2:30)
		//BRST = -8, //Brasilia Summer Time (GMT-2:00)
		FNT = -8, //Fernando de Noronha Time (GMT-2:00)
		SGST = -8, //South Georgia and the South Sandwich Islands (GMT-2:00)
		//PMDT = -8, //Saint Pierre and Miquelon Daylight time (GMT-2:00)
		//UYST = -8, //Uruguay Summer Time (GMT-2:00)
		AZOT = -4, //Azores Standard Time (GMT-1:00)
		CVT = -4, //Cape Verde Time (GMT-1:00)
		EGT = -4, //Eastern Greenland Time (GMT-1:00)
		//AZOST = 0, //Azores Summer Time (GMT+0:00)
		//EGST = 0, //Eastern Greenland Summer Time (GMT+0:00)
		GMT = 0, //Greenwich Mean Time (GMT+0:00)
		UTC = 0, //Coordinated Universal Time (GMT+0:00)
		WET = 0, //Western European Time (GMT+0:00)
		//BST = 4, //British Summer Time (British Standard Time from Feb 1968 to Oct 1971) (GMT+1:00)
		CET = 4, //Central European Time (GMT+1:00)
		DFT = 4, //AIX specific equivalent of Central European Time[5] (GMT+1:00)
		IrST = 4, //Irish Standard Time[6] (GMT+1:00)
		MET = 4, //Middle European Time Same zone as CET (GMT+1:00)
		WAT = 4, //West Africa Time (GMT+1:00)
		//WEST = 4, //Western European Summer Time (GMT+1:00)
		CAT = 8, //Central Africa Time (GMT+2:00)
		//CEST = 8, //Central European Summer Time (Cf. HAEC) (GMT+2:00)
		EET = 8, //Eastern European Time (GMT+2:00)
		HAEC = 8, //Heure Avancée d'Europe Centrale francised name for CEST (GMT+2:00)
		IsST = 8, //Israel Standard Time (GMT+2:00)
		//MEST = 8, //Middle European Summer Time Same zone as CEST (GMT+2:00)
		SAST = 8, //South African Standard Time (GMT+2:00)
		USZ1 = 8, //Kaliningrad Time (GMT+2:00)
		//WAST = 8, //West Africa Summer Time (GMT+2:00)
		AST = 12, //Arabia Standard Time (GMT+3:00)
		EAT = 12, //East Africa Time (GMT+3:00)
		//EEST = 12, //Eastern European Summer Time (GMT+3:00)
		FET = 12, //Further-eastern European Time (GMT+3:00)
		//IDT = 12, //Israel Daylight Time (GMT+3:00)
		IOT = 12, //Indian Ocean Time (GMT+3:00)
		MSK = 12, //Moscow Time (GMT+3:00)
		SYOT = 12, //Showa Station Time (GMT+3:00)
		TRT = 12, //Turkey Time (GMT+3:00)
		IRST = 14, //Iran Standard Time (GMT+3:30)
		ArMT = 16, //Armenia Time (GMT+4:00)
		AZT = 16, //Azerbaijan Time (GMT+4:00)
		GET = 16, //Georgia Standard Time (GMT+4:00)
		GST = 16, //Gulf Standard Time (GMT+4:00)
		MUT = 16, //Mauritius Time (GMT+4:00)
		RET = 16, //Réunion Time (GMT+4:00)
		SAMT = 16, //Samara Time (GMT+4:00)
		SCT = 16, //Seychelles Time (GMT+4:00)
		VOLT = 16, //Volgograd Time (GMT+4:00)
		AFT = 18, //Afghanistan Time (GMT+4:30)
		//IRDT = 18, //Iran Daylight Time (GMT+4:30)
		HMT = 20, //Heard and McDonald Islands Time (GMT+5:00)
		MAWT = 20, //Mawson Station Time (GMT+5:00)
		MVT = 20, //Maldives Time (GMT+5:00)
		ORAT = 20, //Oral Time (GMT+5:00)
		PKT = 20, //Pakistan Standard Time (GMT+5:00)
		TFT = 20, //Indian/Kerguelen (GMT+5:00)
		TJT = 20, //Tajikistan Time (GMT+5:00)
		TMT = 20, //Turkmenistan Time (GMT+5:00)
		UZT = 20, //Uzbekistan Time (GMT+5:00)
		YEKT = 20, //Yekaterinburg Time (GMT+5:00)
		InST = 22, //Indian Standard Time (GMT+5:30)
		SLST = 22, //Sri Lanka Standard Time (GMT+5:30)
		NPT = 23, //Nepal Time (GMT+5:45)
		BIOT = 24, //British Indian Ocean Time (GMT+6:00)
		BST = 24, //Bangladesh Standard Time (GMT+6:00)
		BTT = 24, //Bhutan Time (GMT+6:00)
		KGT = 24, //Kyrgyzstan time (GMT+6:00)
		OMST = 24, //Omsk Time (GMT+6:00)
		VOST = 24, //Vostok Station Time (GMT+6:00)
		ACT = 26, //ASEAN Common Time (GMT+6:30)
		CCT = 26, //Cocos Islands Time (GMT+6:30)
		MMT = 26, //Myanmar Standard Time (GMT+6:30)
		CXT = 28, //Christmas Island Time (GMT+7:00)
		DAVT = 28, //Davis Time (GMT+7:00)
		HOVT = 28, //Khovd Standard Time (GMT+7:00)
		ICT = 28, //Indochina Time (GMT+7:00)
		KRAT = 28, //Krasnoyarsk Time (GMT+7:00)
		THA = 28, //Thailand Standard Time (GMT+7:00)
		WIT = 28, //Western Indonesian Time (GMT+7:00)
		AWST = 32, //Australian Western Standard Time (GMT+8:00)
		BDT = 32, //Brunei Time (GMT+8:00)
		CHOT = 32, //Choibalsan Standard Time (GMT+8:00)
		CIT = 32, //Central Indonesia Time (GMT+8:00)
		CT = 32, //China time (GMT+8:00)
		HKT = 32, //Hong Kong Time (GMT+8:00)
		HOVST = 32, //Khovd Summer Time (GMT+8:00)
		IRKT = 32, //Irkutsk Time (GMT+8:00)
		MYT = 32, //Malaysia Time (GMT+8:00)
		PHT = 32, //Philippine Time (GMT+8:00)
		SGT = 32, //Singapore Time (GMT+8:00)
		ULAT = 32, //Ulaanbaatar Standard Time (GMT+8:00)
		WST = 32, //Western Standard Time (GMT+8:00)
		CWST = 35, //Central Western Standard Time (Australia) unofficial (GMT+8:45)
		//CHOST = 36, //Choibalsan Summer Time (GMT+9:00)
		EIT = 36, //Eastern Indonesian Time (GMT+9:00)
		JST = 36, //Japan Standard Time (GMT+9:00)
		KST = 36, //Korea Standard Time (GMT+9:00)
		TLT = 36, //Timor Leste Time (GMT+9:00)
		ULAST = 36, //Ulaanbaatar Summer Time (GMT+9:00)
		YAKT = 36, //Yakutsk Time (GMT+9:00)
		ACST = 38, //Australian Central Standard Time (GMT+9:30)
		AEST = 40, //Australian Eastern Standard Time (GMT+10:00)
		CHST = 40, //Chamorro Standard Time (GMT+10:00)
		CHUT = 40, //Chuuk Time (GMT+10:00)
		DDUT = 40, //Dumont d'Urville Time (GMT+10:00)
		PGT = 40, //Papua New Guinea Time (GMT+10:00)
		VLAT = 40, //Vladivostok Time (GMT+10:00)
		//ACDT = 42, //Australian Central Daylight Savings Time (GMT+10:30)
		LHST = 42, //Lord Howe Standard Time (GMT+10:30)
		//AEDT = 44, //Australian Eastern Daylight Savings Time (GMT+11:00)
		BVST = 44, //Bougainville Standard Time[3] (GMT+11:00)
		KOST = 44, //Kosrae Time (GMT+11:00)
		MIST = 44, //Macquarie Island Station Time (GMT+11:00)
		NCT = 44, //New Caledonia Time (GMT+11:00)
		NFT = 44, //Norfolk Time (GMT+11:00)
		PONT = 44, //Pohnpei Standard Time (GMT+11:00)
		SAKT = 44, //Sakhalin Island time (GMT+11:00)
		SBT = 44, //Solomon Islands Time (GMT+11:00)
		SRET = 44, //Srednekolymsk Time (GMT+11:00)
		VUT = 44, //Vanuatu Time (GMT+11:00)
		FJT = 48, //Fiji Time (GMT+12:00)
		GILT = 48, //Gilbert Island Time (GMT+12:00)
		MAGT = 48, //Magadan Time (GMT+12:00)
		MHT = 48, //Marshall Islands (GMT+12:00)
		NZST = 48, //New Zealand Standard Time (GMT+12:00)
		PETT = 48, //Kamchatka Time (GMT+12:00)
		TVT = 48, //Tuvalu Time (GMT+12:00)
		WAKT = 48, //Wake Island Time (GMT+12:00)
		CHAST = 51, //Chatham Standard Time (GMT+12:45)
		//NZDT = 52, //New Zealand Daylight Time (GMT+13:00)
		PHOT = 52, //Phoenix Island Time (GMT+13:00)
		TKT = 52, //Tokelau Time (GMT+13:00)
		TOT = 52, //Tonga Time (GMT+13:00)
		//CHADT = 55, //Chatham Daylight Time (GMT+13:45)
		LINT = 56, //Line Islands Time (GMT+14:00)


		Pacific_Midway = -44,
		Pacific_Niue = -44,
		Pacific_Pago_Pago = -44,
		America_Adak = -40,
		Pacific_Honolulu = -40,
		Pacific_Rarotonga = -40,
		Pacific_Tahiti = -40,
		Pacific_Marquesas = -38,
		America_Anchorage = -36,
		America_Juneau = -36,
		America_Metlakatla = -36,
		America_Nome = -36,
		America_Sitka = -36,
		America_Yakutat = -36,
		Pacific_Gambier = -36,
		America_Los_Angeles = -32,
		America_Tijuana = -32,
		America_Vancouver = -32,
		Pacific_Pitcairn = -32,
		America_Boise = -28,
		America_Cambridge_Bay = -28,
		America_Chihuahua = -28,
		America_Creston = -28,
		America_Dawson = -28,
		America_Dawson_Creek = -28,
		America_Denver = -28,
		America_Edmonton = -28,
		America_Fort_Nelson = -28,
		America_Hermosillo = -28,
		America_Inuvik = -28,
		America_Mazatlan = -28,
		America_Ojinaga = -28,
		America_Phoenix = -28,
		America_Whitehorse = -28,
		America_Yellowknife = -28,
		America_Bahia_Banderas = -24,
		America_Belize = -24,
		America_Chicago = -24,
		America_Costa_Rica = -24,
		America_El_Salvador = -24,
		America_Guatemala = -24,
		America_Indiana_Knox = -24,
		America_Indiana_Tell_City = -24,
		America_Managua = -24,
		America_Matamoros = -24,
		America_Menominee = -24,
		America_Merida = -24,
		America_Mexico_City = -24,
		America_Monterrey = -24,
		America_North_Dakota_Beulah = -24,
		America_North_Dakota_Center = -24,
		America_North_Dakota_New_Salem = -24,
		America_Rainy_River = -24,
		America_Rankin_Inlet = -24,
		America_Regina = -24,
		America_Resolute = -24,
		America_Swift_Current = -24,
		America_Tegucigalpa = -24,
		America_Winnipeg = -24,
		Pacific_Easter = -24,
		Pacific_Galapagos = -24,
		America_Atikokan = -20,
		America_Bogota = -20,
		America_Cancun = -20,
		America_Cayman = -20,
		America_Detroit = -20,
		America_Eirunepe = -20,
		America_Grand_Turk = -20,
		America_Guayaquil = -20,
		America_Havana = -20,
		America_Indiana_Indianapolis = -20,
		America_Indiana_Marengo = -20,
		America_Indiana_Petersburg = -20,
		America_Indiana_Vevay = -20,
		America_Indiana_Vincennes = -20,
		America_Indiana_Winamac = -20,
		America_Iqaluit = -20,
		America_Jamaica = -20,
		America_Kentucky_Louisville = -20,
		America_Kentucky_Monticello = -20,
		America_Lima = -20,
		America_Nassau = -20,
		America_New_York = -20,
		America_Nipigon = -20,
		America_Panama = -20,
		America_Pangnirtung = -20,
		America_Port_au_Prince = -20,
		America_Rio_Branco = -20,
		America_Thunder_Bay = -20,
		America_Toronto = -20,
		America_AnguillaSandy_Hill = -16,
		America_Antigua = -16,
		America_Aruba = -16,
		America_Asuncion = -16,
		America_Barbados = -16,
		America_Blanc_Sablon = -16,
		America_Boa_Vista = -16,
		America_Campo_Grande = -16,
		America_Caracas = -16,
		America_Cuiaba = -16,
		America_Curacao = -16,
		America_Dominica = -16,
		America_Glace_Bay = -16,
		America_Goose_Bay = -16,
		America_Grenada = -16,
		America_Guadeloupe = -16,
		America_Guyana = -16,
		America_Halifax = -16,
		America_Kralendijk = -16,
		America_La_Paz = -16,
		America_Lower_Princes = -16,
		America_Manaus = -16,
		America_Marigot = -16,
		America_Martinique = -16,
		America_Moncton = -16,
		America_Montserrat = -16,
		America_Porto_Velho = -16,
		America_Port_of_Spain = -16,
		America_Puerto_Rico = -16,
		America_Santiago = -16,
		America_Santo_Domingo = -16,
		America_St_Barthelemy = -16,
		America_St_Kitts = -16,
		America_St_Lucia = -16,
		America_St_Thomas = -16,
		America_St_Vincent = -16,
		America_Thule = -16,
		America_Tortola = -16,
		Atlantic_Bermuda = -16,
		America_St_Johns = -14,
		America_Araguaina = -12,
		America_Argentina_Buenos_Aires = -12,
		America_Argentina_Catamarca = -12,
		America_Argentina_Cordoba = -12,
		America_Argentina_Jujuy = -12,
		America_Argentina_La_Rioja = -12,
		America_Argentina_Mendoza = -12,
		America_Argentina_Rio_Gallegos = -12,
		America_Argentina_Salta = -12,
		America_Argentina_San_Juan = -12,
		America_Argentina_San_Luis = -12,
		America_Argentina_Tucuman = -12,
		America_Argentina_Ushuaia = -12,
		America_Bahia = -12,
		America_Belem = -12,
		America_Cayenne = -12,
		America_Fortaleza = -12,
		America_Godthab = -12,
		America_Maceio = -12,
		America_Miquelon = -12,
		America_Montevideo = -12,
		America_Paramaribo = -12,
		America_Punta_Arenas = -12,
		America_Recife = -12,
		America_Santarem = -12,
		America_Sao_Paulo = -12,
		Antarctica_Palmer = -12,
		Antarctica_Rothera = -12,
		Atlantic_Stanley = -12,
		America_Noronha = -8,
		Atlantic_South_Georgia = -8,
		America_Scoresbysund = -4,
		Atlantic_Azores = -4,
		Atlantic_Cape_Verde = -4,
		Africa_Abidjan = 0,
		Africa_Accra = 0,
		Africa_Bamako = 0,
		Africa_Banjul = 0,
		Africa_Bissau = 0,
		Africa_Casablanca = 0,
		Africa_Conakry = 0,
		Africa_Dakar = 0,
		Africa_El_Aaiun = 0,
		Africa_Freetown = 0,
		Africa_Lome = 0,
		Africa_Monrovia = 0,
		Africa_Nouakchott = 0,
		Africa_Ouagadougou = 0,
		Africa_Sao_Tome = 0,
		America_Danmarkshavn = 0,
		Antarctica_Troll = 0,
		Atlantic_Canary = 0,
		Atlantic_Faroe = 0,
		Atlantic_Madeira = 0,
		Atlantic_Reykjavik = 0,
		Atlantic_St_Helena = 0,
		Europe_Dublin = 0,
		Europe_Guernsey = 0,
		Europe_Isle_of_Man = 0,
		Europe_Jersey = 0,
		Europe_Lisbon = 0,
		Europe_London = 0,
		Africa_Algiers = 4,
		Africa_Bangui = 4,
		Africa_Brazzaville = 4,
		Africa_Ceuta = 4,
		Africa_Douala = 4,
		Africa_Kinshasa = 4,
		Africa_Lagos = 4,
		Africa_Libreville = 4,
		Africa_Luanda = 4,
		Africa_Malabo = 4,
		Africa_Ndjamena = 4,
		Africa_Niamey = 4,
		Africa_Porto_Novo = 4,
		Africa_Tunis = 4,
		Africa_Windhoek = 4,
		Arctic_Longyearbyen = 4,
		Europe_Amsterdam = 4,
		Europe_Andorra = 4,
		Europe_Belgrade = 4,
		Europe_Berlin = 4,
		Europe_Bratislava = 4,
		Europe_Brussels = 4,
		Europe_Budapest = 4,
		Europe_Copenhagen = 4,
		Europe_Gibraltar = 4,
		Europe_Ljubljana = 4,
		Europe_Luxembourg = 4,
		Europe_Madrid = 4,
		Europe_Malta = 4,
		Europe_Monaco = 4,
		Europe_Oslo = 4,
		Europe_Paris = 4,
		Europe_Podgorica = 4,
		Europe_Prague = 4,
		Europe_Rome = 4,
		Europe_San_Marino = 4,
		Europe_Sarajevo = 4,
		Europe_Skopje = 4,
		Europe_Stockholm = 4,
		Europe_Tirane = 4,
		Europe_Vaduz = 4,
		Europe_Vatican = 4,
		Europe_Vienna = 4,
		Europe_Warsaw = 4,
		Europe_Zagreb = 4,
		Europe_Zurich = 4,
		Africa_Blantyre = 8,
		Africa_Bujumbura = 8,
		Africa_Cairo = 8,
		Africa_Gaborone = 8,
		Africa_Harare = 8,
		Africa_Johannesburg = 8,
		Africa_Juba = 8,
		Africa_Khartoum = 8,
		Africa_Kigali = 8,
		Africa_Lubumbashi = 8,
		Africa_Lusaka = 8,
		Africa_Maputo = 8,
		Africa_Maseru = 8,
		Africa_Mbabane = 8,
		Africa_Tripoli = 8,
		Asia_Amman = 8,
		Asia_Beirut = 8,
		Asia_Damascus = 8,
		Asia_Famagusta = 8,
		Asia_Gaza = 8,
		Asia_Hebron = 8,
		Asia_Jerusalem = 8,
		Asia_Nicosia = 8,
		Europe_Athens = 8,
		Europe_Bucharest = 8,
		Europe_Chisinau = 8,
		Europe_Helsinki = 8,
		Europe_Kaliningrad = 8,
		Europe_Kiev = 8,
		Europe_Mariehamn = 8,
		Europe_Riga = 8,
		Europe_Sofia = 8,
		Europe_Tallinn = 8,
		Europe_Uzhgorod = 8,
		Europe_Vilnius = 8,
		Europe_Zaporozhye = 8,
		Africa_Addis_Ababa = 12,
		Africa_Asmara = 12,
		Africa_Dar_es_Salaam = 12,
		Africa_Djibouti = 12,
		Africa_Kampala = 12,
		Africa_Mogadishu = 12,
		Africa_Nairobi = 12,
		Antarctica_Syowa = 12,
		Asia_Aden = 12,
		Asia_Baghdad = 12,
		Asia_Bahrain = 12,
		Asia_Kuwait = 12,
		Asia_Qatar = 12,
		Asia_Riyadh = 12,
		Europe_Istanbul = 12,
		Europe_Kirov = 12,
		Europe_Minsk = 12,
		Europe_Moscow = 12,
		Europe_Simferopol = 12,
		Europe_Volgograd = 12,
		Indian_Antananarivo = 12,
		Indian_Comoro = 12,
		Indian_Mayotte = 12,
		Asia_Tehran = 14,
		Asia_Baku = 16,
		Asia_Dubai = 16,
		Asia_Muscat = 16,
		Asia_Tbilisi = 16,
		Asia_Yerevan = 16,
		Europe_Astrakhan = 16,
		Europe_Samara = 16,
		Europe_Saratov = 16,
		Europe_Ulyanovsk = 16,
		Indian_Mahe = 16,
		Indian_Mauritius = 16,
		Indian_Reunion = 16,
		Asia_Kabul = 18,
		Antarctica_Mawson = 20,
		Asia_Aqtau = 20,
		Asia_Aqtobe = 20,
		Asia_Ashgabat = 20,
		Asia_Atyrau = 20,
		Asia_Dushanbe = 20,
		Asia_Karachi = 20,
		Asia_Oral = 20,
		Asia_Qyzylorda = 20,
		Asia_Samarkand = 20,
		Asia_Tashkent = 20,
		Asia_Yekaterinburg = 20,
		Indian_Kerguelen = 20,
		Indian_Maldives = 20,
		Asia_Colombo = 22,
		Asia_Kolkata = 22,
		Asia_Kathmandu = 23,
		Antarctica_Vostok = 24,
		Asia_Almaty = 24,
		Asia_Bishkek = 24,
		Asia_Dhaka = 24,
		Asia_Omsk = 24,
		Asia_Qostanay = 24,
		Asia_Thimphu = 24,
		Asia_Urumqi = 24,
		Indian_Chagos = 24,
		Asia_Yangon = 26,
		Indian_Cocos = 26,
		Antarctica_Davis = 28,
		Asia_Bangkok = 28,
		Asia_Barnaul = 28,
		Asia_Hovd = 28,
		Asia_Ho_Chi_Minh = 28,
		Asia_Jakarta = 28,
		Asia_Krasnoyarsk = 28,
		Asia_Novokuznetsk = 28,
		Asia_Novosibirsk = 28,
		Asia_Phnom_Penh = 28,
		Asia_Pontianak = 28,
		Asia_Tomsk = 28,
		Asia_Vientiane = 28,
		Indian_Christmas = 28,
		Asia_Brunei = 32,
		Asia_Choibalsan = 32,
		Asia_Hong_Kong = 32,
		Asia_Irkutsk = 32,
		Asia_Kuala_Lumpur = 32,
		Asia_Kuching = 32,
		Asia_Macau = 32,
		Asia_Makassar = 32,
		Asia_Manila = 32,
		Asia_Shanghai = 32,
		Asia_Singapore = 32,
		Asia_Taipei = 32,
		Asia_Ulaanbaatar = 32,
		Australia_Perth = 32,
		Australia_Eucla = 35,
		Asia_Chita = 36,
		Asia_Dili = 36,
		Asia_Jayapura = 36,
		Asia_Khandyga = 36,
		Asia_Pyongyang = 36,
		Asia_Seoul = 36,
		Asia_Tokyo = 36,
		Asia_Yakutsk = 36,
		Pacific_Palau = 36,
		Australia_Adelaide = 38,
		Australia_Broken_Hill = 38,
		Australia_Darwin = 38,
		Antarctica_DumontDUrville = 40,
		Antarctica_Macquarie = 40,
		Asia_Ust_Nera = 40,
		Asia_Vladivostok = 40,
		Australia_Brisbane = 40,
		Australia_Currie = 40,
		Australia_Hobart = 40,
		Australia_Lindeman = 40,
		Australia_Melbourne = 40,
		Australia_Sydney = 40,
		Pacific_Chuuk = 40,
		Pacific_GuamVillage = 40,
		Pacific_Port_Moresby = 40,
		Pacific_Saipan = 40,
		Australia_Lord_Howe = 42,
		Antarctica_Casey = 44,
		Asia_Magadan = 44,
		Asia_Sakhalin = 44,
		Asia_Srednekolymsk = 44,
		Pacific_Bougainville = 44,
		Pacific_Efate = 44,
		Pacific_Guadalcanal = 44,
		Pacific_Kosrae = 44,
		Pacific_Norfolk = 44,
		Pacific_Noumea = 44,
		Pacific_Pohnpei = 44,
		Antarctica_McMurdo = 48,
		Asia_Anadyr = 48,
		Asia_Kamchatka = 48,
		Pacific_Auckland = 48,
		Pacific_Fiji = 48,
		Pacific_Funafuti = 48,
		Pacific_Kwajalein = 48,
		Pacific_Majuro = 48,
		Pacific_Nauru = 48,
		Pacific_Tarawa = 48,
		Pacific_Wake = 48,
		Pacific_Wallis = 48,
		Pacific_Chatham = 51,
		Pacific_Apia = 52,
		Pacific_Enderbury = 52,
		Pacific_Fakaofo = 52,
		Pacific_Tongatapu = 52,
		Pacific_Kiritimati = 56,
	};

	TimeZones() {}

	TimeZones(const TimeZoneName val) :value(val) {	}

	TimeZones(int8_t val) :value(val) {	}

	operator int8_t() {
		return value;
	}

	operator int8_t() const {
		return value;
	}

	TimeZones& operator=(const int8_t val) {
		value = val;
		return *this;
	}

protected:
	int8_t value = 0;
};


/**
* @struct TimeZone
* @brief Structure, which has information about time zone.
* Time zone has resolution of 15 miutes.
*/
struct TimeZone {

	/**
	* @brief Creates TimeZone with UTC time zone.
	*/
	TimeZone() :
		timeZoneOffset15min(0)
	{ }

	/**
	* @brief Creates TimeZone.
	* @param tzOffsetHours Time zone offset in hours.
	*/
	explicit TimeZone(int8_t tzOffsetHours) :
		timeZoneOffset15min(tzOffsetHours * 4)
	{ }

	/**
	* @brief Creates TimeZone.
	* @param tz Time zone.
	*/
	TimeZone(TimeZones tz) :
		timeZoneOffset15min(tz)
	{ }

	/**
	* @brief Creates TimeZone.
	* @param tz Time zone.
	*/
	TimeZone(TimeZones::TimeZoneName tz) :
		timeZoneOffset15min(tz)
	{ }

	/**
	* @brief Creates TimeZone.
	* @param tzOffsetHours Time zone offset in hours.
	* @param tzOffsetMinutes Time zone offset minutes part. If tzOffsetHours is negative, this has to be also negative.
	*/
	TimeZone(int8_t tzOffsetHours, int8_t tzOffsetMinutes) :
		timeZoneOffset15min((tzOffsetHours * 4) + (tzOffsetMinutes % 60) / 15)
	{ }

	/**
	* @brief Sets time zone offset in hours.
	* @param tzOffsetHours Time zone offset in hours.
	*/
	inline void setTimeZone(int8_t tzOffsetHours) {
		timeZoneOffset15min = tzOffsetHours * 4;
	}

	/**
	* @brief Sets time zone offset from TimeZones.
	* @param tz Time zone.
	*/
	inline void setTimeZone(TimeZones tz) {
		timeZoneOffset15min = (int8_t)tz;
	}

	/**
	* @brief Sets time zone offset in hours.
	* @param tzOffsetHours Time zone offset in hours.
	* @param tzOffsetMinutes Time zone offset minutes part. If tzOffsetHours is negative, this has to be also negative.
	*/
	inline void setTimeZone(int8_t tzOffsetHours, int8_t tzOffsetMinutes) {
		timeZoneOffset15min = (tzOffsetHours * 4) + (tzOffsetMinutes % 60) / 15;
	}

	/**
	* @brief Gets time zone offset in hours.
	* @return Returns time zone offset in hours.
	*/
	inline int8_t getTimeZoneHours() const {
		return timeZoneOffset15min / 4;
	}

	/**
	* @brief Gets minutes part of time zone offset.
	* @return Returns minutes part of time zone offset.
	*/
	inline int8_t getTimeZoneMinutes() const {
		return (timeZoneOffset15min % 4) * 15;
	}

	/**
	* @brief Gets time zone offset as TimeZones.
	* @return Returns time zone offset as TimeZones.
	*/
	inline TimeZones getTimeZone() const {
		return timeZoneOffset15min;
	}

	/**
	* @brief Gets time zone offset represented by TimeSpan.
	*/
	inline TimeSpan getTimeZoneOffset() const {
		return ((int64_t)timeZoneOffset15min) * 15 * MINUTE;
	}

	/**
	* @brief Gets time zone offset in minutes.
	*/
	inline int16_t getTimeZoneOffsetTotalMinutes() const {
		return ((int16_t)timeZoneOffset15min) * 15;
	}

	/**
	* @brief Factory function, which creates TimeZone from minutes offset.
	* @param offset Time zone offset in minutes.
	* @return Returns created time zone.
	*/
	static TimeZone fromTotalMinutesOffset(int16_t offset) {
		TimeZone tz;
		tz.timeZoneOffset15min = offset / 15;
		return tz;
	}

protected:
	int8_t timeZoneOffset15min;
};


/**
* @enum DSTTransitionRuleType
* @brief Enumeration of DST transition rule types.
*/
typedef enum {
	DSTTRT_Fixed = 0,
	DSTTRT_Date = 1,
	DSTTRT_Floating = 2,
	DSTTRT_NoDST = 3
}DSTTransitionRuleType;

/**
* @struct DSTTransitionRule
* @brief Structure, that represents DST transition rule. Transition rule
* says, when DST should change.
* There are 3 types of this rule:
* - **Fixed** - defined by day of year.
* - **Date** - defined by month, day of month and days offset.
* - **Floating** - defined by month, day of week, week of month and days offset.
*/
struct DSTTransitionRule {

	/**
	* @brief Cretes DSTTransitionRule with no DST transition.
	*/
	DSTTransitionRule() : rawValue(DSTTransitionRuleType::DSTTRT_NoDST)
	{ }

	/**
	* @brief Sets fixed DST transition rule. Fixed rule is defined by day of year.
	* @param transitionHour Hour, when transition happends.
	* @param dayOfYear Day of year from range 0-364 or 0-365 for leap years.
	*/
	DSTTransitionRule(hour_t transitionHour, uint16_t dayOfYear) {
		setFixed(transitionHour, dayOfYear);
	}

	/**
	* @brief Sets fixed DST transition rule. Fixed rule is defined by day of year.
	* @param transitionHour Hour, when transition happends.
	* @param dayOfYear Day of year from range 0-364 or 0-365 for leap years.
	*/
	inline static DSTTransitionRule Fixed(hour_t transitionHour, uint16_t dayOfYear) {
		return DSTTransitionRule(transitionHour, dayOfYear);
	}

	/**
	* @brief Sets DST transition rule by date. Date rule is defined by month and day of month.
	* @param transitionHour Hour, when transition happends.
	* @param month Month in range from 1-12.
	* @param dayOfMonth Day of month in range from 1-28/29/30/31.
	* @param dayOffset Days offset in range from -9 to +8.
	*/
	DSTTransitionRule(hour_t transitionHour, Month month, uint8_t dayOfMonth, int8_t dayOffset = 0) {
		setDate(transitionHour, month, dayOfMonth, dayOffset);
	}

	/**
	* @brief Sets DST transition rule by date. Date rule is defined by month and day of month.
	* @param transitionHour Hour, when transition happends.
	* @param month Month in range from 1-12.
	* @param dayOfMonth Day of month in range from 1-28/29/30/31.
	* @param dayOffset Days offset in range from -9 to +8.
	*/
	inline static DSTTransitionRule Date(hour_t transitionHour, Month month, uint8_t dayOfMonth, int8_t dayOffset = 0) {
		return DSTTransitionRule(transitionHour, month, dayOfMonth, dayOffset);
	}

	/**
	* @brief Sets floating DST transition. Floating rule is defined by month, day of week, week of month and days offset.
	* @param transitionHour Hour, when transition happends.
	* @param month Month in range from 1-12.
	* @param dayOfWeek Day of week in range from 1 (Sunday) to 7 (Saturday).
	* @param weekOfMonth Week of month from WeekOfMonth enum.
	* @param dayOffset Days offset in range from -9 to +8.
	*/
	DSTTransitionRule(hour_t transitionHour, Month month, DayOfWeek dayOfWeek, WeekOfMonth weekOfMonth, int8_t dayOffset = 0) {
		setFloating(transitionHour, month, dayOfWeek, weekOfMonth, dayOffset);
	}

	/**
	* @brief Sets floating DST transition. Floating rule is defined by month, day of week, week of month and days offset.
	* @param transitionHour Hour, when transition happends.
	* @param month Month in range from 1-12.
	* @param dayOfWeek Day of week in range from 1 (Sunday) to 7 (Saturday).
	* @param weekOfMonth Week of month from WeekOfMonth enum.
	* @param dayOffset Days offset in range from -9 to +8.
	*/
	inline static DSTTransitionRule Floating(hour_t transitionHour, Month month, DayOfWeek dayOfWeek, WeekOfMonth weekOfMonth, int8_t dayOffset = 0) {
		return DSTTransitionRule(transitionHour, month, dayOfWeek, weekOfMonth, dayOffset);
	}

	/**
	* @brief Sets fixed DST transition rule. Fixed rule is defined by day of year.
	* @param transitionHour Hour, when transition happends.
	* @param dayOfYear Day of year from range 0-364 or 0-365 for leap years.
	*/
	void setFixed(hour_t transitionHour, uint16_t dayOfYear);

	/**
	* @brief Sets DST transition rule by date. Date rule is defined by month and day of month.
	* @param transitionHour Hour, when transition happends.
	* @param month Month in range from 1-12.
	* @param dayOfMonth Day of month in range from 1-28/29/30/31.
	* @param dayOffset Days offset in range from -9 to +8.
	*/
	void setDate(hour_t transitionHour, Month month, uint8_t dayOfMonth, int8_t dayOffset = 0);

	/**
	* @brief Sets floating DST transition. Floating rule is defined by month, day of week, week of month and days offset.
	* @param transitionHour Hour, when transition happends.
	* @param month Month in range from 1-12.
	* @param dayOfWeek Day of week in range from 1 (Sunday) to 7 (Saturday).
	* @param weekOfMonth Week of month from WeekOfMonth enum.
	* @param dayOffset Days offset in range from -9 to +8.
	*/
	void setFloating(hour_t transitionHour, Month month, DayOfWeek dayOfWeek, WeekOfMonth weekOfMonth, int8_t dayOffset = 0);

	bool operator==(const DSTTransitionRule& d) const {
		return d.rawValue == rawValue;
	}

	bool operator!=(const DSTTransitionRule& d) const {
		return d.rawValue != rawValue;
	}

	/**
	* @brief Gets type of rule.
	*/
	inline DSTTransitionRuleType getType() const {
		return (DSTTransitionRuleType)(rawValue & 0x03U);
	}

	/**
	* @brief Checks if no DST is set.
	* @return Returns true if no DST is set and DST adjustment is inactive.
	*/
	inline bool noDST() const {
		return getType() == DSTTransitionRuleType::DSTTRT_NoDST;
	}

	/**
	* @brief Gets time in hours, when transition happends.
	*/
	inline hour_t getTransitionTime() const {
		return ((rawValue >> 16) & 0x3FU);
	}

	/**
	* @brief Set transition time.
	* @param hour Hours, when transition happends.
	*/
	inline void setTransitionTime(hour_t hour) {
		hour.convertTo24();
		rawValue = (rawValue & (~(0x3FUL << 16))) | (((uint32_t)(hour & 0x3FU)) << 16);
	}

	/**
	* @brief Gets days offset in range from -9 to +8.
	*/
	int8_t getDaysOffset() const;

	/**
	* @brief Sets days offset in range from -9 to +8.
	* @param val New days offset in range from -9 to +8 days.
	*/
	void setDaysOffset(int8_t val);

	/**
	* @brief Gets day of year in range from 0-364 or 0-365 for leap year.
	* @note This method can be used only if rule type is Fixed.
	*/
	inline uint16_t getDayOfYear() const {
		return (rawValue >> 2) & 0x03FFU;
	}

	/**
	* @brief Gets month.
	* @note This method can be used only if rule type is Date or Floating.
	*/
	inline Month getMonth() const {
		return (rawValue >> 2) & 0x0FU;
	}

	/**
	* @brief Gets day of month in range from 1 to 28/29/30/31.
	* @note This method can be used only if rule type is Date.
	*/
	inline uint8_t getDayOfMonth() const {
		return ((rawValue >> 6) & 0x3FU);
	}

	/**
	* @brief Gets day of week in range from 1 (Sunday) to 7 (Saturday).
	* @note This method can be used only if rule type is Floating.
	*/
	inline DayOfWeek getDayOfWeek() const {
		return (DayOfWeek)((rawValue >> 6) & 0x07U);
	}

	/**
	* @brief Gets week of month from WeekOfMonth enum.
	* @note This method can be used only if rule type is Floating.
	*/
	inline WeekOfMonth getWeekOfMonth() const {
		return (WeekOfMonth)((rawValue >> 9) & 0x07U);
	}

	/**
	* @brief Gets day of year (zero based), when transition will happend.
	* @param year Transition date will be established for that year. Value can be from MIN_YEAR to MAX_YEAR except 0.
	* @return Returns count of days from first day in year (zero based), when transition will happend. This value can be
	* in range 0-364 or 0-365 for leap year.
	*/
	uint16_t getDayOfYearOfTransition(int32_t year) const;

	/**
	* @brief Gets date and time of transition of current rule in specified year.
	* @param year Transition date will be established for that year. Value can be from MIN_YEAR to MAX_YEAR except 0.
	* @return Returns DateTime of transition. For start rule this value is without DST and for end rule this value is with DST.
	*/
	template<class retT = DateTime>
	retT getDateOfTransition(int32_t year) const;

	/**
	* @brief No DST transition rule.
	*/
	static const DSTTransitionRule NoDST;

protected:

	friend struct DSTAdjustment;

	/**
	* @brief Gets date and time of transition of current rule in specified year.
	* @param year Transition date will be established for that year. Value can be from MIN_YEAR to MAX_YEAR except 0.
	* @return Returns DateTime of transition. For start rule this value is without DST and for end rule this value is with DST.
	*/
	int64_t getDateOfTransitionRaw(int32_t year) const;

	/**
	* @brief Helper function, which gets day of year from transition rule and some pre-calculated arguments.
	* @param daysUntilFirstDayOfYear Days until first day of current year.
	* @param isLeap True if current year is leap day.
	* @return Returns day of year (zero based) from range 0-364 or 0-365 for leap year.
	*/
	uint16_t getDayOfYearOfTransitionHelper(int32_t daysUntilFirstDayOfYear, bool isLeap) const;

	/**
	* @brief Compressed rule to 3 bytes. First 2 bits (LSB) are type of rule:
	* - **Fixed** - exact day of year, when transition happen, is set.
	*   Structure or raw value then looks like this:
	*   | Transition time [21:16] | Offset sign [15] | Offset [14:12] | DayOfYear [11:2] | Type [1:0] |
	*   |-------------------------|------------------|----------------|------------------|------------|
	*   |          8 bits         |      1 bit       |     3 bits     |      10 bits     |   2 bits   |
	*
	* - **Date** - day, when transition happen, is set by month, day of month and days offset.
	*   Structure or raw value then looks like this:
	*   | Transition time [21:16] | Offset sign [15] | Offset [14:12] | DayOfMonth [11:6] | Month [5:2] | Type [1:0] |
	*   |-------------------------|------------------|----------------|-------------------|-------------|------------|
	*   |          8 bits         |      1 bit       |     3 bits     |      6 bits       |   4 bits    |   2 bits   |
	*
	* - **Floating** - day, when transition happen, is set by month, day of week, week of month and days offset.
	*   Structure or raw value then looks like this:
	*   | Transition time [21:16] | Offset sign [15] | Offset [14:12] | WeekOfMonth [11:9] | DayOfWeek [8:6] | Month [5:2] | Type [1:0] |
	*   |-------------------------|------------------|----------------|--------------------|-----------------|-------------|------------|
	*   |          8 bits         |      1 bit       |     3 bits     |       3 bits       |     3 bits      |   4 bits    |   2 bits   |
	*
	* @note Transition time is in hours. Available values are 00 to 23.
	*/
	uint32_t rawValue : 24;
};


template<class retT>
retT DSTTransitionRule::getDateOfTransition(int32_t year) const {
	return (retT)getDateOfTransitionRaw(year);
}

/**
* @struct DSTAdjustment
* @param DST adjustment structure, which defines when DST transition start and end.
* It also defines DST offset time with resolution of 15 minutes. There is flag, which
* can be used to say, if DST is applyied. There are predefined DST adjustment rules for
* some countries and regions. See @ref DSTAdjRules "DST adjustment rules".
* @note All values has to be set in local time zone.
*/
struct DSTAdjustment {
public:

	/**
	* @brief Creates default DSTAdjustment with no DST.
	*/
	DSTAdjustment() :
		DSTOffset15min(0),
		isDST_(0)
	{ }

	/**
	* @brief Creates DSTAdjustment.
	* @param dltStart Rule, which says, when Daylight saving transition starts.
	* @param dltEnd Rule, which says, when Daylight saving transition ends.
	* @param DSTOffsetHours DST offset in hours.
	* @param isDST True if DST is applyied.
	*/
	DSTAdjustment(DSTTransitionRule dltStart, DSTTransitionRule dltEnd, int8_t DSTOffsetHours = 0, bool isDST = false) :
		DaylightTransitionStart(dltStart),
		DaylightTransitionEnd(dltEnd),
		DSTOffset15min(DSTOffsetHours * 4),
		isDST_(isDST)
	{ }

	/**
	* @brief Creates DSTAdjustment.
	* @param dltStart Rule, which says, when Daylight saving transition starts.
	* @param dltEnd Rule, which says, when Daylight saving transition ends.
	* @param DSTOffsetHours DST offset in hours.
	* @param DSTOffsetMinutes DST offset minutes part. If DSTOffsetHours is negative, this has to be also negative.
	* @param isDST True if DST is applyied.
	*/
	DSTAdjustment(DSTTransitionRule dltStart, DSTTransitionRule dltEnd, int8_t DSTOffsetHours, int8_t DSTOffsetMinutes, bool isDST = false) :
		DaylightTransitionStart(dltStart),
		DaylightTransitionEnd(dltEnd),
		DSTOffset15min((DSTOffsetHours * 4) + (DSTOffsetMinutes % 60) / 15),
		isDST_(isDST)
	{ }

	/**
	* @brief Copies DSTAdjustment.
	*/
	DSTAdjustment(const DSTAdjustment& adj, bool isDST) :
		DaylightTransitionStart(adj.DaylightTransitionStart),
		DaylightTransitionEnd(adj.DaylightTransitionEnd),
		DSTOffset15min(adj.DSTOffset15min),
		isDST_(isDST)
	{ }

	/**
	* @brief Creates DSTAdjustment.
	* @param dltStart Rule, which says, when Daylight saving transition starts.
	* @param dltEnd Rule, which says, when Daylight saving transition ends.
	* @param DSTOffsetTotalMinutes DST offset in total minutes.
	* @param isDST True if DST is applyied.
	*/
	static DSTAdjustment fromTotalMinutesOffset(DSTTransitionRule dltStart, DSTTransitionRule dltEnd, int16_t DSTOffsetTotalMinutes, bool isDST = false) {
		DSTAdjustment tmp(dltStart, dltEnd, 0, isDST);
		tmp.DSTOffset15min = DSTOffsetTotalMinutes / 15;
		return tmp;
	}

	/**
	* @brief Rule, which says, when Daylight saving transition starts.
	* @note This rule has to be set **without** applyied DST time.
	*/
	DSTTransitionRule DaylightTransitionStart;

	/**
	* @brief Rule, which says, when Daylight saving transition ends.
	* @note This rule has to be set **with** applyied DST time.
	*/
	DSTTransitionRule DaylightTransitionEnd;

	/**
	* @brief Checks if no DST is set.
	* @return Returns true if no DST is set and DST adjustment is inactive.
	*/
	inline bool noDST() const {
		return DaylightTransitionStart.getType() == DSTTransitionRuleType::DSTTRT_NoDST
			|| DaylightTransitionEnd.getType() == DSTTransitionRuleType::DSTTRT_NoDST
			|| DSTOffset15min == 0;
	}

	/**
	* @brief Sets DST offset in hours.
	* @param DSTOffsetHours DST offset in hours.
	*/
	inline void setDSTOffset(int8_t DSTOffsetHours) {
		DSTOffset15min = DSTOffsetHours * 4;
	}

	/**
	* @brief Sets DST offset in hours.
	* @param DSTOffsetHours DST offset in hours.
	* @param DSTOffsetMinutes DST offset minutes part. If tzOffsetHours is negative, this has to be also negative.
	*/
	inline void setDSTOffset(int8_t DSTOffsetHours, int8_t DSTOffsetMinutes) {
		DSTOffset15min = (DSTOffsetHours * 4) + (DSTOffsetMinutes % 60) / 15;
	}

	/**
	* @brief Gets DST offset represented by TimeSpan.
	*/
	inline TimeSpan getDSTOffset() const {
		return ((int64_t)DSTOffset15min) * 15 * MINUTE;
	}
	
	/**
	* @brief Gets time DST in hours.
	* @return Returns time DST in hours.
	*/
	inline int8_t getDSTOffsetHours() const {
		return DSTOffset15min / 4;
	}

	/**
	* @brief Gets minutes part of DST offset.
	* @return Returns minutes part of DST offset.
	*/
	inline int8_t getDSTOffsetMinutes() const {
		return (DSTOffset15min % 4) * 15;
	}

	/**
	* @brief Gets DST offset in minutes.
	* @return Returns DST offset in minutes.
	*/
	inline int16_t getDSTOffsetTotalMinutes() const {
		return ((int16_t)DSTOffset15min) * 15;
	}

	/**
	* @brief Enables or disabled DST.
	* @param enabled True to enable DST.
	*/
	inline void setDST(bool enabled) {
		isDST_ = enabled;
	}

	/**
	* @brief Gets if DST is enabled.
	* @return Returns true if DST is enabled.
	*/
	inline bool isDST() const {
		return isDST_;
	}

	/**
	* @brief Checks if DateTime (in raw form) is in DST region or not.
	* @param raw DateTime to check in microseconds from the begining of the epoch. This time has to be without applyied DST offset.
	* @return Returns true if specified DateTime is in DST region.
	*/
	bool checkDSTRegion(int64_t rawDT) const;

	/**
	* @brief Checks if DateTime is in DST region or not.
	* @param dt DateTime to check. This time has to be without applyied DST offset. Only DateTime classes, which have DST field
	* (for example DateTimeTZ, ...) can have DST offset applyied, because it will be automatically subtracted.
	* @return Returns true if specified DateTime is in DST region.
	*/
	template<class T>
	bool checkDSTRegion(const DateTimeBase<T>& dt) const;

	/**
	* @brief Gets DateTime of next transition.
	* @param dt Current date and time in microseconds from the begining of the epoch. This time has to be without applyied DST offset.
	* @param[out] nextTransIsDST True if next transition is to DST or false if not.
	* @return Returns DateTime of next transition or DateTime::Zero if there is no next transition date (No DST transition specified, or
	* DST is always applyied). This value has DST applyied only if nextTransIsDST is false.
	*/
	template<class retT = DateTime>
	retT getNextTransitionDate(int64_t raw, bool& nextTransIsDST) const;

	/**
	* @brief Gets DateTime of next transition.
	* @param dt Current date and time. This time has to be without applyied DST offset. Only DateTime classes, which have DST field
	* (for example DateTimeTZ, ...) can have DST offset applyied, because it will be automatically subtracted.
	* @param[out] nextTransIsDST True if next transition is to DST or false if not.
	* @return Returns DateTime of next transition or DateTime::Zero if there is no next transition date (No DST transition specified, or
	* DST is always applyied). This value has DST applyied only if nextTransIsDST is false.
	*/
	template<class T, class retT = DateTime>
	retT getNextTransitionDate(const DateTimeBase<T>& dt, bool& nextTransIsDST) const;

	/**
	 * @defgroup DSTAdjRules DST adjustment rules by countries and regions.
	 * + Last revision: 22nd of May 2022
	 * + Source: https://en.wikipedia.org/wiki/Daylight_saving_time_by_country
	 * @note Rule for Iran is not defined here, because in Iran Solar Hijri calendar is used instead of Gregorian calendar. This
	 * DateTime library works only with Gregorian calendar.
	 * @{
	 */

	 /**
	 * @brief DST adjustment rules for Countries with no DST.
	 */
	static const DSTAdjustment NoDST;

	/**
	* @brief DST adjustment rules for North America. Included regions/countries:
	* + Bahamas
	* + Bermudas
	* + Canada, except Yukon, most of Saskatchewan, and parts of British Columbia (northeast), Nunavut (Southampton Island), Ontario (Atikokan) and Quebec (Le Golfe-du-Saint-Laurent)
	* + Greenland: Thule Air Base
	* + Haiti
	* + Mexico: Baja California, and municipalities within 20 km (12 miles) of the U.S. border except in Sonora
	* + Saint Pierre and Miquelon
	* + Turks and Caicos Islands
	* + United States, except Arizona (without Navajo Nation), Hawaii and U.S. territories
	*/
	static const DSTAdjustment NorthAmerica;

	/**
	* @brief DST adjustment rules for Cuba.
	*/
	static const DSTAdjustment Cuba;

	/**
	* @brief DST adjustment rules for Mexico. Included regions/countries:
	* + Mexico, except Baja California, Quintana Roo, Sonora, municipalities within 20 km (12 miles) of the U.S. border, and Revillagigedo Islands
	*/
	static const DSTAdjustment Mexico;

	/**
	* @brief DST adjustment rules for Western Europe. Included regions/countries:
	* + Faroe Islands
	* + Guernsey
	* + Ireland
	* + Isle of Man
	* + Jersey
	* + Portugal
	* + United Kingdom
	*/
	static const DSTAdjustment WesternEurope;

	/**
	* @brief DST adjustment rules for Central Europe. Included regions/countries:
	* + Albania
	* + Andorra
	* + Belgium
	* + Bosnia and Herzegovina
	* + Croatia
	* + Czech Republic
	* + Denmark
	* + France
	* + Germany
	* + Gibraltar
	* + Hungary
	* + Italy
	* + Kosovo
	* + Liechtenstein
	* + Malta
	* + Monaco
	* + Montenegro
	* + Netherlands
	* + North Macedonia
	* + Norway
	* + Poland
	* + San Marino
	* + Serbia
	* + Slovakia
	* + Slovenia
	* + Spain
	* + Sweden
	* + Switzerland
	* + Vatican City
	*/
	static const DSTAdjustment CentralEurope;

	/**
	* @brief DST adjustment rules for Eastern Europe. Included regions/countries:
	* + Bulgaria
	* + Cyprus
	* + Estonia
	* + Finland
	* + Greece
	* + Latvia
	* + Lithuania
	* + Luxembourg
	* + Northern Cyprus
	* + Romania
	* + Ukraine
	*/
	static const DSTAdjustment EasternEurope;

	/**
	* @brief DST adjustment rules for Greenland, except Danmarkshavn and Thule Air Base.
	*/
	static const DSTAdjustment Greenland;

	/**
	* @brief DST adjustment rules for Moldova. Included regions:
	* + Moldova
	* + Transinistria
	*/
	static const DSTAdjustment Moldova;

	/**
	* @brief DST adjustment rules for Israel.
	*/
	static const DSTAdjustment Israel;

	/**
	* @brief DST adjustment rules for Lebanon.
	*/
	static const DSTAdjustment Lebanon;

	/**
	* @brief DST adjustment rules for Palestine.
	*/
	static const DSTAdjustment Palestine;

	/**
	* @brief DST adjustment rules for Syria.
	*/
	static const DSTAdjustment Syria;

	/**
	* @brief DST adjustment rules for Jordan.
	*/
	static const DSTAdjustment Jordan;

	/**
	* @brief DST adjustment rules for Chile, except Magallanes Region
	*/
	static const DSTAdjustment Chile;

	/**
	* @brief DST adjustment rules for Paraguay.
	*/
	static const DSTAdjustment Paraguay;

	/**
	* @brief DST adjustment rules for Australia. Included regions/countries:
	* + Australian Capital Territory
	* + Jervis Bay Territory
	* + New South Wales (except Lord Howe Island)
	* + Norfolk Island
	* + South Australia
	* + Tasmania and Victoria
	*/
	static const DSTAdjustment Australia;

	/**
	* @brief DST adjustment rules for Australia: Lord Howe Island.
	*/
	static const DSTAdjustment Australia_Lord_Howe_Island;

	/**
	* @brief DST adjustment rules for New Zeland.
	*/
	static const DSTAdjustment NewZeland;

	/**@}*/

protected:

	/**
	* @brief Gets DateTime of next transition.
	* @param dt Current date and time in microseconds from the begining of the epoch. This time has to be without applyied DST offset.
	* @param[out] nextTransIsDST True if next transition is to DST or false if not.
	* @return Returns DateTime of next transition or DateTime::Zero if there is no next transition date (No DST transition specified, or
	* DST is always applyied). This value has DST applyied only if nextTransIsDST is false.
	*/
	int64_t getNextTransitionDateRaw(int64_t raw, bool& nextTransIsDST) const;

	int8_t DSTOffset15min : 7;
	bool isDST_ : 1;
};


template<class T>
bool DSTAdjustment::checkDSTRegion(const DateTimeBase<T>& dt) const {
	if (noDST()) return false;

	int64_t rawDT = dt.getRaw();
	if constexpr (has_getCurrentDSTOffsetMinutes<T>::value) {
		rawDT -= ((int64_t)static_cast<const T*>(&dt)->getCurrentDSTOffsetMinutes()) * MINUTE; //Getting time without DST
	}

	return checkDSTRegion(rawDT);
}

template<class retT>
retT DSTAdjustment::getNextTransitionDate(int64_t raw, bool& nextTransIsDST) const {
	return (retT) getNextTransitionDateRaw(raw, nextTransIsDST);
}

template<class T, class retT>
retT DSTAdjustment::getNextTransitionDate(const DateTimeBase<T>& dt, bool& nextTransIsDST) const {
	nextTransIsDST = false;
	//if (noDST()) return DateTime(0);

	int64_t rawDT = dt.getRaw();
	if constexpr (has_getCurrentDSTOffsetMinutes<T>::value) {
		rawDT -= ((int64_t)static_cast<const T*>(&dt)->getCurrentDSTOffsetMinutes()) * MINUTE; //Getting time without DST
	}

	return (retT)getNextTransitionDate(rawDT, nextTransIsDST);
}



#define TIME_ZONE_INFO_TZ_NAME_SIZE        (45)
#define TIME_ZONE_INFO_TZ_ABR_NAME_SIZE    (8)


/**
* @class TimeZoneInfo
* @brief Class, that contains time zone and DST and it's names.
* @todo Add functions for reading system time zone and DST.
* @todo Documentation
*/
class TimeZoneInfo {
public:

	TimeZoneInfo() {
		standardName[0] = 0;
		daylightName[0] = 0;
		keyName[0] = 0;
		standardABR[0] = 0;
		daylightABR[0] = 0;
	}

	/**
	* @brief Converts POSIX time zone to TimeZoneInfo.
	* @param buffer Buffer, where POSIX time zone is.
	* @param bufferSize Size of buffer including null terminator.
	* @param[out] pos Position, where parsing ended. If error happened during parsing, negative or zero value is returned.
	* If parsing was successfull, positive non-zero value is returned.
	*/
	static TimeZoneInfo fromPOSIX(const char* buffer, int bufferSize, int& pos);

	/**
	* @brief Converts POSIX time zone to TimeZoneInfo.
	* @param buffer Buffer, where POSIX time zone is.
	* @param bufferSize Size of buffer including null terminator.
	*/
	static TimeZoneInfo fromPOSIX(const char* buffer, int bufferSize);

	/**
	* @brief Converts POSIX time zone to TimeZoneInfo.
	* @param text Text, where POSIX time zone is.
	* @param[out] pos Position, where parsing ended. If error happened during parsing, negative or zero value is returned.
	* If parsing was successfull, positive non-zero value is returned.
	*/
#ifdef ARDUINO
	static TimeZoneInfo fromPOSIX(String text, int& pos);
#else
	static TimeZoneInfo fromPOSIX(std::string text, int& pos);
#endif // ARDUINO

	/**
	* @brief Converts POSIX time zone to TimeZoneInfo.
	* @param text Text, where POSIX time zone is.
	*/
#ifdef ARDUINO
	static TimeZoneInfo fromPOSIX(String text);
#else
	static TimeZoneInfo fromPOSIX(std::string text);
#endif // ARDUINO

	

	/**
	* @brief Converts TimeZoneInfo to POSIX time zone.
	* @param buffer Buffer, where POSIX time zone will be written.
	* @param bufferSize Size of buffer including null terminator. (It is recommended to have buffer with size at least 60 characters.)
	* @return Returns pointer to buffer, where null terminator was inserted.
	*/
	char* toPOSIX(char* buffer, int bufferSize) const;

	/**
	* @brief Converts TimeZoneInfo to POSIX time zone string.
	* @param buffer Buffer, where POSIX time zone will be written.
	* @param bufferSize Size of buffer including null terminator. (It is recommended to have buffer with size at least 60 characters.)
	* @return Returns string with POSIX time zone.
	*/
	//TODO !!!!
/*#ifdef ARDUINO
	String toPOSIX(char* buffer, int bufferSize) const;
#else
	std::string toPOSIX(char* buffer, int bufferSize) const;
#endif // ARDUINO*/

	

	/**
	* @brief Time zone offset.
	*/
	TimeZone timeZone;

	/**
	* @brief DST adjustment rules and offset.
	*/
	DSTAdjustment DST;

	/**
	* @brief Name of time zone without DST. On Windows it can be translated to system language.
	*/
	char standardName[TIME_ZONE_INFO_TZ_NAME_SIZE];
	
	/**
	* @brief Name of time zone with DST. On Windows it can be translated to system language.
	*/
	char daylightName[TIME_ZONE_INFO_TZ_NAME_SIZE];

	/**
	* @brief Key name of time zone. On Linux and Mac Olson name (e.g. "Europe/Paris") on Windows
	* Windows time zone name (e.g. "Standard European Time").
	*/
	char keyName[TIME_ZONE_INFO_TZ_NAME_SIZE];

	/**
	* @brief Abbreviation time zone name, for example: "CET".
	*/
	char standardABR[TIME_ZONE_INFO_TZ_ABR_NAME_SIZE];

	/**
	* @brief Abbreviation time zone name with DST, for example: "CEST".
	*/
	char daylightABR[TIME_ZONE_INFO_TZ_ABR_NAME_SIZE];

	/**
	* @brief Empty time zone info.
	*/
	static const TimeZoneInfo Empty;

protected:

	/**
	* @brief Copyies time zone abbreviation name from bufferFrom to bufferTo. Copying stops
	* when end of abbreviation name is found.
	* @param bufferFrom Buffer, to copy abbreviation name from.
	* @param bufferFromSize Size of bufferFrom including null terminator.
	* @param bufferTo Buffer, to copy abbreviation name to.
	* @return Returns pointer to character after last copyied character from bufferFrom.
	*/
	static const char* copyABR(const char* bufferFrom, size_t bufferFromSize, char* bufferTo);

	/**
	* @brief Converts two string with two integer digits with optional + or - sign.
	* @param bufferFrom Buffer, to parse from.
	* @param bufferFromSize Size of bufferFrom including null terminator.
	* @param[out] value Parsed value. If parsing fails INT_MIN is returned.
	* @return Returns pointer to character after last parsed character from bufferFrom. If parsing
	* failed, bufferFrom is returned.
	*/
	static const char* atoiTZ(const char* bufferFrom, int bufferFromSize, int& value, uint8_t digitsLimit);

	/**
	* @brief Parses offset time specified in POSIX time zone.
	* @param bufferFrom Buffer, with POSIX time zone. Contains position of one char after last parsed char or char, where error happened.
	* @param bufferFromSize Size of bufferFrom including null terminator. Parsed char count is subtracted from this value after parsing.
	* @param parsed_min[out] Parsed offset time in minutes.
	* @return Returns true if parsing was successfull.
	*/
	static bool parseOffsTime(const char*& buffer, int& bufferSize, int16_t& parsed_min);

	/**
	* @brief Parses DST transition from POSIX time zone
	* @param bufferFrom Buffer, with POSIX time zone. Contains position of one char after last parsed char or char, where error happened.
	* @param bufferFromSize Size of bufferFrom including null terminator. Parsed char count is subtracted from this value after parsing.
	* @return Returns DSTTransitionRule::NoDST at error;
	*/
	static DSTTransitionRule parseDSTTrans(const char*& buffer, int& bufferSize);

	/**
	* @brief Converts number with three digits to string.
	* @param[in,out] buffer Buffer, where number will be inserted. After calling this function, it will contain pointer to buffer, where null
	* terminator was inserted.
	* @param[in,out] bufferSize Size of buffer including null terminator. After calling this function, amount of inserted characters
	* (except null terminator) is subtracted from this value.
	* @param val Value to insert from range 0 to 999.
	* @return Returns true if there were enough space in buffer.
	*/
	static bool fi3toaHelper(char*& buffer, int& bufferSize, uint16_t val);

	/**
	* @brief Converts offset time to string for POSIX offset and time.
	* @param[out] buffer Buffer, where to write offset time.
	* @param bufferSize Buffer size including null terminator.
	* @param offset Offset in minutes.
	* @return Returns pointer to buffer, where null terminator was inserted.
	*/
	static char* offsTimeToStr(char* buffer, int bufferSize, int offset);
};

#endif // !TIME_ZONE_H