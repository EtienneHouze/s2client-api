#include <iostream>

#include "TestBot.h"

void TestBot::OnGameStart()

{
	std::cout << "Hello, World!" << std::endl;
    vespene_harvesters = std::map<Tag, int>();
    map = MapAnalyzer();
    map.AnalyzeMap(Observation());
    manager = MacroManager();
    last_orders = manager.ThinkAndSendOrders(Observation());
    where_to = Point3D(Observation()->GetGameInfo().enemy_start_locations[0].x,
        Observation()->GetGameInfo().enemy_start_locations[0].y,
        Observation()->GetStartLocation().z);
    float rx = GetRandomScalar();
    float ry = GetRandomScalar();
    rally_point = Point3D(
        Observation()->GetStartLocation().x + 15.f * rx,
        Observation()->GetStartLocation().y + 15.f* ry,
        Observation()->GetStartLocation().z);
    Actions()->SendChat("EZ game, EZ life");
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

    // Get orders from high level
    last_orders = manager.ThinkAndSendOrders(Observation());
    // Do stuff according to the orders
    FollowBuildOrders();
    FollowAttackOrders();
    TryBuildSupplyDepot();
    TryBuildBarracks();
   /* if (CountUnitType(UNIT_TYPEID::TERRAN_REFINERY) < 1)
        TryBuildRefinery();*/
    if (Observation()->GetMinerals() > 400)
        Expand();
    if (CountUnitType(UNIT_TYPEID::TERRAN_MARINE) > 20)
        is_attack_over = false;
    map.PrintDebugInfo(Debug());
    Debug()->SendDebug();

}

void TestBot::OnUnitIdle(const Unit * unit)
{
	switch (unit->unit_type.ToType()) {
	case UNIT_TYPEID::TERRAN_COMMANDCENTER: {
        if (unit->assigned_harvesters < 20)
		    Actions()->UnitCommand(unit, ABILITY_ID::TRAIN_SCV);
		break;
	}
	case UNIT_TYPEID::TERRAN_SCV: {
		const Unit* mineral_target = FindNearestMineralPatch(unit->pos);
		if (!mineral_target) {
            return;
		}
		Actions()->UnitCommand(unit, ABILITY_ID::SMART, mineral_target);
		break;
	}
	case UNIT_TYPEID::TERRAN_BARRACKS: {
        if (true) {
		    Actions()->UnitCommand(unit, ABILITY_ID::TRAIN_MARINE);
        }
		break;
	}
	case UNIT_TYPEID::TERRAN_MARINE: {
		// if an enemy unit is nearby, then we attack it
        const Unit* unit_to_attack = FindNearestEnemyUnit(unit->pos);
        if (unit_to_attack != nullptr) {
            if (Distance2D(unit_to_attack->pos,unit->pos) < 15)
            Actions()->UnitCommand(unit, ABILITY_ID::ATTACK, unit_to_attack);
        }
        if (is_attack_over)
            Actions()->UnitCommand(unit, ABILITY_ID::SMART, rally_point);
        break;
	}
	default: {
		break;
	}
	}
}

void TestBot::OnUnitVoid(const Unit * unit)
{
    
}

void TestBot::OnUnitCreated(const Unit * unit)
{
    switch (unit->unit_type.ToType()) {
    case UNIT_TYPEID::TERRAN_SCV: {
        // If the unit is a scv, we need to fin the closest mineral non_saturated mineral field.
        // We ifrst find the closest non_saturated Command Center.
        const Unit* cc_to_go = nullptr;
        float distance = std::numeric_limits<float>::max();
        std::vector<UNIT_TYPEID> cc_types = { UNIT_TYPEID::TERRAN_COMMANDCENTER,UNIT_TYPEID::TERRAN_ORBITALCOMMAND };
        Units all_cc = Observation()->GetUnits(Unit::Alliance::Self, IsUnits(cc_types));
        for (const auto& cc : all_cc) {
            if (cc->assigned_harvesters < cc->ideal_harvesters) {
                float distance_to_cc = Distance2D(cc->pos, unit->pos);
                if (distance_to_cc < distance) {
                    cc_to_go = cc;
                    distance = distance_to_cc;
                }
            }
        }
        if (cc_to_go == nullptr)
            cc_to_go = unit;
        const Unit* minerals_to_go = FindNearestMineralPatch(cc_to_go->pos);
        Actions()->UnitCommand(unit, ABILITY_ID::SMART, minerals_to_go);
        break;
        }
    default:
        break;
    }
}

