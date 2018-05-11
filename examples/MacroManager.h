#ifndef MACRO_MANAGER_H
#define MACRO_MANAGER_H


#include <sc2api\sc2_api.h>
#include "Orders.h"

class MacroManager
{
public:
	MacroManager();
	~MacroManager();

	Orders ThinkAndSendOrders(const sc2::ObservationInterface* observation);
};

#endif