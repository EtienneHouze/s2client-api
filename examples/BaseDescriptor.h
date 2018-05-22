#ifndef BASE_DESCRIPTOR_H
#define BASE_DESCRIPTOR_H

#include <sc2api\sc2_api.h>
#include <vector>

using namespace sc2;

class BaseDescriptor
{
public:

    Point3D position_center;        //Position of the center of the base, ie where the command center should be placed.
    std::vector<const Unit*> minerals;    // Minerals in the base
    std::vector<const Unit*> gas;         // Gas in the base
    Unit::Alliance occupation;              // Who controls the base.
    int iD;

public:
	BaseDescriptor();
	~BaseDescriptor();
};

#endif