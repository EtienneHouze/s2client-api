#include <sc2api/sc2_api.h>

#include <iostream>

#include "TestBot.h"

using namespace sc2;


int main(int argc, char* argv[]) {
    Coordinator coordinator;
    coordinator.LoadSettings(argc, argv);

    TestBot bot;
    coordinator.SetParticipants({
        CreateParticipant(Race::Terran, &bot),
        CreateComputer(Race::Zerg,Difficulty::Hard)
        });
    coordinator.SetRealtime(false);
    coordinator.LaunchStarcraft();
    coordinator.StartGame(sc2::kMapBelShirVestigeLE);

    while (coordinator.Update()) {
    }

    return 0;
}