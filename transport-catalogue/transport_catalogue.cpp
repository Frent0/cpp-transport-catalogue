#include "transport_catalogue.h"

namespace transport {

    void TransportCatalogue::AddBus(Bus& bus) {
        AllBuses.push_back(bus);
        for (const auto& stop : bus.NameStops) {
            if (StopNameToStop.count(stop)) {
                StopNameToStop.at(stop)->NameBuses.insert(bus.NameBus);
            }
        }
        BusNameToBus[AllBuses.back().NameBus] = &AllBuses.back();
    }

    void TransportCatalogue::AddStop(Stop& stop) {
        AllStops.push_back(stop);
        StopNameToStop[AllStops.back().NameStop] = &AllStops.back();
    }

    const Bus* TransportCatalogue::SearchRoute(const std::string_view name) const {
        return BusNameToBus.count(name) ? BusNameToBus.at(name) : nullptr;
    }

    const Stop* TransportCatalogue::SearchStop(const std::string_view name) const {
        return StopNameToStop.count(name) ? StopNameToStop.at(name) : nullptr;
    }

    Stop* TransportCatalogue::SearchStopSetDistance(const std::string_view name) const {
        return StopNameToStop.count(name) ? StopNameToStop.at(name) : nullptr;
    }

    const std::set<std::string> TransportCatalogue::GetStopRoutes(std::string_view name) const {
        return StopNameToStop.at(name)->NameBuses;
    }

    void TransportCatalogue::SetDistanceBetweenStops(Stop* from, Stop* to, int distance) {
        from->Distance[to->NameStop] = distance;
    }

    int TransportCatalogue::GetDistanceBetweenStops(const Stop* from, const Stop* to) const {
        if (from->Distance.count(to->NameStop)) return from->Distance.at(to->NameStop);
        else if (to->Distance.count(from->NameStop)) return to->Distance.at(from->NameStop);
        else return 0;
    }


    const RouteInformation TransportCatalogue::GetRouteInformation(const std::string& name) const {
        RouteInformation result;

        const Bus* bus = SearchRoute(name);
        
        if (!bus) throw std::invalid_argument("bus not found");

        bus->CircularRoute ?
            result.StopCount = bus->NameStops.size() :
            result.StopCount = bus->NameStops.size() * 2 - 1;

        int route_length = 0;
        double geographic_length = 0.0;

        for (auto it = bus->NameStops.begin(); it + 1 != bus->NameStops.end(); ++it) {
            auto from = StopNameToStop.find(*it)->second;
            auto to = StopNameToStop.find((*(it + 1)))->second;

            if (bus->CircularRoute) {
                route_length += GetDistanceBetweenStops(from, to);
                geographic_length += geo::ComputeDistance(StopNameToStop.find(*it)->second->Coordinates,
                    StopNameToStop.find(*(it + 1))->second->Coordinates);
            }
            else {
                route_length += GetDistanceBetweenStops(from, to) + GetDistanceBetweenStops(to, from);
                geographic_length += geo::ComputeDistance(StopNameToStop.find(*it)->second->Coordinates,
                    StopNameToStop.find(*(it + 1))->second->Coordinates) * 2;
            }
        }

        if (bus->CircularRoute) {
            auto from = StopNameToStop.find(*(bus->NameStops.end() - 1))->second;
            auto to = StopNameToStop.find(*(bus->NameStops.begin()))->second;
            route_length += GetDistanceBetweenStops(from, to);
            geographic_length += geo::ComputeDistance(from->Coordinates, to->Coordinates);
        }

        std::unordered_set<std::string> unique_stops{ bus->NameStops.begin(),bus->NameStops.end() };

        result.NameRoute = bus->NameBus;
        result.UniqueStopCount = unique_stops.size();
        result.RouteLength = route_length;
        result.Curvature = route_length / geographic_length;

        return result;
    }
}
