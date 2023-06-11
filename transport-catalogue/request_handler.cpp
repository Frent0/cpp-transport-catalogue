
#include "request_handler.h"

#include <utility>
#include <sstream>
#include <unordered_set>

using namespace std;

namespace transport {

    RequestHandler::RequestHandler(const transport::TransportCatalogue& catalogue,
        const renderer::MapRenderer& renderer, const transport::Router& router)
        : catalogue_(catalogue)
        , renderer_(renderer)
        , router_(router){
    }

    void RequestHandler::JsonStatRequests(const json::Node& json_input, ostream& output) {
        const json::Array& array = json_input.AsArray();
        json::Array output_array;
        output_array.reserve(array.size());
        for (auto& request_node : array) {
            const json::Dict& request_map = request_node.AsDict();
            const string& type = request_map.at("type"s).AsString();
            if (type == "Stop"s) {
                output_array.push_back(FindStopRequest(request_map));
                continue;
            }
            if (type == "Bus"s) {
                output_array.push_back(FindBusRequest(request_map));
                continue;
            }
            if (type == "Map"s) {
                output_array.push_back(BuildMapRequest(request_map));
                continue;
            }
            if (type == "Route"s) {
                output_array.push_back(BuildRouteRequest(request_map));
                continue;
            }
        }

        json::Print(json::Document(json::Node(move(output_array))), output);
    }

    json::Node RequestHandler::FindStopRequest(const json::Dict& request_map) {
        int id = request_map.at("id").AsInt();
        const string& name = request_map.at("name").AsString();
        if (const domain::Stop* stop = catalogue_.SearchStop(name)) {
            json::Array buses_array;
            const auto& buses_on_stop = catalogue_.GetStopRoutes(stop->NameStop);
            buses_array.reserve(buses_on_stop.size());
            for (const auto& value : buses_on_stop) {
                buses_array.push_back(value->NameBus);
            }
            return json::Builder{}.StartDict()
                .Key("buses"s).Value(buses_array)
                .Key("request_id"s).Value(id)
                .EndDict().Build();
        }
        return json::Builder{}.StartDict()
            .Key("error_message"s).Value("not found"s)
            .Key("request_id"s).Value(id)
            .EndDict().Build();

    }

    json::Node RequestHandler::FindBusRequest(const json::Dict& request_map) {
        int id = request_map.at("id").AsInt();
        const string& name = request_map.at("name").AsString();
        if (const domain::Bus* bus = catalogue_.SearchRoute(name)) {
            int stops_count = bus->Stops.size();
            int distance = 0;
            double straight_distance = 0.0;
            for (int i = 1; i < stops_count; ++i) {
                distance += catalogue_.GetDistanceBetweenStops(bus->Stops[i - 1], bus->Stops[i]);
                straight_distance += geo::ComputeDistance(bus->Stops[i - 1]->Coordinates, bus->Stops[i]->Coordinates);
            }
            double curvature = distance / straight_distance;
            unordered_set<domain::Stop*> unique_stops_set{ bus->Stops.begin(),bus->Stops.end() };
            int unique_stops = unique_stops_set.size();
            return json::Builder{}.StartDict()
                .Key("route_length"s).Value(distance)
                .Key("unique_stop_count"s).Value(unique_stops)
                .Key("stop_count"s).Value(stops_count)
                .Key("curvature"s).Value(curvature)
                .Key("request_id").Value(id)
                .EndDict().Build();
        }
        return json::Builder{}.StartDict()
            .Key("error_message"s).Value("not found"s)
            .Key("request_id"s).Value(id)
            .EndDict().Build();
    }

    json::Node RequestHandler::BuildMapRequest(const json::Dict& request_map) {
        int id = request_map.at("id"s).AsInt();
        svg::Document map = renderer_.GetSvgDocument(catalogue_.GetBusNameToBus());
        ostringstream strm;
        map.Render(strm);
        return  json::Builder{}.StartDict()
            .Key("map"s).Value(strm.str())
            .Key("request_id"s).Value(id)
            .EndDict().Build();
    }

    json::Node RequestHandler::BuildRouteRequest(const json::Dict& request_map) {
        int id = request_map.at("id"s).AsInt();
        const string& name_from = request_map.at("from"s).AsString();
        const string& name_to = request_map.at("to"s).AsString();

        if (const domain::Stop* stop_from = catalogue_.SearchStop(name_from)) {
            if (const domain::Stop * stop_to = catalogue_.SearchStop(name_to)) {
                if (auto info = router_.GetRouteInfo(stop_from, stop_to)) {
                    auto [wieght, edges] = info.value();
                    std::vector<std::variant<transport::Router::StopItems, transport::Router::BusItems>> gap_result = router_.GetEdgesItems(edges);
                    json::Array value;

                    for (std::variant<transport::Router::StopItems, transport::Router::BusItems> type : gap_result) {
                        if (std::holds_alternative<transport::Router::StopItems>(type)) {
                            value.emplace_back(json::Node(json::Dict{
                                {{"stop_name"s},{std::get<transport::Router::StopItems>(type).stop_name}},
                                {{"time"s},{std::get<transport::Router::StopItems>(type).time}},
                                {{"type"s},{"Wait"}}
                                }));
                        }
                        else {
                            value.emplace_back(json::Node(json::Dict{
                                {{"bus"s},{std::get<transport::Router::BusItems>(type).bus}},
                                {{"span_count"s},{std::get<transport::Router::BusItems>(type).span_count}},
                                {{"time"s},{std::get<transport::Router::BusItems>(type).time}},
                                {{"type"s},{"Bus"}}
                                }));
                        }
                    }
                    return json::Node(json::Dict{
                        {{"items"s},{value}},
                        {{"total_time"s},{wieght}},
                        {{"request_id"s},{id}}
                        });
                }
            }
        }
        return json::Builder{}.StartDict()
            .Key("error_message"s).Value("not found"s)
            .Key("request_id"s).Value(id)
            .EndDict().Build();
    }
}