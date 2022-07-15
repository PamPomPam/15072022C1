#include "Globals.h"
#include <cmath>
#include <string>
#include <unordered_map>
#include <algorithm>


const int start_health[] = { 60, 30, 75, 15, 5, 40 };
const int upgrade_health[] = { 120, 30, 75 };
const int damage[] = { 0, 0, 0, 2, 8, 0 };
const float attack_range[] = { 0, 0, 0, 3.5, 4.5, 0 };
const int unit_cost[] = { 1, 4, 2, 1, 3, 1, 100000, 100000};
const int upgrade_cost[] = { 1, 4, 4, 100000, 100000, 100000, 100000 };
std::unordered_map<unit_type, string> unit_names = { {WALL, "FF"}, {FACTORY, "EF"}, {TURRET, "DF"}, {SCOUT, "PI" }, {DEMOLISHER, "EI"}, {INTERCEPTOR, "SI"}, {UPGRADE, "UP"}, {REMOVE, "RM"} };
std::unordered_map<string, unit_type> unit_types = { {"FF", WALL}, {"EF", FACTORY}, {"DF", TURRET}, {"PI", SCOUT}, {"EI", DEMOLISHER}, {"SI", INTERCEPTOR}, {"UP", UPGRADE}, {"RM", REMOVE} };
std::unordered_map<int, movedir> get_movedir = { {100, N}, {-100, Z}, {1, O}, {-1, W} };


bool in_arena(location loc) {
	int x = x_fromloc(loc);
	int y = y_fromloc(loc);
	return (14 <= y && y <= 27 && y - 14 <= x && x <= 41 - y) || (0 <= y && y <= 13 && 13 - y <= x && x <= 14 + y);
}

bool in_arena(int x, int y) {
	return (14 <= y && y <= 27 && y - 14 <= x && x <= 41 - y) || (0 <= y && y <= 13 && 13 - y <= x && x <= 14 + y);
}

location* get_neighbours(location loc) {
    static location output[4];
    output[0] = loc + 100;
    output[1] = loc - 100;
    output[2] = loc + 1;
    output[3] = loc - 1;
    return output;
}
/*
location* get_neighbours(int x, int y) {
    static location output[4] = {loc_fromxy(x+1, y), loc_fromxy(x-1, y), loc_fromxy(x, y+1), loc_fromxy(x, y-1)};
    return output;
}*/


bool within_range(location loc1, location loc2, float range) {
    return dist(loc1, loc2) < range;
}
float dist(location loc1, location loc2) {
    int x1 = x_fromloc(loc1);
    int y1 = y_fromloc(loc1);
    int x2 = x_fromloc(loc2);
    int y2 = y_fromloc(loc2);
    return sqrt(pow(x1 - x2, 2) + pow(y1 - y2, 2));
}


bool is_end_point(location loc, direction dir) {
    switch (dir) {
    case NO:
        return y_fromloc(loc) + x_fromloc(loc) == 41;
    case NW:
        return y_fromloc(loc) - x_fromloc(loc) == 14;
    case ZO:
        return x_fromloc(loc) - y_fromloc(loc) == 14;
    case ZW:
        return y_fromloc(loc) + x_fromloc(loc) == 13;
    default:
        cerr << "Something went wrong" << endl;
        return NULL;
    }
}

location* end_points(direction dir) {
    static location output[14];
    switch (dir) {
    case NO:
        for (int i = 0; i < 14; i++) {output[i] = loc_fromxy(14 + i, 27 - i);}
        return output;
    case NW:
        for (int i = 0; i < 14; i++) {output[i] = loc_fromxy(i, 14 + i);}
        return output;
    case ZO:
        for (int i = 0; i < 14; i++) {output[i] = loc_fromxy(14 + i, i);}
        return output;
    case ZW:
        for (int i = 0; i < 14; i++) {output[i] = loc_fromxy(i, 13 - i);}
        return output;
    default:
        cerr << "Something went wrong" << endl;
        location* p = NULL;
        return p;
    }
}

location* start_points(direction dir) {
    static location output[14];
    switch (dir) {
    case NO:
        for (int i = 0; i < 14; i++) { output[i] = loc_fromxy(i, 13 - i); }
        return output;
    case NW:
        for (int i = 0; i < 14; i++) { output[i] = loc_fromxy(14 + i, i); }
        return output;
    case ZO:
        for (int i = 0; i < 14; i++) { output[i] = loc_fromxy(i, 14 + i); }
        return output;
    case ZW:
        for (int i = 0; i < 14; i++) { output[i] = loc_fromxy(14 + i, 27 - i); }
        return output;
    default:
        cerr << "Something went wrong" << endl;
        location* p = NULL;
        return p;
    }
}


