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

    std::map<Tag, int> vespene_harvesters;

    // The following are for the marines

    Point3D rally_point = Point3D(0.f, 0.f, 0.f);
    std::vector<const Unit*> targets = {};
    bool is_attack_over = true;
    Point3D where_to = Point3D(0, 0, 0);



	// Methods
public:
	virtual void OnGameStart() final;

	virtual void OnStep() final;

	virtual void OnUnitIdle(const Unit* unit) final;

    virtual void OnUnitVoid(const Unit* unit) final;

    virtual void OnUnitCreated(const Unit* unit) final;

    virtual void OnBuildingConstructionComplete(const Unit* unit) final;

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

    const Unit* FindNearestUnit(const Point2D& start, UNIT_TYPEID unit_type, Unit::Alliance all);

    const Unit* FindNearestUnit(const Point2D& start, std::vector<UNIT_TYPEID> unit_type, Unit::Alliance all);


    /*
        Returns the distance to the closest enemy unit.
    */
    float DistanceToClosestEnemyUnit(const Point2D& start);
    /*
        Returns the number of units near a given point
    */
    int NumberOfUnitsNearby(Unit::Alliance alliance, const Point2D& start, float default_distance = 15);
    //std::list<const Unit*> FindNearestUnits(const Point2D& start, std::vector<UNIT_TYPEID> unit_type, Unit::Alliance all, int number);
    /*
    Finds and return the nearest Enemy unit.
    If no enemy is within a certain radius, returns nullptr.
    */
    const Unit* FindNearestEnemyUnit(const Point2D& start, float distance = 225);

    
    // Do the stuff for the marines
    bool FollowAttackOrders();

    // Do the stuff for macro
    bool FollowBuildOrders();

public:
	TestBot();
	~TestBot() {}
};

struct IsCloser {
    Point2D st;
    float dist;
    IsCloser(Point3D start ,float distance) {
        st = start;
        dist = distance;
    }
    IsCloser(const Point2D& start, float distance) {
        st = start;
        dist = distance;
    }

    bool operator() (const Unit& unit) {
        return Distance2D(unit.pos, st) <= dist;
    }
};

#endif