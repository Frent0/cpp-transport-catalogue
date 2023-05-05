#pragma once

#include "transport_catalogue.h"
#include "map_renderer.h"

#include <utility>
#include <string>
#include <string_view>

class RequestHandler {
public:
    RequestHandler(const transport::TransportCatalogue& catalogue, const renderer::MapRenderer& renderer);

    void JsonStatRequests(const json::Node& json_doc, std::ostream& output);

private:
    const transport::TransportCatalogue& catalogue_;
    const renderer::MapRenderer& renderer_;

    json::Node FindStopRequest(const json::Dict& request_map);
    json::Node FindBusRequest(const json::Dict& request_map);
    json::Node BuildMapRequest(const json::Dict& request_map);
};