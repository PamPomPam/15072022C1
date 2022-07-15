
#include "Map.h"
#include "Globals.h"
#include <unordered_set>
#include <unordered_map>
#include <algorithm>
#include <assert.h>
#include <deque>
#include <string>

Unit::Unit() {
    x = 100000;
    y = 100000;
    loc = 100000;
    health = 100000;
    playr = NO_PLAYER;
    type = NO_TYPE;
}
Unit::Unit(int x_, int y_, int health_, unit_type type_, player playr_, bool upgraded_, bool removing_) {
    x = x_;
    y = y_;
    loc = loc_fromxy(x_, y_);
    health = health_;
    type = type_;
    upgraded = upgraded_;
    playr = playr_;
}
/*void Map::add_unit(PyObject* unit_, unit_type unit_type_) {
    int x, y;
    double health;
    if (PyList_Check(unit_) && PySequence_Size(unit_) == 4) {
        x = PyLong_AsLong(PyList_GET_ITEM(unit_, 0));
        y = PyLong_AsLong(PyList_GET_ITEM(unit_, 1));
        health = PyFloat_AsDouble(PyList_GET_ITEM(unit_, 2));
    }
    Unit unit(x, y, health, (unit_type)unit_type_);
    Unit* p_unit = &unit;
    all_units.push_back(p_unit);
    all_locations[loc_fromxy(x, y)] = p_unit;
}*/


void Map::add_unit(int x_, int y_, int health_, unit_type type_, player playr_, bool upgraded_, bool removing_) {
    location loc = loc_fromxy(x_, y_);
    Unit myunit(x_, y_, start_health[type_], type_, playr_, upgraded_, removing_);
    all_locations.emplace(loc, myunit);
    healths[(int)playr_][loc] = (health_);
    switch (type_) {
    case FACTORY:
        factories[(int)playr_].emplace_back(loc, upgraded_);
        return;
    case TURRET:
        turrets[(int)playr_].emplace_back(loc, upgraded_);
    }
}
void Map::upgrade_unit(location loc) {
    Unit unit = all_locations[loc];
    unit.upgraded = true;
    int size;
    switch (unit.type) {
    case FACTORY:
        size = factories[1 - (int)unit.playr].size();
        for (int i = 0; i < size; i ++) {
            auto temporary = factories[1 - (int)unit.playr][i];
            if (temporary.first == loc) {
                factories[(int)unit.playr].emplace(factories[(int)unit.playr].begin() + i, make_pair(loc, true));
                break;
            }
        }
    case TURRET:
        size = turrets[1 - (int)unit.playr].size();
        for (int i = 0; i < size; i++) {
            auto temporary = turrets[1 - (int)unit.playr][i];
            if (temporary.first == loc) {
                turrets[(int)unit.playr].emplace(turrets[(int)unit.playr].begin() + i, make_pair(loc, true));
                break;
            }
        }
    }
}
void Map::plot_map() {
    for (int y = 27; y >= 0; y--) {
        for (int x = 0; x < 28; x++) {
            if (!in_arena(x, y)) {
                cerr << ' ';
                continue;
            }
            if (all_locations.find(loc_fromxy(x, y)) == all_locations.end()) {
                cerr << '.';
            }
            else {
                cerr << 'x';
            }
        }
        cerr << endl;
    }
}
bool Map::is_empty(location loc) {
    return all_locations.find(loc) == all_locations.end();
}
Unit Map::get(location loc) {
    return all_locations[loc];
}
Unit Map::get(int x, int y) {
    return all_locations[loc_fromxy(x, y)];
}

void Map::set_move_map(direction dir, location loc, bool free, bool can_finish, movedir second, movedir first, int dist) {
    movemap[mm_index(loc, dir)] = mm_value(free, can_finish, second, first, dist);
}
void Map::set_first(direction dir, location loc, movedir first) {
    int current = movemap[mm_index(loc, dir)];
    movemap[mm_index(loc, dir)] = current + 90 * (first - get_first_mm(current));
}
void Map::set_second(direction dir, location loc, movedir second) {
    int current = movemap[mm_index(loc, dir)];
    movemap[mm_index(loc, dir)] = current + 90 * 4 * (second - get_second_mm(current));
}
void Map::change_to_next_loc(location& loc, movedir& prev, int value) {
    //plot(loc);
    //cout << "first: " << get_first_mm(value) << " second: " << get_second_mm(value) << " prev: " << prev << endl;
    movedir next = (movedir)get_first_mm(value);
    if (next != prev) {
        loc = get_neighbours(loc)[(int)next];
        prev = next;
    }
    else {
        loc = get_neighbours(loc)[get_second_mm(value)];
        prev = (movedir)get_second_mm(value);
    }
}


