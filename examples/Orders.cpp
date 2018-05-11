#include <iostream>

#include "Orders.h"


Orders::Orders()
{
}


Orders::~Orders()
{
}

void Orders::AddAttackOrder(Order o, int num)
{
	if (num == -1 || num < 0 || num >= attack_orders.size()) {
		attack_orders.push_back(o);
	}
	else {
		attack_orders.insert(attack_orders.begin() + num, o);
	}
}

void Orders::AddBuildOrder(Order o, int num)
{
	if (o.type != Order::ATTACK) {
		std::cerr << "Wrong order type for insertion !" << std::endl;
		return;
	}
	if (num == -1 || num < 0 || num >= attack_orders.size()) {
		attack_orders.push_back(o);
	}
	else {
		attack_orders.insert(attack_orders.begin() + num, o);
	}
}