void TestBot::OnBuildingConstructionComplete(const Unit * unit)
{
    switch (unit->unit_type.ToType()) {
    case UNIT_TYPEID::TERRAN_COMMANDCENTER: {       // When a new cc is built, we check if other CCs are overcrowded. If yes, move SCVs.
        BaseDescriptor corresponding_base = map.ClosestUnoccupiedBase(unit->pos);
        corresponding_base.occupation = Unit::Alliance::Self;
        const Unit* closest_mineral = FindNearestMineralPatch(unit->pos);
        Units all_scv = Observation()->GetUnits(Unit::Alliance::Self, IsUnit(UNIT_TYPEID::TERRAN_SCV));
        for (const auto& scv : all_scv) {
            const Unit* closest_cc = FindNearestUnit(scv->pos, { UNIT_TYPEID::TERRAN_COMMANDCENTER,UNIT_TYPEID::TERRAN_ORBITALCOMMAND },Unit::Alliance::Self);
            if (closest_cc->assigned_harvesters > closest_cc->ideal_harvesters) {

            }
               // Actions()->UnitCommand(scv, ABILITY_ID::SMART, closest_mineral);
        }
        break;
        }
    case UNIT_TYPEID::TERRAN_REFINERY: {        // When a refinery is built, we take 3 random workers to harvest gas.
        Units all_scv = Observation()->GetUnits(Unit::Alliance::Self, IsUnit(UNIT_TYPEID::TERRAN_SCV));
        int count = 0;
        std::cout << "Refinerey Complete" << std::endl;
        for (const auto* scv : all_scv) {
            if (count >= 3)
                break;
            std::vector<UnitOrder> scv_orders = scv->orders;
            bool harvest = true;
            for (auto o : scv_orders) {
                if (o.ability_id == ABILITY_ID::HARVEST_GATHER_SCV
                    || o.ability_id == ABILITY_ID::HARVEST_GATHER
                    || o.ability_id == ABILITY_ID::HARVEST_RETURN_SCV
                    || o.ability_id == ABILITY_ID::HARVEST_RETURN)
                    harvest = true;
            }
            if (harvest) {
                Actions()->UnitCommand(scv, ABILITY_ID::SMART, unit);
                vespene_harvesters.emplace(scv->tag, 1);
                count++;
            }
        }
        for (std::map<Tag, int>::iterator it = vespene_harvesters.begin(); it != vespene_harvesters.end(); it++) {
            std::cout << it->first << std::endl;
        }
        break;
        }
    default:
        break;
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
			if (order.ability_id == ability_type_for_structure && observation->GetMinerals() < 200) {
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

bool TestBot::Research(ABILITY_ID ability_for_research, UNIT_TYPEID unit_to_search) {
    /*
        Starts a research, return false if it fails,  true if success.
        :param:
            ability_for_research : the ability to launch the research
            unit_to_search: the type of unit to search
        :return:
            a boolean.
    */
    const Unit* unit  = nullptr;
    Units units = Observation()->GetUnits();
    for (const auto& u : units) {
        if (u->unit_type == unit_to_search)
            unit = u;
    }
    if (unit == nullptr) {
        std::cerr << "No unit to conduct the research !" << std::endl;
        return false;
    }
    Actions()->UnitCommand(unit, ability_for_research);
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

bool TestBot::TryBuildBarracks()
{
	const ObservationInterface* observation = Observation();

	if (CountUnitType(UNIT_TYPEID::TERRAN_SUPPLYDEPOT) < 1) {
		return false;
	}

	if (CountUnitType(UNIT_TYPEID::TERRAN_BARRACKS) > 4* CountUnitType(UNIT_TYPEID::TERRAN_COMMANDCENTER)) {
		return false;
	}

	return TryBuildStructure(ABILITY_ID::BUILD_BARRACKS);
}

bool TestBot::Expand() {
    /*
        Build another command center.
    */
    BaseDescriptor closest_base = map.ClosestUnoccupiedBase(Observation()->GetStartLocation());    // Finds the closest unccupied base from the start location.
    const Unit* scv = nullptr;
    Units units = Observation()->GetUnits(Unit::Alliance::Self,IsUnit(UNIT_TYPEID::TERRAN_SCV));
    // Selection of the scv to builf the new base.
    for (const auto& u : units) {
        if (u->unit_type == UNIT_TYPEID::TERRAN_SCV) {
            scv = u;
            break;
        }
    }
    int minerals_begin = Observation()->GetMinerals();
    std::cout << "scv_chosen" << std::endl;
    Point3D target_position = closest_base.position_center;
    Point3D actual_target = target_position;
    float x, y;
    x = closest_base.position_center.x;
    y = closest_base.position_center.y;
    std::cout << "Closest base : " << closest_base.iD << " " << x << " " << y << std::endl;
    const ObservationInterface* obs = Observation();
    float distance = std::numeric_limits<float>::max();
    for (int i = -10; i <= 10; i++) {
        for (int j = -10; j <= 10; j++) {
            // Check for possible build positions.
            actual_target = Point3D(x + i, y + j, target_position.z);
            Debug()->DebugBoxOut(actual_target - Point3D(1, 1, 1), actual_target + Point3D(1, 1, 1));
    //        std::cout << actual_target.x << " " << actual_target.y << std::endl;
            Actions()->UnitCommand(scv, ABILITY_ID::BUILD_COMMANDCENTER, actual_target);
        }
    }
    if (Observation()->GetMinerals() < minerals_begin - 200) {  // We check if the cc is being built.
        map.bases[closest_base.iD].occupation = Unit::Alliance::Self;
        return true;
    }
        
    return false;
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

const Unit * TestBot::FindNearestUnit(const Point2D & start, UNIT_TYPEID unit_type, Unit::Alliance all)
{
    Units all_units = Observation()->GetUnits(all, IsUnit(unit_type));
    float distance = std::numeric_limits<float>::max();
    const Unit* ret = nullptr;
    for (const auto u : all_units) {
        float distance_to_u = Distance2D(start, u->pos);
        if (distance_to_u < distance) {
            ret = u;
            distance = distance_to_u;
        }
    }
    return ret;
}

const Unit * TestBot::FindNearestUnit(const Point2D & start, std::vector<UNIT_TYPEID> unit_type, Unit::Alliance all)
{
    Units all_units = Observation()->GetUnits(all, IsUnits(unit_type));
    float distance = std::numeric_limits<float>::max();
    const Unit* ret = nullptr;
    for (const auto u : all_units) {
        float distance_to_u = Distance2D(start, u->pos);
        if (distance_to_u < distance) {
            ret = u;
            distance = distance_to_u;
        }
    }
    return ret;
}


const Unit * TestBot::FindNearestEnemyUnit(const Point2D & start, float distance)
{
    const Unit* ret = nullptr;
    bool is_target_dangerous = false;
    Units units = Observation()->GetUnits(Unit::Alliance::Enemy);
    // We find the closest dangerous target.
    for (const auto& u : units) {
        if (/*(u->unit_type != UNIT_TYPEID::TERRAN_SCV && u->unit_type != UNIT_TYPEID::PROTOSS_PROBE && u->unit_type != UNIT_TYPEID::ZERG_DRONE)*/true) {
            float distance_to_target = DistanceSquared2D(u->pos,start);
            if (distance_to_target < distance) {
                distance = distance_to_target;
                ret = u;
                is_target_dangerous = true;
            }
        }
        else {
            if (!is_target_dangerous) {
                float distance_to_target = DistanceSquared2D(u->pos,start);
                if (distance_to_target < distance) {
                    distance = distance_to_target;
                    ret = u;
                    is_target_dangerous = true;
                }
            }
        }
    }
    return ret;
}

float TestBot::DistanceToClosestEnemyUnit(const Point2D& start) {
    Units all_enemies = Observation()->GetUnits(Unit::Alliance::Enemy);
    float distance = std::numeric_limits<float>::max();
    for (const auto& u : all_enemies) {
        if (Distance2D(start, u->pos) < distance)
            distance = Distance2D(start, u->pos);
    }
    return distance;
}

int TestBot::NumberOfUnitsNearby(Unit::Alliance alliance, const Point2D & start, float default_distance)
{
    Units nearby = Observation()->GetUnits(alliance, IsCloser(start, default_distance));
    return nearby.size();
}

bool TestBot::FollowAttackOrders() {
    Units all_marines = Observation()->GetUnits(Unit::Alliance::Self, IsUnit(UNIT_TYPEID::TERRAN_MARINE));
    std::vector<UNIT_TYPEID> high_prio = {
      UNIT_TYPEID::PROTOSS_STALKER,
      UNIT_TYPEID::PROTOSS_ZEALOT,
      UNIT_TYPEID::PROTOSS_SENTRY,
      UNIT_TYPEID::PROTOSS_ADEPT,
      UNIT_TYPEID::ZERG_ZERGLING,
      UNIT_TYPEID::ZERG_ROACH,
      UNIT_TYPEID::ZERG_QUEEN,
      UNIT_TYPEID::ZERG_RAVAGER,
      UNIT_TYPEID::ZERG_MUTALISK,
      UNIT_TYPEID::TERRAN_MARINE,
      UNIT_TYPEID::TERRAN_MARAUDER,
      UNIT_TYPEID::TERRAN_HELLION
    };
    std::vector<UNIT_TYPEID> medium_prio = {
        UNIT_TYPEID::PROTOSS_PROBE,
        UNIT_TYPEID::TERRAN_SCV,
        UNIT_TYPEID::ZERG_DRONE
    };
    if (!is_attack_over) {
        // check if the attack is over
        int number_of_units_near_target = NumberOfUnitsNearby(Unit::Alliance::Enemy, where_to); // number of enemies near the target
        int number_of_allies_near_target = NumberOfUnitsNearby(Unit::Alliance::Self, where_to);
        if (number_of_allies_near_target > 0 && number_of_units_near_target < 1) {
            is_attack_over = true;
            return true;
        }
        // Now, mmanage the units
        for (const auto& marine : all_marines) {
            int current_hp = marine->health;
            const Unit* closest_enemy = FindNearestEnemyUnit(marine->pos);
            const Unit* closest_dangerous_high = FindNearestUnit(marine->pos, high_prio, Unit::Alliance::Enemy);
            const Unit* closest_dangerous_med = FindNearestUnit(marine->pos, medium_prio, Unit::Alliance::Enemy);
            if (closest_enemy == nullptr) {
                int marines_nearby = Observation()->GetUnits(Unit::Alliance::Self, IsUnit(UNIT_TYPEID::TERRAN_MARINE)).size();
                if (marines_nearby < 10) {
                    Actions()->UnitCommand(marine, ABILITY_ID::SMART, rally_point);
                }
                else {
                    Actions()->UnitCommand(marine, ABILITY_ID::SMART, where_to);
                }
            }
            else {
                if (closest_dangerous_med != nullptr) {
                    if (Distance2D(closest_dangerous_med->pos, marine->pos) < 10)
                        closest_enemy = closest_dangerous_med;
                }
                if (closest_dangerous_high != nullptr) {
                    if (Distance2D(closest_dangerous_high->pos, marine->pos) < 8)
                        closest_enemy = closest_dangerous_high;
                }
                if (current_hp < 10 /*&& Distance2D(marine->pos, closest_enemy->pos) > 4*/ && Distance2D(marine->pos, closest_enemy->pos) < 3) {
                    // If we are low health, we move to the back
                    Point3D t = marine->pos + marine->pos - closest_enemy->pos;
                    Actions()->UnitCommand(marine, ABILITY_ID::SMART, t); // We move to t
                    Debug()->DebugSphereOut(t, 0.5f, Colors::Green);
                }
                else {
                    if (Distance2D(closest_enemy->pos, marine->pos) < 10) {
                        Actions()->UnitCommand(marine, ABILITY_ID::SMART, closest_enemy);
                        Debug()->DebugSphereOut(closest_enemy->pos, 0.5f, Colors::Red);
                    }
                    else {      // Not really so much enemies nearby
                        int marines_nearby = Observation()->GetUnits(Unit::Alliance::Self, IsUnit(UNIT_TYPEID::TERRAN_MARINE)).size();
                        if (marines_nearby < 10) {
                            Actions()->UnitCommand(marine, ABILITY_ID::SMART, rally_point);
                        }
                        else {
                            Actions()->UnitCommand(marine, ABILITY_ID::SMART, where_to);
                        }
                    }
                }
            }

        }


    }
    return true;
}

bool TestBot::FollowBuildOrders() {
    for (auto order : last_orders.GetBuildOrders()) {
        switch (order.what_to_build)
        {
        case UNIT_TYPEID::TERRAN_SCV: {
            Units all_units = Observation()->GetUnits(Unit::Alliance::Self);
            for (int count = order.number; count >= 0; count--) {       // Looping through command centers to build SCVs.
                for (const auto& cc : all_units) {
                    if (cc->unit_type == UNIT_TYPEID::TERRAN_COMMANDCENTER
                        || cc->unit_type == UNIT_TYPEID::TERRAN_ORBITALCOMMAND) {
                        if (count >= 0) {
                            Actions()->UnitCommand(cc, ABILITY_ID::TRAIN_SCV);
                            count--;
                        }
                    }
                }
            }
            break;
        }

        case UNIT_TYPEID::TERRAN_MARINE: {
            Units all_units = Observation()->GetUnits(Unit::Alliance::Self,IsUnit(UNIT_TYPEID::TERRAN_BARRACKS));
            for (int count = order.number; count >= 0; count--) {       // Looping through command barracks to build marines.
                for (const auto& racks : all_units) {
                    if (count >= 0) {
                        Actions()->UnitCommand(racks, ABILITY_ID::TRAIN_MARINE);
                        count--;
                    }
                }
            }
            break;
        }

        case UNIT_TYPEID::TERRAN_SUPPLYDEPOT: {
            TryBuildSupplyDepot();
            break;
        }
        case UNIT_TYPEID::TERRAN_BARRACKS: {
            TryBuildBarracks();
            break;
        }
        case UNIT_TYPEID::TERRAN_COMMANDCENTER: {
            Expand();
        }
        default:
            break;
        }
    }
    return true;
}


TestBot::TestBot() {

}

