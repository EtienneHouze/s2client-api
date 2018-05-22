#ifndef TEST_BOT_H
#define TEST_BOT_H

#include <sc2api\sc2_api.h>

#include "MapAnalyzer.h"
#include "MacroManager.h"
#include "Orders.h"

using namespace sc2;

class TestBot :public Agent {
private:

	// CONSTANTS
    float BUILD_TOLERANCE = 5;


	// private members
	MapAnalyzer map;
    Orders last_orders;
    MacroManager manager;

    std::map<UNIT_TYPEID, int> units_being_built;


	// Methods
public:
	virtual void OnGameStart() final;

	virtual void OnStep() final;

	virtual void OnUnitIdle(const Unit* unit) final;

    virtual void OnUnitVoid(const Unit* unit) final;

private:
	size_t CountUnitType(UNIT_TYPEID unit_type);

	bool TryBuildStructure(ABILITY_ID ability_type_for_structure, UNIT_TYPEID unit_type = UNIT_TYPEID::TERRAN_SCV);

    bool Research(ABILITY_ID ability_for_research, UNIT_TYPEID unit_to_search);

	bool TryBuildSupplyDepot();

	bool TryBuildRefinery();

    bool Expand();

	bool TryBuildBarracks();

	const Unit* FindNearestMineralPatch(const Point2D& start);

	const Unit* FindNearestVespeneGas(const Point2D& start);

public:
	TestBot();
	~TestBot() {}
};

#endif