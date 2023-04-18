#include "transport_catalogue.h"

namespace transport {

    void TransportCatalogue::AddBus(Bus&& bus) {
        all_bus.push_back(bus);
        for (const auto& name_stop : bus.Name_Stops) {
            for (auto& stop : all_stops) {
                if (stop.Name_Stop == name_stop) stop.Name_Buses.insert(bus.Name_Bus);
            }
        }
        BusName_To_Bus[all_bus.back().Name_Bus] = &all_bus.back();
    }

    void TransportCatalogue::AddStop(Stop&& stop) {
        all_stops.push_back(stop);
        StopName_To_Stop[all_stops.back().Name_Stop] = &all_stops.back();
    }

    const Bus* TransportCatalogue::SearchRoute(const std::string& name) const {
        return BusName_To_Bus.count(name) ? BusName_To_Bus.at(name) : nullptr;
    }

    Stop* TransportCatalogue::SearchStop(const std::string& name) const {
        return StopName_To_Stop.count(name) ? StopName_To_Stop.at(name) : nullptr;
    }

    const std::set<std::string> TransportCatalogue::GettingStopRoutes(std::string name) const {
        return StopName_To_Stop.at(name)->Name_Buses;
    }

    void TransportCatalogue::SettingDistanceBetweenStops(Stop* from, Stop* to, int distance) {
        from->distance[to->Name_Stop] = distance;
    }

    int TransportCatalogue::GettingDistanceBetweenStops(Stop* from, Stop* to) const {
        if (from->distance.count(to->Name_Stop)) return from->distance.at(to->Name_Stop);
        else if (to->distance.count(from->Name_Stop)) return to->distance.at(from->Name_Stop);
        else return 0;
    }


    const RouteInformation TransportCatalogue::GettingRouteInformation(const std::string& name) const {
        RouteInformation result;

        const Bus* bus = SearchRoute(name);

        if (!bus) throw std::invalid_argument("bus not found");

        bus->CircularRoute ?
            result.StopCount = bus->Name_Stops.size() :
            result.StopCount = bus->Name_Stops.size() * 2 - 1;

        int route_length = 0;
        double geographic_length = 0.0;

        for (auto it = bus->Name_Stops.begin(); it + 1 != bus->Name_Stops.end(); ++it) {
            auto from = StopName_To_Stop.find(*it)->second;
            auto to = StopName_To_Stop.find((*(it + 1)))->second;

            if (bus->CircularRoute) {
                route_length += GettingDistanceBetweenStops(from, to);
                geographic_length += ComputeDistance(StopName_To_Stop.find(*it)->second->coordinates,
                    StopName_To_Stop.find(*(it + 1))->second->coordinates);
            }
            else {
                route_length += GettingDistanceBetweenStops(from, to) + GettingDistanceBetweenStops(to, from);
                geographic_length += ComputeDistance(StopName_To_Stop.find(*it)->second->coordinates,
                    StopName_To_Stop.find(*(it + 1))->second->coordinates) * 2;
            }
        }

        if (bus->CircularRoute) {
            auto from = StopName_To_Stop.find(*(bus->Name_Stops.end() - 1))->second;
            auto to = StopName_To_Stop.find(*(bus->Name_Stops.begin()))->second;
            route_length += GettingDistanceBetweenStops(from, to);
            geographic_length += ComputeDistance(from->coordinates, to->coordinates);
        }

        std::unordered_set<std::string> unique_stops{ bus->Name_Stops.begin(),bus->Name_Stops.end() };

        result.NameRoute = bus->Name_Bus;
        result.UniqueStopCount = unique_stops.size();
        result.RouteLength = route_length;
        result.Curvature = route_length / geographic_length;

        return result;
    }
}
