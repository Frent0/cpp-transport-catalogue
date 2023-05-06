
#include "request_handler.h"

#include <utility>
#include <sstream>
#include <unordered_set>

using namespace std;

namespace transport {

    RequestHandler::RequestHandler(const TransportCatalogue& catalogue, const renderer::MapRenderer& renderer)
        : catalogue_(catalogue)
        , renderer_(renderer) {
    }

    void RequestHandler::JsonStatRequests(const json::Node& json_input, ostream& output) {
        const json::Array& array = json_input.AsArray();
        json::Array output_array;
        output_array.reserve(array.size());
        for (auto& request_node : array) {
            const json::Dict& request_map = request_node.AsMap();
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
            return json::Node(json::Dict{
                    {{"buses"},{move(buses_array)}},
                    {{"request_id"},{id}}
                });
        }
        string error = "not found";
        return json::Node(json::Dict{
                    {{"request_id"},{id}},
                    {{"error_message"},{std::move(error)}}
            });
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
            return json::Node(json::Dict{
                    {{"route_length"},{distance}},
                    {{"unique_stop_count"},{unique_stops}},
                    {{"stop_count"},{stops_count}},
                    {{"curvature"},{curvature}},
                    {{"request_id"},{id}}
                });
        }
        string error = "not found";
        return json::Node(json::Dict{
                    {{"request_id"},{id}},
                    {{"error_message"},std::move(error)}
            });
    }

    json::Node RequestHandler::BuildMapRequest(const json::Dict& request_map) {
        int id = request_map.at("id"s).AsInt();
        svg::Document map = renderer_.GetSvgDocument(catalogue_.GetBusNameToBus());
        ostringstream strm;
        map.Render(strm);
        return json::Node(json::Dict{
                    {{"map"s},{strm.str()}},
                    {{"request_id"s},{id}}
            });
    }
}