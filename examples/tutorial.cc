#include <sc2api\sc2_api.h>

#include <iostream>
#include <map>

using namespace sc2;

class Bot : public Agent {

private:

    std::map<ABILITY_ID, int> scv_occupations = std::map<ABILITY_ID, int>();

    bool TryBuildStructure(ABILITY_ID ability_type_for_structure, UNIT_TYPEID unit_type = UNIT_TYPEID::TERRAN_SCV) {
        const ObservationInterface* observation = Observation();

        //If a unit is already building the building, do nothing
        const Unit* unit_to_build = nullptr;    // Which unit will build the structure
        Units units = observation->GetUnits(Unit::Alliance::Self);
        int number_of_structure_being_built = 0;
        if (scv_occupations.find(ability_type_for_structure) != scv_occupations.end()) {
            if (scv_occupations[ability_type_for_structure] > 2) {
                return false;
            }
        }
        for (const auto& unit : units) {
            

            if (unit->unit_type == unit_type) {
                unit_to_build = unit;
            }
        }

        if (ability_type_for_structure == ABILITY_ID::BUILD_REFINERY) {
            Point2D pos = FindNearestVespenGas(unit_to_build->pos)->pos;
            std::cout << "Trying to buid a refinery :" << Observation()->GetMinerals() << " minerals in the bank."<< std::endl;
            Actions()->UnitCommand(unit_to_build,
                ability_type_for_structure,
                pos);
        }

        float rx = GetRandomScalar();
        float ry = GetRandomScalar();

        Actions()->UnitCommand(unit_to_build,
            ability_type_for_structure,
            Point2D(unit_to_build->pos.x + rx * 15.0f, unit_to_build->pos.y + ry * 15.0f));
        
        return true;
        
    }

    bool TryBuildSupplyDepot() {
        /*
        Builds a supply depot if possible and needed.
        */
        const ObservationInterface* observatoin = Observation();
        if (observatoin->GetFoodUsed() <= observatoin->GetFoodCap() - 2) {
            return false;
        }
        if (scv_occupations.find(ABILITY_ID::BUILD_SUPPLYDEPOT) != scv_occupations.end()) {
            if (scv_occupations[ABILITY_ID::BUILD_SUPPLYDEPOT] > 2)
                return false;
        }
        return TryBuildStructure(ABILITY_ID::BUILD_SUPPLYDEPOT);
    }

    bool TryBuildVespene() {
        const ObservationInterface* observation = Observation();
        if (scv_occupations.find(ABILITY_ID::BUILD_REFINERY) != scv_occupations.end())
            return false;

        return TryBuildStructure(ABILITY_ID::BUILD_EXTRACTOR);
    }

    const Unit* FindNearestMineralPatch(const Point2D& start) {
        Units units = Observation()->GetUnits(Unit::Alliance::Neutral);
        float distance = std::numeric_limits<float>::max();
        const Unit* target = nullptr;
        for (const auto& u : units) {
            if (u->unit_type == UNIT_TYPEID::NEUTRAL_VESPENEGEYSER) {
                float d = DistanceSquared2D(u->pos, start);
                if (d < distance) {
                    distance = d;
                    target = u;
                }
            }
        }
        return target;
    }

    const Unit* FindNearestVespenGas(const Point2D& start) {
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

public:
    virtual void OnGameStart() final {
        std::cout << "Hello World!" << std::endl;
    }

    virtual void OnStep() final {
        std::cout << Observation()->GetGameLoop();
        std::cout << ", Minerals : " << Observation()->GetMinerals();
        std::cout << ", Vespene " << Observation()->GetVespene() <<std::endl;
        // Update the occupations of the units
        scv_occupations.clear();
        const Units units = Observation()->GetUnits(Unit::Alliance::Self);
        for (const auto& unit : units) {
            for (const auto& order : unit->orders) {
                if (scv_occupations.find(order.ability_id) != scv_occupations.end())
                    scv_occupations[order.ability_id] ++;
                else {
                    scv_occupations.emplace(order.ability_id, 1);
                }
            }
        }
        int building_scv = 0;
        if (scv_occupations.find(ABILITY_ID::BUILD_REFINERY) != scv_occupations.end())
            std::cout << "Number of SCVs buildinng a refinery : " << scv_occupations[ABILITY_ID::BUILD_REFINERY] << std::endl;
        if (scv_occupations.find(ABILITY_ID::HARVEST_GATHER_SCV) != scv_occupations.end())
            std::cout << "Number of SCVs buildinng a refinery : " << scv_occupations[ABILITY_ID::HARVEST_GATHER_SCV] << std::endl;
        TryBuildSupplyDepot();
        TryBuildVespene();
    }

    virtual void OnUnitIdle(const Unit * unit) final {
        switch (unit->unit_type.ToType())
        {
        case (UNIT_TYPEID::TERRAN_COMMANDCENTER): {
            Actions()->UnitCommand(unit, ABILITY_ID::TRAIN_SCV);
            break;
            }
        case (UNIT_TYPEID::TERRAN_SCV): {
            const Unit* mineral_target = FindNearestMineralPatch(unit->pos);
            if (!mineral_target)
                break;
            Actions()->UnitCommand(unit, ABILITY_ID::SMART, mineral_target->pos);
            break;
        }
        default:
            break;
        }
    }

    virtual void OnUnitCreated(const Unit * unit) final {
        if (unit->unit_type.ToType() == UNIT_TYPEID::TERRAN_SCV) {
            std::cout << unit->tag << std::endl;
        }

    }
};

int main(int argc, char* argv[]) {
    Coordinator coordinator;
    coordinator.LoadSettings(argc, argv);

    Bot bot;
    coordinator.SetParticipants({
        CreateParticipant(Race::Terran,&bot),
        CreateComputer(Race::Zerg)
        });

    coordinator.LaunchStarcraft();
    coordinator.StartGame(sc2::kMapBelShirVestigeLE);

    while (coordinator.Update()) {

    }

    return 0;
}
