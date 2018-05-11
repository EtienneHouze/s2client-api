#include <iostream>

#include "TestBot.h"

void TestBot::OnGameStart()

{
	std::cout << "Hello, World!" << std::endl;

    manager = MacroManager();
    last_orders = manager.ThinkAndSendOrders(Observation());
}

void TestBot::OnStep()
{
	/*TryBuildSupplyDepot();
	TryBuildRefinery();
	TryBuildBarracks();
	Units units = Observation()->GetUnits();
	for (const auto& unit : units) {
		if (unit->unit_type == UNIT_TYPEID::TERRAN_COMMANDCENTER) {
			Point3D ones = Point3D(10, 10, 10);
			Debug()->DebugBoxOut(unit->pos - ones, unit->pos + ones);
			
		}
	}
	map.PrintDebugInfo(Debug());
	Debug()->SendDebug();*/
    last_orders = manager.ThinkAndSendOrders(Observation());

}

void TestBot::OnUnitIdle(const Unit * unit)
{
	switch (unit->unit_type.ToType()) {
	case UNIT_TYPEID::TERRAN_COMMANDCENTER: {
		Actions()->UnitCommand(unit, ABILITY_ID::TRAIN_SCV);
		break;
	}
	case UNIT_TYPEID::TERRAN_SCV: {
		const Unit* mineral_target = FindNearestMineralPatch(unit->pos);
		if (!mineral_target) {
			break;
		}
		Actions()->UnitCommand(unit, ABILITY_ID::SMART, mineral_target);
		break;
	}
	case UNIT_TYPEID::TERRAN_BARRACKS: {
		Actions()->UnitCommand(unit, ABILITY_ID::TRAIN_MARINE);
		break;
	}
	case UNIT_TYPEID::TERRAN_MARINE: {
		const GameInfo& game_info = Observation()->GetGameInfo();
		Actions()->UnitCommand(unit, ABILITY_ID::ATTACK_ATTACK, game_info.enemy_start_locations.front());
		break;
	}
	default: {
		break;
	}
	}
}

size_t TestBot::CountUnitType(UNIT_TYPEID unit_type)
{
	return Observation()->GetUnits(Unit::Alliance::Self, IsUnit(unit_type)).size();
}

bool TestBot::TryBuildStructure(ABILITY_ID ability_type_for_structure, UNIT_TYPEID unit_type)
{
	const ObservationInterface* observation = Observation();

	// If a unit already is building a supply structure of this type, do nothing.
	// Also get an scv to build the structure.
	const Unit* unit_to_build = nullptr;
	Units units = observation->GetUnits(Unit::Alliance::Self);
	for (const auto& unit : units) {
		for (const auto& order : unit->orders) {
			if (order.ability_id == ability_type_for_structure) {
				return false;
			}
		}

		if (unit->unit_type == unit_type) {
			unit_to_build = unit;
		}
	}

	if (ability_type_for_structure == ABILITY_ID::BUILD_REFINERY) {
		const Unit* closest_geyser = FindNearestVespeneGas(unit_to_build->pos);
		Actions()->UnitCommand(unit_to_build, ability_type_for_structure, closest_geyser);
		return true;
	}

	float rx = GetRandomScalar();
	float ry = GetRandomScalar();

	Actions()->UnitCommand(unit_to_build,
		ability_type_for_structure,
		Point2D(unit_to_build->pos.x + rx * 15.0f, unit_to_build->pos.y + ry * 15.0f));

	return true;
}

bool TestBot::TryBuildSupplyDepot()
{
	const ObservationInterface* observation = Observation();

	// If we are not supply capped, don't build a supply depot.
	if (observation->GetFoodUsed() <= observation->GetFoodCap() - 2)
		return false;

	// Try and build a depot. Find a random SCV and give it the order.
	return TryBuildStructure(ABILITY_ID::BUILD_SUPPLYDEPOT);
}

bool TestBot::TryBuildRefinery()
{
	const ObservationInterface* observation = Observation();
	int number_of_refineries = CountUnitType(UNIT_TYPEID::TERRAN_REFINERY);
	if (number_of_refineries == 0) {
		return TryBuildStructure(ABILITY_ID::BUILD_REFINERY);
	}
}

const Unit * TestBot::FindNearestMineralPatch(const Point2D & start)
{
	Units units = Observation()->GetUnits(Unit::Alliance::Neutral);
	float distance = std::numeric_limits<float>::max();
	const Unit* target = nullptr;
	for (const auto& u : units) {
		if (u->unit_type == UNIT_TYPEID::NEUTRAL_MINERALFIELD) {
			float d = DistanceSquared2D(u->pos, start);
			if (d < distance) {
				distance = d;
				target = u;
			}
		}
	}
	return target;
}

const Unit * TestBot::FindNearestVespeneGas(const Point2D & start)
{
	Units units = Observation()->GetUnits(Unit::Alliance::Neutral);
	float distance = std::numeric_limits<float>::max();
	const Unit* target = nullptr;
	for (const auto& u : units) {
		if (u->unit_type == UNIT_TYPEID::NEUTRAL_VESPENEGEYSER ||
			u->unit_type == UNIT_TYPEID::NEUTRAL_RICHVESPENEGEYSER ||
			u->unit_type == UNIT_TYPEID::NEUTRAL_PURIFIERVESPENEGEYSER ||
			u->unit_type == UNIT_TYPEID::NEUTRAL_SHAKURASVESPENEGEYSER ||
			u->unit_type == UNIT_TYPEID::NEUTRAL_PROTOSSVESPENEGEYSER) {
			float d = DistanceSquared2D(u->pos, start);
			if (d < distance) {
				distance = d;
				target = u;
			}
		}
	}
	return target;
}

bool TestBot::TryBuildBarracks()
{
	const ObservationInterface* observation = Observation();

	if (CountUnitType(UNIT_TYPEID::TERRAN_SUPPLYDEPOT) < 1) {
		return false;
	}

	if (CountUnitType(UNIT_TYPEID::TERRAN_BARRACKS) > 0) {
		return false;
	}

	return TryBuildStructure(ABILITY_ID::BUILD_BARRACKS);
}


TestBot::TestBot() {

}

