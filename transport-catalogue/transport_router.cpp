#include "transport_router.h"

#include <string>
#include <string_view>
#include <map>
#include <utility>
#include <vector>
#include <algorithm>

using namespace std;

namespace transport {

    Router::Router(domain::GapRouterInfo settings_node) {
        SetSettings(settings_node);
    }

    Router::Router(domain::GapRouterInfo settings_node, const TransportCatalogue& tcat) {
        SetSettings(settings_node);
        BuildGraph(tcat);
    }

    const graph::DirectedWeightedGraph<double>& Router::BuildGraph(const TransportCatalogue& tcat) {
        const map<string_view, domain::Stop*>& all_stops = tcat.GetStopNameToStop();
        const map<string_view, domain::Bus*>& all_buses = tcat.GetBusNameToBus();
        graph::DirectedWeightedGraph<double> stops_graph(all_stops.size() * 2);
        map<std::string, graph::VertexId> stop_ids;
        graph::VertexId vertex_id = 0;
        for (const auto& [stop_name, stop_ptr] : all_stops) {
            stop_ids[stop_ptr->NameStop] = vertex_id;
            stops_graph.AddEdge({ stop_ptr->NameStop,
                                  0,
                                  vertex_id,
                                  ++vertex_id,
                                  static_cast<double>(BusWaitTime) });
            ++vertex_id;
        }
        StopIds = move(stop_ids);
        for_each(
            all_buses.begin(),
            all_buses.end(),
            [&stops_graph, this, &tcat](const auto& item)
            {
                const auto& bus_ptr = item.second;
        const std::vector<domain::Stop*>& stops = bus_ptr->Stops;
        size_t stops_count = stops.size();
        for (size_t i = 0; i < stops_count; ++i) {
            for (size_t j = i + 1; j < stops_count; ++j) {
                const domain::Stop* stop_from = stops[i];
                const domain::Stop* stop_to = stops[j];
                int dist_sum = 0;
                for (size_t k = i + 1; k <= j; ++k) {
                    dist_sum += tcat.GetDistanceBetweenStops(stops[k - 1], stops[k]);
                }
                stops_graph.AddEdge({ bus_ptr->NameBus,
                                      j - i,
                                      StopIds.at(stop_from->NameStop) + 1,
                                      StopIds.at(stop_to->NameStop),
                                      static_cast<double>(dist_sum) / (BusVelocity * (100.0 / 6.0)) });
                if (!bus_ptr->CircularRoute && stop_to == bus_ptr->FinalStop && j == stops_count / 2) break;
            }
        }
            });

        Graph = move(stops_graph);
        RouterPtr = std::make_unique<graph::Router<double>>(Graph);
        return Graph;
    }

    std::vector<std::variant<transport::Router::StopItems, transport::Router::BusItems>> Router::GetEdgesItems(const std::vector<graph::EdgeId>& edges) const {
        std::vector<std::variant<StopItems, BusItems>> result;
        result.reserve(edges.size());

        for (auto& edge_id : edges) {
            const graph::Edge<double>& edge = Graph.GetEdge(edge_id);
            if (edge.quality == 0) {
                result.emplace_back(
                   StopItems{static_cast<string>(edge.name), edge.weight}
                );
            }
            else {
                result.emplace_back(
                    BusItems{static_cast<string>(edge.name), static_cast<int>(edge.quality), edge.weight}
                );
            }
        }
        return result;
    }



    std::optional<graph::Router<double>::RouteInfo> Router::GetRouteInfo(const domain::Stop* from, const domain::Stop* to) const {
        return RouterPtr->BuildRoute(StopIds.at(from->NameStop), StopIds.at(to->NameStop));
    }

    void Router::SetSettings(domain::GapRouterInfo settings_node){
        if (settings_node.IsNull) return;
        BusWaitTime = settings_node.BusWaitTime;
        BusVelocity = settings_node.BusVelocity;
    }

} 