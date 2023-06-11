#pragma once
#include "domain.h"
#include "json_builder.h"
#include "transport_catalogue.h"
#include "graph.h"
#include "router.h"

#include <optional>
#include <string>
#include <memory>
#include <string_view>

namespace transport {

    class Router {
    public:

        struct BusItems {
            std::string bus;
            int span_count;
            double time;
        };

        struct StopItems {
            std::string stop_name;
            double time;
        };

        Router() = default;

        explicit Router(domain::GapRouterInfo settings_node);
        explicit Router(domain::GapRouterInfo settings_node, const TransportCatalogue& tcat);

        const graph::DirectedWeightedGraph<double>& BuildGraph(const TransportCatalogue& tcat);

        std::vector<std::variant<StopItems,BusItems>> GetEdgesItems(const std::vector<graph::EdgeId>& edges) const;

        std::optional<graph::Router<double>::RouteInfo> GetRouteInfo(const domain::Stop* from, const domain::Stop* to) const;

    private:

        int BusWaitTime = 0;
        double BusVelocity = 0;

        graph::DirectedWeightedGraph<double> Graph;
        std::map<std::string, graph::VertexId> StopIds;

        std::unique_ptr<graph::Router<double>> RouterPtr = nullptr;

    private:

        void SetSettings(domain::GapRouterInfo settings_node);
    };

} 