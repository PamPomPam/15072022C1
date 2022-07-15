#pragma once


struct Gamestate {
	int turnPhase;
	int turnNumber;
	int turnFrame; // irrelevant
	int turnTotalFrame; // irrelevant

	int p1_health;
	double p1_structP;
	double p1_mobileP;
	int p1_time_ms; // irrelevant

	int p2_health;
	double p2_structP;
	double p2_mobileP;
	int p2_time_ms; // irrelevant
};