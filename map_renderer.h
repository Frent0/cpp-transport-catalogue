#pragma once

#include "geo.h"
#include "svg.h"
#include "json.h"
#include "domain.h"

#include <vector>
#include <string>
#include <string_view>
#include <stdexcept>
#include <algorithm>
#include <map>
#include <optional>

namespace transport {

    namespace renderer {

        class SphereProjector;

        struct RendererInfo {

            RendererInfo() = default;

            double Width = 0;
            double Height = 0;
            double Padding = 0;
            double StopRadius = 0;
            double LineWidth = 0;
            int BusLlabelFontSize = 0;
            svg::Point BusLabelOffset = { 0.0, 0.0 };
            svg::Point StopLabelOffset = { 0.0, 0.0 };
            int StopLabelFontSize = 0;
            svg::Color UnderlayerColor;
            double UnderlayerWidth = 0;
            std::vector<svg::Color> ColorPalette = {};
        };

        class MapRenderer {
        public:
            MapRenderer() = default;

            MapRenderer(const renderer::RendererInfo& renderer);

            std::vector<svg::Polyline> GetBusLines(const std::map<std::string_view, domain::Bus*>& buses, const SphereProjector& sp) const;

            std::vector<svg::Text> GetBusLabels(const std::map<std::string_view, domain::Bus*>& buses, const SphereProjector& sp) const;

            std::vector<svg::Text> GetStopLabels(const std::map<std::string_view, domain::Stop*>& stops, const SphereProjector& sp) const;

            std::vector<svg::Circle> GetStopCircles(const std::map<std::string_view, domain::Stop*>& stops, const SphereProjector& sp) const;

            svg::Document GetSvgDocument(const std::map<std::string_view, domain::Bus*>& buses) const;



        private:
            renderer::RendererInfo renderer_;
        };

        inline const double EPSILON = 1e-6;
        bool IsZero(double value);

        class SphereProjector {
        public:
            template <typename PointInputIt>
            SphereProjector(PointInputIt points_begin, PointInputIt points_end, double max_width,
                double max_height, double padding)
                : padding_(padding) {
                if (points_begin == points_end) {
                    return;
                }

                const auto [left_it, right_it]
                    = std::minmax_element(points_begin, points_end, [](auto lhs, auto rhs) {
                    return lhs.lng < rhs.lng;
                        });
                min_lon_ = left_it->lng;
                const double max_lon = right_it->lng;

                const auto [bottom_it, top_it]
                    = std::minmax_element(points_begin, points_end, [](auto lhs, auto rhs) {
                    return lhs.lat < rhs.lat;
                        });
                const double min_lat = bottom_it->lat;
                max_lat_ = top_it->lat;

                std::optional<double> width_zoom;
                if (!IsZero(max_lon - min_lon_)) {
                    width_zoom = (max_width - 2 * padding) / (max_lon - min_lon_);
                }

                std::optional<double> height_zoom;
                if (!IsZero(max_lat_ - min_lat)) {
                    height_zoom = (max_height - 2 * padding) / (max_lat_ - min_lat);
                }

                if (width_zoom && height_zoom) {
                    zoom_coeff_ = std::min(*width_zoom, *height_zoom);
                }
                else if (width_zoom) {
                    zoom_coeff_ = *width_zoom;
                }
                else if (height_zoom) {
                    zoom_coeff_ = *height_zoom;
                }
            }

            svg::Point operator()(geo::Coordinates coords) const;

        private:
            double padding_;
            double min_lon_ = 0;
            double max_lat_ = 0;
            double zoom_coeff_ = 0;
        };
    }
}