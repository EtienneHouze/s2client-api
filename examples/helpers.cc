#include <sc2api\sc2_api.h>

#include <cmath>

float EuclideanDistance(const sc2::Point3D& p1, const sc2::Point3D& p2) {
    sc2::Point3D difference = sc2::Point3D(p1.x - p2.x, p1.y - p2.y, p1.z - p2.z);
    return pow(pow(difference.x, 2) + pow(difference.y, 2) + pow(difference.z, 2), 0.5);
}