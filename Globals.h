#pragma once

#include <Python.h>
#include <unordered_map>
#include <vector>
#include <iostream>
#include <tuple>
#include <utility> 
#include <unordered_set>

using namespace std;

typedef int location;
typedef std::vector<location> path;
//typedef bool (*compfunction)(location, location);
typedef int (*idealityfunction)(location);
typedef location* (*dirfunction)(location);

enum direction { NO, NW, ZO, ZW };
enum movedir { N, Z, O, W };
enum unit_type { WALL, FACTORY, TURRET, SCOUT, DEMOLISHER, INTERCEPTOR, UPGRADE, REMOVE, NO_TYPE};
enum player {PLAYER1, PLAYER2, NO_PLAYER};

/*inline int y_fromloc(location loc);
inline int x_fromloc(location loc);
inline location loc_fromxy(int x, int y);
inline int relative_y(int y, player playr);*/


inline int y_fromloc(location loc) { return loc / 100; }
inline int x_fromloc(location loc) { return loc % 100; }
inline location loc_fromxy(int x, int y) { return y * 100 + x; }
inline int dist_to_oppon(int y, player playr) { return (2 * (int)playr - 1) * y + 27 - (int)playr * 27; }
inline int dist_to_self(int y, player playr) { return (1 - 2 * (int)playr) * y + (int)playr * 27; }
inline int dist_from_edge(int x) { return -abs((int)(2 * (x - 13.5))) + 29; }


inline int attack_value(float dist, int building_health, int dist_to_oppon, location temp) {
	return ((int)(dist * 39 / 5)) * 15 * 121 * 3000 +
		building_health * 15 * 3000 + dist_to_oppon * 3000 + temp;
}
inline int get_dist_attack(int sort) {
	return ((int)(sort / (15 * 121 * 3000))) * 5 / 39;
}
inline location get_location_attack(int sort) {
	return (sort % 3000);
}
inline int attack_value2(float dist, int building_health, int dist_to_oppon, location temp, int dist2) {
	return ((int)(dist * 39 / 5)) * 15 * 121 * 30 +
		building_health * 15 * 30 + dist_to_oppon * 30 + dist2 ;
}
inline int get_dist_attack2(int sort) {
	return ((int)(sort / (15 * 121  * 30))) * 5 / 39;
}





inline int mm_index(location loc, direction dir) { return 28 * 28 * (int)dir + 28 * y_fromloc(loc) + x_fromloc(loc); }
inline int mm_value(bool free, bool can_finish, movedir second, movedir first, int dist)
{
	return (int)can_finish * 90 * 32 + (int)free * 90 * 16 + (int)second * 90 * 4 + (int)first * 90 + dist;
}
inline int get_can_finish_mm(int value) { return (int)value / (90 * 32); }
inline int get_free_mm(int value) { return (int)(value % (32 * 90)) / (90 * 16); }
inline int get_second_mm(int value) { return (int)(value % (90 * 16)) / (90 * 4); }
inline int get_first_mm(int value) { return (int)(value % (90 * 4)) / 90; }
inline int get_dist_mm(int value) { return (int)value % 90; }

inline int path_value(bool can_finish, int dist) {return 1000-1000 * (int)can_finish + dist;}
inline int get_dist_path(int val) { return val % 1000; };
inline int get_can_finish_path(int val) { return 1 - (int)(val / 1000); };


bool in_arena(location loc);
bool in_arena(int x, int y);
location* get_neighbours(location loc);
//location* get_neighbours(int x, int y);
bool within_range(location loc1, location loc2, float range);
float dist(location loc1, location loc2);

bool is_end_point(location loc, direction dir);
location* end_points(direction dir);
location* start_points(direction dir);
//compfunction location_ideality(direction dir);
idealityfunction location_ideality(direction dir);
dirfunction preferred_directions(direction dir);

void plot_path(path input);
void plot_dist(unordered_map<location, int>* input);
void print(location loc);
void print(unordered_map<location, int> printthing);
void print(vector<pair<location, bool>>* printthing);
void print(unordered_set<location>* printthing);

typedef string name;
typedef tuple<string, int, int> instruction;
typedef vector<instruction> instructionlist;
typedef pair<instructionlist, instructionlist> final_output;

extern const int start_health[6];
extern const int upgrade_health[3];
extern const int damage[6];
extern const float attack_range[6];
extern const int unit_cost[8];
extern const int upgrade_cost[8];
extern std::unordered_map<unit_type, string> unit_names;
extern std::unordered_map<string, unit_type> unit_types;
extern std::unordered_map<int, movedir> get_movedir;
