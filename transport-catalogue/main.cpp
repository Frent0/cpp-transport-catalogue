#include "json_reader.h"
#include "transport_router.h"
#include "request_handler.h"

int main() {

    json::Document document = json::Load(std::cin);

    transport::JsonReader input_json(document);
    transport::TransportCatalogue catalogue;
    transport::renderer::MapRenderer render(input_json.GetFillRenderer());

    const auto& values_ = document.GetRoot().AsDict();

    for (const auto& value_ : values_) {
        if (value_.first == "base_requests") {
            input_json.FillCatalogue(catalogue);
        }
        if (value_.first == "stat_requests") {
            transport::Router router(input_json.GetRoutingSettings(), catalogue);
            transport::RequestHandler out(catalogue, render, router);
            out.JsonStatRequests(input_json.GetStatRequest(), std::cout);
        }

    }

}