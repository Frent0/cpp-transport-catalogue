#include "map_renderer.h"

#include <vector>

namespace renderer {

    MapRenderer::MapRenderer(const renderer::RendererInfo& renderer):renderer_(renderer){
    }

    std::vector<svg::Polyline> MapRenderer::GetBusLines(const std::map<std::string_view,domain::Bus*>& buses, const SphereProjector& sp) const {
        std::vector<svg::Polyline> result;
        unsigned color_num = 0;
        for (auto& [bus_name, bus_ptr] : buses) {
            if (bus_ptr->Stops.size() == 0) continue;
            svg::Polyline line;
            std::vector<geo::Coordinates> points;
            for (auto stop : bus_ptr->Stops) {
                line.AddPoint(sp(stop->Coordinates));
            }
            line.SetFillColor("none");
            line.SetStrokeColor(renderer_.color_palette_[color_num]);
            line.SetStrokeWidth(renderer_.line_width_);
            line.SetStrokeLineCap(svg::StrokeLineCap::ROUND);
            line.SetStrokeLineJoin(svg::StrokeLineJoin::ROUND);
            if (color_num < (renderer_.color_palette_.size() - 1)) {
                ++color_num;
            }
            else color_num = 0;
            result.push_back(line);
        }
        return result;

    }

    std::vector<svg::Text> MapRenderer::GetBusLabels(const std::map<std::string_view, domain::Bus*>& buses,const SphereProjector& sp) const {
        std::vector<svg::Text> result;
        unsigned color_num = 0;
        for (auto& [bus_name, bus_ptr] : buses) {
            if (bus_ptr->Stops.size() == 0) continue;
            svg::Text text_underlayer;
            svg::Text text;
            text_underlayer.SetData(bus_ptr->NameBus);
            text.SetData(bus_ptr->NameBus);
            text.SetFillColor(renderer_.color_palette_[color_num]);
            if (color_num < (renderer_.color_palette_.size() - 1)) {
                ++color_num;
            }
            else color_num = 0;
            text_underlayer.SetFillColor(renderer_.underlayer_color_);
            text_underlayer.SetStrokeColor(renderer_.underlayer_color_);
            text.SetFontFamily("Verdana");
            text_underlayer.SetFontFamily("Verdana");
            text.SetFontSize(renderer_.bus_label_font_size_);
            text_underlayer.SetFontSize(renderer_.bus_label_font_size_);
            text.SetFontWeight("bold");
            text_underlayer.SetFontWeight("bold");
            text.SetOffset(renderer_.bus_label_offset_);
            text_underlayer.SetOffset(renderer_.bus_label_offset_);
            text_underlayer.SetStrokeWidth(renderer_.underlayer_width_);
            text_underlayer.SetStrokeLineCap(svg::StrokeLineCap::ROUND);
            text_underlayer.SetStrokeLineJoin(svg::StrokeLineJoin::ROUND);
            text.SetPosition(sp(bus_ptr->Stops[0]->Coordinates));
            text_underlayer.SetPosition(sp(bus_ptr->Stops[0]->Coordinates));
            result.push_back(text_underlayer);
            result.push_back(text);
            if ((!bus_ptr->CircularRoute) && (bus_ptr->final_stop) && (bus_ptr->final_stop->NameStop != bus_ptr->Stops[0]->NameStop)) {
                svg::Text text2 = text;
                svg::Text text2_underlayer = text_underlayer;
                text2.SetPosition(sp(bus_ptr->final_stop->Coordinates));
                text2_underlayer.SetPosition(sp(bus_ptr->final_stop->Coordinates));
                result.push_back(text2_underlayer);
                result.push_back(text2);
            }
        }
        return result;
    }

    std::vector<svg::Text> MapRenderer::GetStopLabels(const std::map<std::string_view, domain::Stop*>& stops, const SphereProjector& sp) const {
        std::vector<svg::Text> result;
        for (auto& [stop_name, stop_ptr] : stops) {
            svg::Text text, text_underlayer;
            text.SetPosition(sp(stop_ptr->Coordinates));
            text.SetOffset(renderer_.stop_label_offset_);
            text.SetFontSize(renderer_.stop_label_font_size_);
            text.SetFontFamily("Verdana");
            text.SetData(stop_ptr->NameStop);
            text.SetFillColor("black");
            text_underlayer.SetPosition(sp(stop_ptr->Coordinates));
            text_underlayer.SetOffset(renderer_.stop_label_offset_);
            text_underlayer.SetFontSize(renderer_.stop_label_font_size_);
            text_underlayer.SetFontFamily("Verdana");
            text_underlayer.SetData(stop_ptr->NameStop);
            text_underlayer.SetFillColor(renderer_.underlayer_color_);
            text_underlayer.SetStrokeColor(renderer_.underlayer_color_);
            text_underlayer.SetStrokeWidth(renderer_.underlayer_width_);
            text_underlayer.SetStrokeLineCap(svg::StrokeLineCap::ROUND);
            text_underlayer.SetStrokeLineJoin(svg::StrokeLineJoin::ROUND);
            result.push_back(text_underlayer);
            result.push_back(text);
        }
        return result;
    }

    std::vector<svg::Circle> MapRenderer::GetStopCircles(const std::map<std::string_view, domain::Stop*>& stops,const SphereProjector& sp) const {
        std::vector<svg::Circle> result;
        for (auto& [stop_name, stop_ptr] : stops) {
            svg::Circle circle;
            circle.SetCenter(sp(stop_ptr->Coordinates));
            circle.SetRadius(renderer_.stop_radius_);
            circle.SetFillColor("white");
            result.push_back(circle);
        }
        return result;
    }


    svg::Document MapRenderer::GetSvgDocument(const std::map<std::string_view, domain::Bus*>& buses) const {
        std::map<std::string_view, domain::Stop*> all_stops;
        std::vector<geo::Coordinates> all_coords;
        svg::Document result;
        for (const auto& [bus_name, bus_ptr] : buses) {
            if (bus_ptr->Stops.size() == 0) continue;
            for (const auto& stop : bus_ptr->Stops) {
                all_stops[stop->NameStop] = stop;
                all_coords.push_back(stop->Coordinates);
            }
        }
        SphereProjector sp(all_coords.begin(), all_coords.end(), renderer_.width_, renderer_.height_, renderer_.padding_);
        for (const auto& line : GetBusLines(buses, sp)) {
            result.Add(line);
        }
        for (const auto& text : GetBusLabels(buses, sp)) {
            result.Add(text);
        }
        for (const auto& circle : GetStopCircles(all_stops, sp)) {
            result.Add(circle);
        }
        for (const auto& text : GetStopLabels(all_stops, sp)) {
            result.Add(text);
        }
        return result;
    }

    bool IsZero(double value) {
        return std::abs(value) < EPSILON;
    }

    svg::Point SphereProjector::operator()(geo::Coordinates coords) const {
        return { (coords.lng - min_lon_) * zoom_coeff_ + padding_,
                (max_lat_ - coords.lat) * zoom_coeff_ + padding_ };
    }
}