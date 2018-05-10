#ifndef TEST_BOT_H
#define TEST_BOT_H

#include <sc2api\sc2_api.h>

#include "MapAnalyzer.h"

using namespace sc2;

class TestBot :public Agent {
private:
	MapAnalyzer map;

public:
	virtual void OnGameStart() final;

	virtual void OnStep() final;

	virtual void OnUnitIdle(const Unit* unit) final;
private:
	size_t CountUnitType(UNIT_TYPEID unit_type);

	bool TryBuildStructure(ABILITY_ID ability_type_for_structure, UNIT_TYPEID unit_type = UNIT_TYPEID::TERRAN_SCV);

	bool TryBuildSupplyDepot();

	bool TryBuildRefinery();

	const Unit* FindNearestMineralPatch(const Point2D& start);

	const Unit* FindNearestVespeneGas(const Point2D& start);

	bool TryBuildBarracks();
public:
	TestBot();
	~TestBot() {}
};

#endif