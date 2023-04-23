#include "stat_reader.h"

#include <iomanip>

namespace transport {

    void GetInformation(const TransportCatalogue& catalogue) {
        size_t requests_count;
        std::cin >> requests_count;
        for (size_t i = 0; i < requests_count; ++i) {
            std::string requests_name, line;
            std::cin >> requests_name;
            std::getline(std::cin, line);
            if (requests_name == "Bus") {
                information::PrintRoute(line, catalogue);
            }
            if (requests_name == "Stop") {
                information::PrintStop(line, catalogue);
            }
        }
    }

    namespace information {
        void PrintRoute(std::string& line, const TransportCatalogue& catalogue) {
            std::string route_number = line.substr(1, line.npos);

            if (catalogue.SearchRoute(route_number)) {
                RouteInformation bus = catalogue.GetRouteInformation(route_number);

                std::cout << "Bus " << bus.NameRoute
                    << ": " << bus.StopCount << " stops on route, "
                    << bus.UniqueStopCount << " unique stops, " << std::setprecision(6)
                    << bus.RouteLength << " route length, "
                    << bus.Curvature << " curvature" <<
                    std::endl;

            }
            else {
                std::cout << "Bus " << route_number << ": not found" << std::endl;
            }
        }

        void PrintStop(std::string& line, const TransportCatalogue& catalogue) {
            std::string stop_name = line.substr(1, line.npos);
            if (catalogue.SearchStop(stop_name)) {
                std::cout << "Stop " << stop_name << ": ";
                std::set<std::string> buses = catalogue.GetStopRoutes(stop_name);
                if (!buses.empty()) {
                    std::cout << "buses ";
                    for (const auto& bus : buses) {
                        std::cout << bus << " ";
                    }
                    std::cout << std::endl;
                }
                else {
                    std::cout << "no buses" << std::endl;
                }
            }
            else {
                std::cout << "Stop " << stop_name << ": not found" << std::endl;
            }
        }

    }

}

