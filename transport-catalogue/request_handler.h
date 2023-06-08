#pragma once

#include "transport_catalogue.h"
#include "transport_router.h"
#include "map_renderer.h"
#include "json_builder.h"

#include <utility>
#include <string>
#include <string_view>

namespace transport {

    class RequestHandler {
    public:
        RequestHandler(const transport::TransportCatalogue& catalogue,
            const renderer::MapRenderer& renderer, const transport::Router& router);

        void JsonStatRequests(const json::Node& json_doc, std::ostream& output);

    private:
        const transport::TransportCatalogue& catalogue_;
        const renderer::MapRenderer& renderer_;
        const transport::Router& router_;

        json::Node FindStopRequest(const json::Dict& request_map);
        json::Node FindBusRequest(const json::Dict& request_map);
        json::Node BuildMapRequest(const json::Dict& request_map);
        json::Node BuildRouteRequest(const json::Dict& request_map);
    };
}