#include "MacroManager.h"

using namespace sc2;

MacroManager::MacroManager()
{
}

MacroManager::~MacroManager()
{
}


Orders MacroManager::ThinkAndSendOrders(const sc2::ObservationInterface* observation) {
	Orders orders = Orders();

	Units populationSize = observation->GetUnits(Unit::Alliance::Ally);
	int pop_size = observation->GetFoodUsed();
	// Standard build
	int ordersSize = 0;
	if (pop_size == 14){
		orders.AddBuildOrder(Order(Point3D(0, 0, 0), UNIT_TYPEID::TERRAN_SUPPLYDEPOT, static_cast<int>(populationSize.size())), 0);
		ordersSize++;
	}
	if (pop_size == 16) {
		orders.AddBuildOrder(Order(Point3D(0, 0, 0), UNIT_TYPEID::TERRAN_BARRACKS, static_cast<int>(populationSize.size())), 0);
		ordersSize++;
	}
	if (pop_size > 16) {
		orders.AddBuildOrder(Order(Point3D(0, 0, 0), UNIT_TYPEID::TERRAN_MARINE, static_cast<int>(populationSize.size())), static_cast<int>(populationSize.size()) + ordersSize);
		ordersSize++;
	}
	orders.AddBuildOrder(Order(Point3D(0, 0, 0), UNIT_TYPEID::TERRAN_SCV, static_cast<int>(populationSize.size()) + ordersSize), ordersSize - 1);

	// Attack orders

	orders.AddAttackOrder(Order(
		Point3D(
			observation->GetGameInfo().enemy_start_locations[0].x,
			observation->GetGameInfo().enemy_start_locations[0].y,
			observation->GetStartLocation().z), ordersSize),
		0);

	return orders;
}