//bool valueNO(location loc1, location loc2) { return 28 * y_fromloc(loc2) + x_fromloc(loc2) > 28 * y_fromloc(loc1) - x_fromloc(loc1); }
//bool valueNW(location loc1, location loc2) { return 28 * y_fromloc(loc1) - x_fromloc(loc1) > 28 * y_fromloc(loc1) + x_fromloc(loc1); }
//bool valueZO(location loc1, location loc2) { return 28 * y_fromloc(loc1) + x_fromloc(loc1) < 28 * y_fromloc(loc1) - x_fromloc(loc1); }
//bool valueZW(location loc1, location loc2) { return 28 * y_fromloc(loc1) - x_fromloc(loc1) < 28 * y_fromloc(loc1) + x_fromloc(loc1); }
int valueNO(location loc) { return 28 * y_fromloc(loc) + x_fromloc(loc); }
int valueNW(location loc) { return 28 * y_fromloc(loc) - x_fromloc(loc); }
int valueZO(location loc) { return - 28 * y_fromloc(loc) + x_fromloc(loc); }
int valueZW(location loc) { return - 28 * y_fromloc(loc) - x_fromloc(loc); }
/*compfunction location_ideality(direction dir) {
    switch (dir) {
    case NO:
        return &valueNO;
    case NW:
        return &valueNW;
    case ZO:
        return &valueZO;
    case ZW:
        return &valueZW;
    default:
        cerr << "Something went wrong" << endl;
        compfunction p = NULL;
        return p;
    }
}*/
idealityfunction location_ideality(direction dir) {
    switch (dir) {
    case NO:
        return &valueNO;
    case NW:
        return &valueNW;
    case ZO:
        return &valueZO;
    case ZW:
        return &valueZW;
    default:
        cerr << "Something went wrong" << endl;
        idealityfunction p = NULL;
        return p;
    }
}


location* directions_NO(location loc) {
    static location output[4] = { loc + 100, loc + 1, loc - 100, loc - 1 };
    return output;
}
location* directions_NW(location loc) {
    static location output[4] = { loc + 100, loc - 1, loc - 100, loc + 1 };
    return output;
}
location* directions_ZO(location loc) {
    static location output[4] = { loc - 100, loc + 1, loc + 100, loc - 1 };
    return output;
}
location* directions_ZW(location loc) {
    static location output[4] = { loc - 100, loc - 1, loc + 100, loc + 1 };
    return output;
}
dirfunction preferred_directions(direction dir) {
    switch (dir) {
    case NO:
        return &directions_NO;
    case NW:
        return &directions_NW;
    case ZO:
        return &directions_ZO;
    case ZW:
        return &directions_ZW;
    default:
        cerr << "Something went wrong" << endl;
        dirfunction p = NULL;
        return p;
    }
}

void plot_path(path input) {
    for (int y = 27; y >= 0; y--) {
        for (int x = 0; x < 28; x++) {
            if (!in_arena(x, y)) {
                cerr << ' ';
                continue;
            }
            if (find(input.begin(), input.end(), loc_fromxy(x, y)) != input.end()) {
                cerr << 'x';
            }
            else {
                cerr << '.';
            }
        }
        cerr << endl;
    }
}

#include <string>
void plot_dist(unordered_map<location, int>* input) {

    for (int y = 27; y >= 0; y--) {
        for (int x = 0; x < 28; x++) {
            if (!in_arena(x, y)) {
                cerr << "  ";
                continue;
            }
            if (input->find(loc_fromxy(x, y)) != input->end()) {
                int dist = (*input)[loc_fromxy(x, y)];
                string str = to_string(dist);
                cerr << dist;
                if (str.length() == 1) {
                    cerr << " ";
                }
            }
            else {
                cerr << "..";
            }
        }
        cerr << endl;
    }
}
void print(location loc) {
    cerr << "Location: " << x_fromloc(loc) << ' ' << y_fromloc(loc) << endl;
}
void print(unordered_map<location, int> printthing) {
    for (auto& it : printthing) {
        cerr << '(' << x_fromloc(it.first) << " " << y_fromloc(it.first) << "): " << it.second << endl;
    }
    cerr << endl;
}
void print(vector<pair<location, bool>>* printthing) {
    for (auto& it : *printthing) {
        cerr << '(' << x_fromloc(it.first) << " " << y_fromloc(it.first) << ") ";
    }
    cerr << endl;
}

void print(unordered_set<location>* printthing) {
    for (auto& it : *printthing) {
        cerr << '(' << x_fromloc(it) << " " << y_fromloc(it) << ") ";
    }
    cerr << endl;
}