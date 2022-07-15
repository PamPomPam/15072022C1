#pragma once

#include "Globals.h"
#include <string>
#include <sstream>

static string hardcoded_defend_plan = "FF 07 09 FF 08 08 FF 09 07 FF 10 06 FF 11 05 FF 12 04 FF 13 03 FF 14 03 FF 15 04 FF 16 05"
"FF 17 06 FF 18 07 FF 19 08 FF 20 09 DF 05 11 UP 05 11 DF 22 11 UP 22 11 DF 02 13 UP 02 13"
"DF 25 13 UP 25 13 DF 01 13 UP 01 13 DF 26 13 UP 26 13 DF 24 12 UP 24 12 DF 03 12 UP 03 12"
"DF 04 12 UP 04 12 DF 23 12 UP 23 12 DF 27 13 UP 27 13 DF 00 13 UP 00 13 FF 03 13 UP 03 13"
"FF 24 13 UP 24 13 FF 05 12 UP 05 12 FF 22 12 UP 22 12 FF 04 13 UP 04 13 FF 23 13 UP 23 13"
"DF 02 12 UP 02 12 DF 25 12 UP 25 12 DF 01 12 UP 01 12 DF 26 12 UP 26 12 FF 06 11 UP 06 11"
"FF 21 11 UP 21 11 DF 04 11 UP 04 11 DF 23 11 UP 23 11 DF 05 10 UP 05 10 DF 22 10 UP 22 10"
"DF 06 09 UP 06 09 DF 21 09 UP 21 09 FF 21 10 UP 21 10 FF 06 10 UP 06 10 DF 02 11 UP 02 11"
"DF 25 11 UP 25 11 FF 05 13 UP 05 13 FF 22 13 UP 22 13 FF 06 12 UP 06 12 FF 21 12 UP 21 12"
"EF 07 08 UP 07 08 EF 20 08 UP 20 08 EF 19 07 UP 19 07 EF 08 07 UP 08 07 EF 09 06 EF 10 05"
"EF 11 04 EF 12 03 EF 13 02 EF 14 02 EF 15 03 EF 16 04 EF 17 05 EF 18 06 EF 10 07 UP 10 07"
"EF 17 07 UP 17 07 EF 11 07 UP 11 07 EF 16 07 UP 16 07 EF 12 07 UP 12 07 EF 15 07 UP 15 07"
"EF 13 07 UP 13 07 EF 14 07 UP 14 07 EF 11 06 UP 11 06 EF 16 06 UP 16 06 EF 12 06 UP 12 06"
"EF 15 06 UP 15 06 EF 13 06 UP 13 06 EF 14 06 UP 14 06";

// decoration
/*
void parse_plan() {
	string name;
	int x, y;
	istringstream iss(hardcoded_defend_plan);
	unit_type type;
	while (iss >> name) {
		iss >> x;
		iss >> y;
		type = unit_types[name];
		// do something with x, y and type
	}
}*/

