#include "transport_catalogue.h"

namespace transport {

    void TransportCatalogue::AddBus(std::string_view name, std::vector<const Stop*>& stops, bool is_circle) {
        std::vector<Stop*> FreeStops;

        for (const Stop* stop : stops) {
            FreeStops.push_back(SearchStopSetDistance(stop->NameStop));
        }

        AllBuses.push_back(Bus(name, FreeStops, is_circle));
        for (const auto& stop : stops) {
            if (StopNameToStop.count(stop->NameStop)) {
                StopNameToStop.at(stop->NameStop)->NameBuses.insert(&AllBuses.back());
            }
        }
        BusNameToBus[AllBuses.back().NameBus] = &AllBuses.back();
    }

    void TransportCatalogue::AddStop(std::string_view name, const geo::Coordinates& coordinates) {
        AllStops.push_back(Stop(name, coordinates));
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

    const std::set<Bus*, Bus::cmp_ptr> TransportCatalogue::GetStopRoutes(std::string_view name) const {
        return StopNameToStop.at(name)->NameBuses;
    }

    void TransportCatalogue::SetDistanceBetweenStops(const Stop* from, const Stop* to, int distance) {
        Stop* stop_from = SearchStopSetDistance(from->NameStop);
        stop_from->Distance[to] = distance;
    }

    int TransportCatalogue::GetDistanceBetweenStops(const Stop* from, const Stop* to) const {
        if (from->Distance.count(to)) return from->Distance.at(to);
        else if (to->Distance.count(from)) return to->Distance.at(from);
        else return 0;
    }


    const RouteInformation TransportCatalogue::GetRouteInformation(std::string_view name) const {
        RouteInformation result;

        const Bus* bus = SearchRoute(name);

        if (!bus) throw std::invalid_argument("bus not found");

        bus->CircularRoute ?
            result.StopCount = bus->Stops.size() :
            result.StopCount = bus->Stops.size() * 2 - 1;

        int route_length = 0;
        double geographic_length = 0.0;

        for (auto it = bus->Stops.begin(); it + 1 != bus->Stops.end(); ++it) {

            const auto from = (*it);
            const auto to = *(it + 1);

            if (bus->CircularRoute) {
                route_length += GetDistanceBetweenStops(from, to);
                geographic_length += geo::ComputeDistance((*it)->Coordinates,
                    (*(it + 1))->Coordinates);
            }
            else {
                route_length += GetDistanceBetweenStops(from, to) + GetDistanceBetweenStops(to, from);
                geographic_length += geo::ComputeDistance((*it)->Coordinates,
                    (*(it + 1))->Coordinates) * 2;
            }
        }

        if (bus->CircularRoute) {
            auto from = *(bus->Stops.end() - 1);
            auto to = (*(bus->Stops.begin()));
            route_length += GetDistanceBetweenStops(from, to);
            geographic_length += geo::ComputeDistance(from->Coordinates, to->Coordinates);
        }

        std::unordered_set<Stop*> unique_stops{ bus->Stops.begin(),bus->Stops.end() };

        result.NameRoute = bus->NameBus;
        result.UniqueStopCount = unique_stops.size();
        result.RouteLength = route_length;
        result.Curvature = route_length / geographic_length;

        return result;
    }
}