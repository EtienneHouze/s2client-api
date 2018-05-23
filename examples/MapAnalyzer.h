#ifndef MAP_ANALYZER_H
#define MAP_ANALYZER_H

#include <sc2api\sc2_api.h>

#include <vector>
#include <cmath>

#include "BaseDescriptor.h"

using namespace sc2;

class MapAnalyzer
{
	/*
		A class to analyze map features and store them, such as base locations, minerals, ressources, etc...
	*/

	// CONSTANTS
	const float BASE_SIZE = 11.0f;			// The size of a base.
	const Point3D tens = Point3D(10, 10, 10);
	const Point3D ones = Point3D(1, 1, 1);
	// ===============================================
    friend class Agent;

public:
	std::vector<BaseDescriptor> bases;		// A vector of all the bases on the map.
	



public:

	MapAnalyzer();					// Default void constructor.

	void AnalyzeMap(const ObservationInterface* obs);

	void PrintDebugInfo(DebugInterface* debug);

	MapAnalyzer & operator=(const MapAnalyzer& other) {
		MapAnalyzer newMap = MapAnalyzer();
		newMap.bases = other.bases;
		return newMap;
	}

    /*
        Return a point to the closest base to the given location.
        If no neutral base exists, returns a null pointer.
    */
    BaseDescriptor ClosestUnoccupiedBase(Point3D position);

	~MapAnalyzer();
};

#endif