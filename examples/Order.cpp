#include "Order.h"



Order::Order()
{
}


Order::~Order()
{
}

Order::Order(Type t)
{
	type = t;
	target = sc2::Point3D(0, 0, 0);
	what_to_build = sc2::UNIT_TYPEID::INVALID;
	number = -1;
}

Order::Order(sc2::Point3D tar, sc2::UNIT_TYPEID unitType, int n)
{
	target = tar;
	what_to_build = unitType;
	number = n;
}

Order::Order(sc2::Point3D tar, int n)
{
	target = tar;
	number = n;
}

Order Order::CreateAttackOrder(sc2::Point3D target, int n)
{
	Order ret = Order(Order::Type::ATTACK);
	ret.target = target;
	ret.number = n;
	return ret;
}

Order Order::CreateBuildOrder(sc2::Point3D target, sc2::UNIT_TYPEID what, int n)
{
	Order ret = Order(Order::Type::BUILD);
	ret.what_to_build = what;
	ret.number = n;
	return ret;
}