// pathfinding
location Map::get_target(location start_point, direction dir) {
    vector<location> used_locs = { start_point };
    unordered_set<location> acces_points = { start_point };
    location acces_point;
    location* neighbours;
    location neighbour;
    idealityfunction val = location_ideality(dir);
    location best = start_point;
    location highest = - 10000;
    
    while (!acces_points.empty()) {
        acces_point = *acces_points.begin();
        acces_points.erase(acces_point);
        neighbours = get_neighbours(acces_point);
        for (int i = 0; i < 4; i++) {
            neighbour = neighbours[i];
            if (in_arena(neighbour) && find(used_locs.begin(), used_locs.end(), neighbour) == used_locs.end() && is_empty(neighbour)) {
                used_locs.emplace_back(neighbour);
                acces_points.insert(neighbour);
                if (val(neighbour) > highest) {
                    highest = val(neighbour);
                    best = neighbour;
                }
            }
        }
    }
    return best;
}
void Map::create_move_map_target(location target, direction dir) {
    added_locs.insert(target);
    set_move_map(dir, target, is_empty(target), false, N, N, 0);
    deque<location> acces_points = {target};
    location* neighbours;
    location acces_point;
    location neighbour;
    int dist;
    while (!acces_points.empty()) {
        acces_point = *acces_points.begin();
        acces_points.pop_front();
        neighbours = get_neighbours(acces_point);
        dist = get_dist_mm(movemap[mm_index(acces_point, dir)]);
        cerr << dist << endl;
        for (int i = 0; i < 4; i++) {
            neighbour = neighbours[i];
            if (added_locs.find(neighbour) != added_locs.end() || !in_arena(neighbour)) {
                continue;
            }
            added_locs.insert(neighbour);
            set_move_map(dir, neighbour, is_empty(neighbour), false, N, N, dist + 1);
            if (is_empty(neighbour)) {
                acces_points.emplace_back(neighbour);
            }
        }
    }
}
void Map::create_move_map(direction dir) {
    location* end_points_ = end_points(dir);
    location end_point;
    deque<location> acces_points = {};
    //cerr << 'a';
    for (int i = 0; i < 14; i++) {
        end_point = end_points_[i];
        added_locs.insert(end_point);
        set_move_map(dir, end_point, is_empty(end_point), true, N, N, 0);
        if (is_empty(end_point)) {
            acces_points.emplace_back(end_point);
        }
    }

    location* neighbours;
    location neighbour;
    location acces_point;
    int dist;
    //cerr << 'b';
    while (!acces_points.empty()) {
        acces_point = *acces_points.begin();
        acces_points.pop_front();
        neighbours = get_neighbours(acces_point);
        dist = get_dist_mm(movemap[mm_index(acces_point, dir)]);
        //cerr << "---" << endl;
        //print(acces_point);
        for (int i = 0; i < 4; i++) {
            neighbour = neighbours[i];
            //print(neighbour);
            if (added_locs.find(neighbour) != added_locs.end() || !in_arena(neighbour)) {
                continue;
            }
            added_locs.insert(neighbour);
            set_move_map(dir, neighbour, is_empty(neighbour), true, N, N, dist + 1);
            if (is_empty(neighbour)) {
                acces_points.emplace_back(neighbour);
            }
        }
    }
    // now we have a map with distances to end_points
    // if some start_points can't reach this we'll check for this now:
    location* start_points_ = start_points(dir);
    location start_point;
    location target;
    for (int i = 0; i < 14; i++) {
        start_point = start_points_[i];
        cerr << 'a' << endl;
        print(start_point);
        if (is_empty(start_point) && added_locs.find(start_point) == added_locs.end()) {
            target = get_target(start_point, dir);
            print(target);
            create_move_map_target(target, dir);
        }
    }
    // add directions (N, Z, ... ) for each point on map
    bool first_done;
    for (const auto& loc : added_locs) {
        if (is_empty(loc)) {
            neighbours = get_neighbours(loc);
            //cerr << 'q';
         
            //cerr << loc << endl;
            dist = get_dist_mm(movemap[mm_index(loc, dir)]);
            //cerr << 'r';
            first_done = false;
            for (int i = 0; i < 4; i++) {
                neighbour = neighbours[i];
                if (in_arena(neighbour) && is_empty(neighbour) && get_dist_mm(movemap[mm_index(neighbour, dir)]) == dist - 1) {
                    if (!first_done) {
                        set_first(dir, loc, (movedir)i);
                        set_second(dir, loc, (movedir)i);
                        first_done = true;
                    }
                    else {
                        set_second(dir, loc, (movedir)i);
                        break;
                    }
                }   
            }
        }
    }
    added_locs.clear();
    plot_movemap_dist(this, dir);
    plot_movemap_reachandempy(this, dir);
    plot_movemap_moves(this, dir);
}
void Map::create_all_move_maps() {
    create_move_map(NO);
    create_move_map(NW);
    create_move_map(ZO);
    create_move_map(ZW);
}
void Map::remove_building(location destr, direction dir) {
    int val_mm;
    int val_path;
    location* neighbours;
    location neighbour;
    
    neighbours = get_neighbours(destr);
    int lowest = 100000;
    location lowest_loc;
    movedir first;
    movedir second;

    // search for most desirable of the four neighbours
    for (int i = 0; i < 4; i++) {
        neighbour = neighbours[i];
        val_mm = movemap[mm_index(neighbour, dir)];
        val_path = path_value(get_can_finish_mm(val_mm), get_dist_mm(val_mm));
        if (val_path == lowest) {
            second = (movedir)((i + 2) % 4);
        }
        else if (val_path < lowest) {
            lowest = val_path;
            lowest_loc = neighbour;
            first = (movedir)((i + 2) % 4);
            second = first;
        }
    }
    // center (destroyed) is one more than the most desirable
    set_move_map(dir, destr, true, get_can_finish_path(lowest), second, first, get_dist_path(lowest)+1);
    
    
    // update the map 'recursively'
    deque<location> acces_points = { destr };
    location acces_point;
    added_locs.insert(destr);
    added_locs.insert(lowest_loc);
    int val_path2;
    int val_mm2;
    movedir third;

    while (!acces_points.empty()) {
        acces_point = *acces_points.begin();
        acces_points.pop_front();
        neighbours = get_neighbours(acces_point);

        val_mm = movemap[mm_index(acces_point, dir)];
        val_path = path_value(get_can_finish_mm(val_mm), get_dist_mm(val_mm));

        for (int i = 0; i < 4; i++) {
            neighbour = neighbours[i];
            if (added_locs.find(neighbour) != added_locs.end() || !in_arena(neighbour) || !is_empty(neighbour)) {
                continue;
            }
            added_locs.insert(neighbour);
            val_mm2 = movemap[mm_index(neighbour, dir)];
            val_path2 = path_value(get_can_finish_mm(val_mm2), get_dist_mm(val_mm2));


            if (val_path2 > val_path + 1) {
                first = (movedir)((i + 2) % 4);
                set_move_map(dir, neighbour, true, get_can_finish_path(val_path), first, first, get_dist_path(val_path) + 1);
                acces_points.emplace_back(neighbour);
            }
            else if (val_path2 == val_path + 1) {
                // only change possible paths from this new location: do not append to acces point because no improvement
                first = (movedir)get_first_mm(val_mm2);
                second = (movedir)get_second_mm(val_mm2);
                third = (movedir)((i + 2) % 4);
                if (third < first) {
                    set_first(dir, neighbour, third);
                    set_second(dir, neighbour, third);
                }
                else if (third < second) {
                    set_second(dir, neighbour, third);
                }
            }
        }
    }
    added_locs.clear();
}
void Map::add_building(location build, direction dir) {
    int val_mm;
    int val_path;
    location* neighbours;
    location neighbour;
    location* neighbours2;
    location neighbour2;

    deque<location> acces_points = { build };
    location acces_point;
    added_locs.insert(build);
    int val_path2;
    int val_mm2;
    int val_path3;
    int val_mm3;

    movedir first;
    movedir second;
    movedir newfirst;
    movedir newsecond;

    int lowest = 1000000;

    while (!acces_points.empty()) {
        acces_point = *acces_points.begin();
        acces_points.pop_front();
        neighbours = get_neighbours(acces_point);
        val_mm = movemap[mm_index(acces_point, dir)];

        for (int i = 0; i < 4; i++) {
            neighbour = neighbours[i];
            if (added_locs.find(neighbour) != added_locs.end() || !in_arena(neighbour) || !is_empty(neighbour)) {
                continue;
            }
            added_locs.insert(neighbour);
            val_mm2 = movemap[mm_index(neighbour, dir)];
            first = (movedir)get_first_mm(val_mm2);
            second = (movedir)get_second_mm(val_mm2);
            if (first == acces_point || second == acces_point) {
                
                
                
                neighbours2 = get_neighbours(neighbour);
                for (int j = 0; j < 4; j++) {
                    neighbour2 = neighbours2[j];
                    if (in_arena(neighbour2) && is_empty(neighbour2)) {
                        val_mm3 = movemap[mm_index(neighbour2, dir)];
                        val_path3 = path_value(get_can_finish_mm(val_mm3), get_dist_mm(val_mm3));
                        if (val_path3 == lowest) {
                            newsecond = (movedir)((j + 2) % 4);
                        }
                        else if (val_path3 < lowest) {
                            lowest = val_path3;
                            newfirst = (movedir)((j + 2) % 4);
                            newsecond = first;
                        }
                    }
                }
                set_move_map(dir, neighbour, true, get_can_finish_path(lowest), newfirst, newsecond, get_dist_path(lowest));
                if (newfirst != first || newsecond != second) {
                    acces_points.emplace_back(neighbour);
                }


            }
        }
    }
    added_locs.clear();
}



