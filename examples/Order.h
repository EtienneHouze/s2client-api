#ifndef ORDER_H
#define ORDER_H

#include <sc2api\sc2_api.h>

class Order
{
public:
    static enum Type {
        BUILD,
        ATTACK
    };

    Type type;                          // type of the order
    sc2::Point3D target;                // target position of the order
    sc2::UNIT_TYPEID what_to_build;     // id of the unit to build
    int number;                         // if the order is attack, number of attackers, if the order is build, number of units to build.

public:
    Order();
    Order(Type t);

	Order(sc2::Point3D tar, sc2::UNIT_TYPEID unitType, int n);

	Order(sc2::Point3D tar, int n);

    static Order CreateAttackOrder(sc2::Point3D target, int n);
    static Order CreateBuildOrder(sc2::Point3D target, sc2::UNIT_TYPEID what, int n);
    ~Order();
};

#endif