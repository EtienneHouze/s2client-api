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

	// Standard build
	int ordersSize = 0;
	if (populationSize.size() == 14){
		orders.AddBuildOrder(Order(Point3D(0, 0, 0), UNIT_TYPEID::TERRAN_SUPPLYDEPOT, static_cast<int>(populationSize.size())), 0);
		ordersSize++;
	}
	if (populationSize.size() == 16) {
		orders.AddBuildOrder(Order(Point3D(0, 0, 0), UNIT_TYPEID::TERRAN_BARRACKS, static_cast<int>(populationSize.size())), 0);
		ordersSize++;
	}
	if (populationSize.size() > 16) {
		orders.AddBuildOrder(Order(Point3D(0, 0, 0), UNIT_TYPEID::TERRAN_MARINE, static_cast<int>(populationSize.size())), static_cast<int>(populationSize.size()) + ordersSize);
		ordersSize++;
	}
	orders.AddBuildOrder(Order(Point3D(0, 0, 0), UNIT_TYPEID::TERRAN_SCV, static_cast<int>(populationSize.size()) + ordersSize), ordersSize - 1);

	// Attack orders

	orders.AddAttackOrder(Order(Point3D(0, 0, 0), ordersSize), 0);

	return orders;
}