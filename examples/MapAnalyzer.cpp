#include <iostream>

#include "MapAnalyzer.h"

using namespace sc2;

MapAnalyzer::MapAnalyzer()
{
}

void MapAnalyzer::AnalyzeMap(const ObservationInterface* obs)
{
	std::cout << "beginning of the map analysis" << std::endl;
	const ObservationInterface* observation = obs;
	Units all_neutrals = observation->GetUnits(Unit::Alliance::Neutral);		// get all neutral features on the map.
	std::vector<const Unit*> all_vespenes = {};									// All vespene units.
	std::vector<const Unit*> all_minerals = {};									// All mineral units.
	std::cout << "Number of all neutral units : " << all_neutrals.size() << std::endl;
	for (const auto& unit : all_neutrals) {										// Loop to find all ressources
		if (unit->unit_type == UNIT_TYPEID::NEUTRAL_VESPENEGEYSER ||
			unit->unit_type == UNIT_TYPEID::NEUTRAL_PURIFIERVESPENEGEYSER ||
			unit->unit_type == UNIT_TYPEID::NEUTRAL_PROTOSSVESPENEGEYSER ||
			unit->unit_type == UNIT_TYPEID::NEUTRAL_RICHVESPENEGEYSER ||
			unit->unit_type == UNIT_TYPEID::NEUTRAL_SHAKURASVESPENEGEYSER)
			all_vespenes.push_back(unit);
		else {
			if (unit->unit_type == UNIT_TYPEID::NEUTRAL_MINERALFIELD ||
				unit->unit_type == UNIT_TYPEID::NEUTRAL_MINERALFIELD750 ||
				unit->unit_type == UNIT_TYPEID::NEUTRAL_RICHMINERALFIELD ||
				unit->unit_type == UNIT_TYPEID::NEUTRAL_RICHMINERALFIELD750) {
				all_minerals.push_back(unit);
			}
		}
	}
	std::cout << all_vespenes.size() << "  " << all_minerals.size() << std::endl;
	Point3D start_location = observation->GetStartLocation();
	BaseDescriptor start_base = BaseDescriptor();
	start_base.position_center = start_location;
	std::vector<const Unit*> old_vespenes = all_vespenes;
	std::vector<const Unit*> old_minerals = all_minerals;
	all_vespenes = {};
	all_minerals = {};
	while (old_vespenes.size() > 0) {
		const Unit* vespene = old_vespenes.back();
		old_vespenes.pop_back();
		Point3D difference = vespene->pos - start_location;
		if (abs(difference.x) < 10 && abs(difference.y) < 10 && abs(difference.z) < 10) {
			start_base.gas.push_back(vespene);
		}
		else
			all_vespenes.push_back(vespene);
	}
	while (old_minerals.size() > 0) {
		const Unit* mineral = old_minerals.back();
		old_minerals.pop_back();
		Point3D difference = mineral->pos - start_location;
		if (abs(difference.x) < BASE_SIZE && abs(difference.y) < BASE_SIZE && abs(difference.z) < BASE_SIZE) {
			start_base.minerals.push_back(mineral);
		}
		else {
			all_minerals.push_back(mineral);
		}
	}
	std::cout << all_vespenes.size() << "   " << all_minerals.size() << std::endl;
	start_base.iD = 1;
	bases.push_back(start_base);

	int newID = 2;
	// Now, we look at the other bases on the map.
	while (all_minerals.size() > 0) {
		BaseDescriptor newBase = BaseDescriptor();
		newBase.iD = newID;
		newID++;
		newBase.position_center = all_minerals.back()->pos;		// We initialize the position of the new base as one of the minerals.
		old_minerals = all_minerals;
		old_vespenes = all_vespenes;
		all_minerals = {};
		all_vespenes = {};
		const Unit* top_mineral = nullptr;		// initialization to find the most top and bottom mineral fields.
		const Unit* bottom_mineral = nullptr;
		float top,bottom;
		top = std::numeric_limits<float>::min();
		bottom = std::numeric_limits<float>::max();
		while (old_minerals.size() > 0) {
			const Unit* mineral = old_minerals.back();
			old_minerals.pop_back();
			Point3D difference = mineral->pos - newBase.position_center;
			if (abs(difference.x) < BASE_SIZE && abs(difference.y) < BASE_SIZE && abs(difference.z) < BASE_SIZE) {
				if (mineral->pos.y > top) {
					top = mineral->pos.y;
					top_mineral = mineral;
				}
				if (mineral->pos.y < bottom) {
					bottom = mineral->pos.y;
					bottom_mineral = mineral;
				}
				newBase.minerals.push_back(mineral);
			}
			else {
				all_minerals.push_back(mineral);
			}
		}
		while (old_vespenes.size() > 0) {
			const Unit* vespene = old_vespenes.back();
			old_vespenes.pop_back();
			Point3D difference = vespene->pos - newBase.position_center;
			if (abs(difference.x) < BASE_SIZE && abs(difference.y) < BASE_SIZE && abs(difference.z) < BASE_SIZE) {
				newBase.gas.push_back(vespene);
			}
			else {
				all_vespenes.push_back(vespene);
			}
		}
		
		
		bases.push_back(newBase);
		
	}
}

void MapAnalyzer::PrintDebugInfo(DebugInterface* debug)
{
	for (const BaseDescriptor base : bases) {
		std::string position_string = std::to_string(base.position_center.x) + "," + std::to_string(base.position_center.y);
		debug->DebugBoxOut(base.position_center - (2*ones), base.position_center + (2*ones));
		debug->DebugTextOut("Base #"+std::to_string(base.iD), base.position_center);
		for (const Unit* vespene : base.gas) {
			position_string = std::to_string(vespene->pos.x) + "," + std::to_string(vespene->pos.y);
			debug->DebugBoxOut(vespene->pos - ones, vespene->pos + ones);
			debug->DebugTextOut(position_string, vespene->pos);
		}
		for (const Unit* mineral : base.minerals) {
			position_string = std::to_string(mineral->pos.x) + "," + std::to_string(mineral->pos.y);
			debug->DebugBoxOut(mineral->pos - ones, mineral->pos + ones);
			debug->DebugTextOut(position_string, mineral->pos);
		}
	}
}


MapAnalyzer::~MapAnalyzer()
{
}
