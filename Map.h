#pragma once

#include "Globals.h"
#include <vector>
#include <array>
#include <unordered_set>


struct Unit {
    int x;
    int y;
    int health;
    location loc;
    unit_type type;
    player playr;
    bool upgraded = false;
    bool removing = false;
    Unit();
    Unit(int x_, int y_, int health_, unit_type type_, player playr_, bool upgraded_, bool removing_);
};



typedef std::unordered_map<location, Unit> location_dict;

struct Map {

    std::unordered_map<location, int> healths[2] = { {}, {} };
    std::vector<pair<location, bool>> factories[2] = { {}, {} };
    std::vector<pair<location, bool>> turrets[2] = { {}, {} };
    location_dict all_locations = {};
    std::array<int, 4*28*28> movemap;
    std::unordered_set<location> added_locs;

    //void add_unit(PyObject* unit_, unit_type unit_type_);
    void add_unit(int x_, int y_, int health_, unit_type type_, player playr_, bool upgraded_, bool removing_);
    void upgrade_unit(location loc);
    void plot_map();
    bool is_empty(location loc);
    Unit get(location loc);
    Unit get(int x, int y);

    // pathfinding related

    int get_target(location start_point, direction dir);
    void create_move_map_target(location target, direction dir);
    void create_move_map(direction dir);
    void create_all_move_maps();
    void remove_building(location destr, direction dir);
    void add_building(location build, direction dir);
    
    void set_move_map(direction dir, location loc, bool free, bool can_finish, movedir first, movedir second, int dist);
    void set_first(direction dir, location loc, movedir first);
    void set_second(direction dir, location loc, movedir first);
    void change_to_next_loc(location &loc, movedir &prev, int value);
    //void update_move_map(direction dir, location loc);


    // simulation related
    //int simulate(location start_point, direction dir, unit_type unit_type_, int& n, player playr);
    //int simulate(direction dir, path path_, unit_type unit_type_, int& n, player playr);
    void simulate_turn(location loc, unit_type unit_type_, int& n, player playr, float& health, float& damage_taken, int& damage_given, vector<pair<location, bool>>* p_factories);
    void simulate_turn2(location loc, unit_type unit_type_, int& n, player playr, float& health, float& damage_taken, int& damage_given, vector<pair<location, bool>>* p_factories);
    void simulate_destruction(location loc, unit_type unit_type_, int& n, player playr, int& damage_given);
    int simulate(direction dir, location start_point, unit_type unit_type_, int& n, player playr);

};

void plot_movemap_dist(Map* mymap, direction dir);
void plot_movemap_moves(Map* mymap, direction dir);
void plot_movemap_reachandempy(Map* mymap, direction dir);


