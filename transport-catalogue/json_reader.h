#pragma once

#include "map_renderer.h"
#include "transport_catalogue.h"

#include <unordered_map>
#include <string>
#include <string_view>
#include <vector>

class JsonReader {
public:
    JsonReader(json::Document input_json);

    const json::Node& GetBaseRequest() const;
    const json::Node& GetStatRequest() const;
    const json::Node& GetRenderSettings() const;

    void FillCatalogue(transport::TransportCatalogue& catalogue) const;
    const renderer::RendererInfo GetFillRenderer() const;

private:
    json::Document input_json_;
    json::Node null_{ nullptr };

    struct Gap_Bus_info {
        std::vector<std::string_view> stops;
        std::string_view final_stop;
        bool is_circle;
    };

    using StopsDistanceMap = std::unordered_map<std::string_view, std::unordered_map<std::string_view, int>>;
    using BusesInformationMap = std::unordered_map<std::string_view, Gap_Bus_info>;

    void ParseStopAddRequest(transport::TransportCatalogue& catalogue, const json::Dict& request_map,StopsDistanceMap& stop_to_stops_distance) const;
    void SetStopsDistances(transport::TransportCatalogue& catalogue,const StopsDistanceMap& stop_to_stops_distance) const;
    void ParseBusAddRequest(const json::Dict& request_map, BusesInformationMap& buses_info) const;
    void BusesAddProcess(transport::TransportCatalogue& catalogue, const BusesInformationMap& buses_info) const;
    void SetFinals(transport::TransportCatalogue& catalogue, const BusesInformationMap& buses_info) const;
};