void Map::simulate_turn(location loc, unit_type unit_type_, int& n, player playr, float& health, float& damage_taken, int& damage_given, vector<pair<location, bool>>* p_factories) {
    cerr << "-----" << endl;
    print(loc);
    cerr << "n = " << n << ", damage_taken = " << damage_taken << endl;
    
    
    location temp;
    int size = p_factories->size();
    int i = 0;
    pair<location, bool> temporary;
    //cerr << 'h';

    while (i < size) {
        //print(p_factories);
        temporary = (*p_factories)[i];
        temp = temporary.first;
        //print(temp);
        if (temporary.second) {
            if (within_range(temp, loc, 7)) {
                health += 4 + 0.3 * dist_to_self(y_fromloc(temp), playr);
                p_factories->erase(p_factories->begin()+i);
                size--;
            }
            else {
                i++;
            }
        }
        else {
            if (within_range(temp, loc, 3.5)) {
                health += 3;
                p_factories->erase(p_factories->begin()+i);
                size--;
            }
            else {
                i++;
            }
        }
    }
    cerr << "Now their health is: " << health << endl;

    int damag = n * damage[unit_type_]; // important to calculate this before deaths!
    bool death;
    for (auto& tup : turrets[1 - (int)playr]) {
        temp = tup.first;
        if (tup.second) {
            if (within_range(temp, loc, 3.5)) {
                //cerr << "dam from lv 2" << endl;
                //print(temp);
                death = (int)((damage_taken + 16) / (health-0.01));
                //cerr << "Death: " << death << endl;
                n -= death;
                damage_taken = !death * (damage_taken + 16);
                //cerr << damage_taken << endl;
            }
        }
        else {
            if (within_range(temp, loc, 2.5)) {
                //cerr << "dam from lv 1" << endl;
                //print(temp);
                death = (int)((damage_taken + 5) / (health - 0.01));
                //cerr << "Death: " << death << endl;
                n -= death;
                damage_taken = !death * (damage_taken + 5);
                //cerr << damage_taken << endl;
            }
        }
    }




    float range = attack_range[unit_type_];
    size = healths[1 - (int)playr].size();
    int* arr = new int[size];
    i = 0;
    int building_health;
    for (auto& x : healths[1 - (int)playr]) {
        temp = x.first;
        building_health = x.second;
        arr[i] = attack_value(dist(temp, loc), building_health, dist_to_oppon(y_fromloc(temp), playr), temp);
        i++;
    }
    
    float distd;
    
    cerr << "Attacked:" << endl;
    sort(arr, arr + size);
    for (int i = 0; i < size; i++) {
        temp = get_location_attack(arr[i]);
        print(temp);
        if (get_dist_attack(arr[i]) > range) {
            break;
        }
        cerr << "hit!" << endl;
        building_health = healths[1-(int)playr][temp];
        if (damag < building_health) {
            // not destroyed
            //cerr << "nd" << damag << endl;
            healths[1 - (int)playr][temp] -= damag;
            damage_given += damag;
            break;
        }
        else {
            // destroyed !!! muhaha
            //cerr << "d" << building_health << endl;
            damag -= building_health;
            damage_given += building_health;
            healths[1 - (int)playr].erase(temp);
        }
    }
    //cerr << "damage_given: " << damage_given << endl;
    //print(healths[1]);
    delete[] arr;
}
void Map::simulate_turn2(location loc, unit_type unit_type_, int& n, player playr, float& health, float& damage_taken, int& damage_given, vector<pair<location, bool>>* p_factories) {
    cerr << "-----" << endl;
    print(loc);
    cerr << "n = " << n << ", damage_taken = " << damage_taken << endl;


    location temp;
    int size = p_factories->size();
    int i = 0;
    pair<location, bool> temporary;
    //cerr << 'h';

    while (i < size) {
        //print(p_factories);
        temporary = (*p_factories)[i];
        temp = temporary.first;
        //print(temp);
        if (temporary.second) {
            if (within_range(temp, loc, 7)) {
                health += 4 + 0.3 * dist_to_self(y_fromloc(temp), playr);
                p_factories->erase(p_factories->begin() + i);
                size--;
            }
            else {
                i++;
            }
        }
        else {
            if (within_range(temp, loc, 3.5)) {
                health += 3;
                p_factories->erase(p_factories->begin() + i);
                size--;
            }
            else {
                i++;
            }
        }
    }
    cerr << "Now their health is: " << health << endl;

    int damag = n * damage[unit_type_]; // important to calculate this before deaths!
    bool death;
    for (auto& tup : turrets[1 - (int)playr]) {
        temp = tup.first;
        if (tup.second) {
            if (within_range(temp, loc, 3.5)) {
                //cerr << "dam from lv 2" << endl;
                //print(temp);
                death = (int)((damage_taken + 16) / (health - 0.01));
                //cerr << "Death: " << death << endl;
                n -= death;
                damage_taken = !death * (damage_taken + 16);
                //cerr << damage_taken << endl;
            }
        }
        else {
            if (within_range(temp, loc, 2.5)) {
                //cerr << "dam from lv 1" << endl;
                //print(temp);
                death = (int)((damage_taken + 5) / (health - 0.01));
                //cerr << "Death: " << death << endl;
                n -= death;
                damage_taken = !death * (damage_taken + 5);
                //cerr << damage_taken << endl;
            }
        }
    }




    
    size = healths[1 - (int)playr].size();
    int* arr = new int[size];
    i = 0;
    
    float range = attack_range[unit_type_];
    int building_health;
    int lowest;
    int lowest_loc;
    int value;
    int distanc;
    while (true) {
        lowest = INT_MAX;
        for (auto& x : healths[1 - (int)playr]) {
            temp = x.first;
            distanc = dist(temp, loc);
            if (distanc > range) {
                continue;
            }
            building_health = x.second;
            value = attack_value2(distanc, building_health, dist_to_oppon(y_fromloc(temp), playr), temp, dist_from_edge(x_fromloc(temp)));
            if (value < lowest) {
                lowest = value;
                lowest_loc = temp;
            }
        }
        if (lowest == INT_MAX) {
            break;
        }

        building_health = healths[1 - (int)playr][lowest_loc];
        if (damag < building_health) {
            // not destroyed
            healths[1 - (int)playr][temp] -= damag;
            damage_given += damag;
            break;
        }
        else {
            // destroyed !!! muhaha
            damag -= building_health;
            damage_given += building_health;
            healths[1 - (int)playr].erase(temp);
        }
    }
    //cerr << "damage_given: " << damage_given << endl;
    //print(healths[1]);
    delete[] arr;
}
void Map::simulate_destruction(location loc, unit_type unit_type_, int& n, player playr, int& damage_given) {
    cerr << "DESTRUCTION!!" << endl;
    int damag = n * start_health[unit_type_];
    location temp;
    int building_health;
    for (auto& x : healths[playr]) {
        temp = x.first;
        if (!within_range(temp, loc, 1.5)) {
            continue;
        }
        building_health = healths[playr][temp];
        if (damag < building_health) {
            // not destroyed
            healths[playr][temp] -= damag;
            damage_given += damag;
        }
        else {
            // destroyed !!! muhaha
            damage_given += building_health;
            healths[playr].erase(temp);
        }
    }
    n = 0;
}
int Map::simulate(direction dir, location start_point, unit_type unit_type_, int& n, player playr) {
    location temp = start_point;
    float damage_taken = 0;
    int damage_given = 0;
    float health = start_health[(int)unit_type_];
    std::vector<pair<location, bool>> factories_copy = factories[(int)playr];
    auto p_factories_copy = &factories_copy;
    movedir prev = W;
    int value;
    while (true) {
        value = movemap[mm_index(temp, dir)];
        if (get_dist_mm(value) == 0) {
            if (!get_can_finish_mm(value)) {
                simulate_destruction(temp, unit_type_, n, playr, damage_given);
            }
            return damage_given; // reached the end 
        }
        change_to_next_loc(temp, prev, value);


        simulate_turn(temp, unit_type_, n, playr, health, damage_taken, damage_given, p_factories_copy);
        if (n <= 0) {
            n = 0;
            return damage_given;
        }

    }
}


