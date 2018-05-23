#ifndef MACRO_MANAGER_H
#define MACRO_MANAGER_H


#include <sc2api\sc2_api.h>
#include "Orders.h"
#include "Order.h"

using namespace sc2;

class MacroManager
{
public:
	MacroManager();
	void GetObservations(const ObservationInterface * obs);
	~MacroManager();

	Orders ThinkAndSendOrders(const sc2::ObservationInterface* observation);
};

#endif