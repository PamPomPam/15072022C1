
#pragma once

#include "Globals.h"
#include "Map.h"
#include <string>
#include "Gamestate.h"
#include <sstream>
#include "Hardcoded.h"


void Main(Map* mymap, Gamestate* mystate, final_output* output) {
	cerr << "Started main" << endl;

	mymap->create_all_move_maps();
	cerr << "Finished creating move_maps" << endl;
	int n =6;
	int lala = mymap->simulate(NO, loc_fromxy(11, 2), SCOUT, n, PLAYER1);
	cerr << "Damage given: " << lala << endl;

	
	//Attack(mymap, mystate, &(output->second));
	//Defend(mymap, mystate, &(output->first));
	cerr << "Finished main" << endl;
}

void Defend(Map* mymap, Gamestate* mystate, instructionlist* defend_list) {
	istringstream iss(hardcoded_defend_plan);
	location loc;
	unit_type type;
	Unit temp;
	int cost;
	string name;
	int x, y;
	while (iss >> name) {
		iss >> x;
		iss >> y;
		type = unit_types[name];
		loc = loc_fromxy(x, y);
		//cerr << x << ' ' << y << endl;
		if (type == UPGRADE) {
			temp = mymap->all_locations[loc_fromxy(x, y)];
			if (temp.upgraded)
				continue;
			type = temp.type;
			cost = upgrade_cost[type];
			if (cost < mystate->p1_structP) {
				mystate->p1_structP -= cost;
				mymap->upgrade_unit(loc);
				instruction myinstruction(name, x, y);
				defend_list->emplace_back(myinstruction);
			}
			else {
				break;
			}
		}
		else {
			if (mymap->all_locations.find(loc) != mymap->all_locations.end())
				continue;
			cost = unit_cost[type];
			if (cost < mystate->p1_structP) {
				mystate->p1_structP -= cost;
				static Unit myunit(x, y, start_health[type], type, PLAYER1, false, false);
				mymap->add_unit(x, y, start_health[type], type, PLAYER1, false, false);
				instruction myinstruction(name, x, y);
				defend_list->emplace_back(myinstruction);
			}
			else {
				break;
			}

		}
	}
}


void Attack(Map* mymap, Gamestate* mystate, instructionlist* attack_list) {
}


//Map* build_mapcopy(Map* mymap, Gamestate* mystate, instructionlist* defend_list) {
//	Map mymap_copy = *mymap;
//
//}