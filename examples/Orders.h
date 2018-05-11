#ifndef ORDERS_H
#define ORDERS_H

#include "Order.h"

class Orders
{
public:
    enum Priority {
		ATTACK,
		ECONOMY
    };

private:
    std::vector<Order> attack_orders;
    std::vector<Order> build_orders;
    Priority priority;

public:
	Orders();
	~Orders();

    const std::vector<Order> GetAttackOrders() {
        return attack_orders;
    }

    const std::vector<Order> GetBuildOrders() const {
        return build_orders;
    }

	/*
		Add an order to the orders list at the specified position. If posistion is invalid, adds the order at the end.
    */
    void AddAttackOrder(Order o, int num = -1);


	/*
		Add an order to the orders list at the specified position. If posistion is invalid, adds the order at the end.
    */
    void AddBuildOrder(Order o, int num = -1);

};

#endif