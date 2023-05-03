#include "json_reader.h"

using namespace std;

JsonReader::JsonReader(json::Document input_json)
    : input_json_(input_json) {}

const json::Node& JsonReader::GetBaseRequest() const {
    if (input_json_.GetRoot().AsMap().count("base_requests"s))
        return input_json_.GetRoot().AsMap().at("base_requests"s);
    else return null_;
}

const json::Node& JsonReader::GetStatRequest() const {
    if (input_json_.GetRoot().AsMap().count("stat_requests"s))
        return input_json_.GetRoot().AsMap().at("stat_requests"s);
    else return null_;
}

const json::Node& JsonReader::GetRenderSettings() const {
    if (input_json_.GetRoot().AsMap().count("render_settings"s))
        return input_json_.GetRoot().AsMap().at("render_settings"s);
    else return null_;
}

void JsonReader::FillCatalogue(transport::TransportCatalogue& catalogue) const {
    const json::Array& arr = GetBaseRequest().AsArray();
    StopsDistanceMap stop_to_stops_distance;
    BusesInformationMap buses_info;
    for (const auto& request_node : arr) {
        const json::Dict& request_map = request_node.AsMap();
        const string& type = request_map.at("type"s).AsString();
        if (type == "Stop"s) {
            ParseStopAddRequest(catalogue, request_map, stop_to_stops_distance);
        }
        if (type == "Bus"s) {
            ParseBusAddRequest(request_map, buses_info);
        }
    }
    SetStopsDistances(catalogue, stop_to_stops_distance);
    BusesAddProcess(catalogue, buses_info);
    SetFinals(catalogue, buses_info);
}

void JsonReader::ParseStopAddRequest(transport::TransportCatalogue& catalogue, const json::Dict& request_map,StopsDistanceMap& stop_to_stops_distance) const {
    const string& stop_name = request_map.at("name"s).AsString();
    catalogue.AddStop(stop_name, {request_map.at("latitude"s).AsDouble(),request_map.at("longitude"s).AsDouble() });
    const json::Dict& near_stops = request_map.at("road_distances"s).AsMap();
    for (const auto& [key_stop_name, dist_node] : near_stops) {
        stop_to_stops_distance[stop_name][key_stop_name] = dist_node.AsInt();
    }
}

void JsonReader::SetStopsDistances(transport::TransportCatalogue& catalogue,const StopsDistanceMap& stop_to_stops_distance) const {
    for (const auto& [stop, near_stops] : stop_to_stops_distance) {
        for (const auto& [stop_name, distance] : near_stops) {
            catalogue.SetDistanceBetweenStops(catalogue.SearchStop(stop), catalogue.SearchStop(stop_name), distance);
        }
    }
}

void JsonReader::ParseBusAddRequest(const json::Dict& request_map, BusesInformationMap& buses_info) const {
    const string& bus_name = request_map.at("name"s).AsString();
    const json::Array& bus_stops = request_map.at("stops"s).AsArray();
    size_t stops_count = bus_stops.size();
    bool is_roundtrip = request_map.at("is_roundtrip"s).AsBool();
    buses_info[bus_name].is_circle = is_roundtrip;
    auto& stops = buses_info[bus_name].stops;
    if (stops_count > 0) stops.reserve(is_roundtrip ? stops_count : stops_count * 2);
    for (size_t i = 0; i < bus_stops.size(); ++i) {
        stops.push_back(bus_stops[i].AsString());
        if (i == bus_stops.size() - 1) {
            if (!is_roundtrip) {
                buses_info[bus_name].final_stop = bus_stops[i].AsString();
                for (int j = stops.size() - 2; j >= 0; --j) {
                    stops.push_back(stops[j]);
                }
            }
            else {
                buses_info[bus_name].final_stop = bus_stops[0].AsString();
            }
        }
    }
}

void JsonReader::BusesAddProcess(transport::TransportCatalogue& catalogue, const BusesInformationMap& buses_info) const {
    for (const auto& [name, info] : buses_info) {
        vector<const domain::Stop*> stop_ptrs;
        const auto& stops = info.stops;
        stop_ptrs.reserve(stops.size());
        for (const auto& stop : stops) {
            stop_ptrs.push_back(catalogue.SearchStop(stop));
        }
        catalogue.AddBus(name, stop_ptrs, info.is_circle);
    }
}

void JsonReader::SetFinals(transport::TransportCatalogue& catalogue, const BusesInformationMap& buses_info) const {
    for (auto& [bus_name, info] : buses_info) {
        if (const domain::Bus* bus = catalogue.SearchRoute(bus_name)) {
            if (const domain::Stop* stop = catalogue.SearchStop(info.final_stop)) {
                catalogue.SetFinalStop(bus,stop);
            }
        }
    }
}
