#include "transport_catalogue.h"


namespace transport {

    void TransportCatalogue::AddBus(std::string_view name, std::vector<const domain::Stop*>& stops, bool is_circle) {
        std::vector<domain::Stop*> FreeStops;

        for (const domain::Stop* stop : stops) {
            FreeStops.push_back(SearchStopFree(stop->NameStop));
        }

        AllBuses.push_back(domain::Bus(name, FreeStops, is_circle));
        for (const auto& stop : stops) {
            if (StopNameToStop.count(stop->NameStop)) {
                StopNameToStop.at(stop->NameStop)->NameBuses.insert(&AllBuses.back());
            }
        }
        BusNameToBus[AllBuses.back().NameBus] = &AllBuses.back();
    }

    void TransportCatalogue::AddStop(std::string_view name, const geo::Coordinates& coordinates) {
        AllStops.push_back(domain::Stop(name, coordinates));
        StopNameToStop[AllStops.back().NameStop] = &AllStops.back();
    }

    const domain::Bus* TransportCatalogue::SearchRoute(const std::string_view name) const {
        return BusNameToBus.count(name) ? BusNameToBus.at(name) : nullptr;
    }

    const domain::Stop* TransportCatalogue::SearchStop(const std::string_view name) const {
        return StopNameToStop.count(name) ? StopNameToStop.at(name) : nullptr;
    }

    domain::Stop* TransportCatalogue::SearchStopFree(const std::string_view name) const {
        return StopNameToStop.count(name) ? StopNameToStop.at(name) : nullptr;
    }

    domain::Bus* TransportCatalogue::SearchBusFree(const std::string_view name) const {
        return BusNameToBus.count(name) ? BusNameToBus.at(name) : nullptr;
    }

    const std::set<domain::Bus*, domain::Bus::cmp_ptr> TransportCatalogue::GetStopRoutes(std::string_view name) const {
        return StopNameToStop.at(name)->NameBuses;
    }

    void TransportCatalogue::SetDistanceBetweenStops(const domain::Stop* from, const domain::Stop* to, int distance) {
        domain::Stop* stop_from = SearchStopFree(from->NameStop);
        stop_from->Distance[to] = distance;
    }

    int TransportCatalogue::GetDistanceBetweenStops(const domain::Stop* from, const domain::Stop* to) const {
        if (from->Distance.count(to)) return from->Distance.at(to);
        else if (to->Distance.count(from)) return to->Distance.at(from);
        else return 0;
    }

    const std::map<std::string_view, domain::Bus*>& TransportCatalogue::GetBusNameToBus() const {
        return BusNameToBus;
    }

    const std::map<std::string_view, domain::Stop*>& TransportCatalogue::GetStopNameToStop() const {
        return StopNameToStop;
    }

    void TransportCatalogue::SetFinalStop(const domain::Bus* from, const domain::Stop* stop_) {
        domain::Stop* stop = SearchStopFree(stop_->NameStop);
        BusNameToBus.at(from->NameBus)->FinalStop = stop;
    }
}