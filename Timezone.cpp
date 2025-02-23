//
//  Timezone.cpp
//
//  Created by Daniil A Megrabyan on 14.02.17.
//  Copyright © 2017 Cadabra Systems. All rights reserved.
//

#include "Timezone.hpp"

#include "String.hpp"

namespace chaos {
	const bimap<timezone::code, std::string> timezone::map = {
		{
			{timezone::code::africa_abidjan, "africa/abidjan"},
			{timezone::code::africa_accra, "africa/accra"},
			{timezone::code::africa_addis_ababa, "africa/addis_ababa"},
			{timezone::code::africa_algiers, "africa/algiers"},
			{timezone::code::africa_asmara, "africa/asmara"},
			{timezone::code::africa_bamako, "africa/bamako"},
			{timezone::code::africa_bangui, "africa/bangui"},
			{timezone::code::africa_banjul, "africa/banjul"},
			{timezone::code::africa_bissau, "africa/bissau"},
			{timezone::code::africa_blantyre, "africa/blantyre"},
			{timezone::code::africa_brazzaville, "africa/brazzaville"},
			{timezone::code::africa_bujumbura, "africa/bujumbura"},
			{timezone::code::africa_cairo, "africa/cairo"},
			{timezone::code::africa_casablanca, "africa/casablanca"},
			{timezone::code::africa_ceuta, "africa/ceuta"},
			{timezone::code::africa_conakry, "africa/conakry"},
			{timezone::code::africa_dakar, "africa/dakar"},
			{timezone::code::africa_dar_es_salaam, "africa/dar_es_salaam"},
			{timezone::code::africa_djibouti, "africa/djibouti"},
			{timezone::code::africa_douala, "africa/douala"},
			{timezone::code::africa_el_aaiun, "africa/el_aaiun"},
			{timezone::code::africa_freetown, "africa/freetown"},
			{timezone::code::africa_gaborone, "africa/gaborone"},
			{timezone::code::africa_harare, "africa/harare"},
			{timezone::code::africa_johannesburg, "africa/johannesburg"},
			{timezone::code::africa_juba, "africa/juba"},
			{timezone::code::africa_kampala, "africa/kampala"},
			{timezone::code::africa_khartoum, "africa/khartoum"},
			{timezone::code::africa_kigali, "africa/kigali"},
			{timezone::code::africa_kinshasa, "africa/kinshasa"},
			{timezone::code::africa_lagos, "africa/lagos"},
			{timezone::code::africa_libreville, "africa/libreville"},
			{timezone::code::africa_lome, "africa/lome"},
			{timezone::code::africa_luanda, "africa/luanda"},
			{timezone::code::africa_lubumbashi, "africa/lubumbashi"},
			{timezone::code::africa_lusaka, "africa/lusaka"},
			{timezone::code::africa_malabo, "africa/malabo"},
			{timezone::code::africa_maputo, "africa/maputo"},
			{timezone::code::africa_maseru, "africa/maseru"},
			{timezone::code::africa_mbabane, "africa/mbabane"},
			{timezone::code::africa_mogadishu, "africa/mogadishu"},
			{timezone::code::africa_monrovia, "africa/monrovia"},
			{timezone::code::africa_nairobi, "africa/nairobi"},
			{timezone::code::africa_ndjamena, "africa/ndjamena"},
			{timezone::code::africa_niamey, "africa/niamey"},
			{timezone::code::africa_nouakchott, "africa/nouakchott"},
			{timezone::code::africa_ouagadougou, "africa/ouagadougou"},
			{timezone::code::africa_porto_novo, "africa/porto-novo"},
			{timezone::code::africa_sao_tome, "africa/sao_tome"},
			{timezone::code::africa_timbuktu, "africa/timbuktu"},
			{timezone::code::africa_tripoli, "africa/tripoli"},
			{timezone::code::africa_tunis, "africa/tunis"},
			{timezone::code::africa_windhoek, "africa/windhoek"},
			{timezone::code::america_adak, "america/adak"},
			{timezone::code::america_anchorage, "america/anchorage"},
			{timezone::code::america_anguilla, "america/anguilla"},
			{timezone::code::america_antigua, "america/antigua"},
			{timezone::code::america_araguaina, "america/araguaina"},
			{timezone::code::america_argentina_buenos_aires, "america/argentina/buenos_aires"},
			{timezone::code::america_argentina_catamarca, "america/argentina/catamarca"},
			{timezone::code::america_argentina_comodrivadavia, "america/argentina/comodrivadavia"},
			{timezone::code::america_argentina_cordoba, "america/argentina/cordoba"},
			{timezone::code::america_argentina_jujuy, "america/argentina/jujuy"},
			{timezone::code::america_argentina_la_rioja, "america/argentina/la_rioja"},
			{timezone::code::america_argentina_mendoza, "america/argentina/mendoza"},
			{timezone::code::america_argentina_rio_gallegos, "america/argentina/rio_gallegos"},
			{timezone::code::america_argentina_salta, "america/argentina/salta"},
			{timezone::code::america_argentina_san_juan, "america/argentina/san_juan"},
			{timezone::code::america_argentina_san_luis, "america/argentina/san_luis"},
			{timezone::code::america_argentina_tucuman, "america/argentina/tucuman"},
			{timezone::code::america_argentina_ushuaia, "america/argentina/ushuaia"},
			{timezone::code::america_aruba, "america/aruba"},
			{timezone::code::america_asuncion, "america/asuncion"},
			{timezone::code::america_atikokan, "america/atikokan"},
			{timezone::code::america_atka, "america/atka"},
			{timezone::code::america_bahia, "america/bahia"},
			{timezone::code::america_bahia_banderas, "america/bahia_banderas"},
			{timezone::code::america_barbados, "america/barbados"},
			{timezone::code::america_belem, "america/belem"},
			{timezone::code::america_belize, "america/belize"},
			{timezone::code::america_blanc_sablon, "america/blanc-sablon"},
			{timezone::code::america_boa_vista, "america/boa_vista"},
			{timezone::code::america_bogota, "america/bogota"},
			{timezone::code::america_boise, "america/boise"},
			{timezone::code::america_buenos_aires, "america/buenos_aires"},
			{timezone::code::america_cambridge_bay, "america/cambridge_bay"},
			{timezone::code::america_campo_grande, "america/campo_grande"},
			{timezone::code::america_cancun, "america/cancun"},
			{timezone::code::america_caracas, "america/caracas"},
			{timezone::code::america_catamarca, "america/catamarca"},
			{timezone::code::america_cayenne, "america/cayenne"},
			{timezone::code::america_cayman, "america/cayman"},
			{timezone::code::america_chicago, "america/chicago"},
			{timezone::code::america_chihuahua, "america/chihuahua"},
			{timezone::code::america_coral_harbour, "america/coral_harbour"},
			{timezone::code::america_cordoba, "america/cordoba"},
			{timezone::code::america_costa_rica, "america/costa_rica"},
			{timezone::code::america_creston, "america/creston"},
			{timezone::code::america_cuiaba, "america/cuiaba"},
			{timezone::code::america_curacao, "america/curacao"},
			{timezone::code::america_danmarkshavn, "america/danmarkshavn"},
			{timezone::code::america_dawson, "america/dawson"},
			{timezone::code::america_dawson_creek, "america/dawson_creek"},
			{timezone::code::america_denver, "america/denver"},
			{timezone::code::america_detroit, "america/detroit"},
			{timezone::code::america_dominica, "america/dominica"},
			{timezone::code::america_edmonton, "america/edmonton"},
			{timezone::code::america_eirunepe, "america/eirunepe"},
			{timezone::code::america_el_salvador, "america/el_salvador"},
			{timezone::code::america_ensenada, "america/ensenada"},
			{timezone::code::america_fort_nelson, "america/fort_nelson"},
			{timezone::code::america_fort_wayne, "america/fort_wayne"},
			{timezone::code::america_fortaleza, "america/fortaleza"},
			{timezone::code::america_glace_bay, "america/glace_bay"},
			{timezone::code::america_godthab, "america/godthab"},
			{timezone::code::america_goose_bay, "america/goose_bay"},
			{timezone::code::america_grand_turk, "america/grand_turk"},
			{timezone::code::america_grenada, "america/grenada"},
			{timezone::code::america_guadeloupe, "america/guadeloupe"},
			{timezone::code::america_guatemala, "america/guatemala"},
			{timezone::code::america_guayaquil, "america/guayaquil"},
			{timezone::code::america_guyana, "america/guyana"},
			{timezone::code::america_halifax, "america/halifax"},
			{timezone::code::america_havana, "america/havana"},
			{timezone::code::america_hermosillo, "america/hermosillo"},
			{timezone::code::america_indiana_indianapolis, "america/indiana/indianapolis"},
			{timezone::code::america_indiana_knox, "america/indiana/knox"},
			{timezone::code::america_indiana_marengo, "america/indiana/marengo"},
			{timezone::code::america_indiana_petersburg, "america/indiana/petersburg"},
			{timezone::code::america_indiana_tell_city, "america/indiana/tell_city"},
			{timezone::code::america_indiana_vevay, "america/indiana/vevay"},
			{timezone::code::america_indiana_vincennes, "america/indiana/vincennes"},
			{timezone::code::america_indiana_winamac, "america/indiana/winamac"},
			{timezone::code::america_indianapolis, "america/indianapolis"},
			{timezone::code::america_inuvik, "america/inuvik"},
			{timezone::code::america_iqaluit, "america/iqaluit"},
			{timezone::code::america_jamaica, "america/jamaica"},
			{timezone::code::america_jujuy, "america/jujuy"},
			{timezone::code::america_juneau, "america/juneau"},
			{timezone::code::america_kentucky_louisville, "america/kentucky/louisville"},
			{timezone::code::america_kentucky_monticello, "america/kentucky/monticello"},
			{timezone::code::america_knox_in, "america/knox_in"},
			{timezone::code::america_kralendijk, "america/kralendijk"},
			{timezone::code::america_la_paz, "america/la_paz"},
			{timezone::code::america_lima, "america/lima"},
			{timezone::code::america_los_angeles, "america/los_angeles"},
			{timezone::code::america_louisville, "america/louisville"},
			{timezone::code::america_lower_princes, "america/lower_princes"},
			{timezone::code::america_maceio, "america/maceio"},
			{timezone::code::america_managua, "america/managua"},
			{timezone::code::america_manaus, "america/manaus"},
			{timezone::code::america_marigot, "america/marigot"},
			{timezone::code::america_martinique, "america/martinique"},
			{timezone::code::america_matamoros, "america/matamoros"},
			{timezone::code::america_mazatlan, "america/mazatlan"},
			{timezone::code::america_mendoza, "america/mendoza"},
			{timezone::code::america_menominee, "america/menominee"},
			{timezone::code::america_merida, "america/merida"},
			{timezone::code::america_metlakatla, "america/metlakatla"},
			{timezone::code::america_mexico_city, "america/mexico_city"},
			{timezone::code::america_miquelon, "america/miquelon"},
			{timezone::code::america_moncton, "america/moncton"},
			{timezone::code::america_monterrey, "america/monterrey"},
			{timezone::code::america_montevideo, "america/montevideo"},
			{timezone::code::america_montreal, "america/montreal"},
			{timezone::code::america_montserrat, "america/montserrat"},
			{timezone::code::america_nassau, "america/nassau"},
			{timezone::code::america_new_york, "america/new_york"},
			{timezone::code::america_nipigon, "america/nipigon"},
			{timezone::code::america_nome, "america/nome"},
			{timezone::code::america_noronha, "america/noronha"},
			{timezone::code::america_north_dakota_beulah, "america/north_dakota/beulah"},
			{timezone::code::america_north_dakota_center, "america/north_dakota/center"},
			{timezone::code::america_north_dakota_new_salem, "america/north_dakota/new_salem"},
			{timezone::code::america_ojinaga, "america/ojinaga"},
			{timezone::code::america_panama, "america/panama"},
			{timezone::code::america_pangnirtung, "america/pangnirtung"},
			{timezone::code::america_paramaribo, "america/paramaribo"},
			{timezone::code::america_phoenix, "america/phoenix"},
			{timezone::code::america_port_of_spain, "america/port_of_spain"},
			{timezone::code::america_port_au_prince, "america/port-au-prince"},
			{timezone::code::america_porto_acre, "america/porto_acre"},
			{timezone::code::america_porto_velho, "america/porto_velho"},
			{timezone::code::america_puerto_rico, "america/puerto_rico"},
			{timezone::code::america_punta_arenas, "america/punta_arenas"},
			{timezone::code::america_rainy_river, "america/rainy_river"},
			{timezone::code::america_rankin_inlet, "america/rankin_inlet"},
			{timezone::code::america_recife, "america/recife"},
			{timezone::code::america_regina, "america/regina"},
			{timezone::code::america_resolute, "america/resolute"},
			{timezone::code::america_rio_branco, "america/rio_branco"},
			{timezone::code::america_rosario, "america/rosario"},
			{timezone::code::america_santa_isabel, "america/santa_isabel"},
			{timezone::code::america_santarem, "america/santarem"},
			{timezone::code::america_santiago, "america/santiago"},
			{timezone::code::america_santo_domingo, "america/santo_domingo"},
			{timezone::code::america_sao_paulo, "america/sao_paulo"},
			{timezone::code::america_scoresbysund, "america/scoresbysund"},
			{timezone::code::america_shiprock, "america/shiprock"},
			{timezone::code::america_sitka, "america/sitka"},
			{timezone::code::america_st_barthelemy, "america/st_barthelemy"},
			{timezone::code::america_st_johns, "america/st_johns"},
			{timezone::code::america_st_kitts, "america/st_kitts"},
			{timezone::code::america_st_lucia, "america/st_lucia"},
			{timezone::code::america_st_thomas, "america/st_thomas"},
			{timezone::code::america_st_vincent, "america/st_vincent"},
			{timezone::code::america_swift_current, "america/swift_current"},
			{timezone::code::america_tegucigalpa, "america/tegucigalpa"},
			{timezone::code::america_thule, "america/thule"},
			{timezone::code::america_thunder_bay, "america/thunder_bay"},
			{timezone::code::america_tijuana, "america/tijuana"},
			{timezone::code::america_toronto, "america/toronto"},
			{timezone::code::america_tortola, "america/tortola"},
			{timezone::code::america_vancouver, "america/vancouver"},
			{timezone::code::america_virgin, "america/virgin"},
			{timezone::code::america_whitehorse, "america/whitehorse"},
			{timezone::code::america_winnipeg, "america/winnipeg"},
			{timezone::code::america_yakutat, "america/yakutat"},
			{timezone::code::america_yellowknife, "america/yellowknife"},
			{timezone::code::antarctica_casey, "antarctica/casey"},
			{timezone::code::antarctica_davis, "antarctica/davis"},
			{timezone::code::antarctica_dumontdurville, "antarctica/dumontdurville"},
			{timezone::code::antarctica_macquarie, "antarctica/macquarie"},
			{timezone::code::antarctica_mawson, "antarctica/mawson"},
			{timezone::code::antarctica_mcmurdo, "antarctica/mcmurdo"},
			{timezone::code::antarctica_palmer, "antarctica/palmer"},
			{timezone::code::antarctica_rothera, "antarctica/rothera"},
			{timezone::code::antarctica_south_pole, "antarctica/south_pole"},
			{timezone::code::antarctica_syowa, "antarctica/syowa"},
			{timezone::code::antarctica_troll, "antarctica/troll"},
			{timezone::code::antarctica_vostok, "antarctica/vostok"},
			{timezone::code::arctic_longyearbyen, "arctic/longyearbyen"},
			{timezone::code::asia_aden, "asia/aden"},
			{timezone::code::asia_almaty, "asia/almaty"},
			{timezone::code::asia_amman, "asia/amman"},
			{timezone::code::asia_anadyr, "asia/anadyr"},
			{timezone::code::asia_aqtau, "asia/aqtau"},
			{timezone::code::asia_aqtobe, "asia/aqtobe"},
			{timezone::code::asia_ashgabat, "asia/ashgabat"},
			{timezone::code::asia_ashkhabad, "asia/ashkhabad"},
			{timezone::code::asia_atyrau, "asia/atyrau"},
			{timezone::code::asia_baghdad, "asia/baghdad"},
			{timezone::code::asia_bahrain, "asia/bahrain"},
			{timezone::code::asia_baku, "asia/baku"},
			{timezone::code::asia_bangkok, "asia/bangkok"},
			{timezone::code::asia_barnaul, "asia/barnaul"},
			{timezone::code::asia_beirut, "asia/beirut"},
			{timezone::code::asia_bishkek, "asia/bishkek"},
			{timezone::code::asia_brunei, "asia/brunei"},
			{timezone::code::asia_calcutta, "asia/calcutta"},
			{timezone::code::asia_chita, "asia/chita"},
			{timezone::code::asia_choibalsan, "asia/choibalsan"},
			{timezone::code::asia_chongqing, "asia/chongqing"},
			{timezone::code::asia_chungking, "asia/chungking"},
			{timezone::code::asia_colombo, "asia/colombo"},
			{timezone::code::asia_dacca, "asia/dacca"},
			{timezone::code::asia_damascus, "asia/damascus"},
			{timezone::code::asia_dhaka, "asia/dhaka"},
			{timezone::code::asia_dili, "asia/dili"},
			{timezone::code::asia_dubai, "asia/dubai"},
			{timezone::code::asia_dushanbe, "asia/dushanbe"},
			{timezone::code::asia_famagusta, "asia/famagusta"},
			{timezone::code::asia_gaza, "asia/gaza"},
			{timezone::code::asia_harbin, "asia/harbin"},
			{timezone::code::asia_hebron, "asia/hebron"},
			{timezone::code::asia_ho_chi_minh, "asia/ho_chi_minh"},
			{timezone::code::asia_hong_kong, "asia/hong_kong"},
			{timezone::code::asia_hovd, "asia/hovd"},
			{timezone::code::asia_irkutsk, "asia/irkutsk"},
			{timezone::code::asia_istanbul, "asia/istanbul"},
			{timezone::code::asia_jakarta, "asia/jakarta"},
			{timezone::code::asia_jayapura, "asia/jayapura"},
			{timezone::code::asia_jerusalem, "asia/jerusalem"},
			{timezone::code::asia_kabul, "asia/kabul"},
			{timezone::code::asia_kamchatka, "asia/kamchatka"},
			{timezone::code::asia_karachi, "asia/karachi"},
			{timezone::code::asia_kashgar, "asia/kashgar"},
			{timezone::code::asia_kathmandu, "asia/kathmandu"},
			{timezone::code::asia_katmandu, "asia/katmandu"},
			{timezone::code::asia_khandyga, "asia/khandyga"},
			{timezone::code::asia_kolkata, "asia/kolkata"},
			{timezone::code::asia_krasnoyarsk, "asia/krasnoyarsk"},
			{timezone::code::asia_kuala_lumpur, "asia/kuala_lumpur"},
			{timezone::code::asia_kuching, "asia/kuching"},
			{timezone::code::asia_kuwait, "asia/kuwait"},
			{timezone::code::asia_macao, "asia/macao"},
			{timezone::code::asia_macau, "asia/macau"},
			{timezone::code::asia_magadan, "asia/magadan"},
			{timezone::code::asia_makassar, "asia/makassar"},
			{timezone::code::asia_manila, "asia/manila"},
			{timezone::code::asia_muscat, "asia/muscat"},
			{timezone::code::asia_novokuznetsk, "asia/novokuznetsk"},
			{timezone::code::asia_novosibirsk, "asia/novosibirsk"},
			{timezone::code::asia_omsk, "asia/omsk"},
			{timezone::code::asia_oral, "asia/oral"},
			{timezone::code::asia_phnom_penh, "asia/phnom_penh"},
			{timezone::code::asia_pontianak, "asia/pontianak"},
			{timezone::code::asia_pyongyang, "asia/pyongyang"},
			{timezone::code::asia_qatar, "asia/qatar"},
			{timezone::code::asia_qyzylorda, "asia/qyzylorda"},
			{timezone::code::asia_rangoon, "asia/rangoon"},
			{timezone::code::asia_riyadh, "asia/riyadh"},
			{timezone::code::asia_saigon, "asia/saigon"},
			{timezone::code::asia_sakhalin, "asia/sakhalin"},
			{timezone::code::asia_samarkand, "asia/samarkand"},
			{timezone::code::asia_seoul, "asia/seoul"},
			{timezone::code::asia_shanghai, "asia/shanghai"},
			{timezone::code::asia_singapore, "asia/singapore"},
			{timezone::code::asia_srednekolymsk, "asia/srednekolymsk"},
			{timezone::code::asia_taipei, "asia/taipei"},
			{timezone::code::asia_tashkent, "asia/tashkent"},
			{timezone::code::asia_tbilisi, "asia/tbilisi"},
			{timezone::code::asia_tehran, "asia/tehran"},
			{timezone::code::asia_tel_aviv, "asia/tel_aviv"},
			{timezone::code::asia_thimbu, "asia/thimbu"},
			{timezone::code::asia_thimphu, "asia/thimphu"},
			{timezone::code::asia_tokyo, "asia/tokyo"},
			{timezone::code::asia_tomsk, "asia/tomsk"},
			{timezone::code::asia_ujung_pandang, "asia/ujung_pandang"},
			{timezone::code::asia_ulaanbaatar, "asia/ulaanbaatar"},
			{timezone::code::asia_ulan_bator, "asia/ulan_bator"},
			{timezone::code::asia_urumqi, "asia/urumqi"},
			{timezone::code::asia_ust_nera, "asia/ust-nera"},
			{timezone::code::asia_vientiane, "asia/vientiane"},
			{timezone::code::asia_vladivostok, "asia/vladivostok"},
			{timezone::code::asia_yakutsk, "asia/yakutsk"},
			{timezone::code::asia_yangon, "asia/yangon"},
			{timezone::code::asia_yekaterinburg, "asia/yekaterinburg"},
			{timezone::code::asia_yerevan, "asia/yerevan"},
			{timezone::code::atlantic_azores, "atlantic/azores"},
			{timezone::code::atlantic_bermuda, "atlantic/bermuda"},
			{timezone::code::atlantic_canary, "atlantic/canary"},
			{timezone::code::atlantic_cape_verde, "atlantic/cape_verde"},
			{timezone::code::atlantic_faeroe, "atlantic/faeroe"},
			{timezone::code::atlantic_faroe, "atlantic/faroe"},
			{timezone::code::atlantic_jan_mayen, "atlantic/jan_mayen"},
			{timezone::code::atlantic_madeira, "atlantic/madeira"},
			{timezone::code::atlantic_reykjavik, "atlantic/reykjavik"},
			{timezone::code::atlantic_south_georgia, "atlantic/south_georgia"},
			{timezone::code::atlantic_st_helena, "atlantic/st_helena"},
			{timezone::code::atlantic_stanley, "atlantic/stanley"},
			{timezone::code::australia_act, "australia/act"},
			{timezone::code::australia_adelaide, "australia/adelaide"},
			{timezone::code::australia_brisbane, "australia/brisbane"},
			{timezone::code::australia_broken_hill, "australia/broken_hill"},
			{timezone::code::australia_canberra, "australia/canberra"},
			{timezone::code::australia_currie, "australia/currie"},
			{timezone::code::australia_darwin, "australia/darwin"},
			{timezone::code::australia_eucla, "australia/eucla"},
			{timezone::code::australia_hobart, "australia/hobart"},
			{timezone::code::australia_lhi, "australia/lhi"},
			{timezone::code::australia_lindeman, "australia/lindeman"},
			{timezone::code::australia_lord_howe, "australia/lord_howe"},
			{timezone::code::australia_melbourne, "australia/melbourne"},
			{timezone::code::australia_north, "australia/north"},
			{timezone::code::australia_nsw, "australia/nsw"},
			{timezone::code::australia_perth, "australia/perth"},
			{timezone::code::australia_queensland, "australia/queensland"},
			{timezone::code::australia_south, "australia/south"},
			{timezone::code::australia_sydney, "australia/sydney"},
			{timezone::code::australia_tasmania, "australia/tasmania"},
			{timezone::code::australia_victoria, "australia/victoria"},
			{timezone::code::australia_west, "australia/west"},
			{timezone::code::australia_yancowinna, "australia/yancowinna"},
			{timezone::code::brazil_acre, "brazil/acre"},
			{timezone::code::brazil_denoronha, "brazil/denoronha"},
			{timezone::code::brazil_east, "brazil/east"},
			{timezone::code::brazil_west, "brazil/west"},
			{timezone::code::canada_atlantic, "canada/atlantic"},
			{timezone::code::canada_central, "canada/central"},
			{timezone::code::canada_eastern, "canada/eastern"},
			{timezone::code::canada_mountain, "canada/mountain"},
			{timezone::code::canada_newfoundland, "canada/newfoundland"},
			{timezone::code::canada_pacific, "canada/pacific"},
			{timezone::code::canada_saskatchewan, "canada/saskatchewan"},
			{timezone::code::canada_yukon, "canada/yukon"},
			{timezone::code::cet, "cet"},
			{timezone::code::chile_continental, "chile/continental"},
			{timezone::code::chile_easterisland, "chile/easterisland"},
			{timezone::code::cst6cdt, "cst6cdt"},
			{timezone::code::cuba, "cuba"},
			{timezone::code::eet, "eet"},
			{timezone::code::egypt, "egypt"},
			{timezone::code::eire, "eire"},
			{timezone::code::est, "est"},
			{timezone::code::est5edt, "est5edt"},
			{timezone::code::etc_gmt, "etc/gmt"},
			{timezone::code::etc_gmt_plus_0, "etc/gmt+0"},
			{timezone::code::etc_gmt_plus_1, "etc/gmt+1"},
			{timezone::code::etc_gmt_plus_10, "etc/gmt+10"},
			{timezone::code::etc_gmt_plus_11, "etc/gmt+11"},
			{timezone::code::etc_gmt_plus_12, "etc/gmt+12"},
			{timezone::code::etc_gmt_plus_2, "etc/gmt+2"},
			{timezone::code::etc_gmt_plus_3, "etc/gmt+3"},
			{timezone::code::etc_gmt_plus_4, "etc/gmt+4"},
			{timezone::code::etc_gmt_plus_5, "etc/gmt+5"},
			{timezone::code::etc_gmt_plus_6, "etc/gmt+6"},
			{timezone::code::etc_gmt_plus_7, "etc/gmt+7"},
			{timezone::code::etc_gmt_plus_8, "etc/gmt+8"},
			{timezone::code::etc_gmt_plus_9, "etc/gmt+9"},
			{timezone::code::etc_gmt0, "etc/gmt0"},
			{timezone::code::etc_gmt_minus_0, "etc/gmt-0"},
			{timezone::code::etc_gmt_minus_1, "etc/gmt-1"},
			{timezone::code::etc_gmt_minus_10, "etc/gmt-10"},
			{timezone::code::etc_gmt_minus_11, "etc/gmt-11"},
			{timezone::code::etc_gmt_minus_12, "etc/gmt-12"},
			{timezone::code::etc_gmt_minus_13, "etc/gmt-13"},
			{timezone::code::etc_gmt_minus_14, "etc/gmt-14"},
			{timezone::code::etc_gmt_minus_2, "etc/gmt-2"},
			{timezone::code::etc_gmt_minus_3, "etc/gmt-3"},
			{timezone::code::etc_gmt_minus_4, "etc/gmt-4"},
			{timezone::code::etc_gmt_minus_5, "etc/gmt-5"},
			{timezone::code::etc_gmt_minus_6, "etc/gmt-6"},
			{timezone::code::etc_gmt_minus_7, "etc/gmt-7"},
			{timezone::code::etc_gmt_minus_8, "etc/gmt-8"},
			{timezone::code::etc_gmt_minus_9, "etc/gmt-9"},
			{timezone::code::etc_greenwich, "etc/greenwich"},
			{timezone::code::etc_uct, "etc/uct"},
			{timezone::code::etc_universal, "etc/universal"},
			{timezone::code::etc_utc, "etc/utc"},
			{timezone::code::etc_zulu, "etc/zulu"},
			{timezone::code::europe_amsterdam, "europe/amsterdam"},
			{timezone::code::europe_andorra, "europe/andorra"},
			{timezone::code::europe_astrakhan, "europe/astrakhan"},
			{timezone::code::europe_athens, "europe/athens"},
			{timezone::code::europe_belfast, "europe/belfast"},
			{timezone::code::europe_belgrade, "europe/belgrade"},
			{timezone::code::europe_berlin, "europe/berlin"},
			{timezone::code::europe_bratislava, "europe/bratislava"},
			{timezone::code::europe_brussels, "europe/brussels"},
			{timezone::code::europe_bucharest, "europe/bucharest"},
			{timezone::code::europe_budapest, "europe/budapest"},
			{timezone::code::europe_busingen, "europe/busingen"},
			{timezone::code::europe_chisinau, "europe/chisinau"},
			{timezone::code::europe_copenhagen, "europe/copenhagen"},
			{timezone::code::europe_dublin, "europe/dublin"},
			{timezone::code::europe_gibraltar, "europe/gibraltar"},
			{timezone::code::europe_guernsey, "europe/guernsey"},
			{timezone::code::europe_helsinki, "europe/helsinki"},
			{timezone::code::europe_isle_of_man, "europe/isle_of_man"},
			{timezone::code::europe_istanbul, "europe/istanbul"},
			{timezone::code::europe_jersey, "europe/jersey"},
			{timezone::code::europe_kaliningrad, "europe/kaliningrad"},
			{timezone::code::europe_kiev, "europe/kiev"},
			{timezone::code::europe_kirov, "europe/kirov"},
			{timezone::code::europe_lisbon, "europe/lisbon"},
			{timezone::code::europe_ljubljana, "europe/ljubljana"},
			{timezone::code::europe_london, "europe/london"},
			{timezone::code::europe_luxembourg, "europe/luxembourg"},
			{timezone::code::europe_madrid, "europe/madrid"},
			{timezone::code::europe_malta, "europe/malta"},
			{timezone::code::europe_mariehamn, "europe/mariehamn"},
			{timezone::code::europe_minsk, "europe/minsk"},
			{timezone::code::europe_monaco, "europe/monaco"},
			{timezone::code::europe_moscow, "europe/moscow"},
			{timezone::code::asia_nicosia, "asia/nicosia"},
			{timezone::code::europe_oslo, "europe/oslo"},
			{timezone::code::europe_paris, "europe/paris"},
			{timezone::code::europe_podgorica, "europe/podgorica"},
			{timezone::code::europe_prague, "europe/prague"},
			{timezone::code::europe_riga, "europe/riga"},
			{timezone::code::europe_rome, "europe/rome"},
			{timezone::code::europe_samara, "europe/samara"},
			{timezone::code::europe_san_marino, "europe/san_marino"},
			{timezone::code::europe_sarajevo, "europe/sarajevo"},
			{timezone::code::europe_saratov, "europe/saratov"},
			{timezone::code::europe_simferopol, "europe/simferopol"},
			{timezone::code::europe_skopje, "europe/skopje"},
			{timezone::code::europe_sofia, "europe/sofia"},
			{timezone::code::europe_stockholm, "europe/stockholm"},
			{timezone::code::europe_tallinn, "europe/tallinn"},
			{timezone::code::europe_tirane, "europe/tirane"},
			{timezone::code::europe_tiraspol, "europe/tiraspol"},
			{timezone::code::europe_ulyanovsk, "europe/ulyanovsk"},
			{timezone::code::europe_uzhgorod, "europe/uzhgorod"},
			{timezone::code::europe_vaduz, "europe/vaduz"},
			{timezone::code::europe_vatican, "europe/vatican"},
			{timezone::code::europe_vienna, "europe/vienna"},
			{timezone::code::europe_vilnius, "europe/vilnius"},
			{timezone::code::europe_volgograd, "europe/volgograd"},
			{timezone::code::europe_warsaw, "europe/warsaw"},
			{timezone::code::europe_zagreb, "europe/zagreb"},
			{timezone::code::europe_zaporozhye, "europe/zaporozhye"},
			{timezone::code::europe_zurich, "europe/zurich"},
			{timezone::code::gb, "gb"},
			{timezone::code::gb_eire, "gb-eire"},
			{timezone::code::gmt, "gmt"},
			{timezone::code::gmt_plus_0, "gmt+0"},
			{timezone::code::gmt0, "gmt0"},
			{timezone::code::gmt_minus_0, "gmt−0"},
			{timezone::code::greenwich, "greenwich"},
			{timezone::code::hongkong, "hongkong"},
			{timezone::code::hst, "hst"},
			{timezone::code::iceland, "iceland"},
			{timezone::code::indian_antananarivo, "indian/antananarivo"},
			{timezone::code::indian_chagos, "indian/chagos"},
			{timezone::code::indian_christmas, "indian/christmas"},
			{timezone::code::indian_cocos, "indian/cocos"},
			{timezone::code::indian_comoro, "indian/comoro"},
			{timezone::code::indian_kerguelen, "indian/kerguelen"},
			{timezone::code::indian_mahe, "indian/mahe"},
			{timezone::code::indian_maldives, "indian/maldives"},
			{timezone::code::indian_mauritius, "indian/mauritius"},
			{timezone::code::indian_mayotte, "indian/mayotte"},
			{timezone::code::indian_reunion, "indian/reunion"},
			{timezone::code::iran, "iran"},
			{timezone::code::israel, "israel"},
			{timezone::code::jamaica, "jamaica"},
			{timezone::code::japan, "japan"},
			{timezone::code::kwajalein, "kwajalein"},
			{timezone::code::libya, "libya"},
			{timezone::code::met, "met"},
			{timezone::code::mexico_bajanorte, "mexico/bajanorte"},
			{timezone::code::mexico_bajasur, "mexico/bajasur"},
			{timezone::code::mexico_general, "mexico/general"},
			{timezone::code::mst, "mst"},
			{timezone::code::mst7mdt, "mst7mdt"},
			{timezone::code::navajo, "navajo"},
			{timezone::code::nz, "nz"},
			{timezone::code::nz_chat, "nz-chat"},
			{timezone::code::pacific_apia, "pacific/apia"},
			{timezone::code::pacific_auckland, "pacific/auckland"},
			{timezone::code::pacific_bougainville, "pacific/bougainville"},
			{timezone::code::pacific_chatham, "pacific/chatham"},
			{timezone::code::pacific_chuuk, "pacific/chuuk"},
			{timezone::code::pacific_easter, "pacific/easter"},
			{timezone::code::pacific_efate, "pacific/efate"},
			{timezone::code::pacific_enderbury, "pacific/enderbury"},
			{timezone::code::pacific_fakaofo, "pacific/fakaofo"},
			{timezone::code::pacific_fiji, "pacific/fiji"},
			{timezone::code::pacific_funafuti, "pacific/funafuti"},
			{timezone::code::pacific_galapagos, "pacific/galapagos"},
			{timezone::code::pacific_gambier, "pacific/gambier"},
			{timezone::code::pacific_guadalcanal, "pacific/guadalcanal"},
			{timezone::code::pacific_guam, "pacific/guam"},
			{timezone::code::pacific_honolulu, "pacific/honolulu"},
			{timezone::code::pacific_johnston, "pacific/johnston"},
			{timezone::code::pacific_kiritimati, "pacific/kiritimati"},
			{timezone::code::pacific_kosrae, "pacific/kosrae"},
			{timezone::code::pacific_kwajalein, "pacific/kwajalein"},
			{timezone::code::pacific_majuro, "pacific/majuro"},
			{timezone::code::pacific_marquesas, "pacific/marquesas"},
			{timezone::code::pacific_midway, "pacific/midway"},
			{timezone::code::pacific_nauru, "pacific/nauru"},
			{timezone::code::pacific_niue, "pacific/niue"},
			{timezone::code::pacific_norfolk, "pacific/norfolk"},
			{timezone::code::pacific_noumea, "pacific/noumea"},
			{timezone::code::pacific_pago_pago, "pacific/pago_pago"},
			{timezone::code::pacific_palau, "pacific/palau"},
			{timezone::code::pacific_pitcairn, "pacific/pitcairn"},
			{timezone::code::pacific_pohnpei, "pacific/pohnpei"},
			{timezone::code::pacific_ponape, "pacific/ponape"},
			{timezone::code::pacific_port_moresby, "pacific/port_moresby"},
			{timezone::code::pacific_rarotonga, "pacific/rarotonga"},
			{timezone::code::pacific_saipan, "pacific/saipan"},
			{timezone::code::pacific_samoa, "pacific/samoa"},
			{timezone::code::pacific_tahiti, "pacific/tahiti"},
			{timezone::code::pacific_tarawa, "pacific/tarawa"},
			{timezone::code::pacific_tongatapu, "pacific/tongatapu"},
			{timezone::code::pacific_truk, "pacific/truk"},
			{timezone::code::pacific_wake, "pacific/wake"},
			{timezone::code::pacific_wallis, "pacific/wallis"},
			{timezone::code::pacific_yap, "pacific/yap"},
			{timezone::code::poland, "poland"},
			{timezone::code::portugal, "portugal"},
			{timezone::code::prc, "prc"},
			{timezone::code::pst8pdt, "pst8pdt"},
			{timezone::code::roc, "roc"},
			{timezone::code::rok, "rok"},
			{timezone::code::singapore, "singapore"},
			{timezone::code::turkey, "turkey"},
			{timezone::code::uct, "uct"},
			{timezone::code::universal, "universal"},
			{timezone::code::us_alaska, "us/alaska"},
			{timezone::code::us_aleutian, "us/aleutian"},
			{timezone::code::us_arizona, "us/arizona"},
			{timezone::code::us_central, "us/central"},
			{timezone::code::us_eastern, "us/eastern"},
			{timezone::code::us_east_indiana, "us/east-indiana"},
			{timezone::code::us_hawaii, "us/hawaii"},
			{timezone::code::us_indiana_starke, "us/indiana-starke"},
			{timezone::code::us_michigan, "us/michigan"},
			{timezone::code::us_mountain, "us/mountain"},
			{timezone::code::us_pacific, "us/pacific"},
			{timezone::code::us_pacific_new, "us/pacific-new"},
			{timezone::code::us_samoa, "us/samoa"},
			{timezone::code::utc, "utc"},
			{timezone::code::wet, "wet"},
			{timezone::code::w_su, "w_su"},
			{timezone::code::zulu, "zulu"}
		},
		timezone::code::gmt
	};

	timezone::timezone(const std::string& name)
	:
		_code(timezone::map.at(string::lowercased_string(name), true))
	{

	}

	timezone::timezone(timezone::code code)
	:
		_code(code)
	{

	}

	timezone::timezone(const timezone& origin)
	:
		_code(origin._code)
	{

	}

	timezone::timezone(timezone&& origin)
	:
		_code(std::move(origin._code))
	{

	}

	timezone::operator std::string() const
	{
		return timezone::map.at(_code, true);
	}

	void timezone::set(timezone::code code)
	{
		_code = code;
	}

	timezone::code timezone::get() const
	{
		return _code;
	}
}
