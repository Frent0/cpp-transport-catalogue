#include "json.h"
#include "json_reader.h"
#include "map_renderer.h"
#include "request_handler.h"

int main() {

    json::Document test = json::Load(std::cin);

    JsonReader input_json(test);
    transport::TransportCatalogue catalogue;
    renderer::MapRenderer render(input_json.GetRenderSettings());
    RequestHandler out(catalogue,render);

    const auto& values_ = test.GetRoot().AsMap();
    for (const auto& value_ : values_) {
        if (value_.first == "base_requests") {
            input_json.FillCatalogue(catalogue);
        }
        if (value_.first == "stat_requests") {
            out.JsonStatRequests(input_json.GetStatRequest(), std::cout);
        }
    }
}