#include "DataBase.h"

#include <math.h>

double ToRadians(double deg) {
    return deg * M_PI / 180.0;
}

double Haversine(double lon1, double lat1,
                 double lon2, double lat2) {

    const double R = 6372.8;

    lat1 = ToRadians(lat1);
    lon1 = ToRadians(lon1);
    lat2 = ToRadians(lat2);
    lon2 = ToRadians(lon2);

    double dLat = lat2 - lat1;
    double dLon = lon2 - lon1;

    double a = std::pow(std::sin(dLat/2), 2) +
               std::cos(lat1) * std::cos(lat2) *
               std::pow(std::sin(dLon/2), 2);

    double c = 2 * std::asin(std::sqrt(a));

    return R * c;
}