void plot_movemap_dist(Map* mymap, direction dir) {
    for (int y = 27; y >= 0; y--) {
        for (int x = 0; x < 28; x++) {
            if (!in_arena(x, y)) {
                cerr << "  ";
                continue;
            }
            if (!get_free_mm(mymap->movemap[mm_index(loc_fromxy(x, y), dir)])) {
                cerr << "--";
                continue;
            }
            int dist = get_dist_mm(mymap->movemap[mm_index(loc_fromxy(x, y), dir)]);
            string str = to_string(dist);
            if (str.length() == 1) {
                cerr << " ";
            }
            cerr << dist;
        }
        cerr << endl;
    }
}
void plot_movemap_moves(Map* mymap, direction dir) {
    for (int y = 27; y >= 0; y--) {
        for (int x = 0; x < 28; x++) {
            if (!in_arena(x, y)) {
                cerr << "  ";
                continue;
            }
            if (!get_free_mm(mymap->movemap[mm_index(loc_fromxy(x, y), dir)])) {
                cerr << "--";
                continue;
            }
            int dist = (int)get_first_mm(mymap->movemap[mm_index(loc_fromxy(x, y), dir)]);
            string str = to_string(dist);

            cerr << dist;
            dist = (int)get_second_mm(mymap->movemap[mm_index(loc_fromxy(x, y), dir)]);
            str = to_string(dist);

            cerr << dist;
        }
        cerr << endl;
    }
}
void plot_movemap_reachandempy(Map* mymap, direction dir) {
    for (int y = 27; y >= 0; y--) {
        for (int x = 0; x < 28; x++) {
            if (!in_arena(x, y)) {
                cerr << "  ";
                continue;
            }
            if (!get_free_mm(mymap->movemap[mm_index(loc_fromxy(x, y), dir)])) {
                cerr << "x";
            }
            else {
                cerr << ".";
            }
            if (!get_can_finish_mm(mymap->movemap[mm_index(loc_fromxy(x, y), dir)])) {
                cerr << "o";
            }
            else {
                cerr << ".";
            }
        }
        cerr << endl;
    }
}