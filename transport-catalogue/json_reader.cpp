#include "json_reader.h"

using namespace std;

namespace transport {

    JsonReader::JsonReader(json::Document input_json)
        : input_json_(input_json) {}

    const json::Node& JsonReader::GetBaseRequest() const {
        if (input_json_.GetRoot().AsDict().count("base_requests"s))
            return input_json_.GetRoot().AsDict().at("base_requests"s);
        else return null_;
    }

    const json::Node& JsonReader::GetStatRequest() const {
        if (input_json_.GetRoot().AsDict().count("stat_requests"s))
            return input_json_.GetRoot().AsDict().at("stat_requests"s);
        else return null_;
    }

    const json::Node& JsonReader::GetRenderSettings() const {
        if (input_json_.GetRoot().AsDict().count("render_settings"s))
            return input_json_.GetRoot().AsDict().at("render_settings"s);
        else return null_;
    }

    const json::Node& JsonReader::GetRoutingSettings() const {
        if (input_json_.GetRoot().AsDict().count("routing_settings"s))
            return input_json_.GetRoot().AsDict().at("routing_settings"s);
        else return null_;
    }

    const renderer::RendererInfo JsonReader::GetFillRenderer() const {
        json::Node render_settings = GetRenderSettings();
        if (render_settings.IsNull()) return{};

        renderer::RendererInfo result;
        const json::Dict& settings_map = render_settings.AsDict();
        result.Width = settings_map.at("width").AsDouble();
        result.Height = settings_map.at("height").AsDouble();
        result.Padding = settings_map.at("padding").AsDouble();
        result.StopRadius = settings_map.at("stop_radius").AsDouble();
        result.LineWidth = settings_map.at("line_width").AsDouble();
        result.BusLlabelFontSize = settings_map.at("bus_label_font_size").AsInt();
        const json::Array& bus_label_offset = settings_map.at("bus_label_offset").AsArray();
        result.BusLabelOffset = { bus_label_offset[0].AsDouble(),
                              bus_label_offset[1].AsDouble() };
        result.StopLabelFontSize = settings_map.at("stop_label_font_size").AsInt();
        const json::Array& stop_label_offset = settings_map.at("stop_label_offset").AsArray();
        result.StopLabelOffset = { stop_label_offset[0].AsDouble(),
                               stop_label_offset[1].AsDouble() };
        if (settings_map.at("underlayer_color").IsArray()) {
            const json::Array& arr = settings_map.at("underlayer_color").AsArray();
            if (arr.size() == 3) {
                svg::Rgb rgb_colors(arr[0].AsInt(), arr[1].AsInt(), arr[2].AsInt());
                result.UnderlayerColor = rgb_colors;
            }
            else if (arr.size() == 4) {
                svg::Rgba rgba_colors(arr[0].AsInt(), arr[1].AsInt(), arr[2].AsInt(), arr[3].AsDouble());
                result.UnderlayerColor = rgba_colors;
            }
        }
        else if (settings_map.at("underlayer_color").IsString()) {
            result.UnderlayerColor = settings_map.at("underlayer_color").AsString();
        }
        result.UnderlayerWidth = settings_map.at("underlayer_width").AsDouble();
        const json::Array& color_palette = settings_map.at("color_palette").AsArray();
        for (const json::Node& node : color_palette) {
            if (node.IsArray()) {
                const json::Array& arr = node.AsArray();
                if (arr.size() == 3) {
                    svg::Rgb rgb_colors(arr[0].AsInt(), arr[1].AsInt(), arr[2].AsInt());
                    result.ColorPalette.push_back(rgb_colors);
                }
                else if (arr.size() == 4) {
                    svg::Rgba rgba_colors(arr[0].AsInt(), arr[1].AsInt(), arr[2].AsInt(), arr[3].AsDouble());
                    result.ColorPalette.push_back(rgba_colors);
                }
            }
            else if (node.IsString()) {
                result.ColorPalette.push_back(node.AsString());
            }
        }
        return result;
    }

    void JsonReader::FillCatalogue(transport::TransportCatalogue& catalogue) const {
        const json::Array& arr = GetBaseRequest().AsArray();
        StopsDistanceMap stop_to_stops_distance;
        BusesInformationMap buses_info;
        for (const auto& request_node : arr) {
            const json::Dict& request_map = request_node.AsDict();
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

    void JsonReader::ParseStopAddRequest(transport::TransportCatalogue& catalogue, const json::Dict& request_map, StopsDistanceMap& stop_to_stops_distance) const {
        const string& stop_name = request_map.at("name"s).AsString();
        catalogue.AddStop(stop_name, { request_map.at("latitude"s).AsDouble(),request_map.at("longitude"s).AsDouble() });
        const json::Dict& near_stops = request_map.at("road_distances"s).AsDict();
        for (const auto& [key_stop_name, dist_node] : near_stops) {
            stop_to_stops_distance[stop_name][key_stop_name] = dist_node.AsInt();
        }
    }

    void JsonReader::SetStopsDistances(transport::TransportCatalogue& catalogue, const StopsDistanceMap& stop_to_stops_distance) const {
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
        buses_info[bus_name].IsCircle = is_roundtrip;
        auto& stops = buses_info[bus_name].Stops;
        if (stops_count > 0) stops.reserve(is_roundtrip ? stops_count : stops_count * 2);
        for (size_t i = 0; i < bus_stops.size(); ++i) {
            stops.push_back(bus_stops[i].AsString());
            if (i == bus_stops.size() - 1) {
                if (!is_roundtrip) {
                    buses_info[bus_name].FinalStop = bus_stops[i].AsString();
                    for (int j = stops.size() - 2; j >= 0; --j) {
                        stops.push_back(stops[j]);
                    }
                }
                else {
                    buses_info[bus_name].FinalStop = bus_stops[0].AsString();
                }
            }
        }
    }

    void JsonReader::BusesAddProcess(transport::TransportCatalogue& catalogue, const BusesInformationMap& buses_info) const {
        for (const auto& [name, info] : buses_info) {
            vector<const domain::Stop*> stop_ptrs;
            const auto& stops = info.Stops;
            stop_ptrs.reserve(stops.size());
            for (const auto& stop : stops) {
                stop_ptrs.push_back(catalogue.SearchStop(stop));
            }
            catalogue.AddBus(name, stop_ptrs, info.IsCircle);
        }
    }

    void JsonReader::SetFinals(transport::TransportCatalogue& catalogue, const BusesInformationMap& buses_info) const {
        for (auto& [bus_name, info] : buses_info) {
            if (const domain::Bus* bus = catalogue.SearchRoute(bus_name)) {
                if (const domain::Stop* stop = catalogue.SearchStop(info.FinalStop)) {
                    catalogue.SetFinalStop(bus, stop);
                }
            }
        }
    }
}