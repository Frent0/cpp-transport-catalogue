#pragma once

#include "geo.h"
#include "domain.h"

#include <iostream>
#include <deque>
#include <set>
#include <vector>
#include <string_view>
#include <string>
#include <map>
#include <stdexcept>
#include <unordered_set>

namespace transport {

	class TransportCatalogue {
	public:

		void AddBus(std::string_view num, std::vector<const domain::Stop*>& stops, bool is_circle);
		void AddStop(std::string_view name, const geo::Coordinates& coordinates);

		const domain::Bus* SearchRoute(std::string_view name) const;
		const domain::Stop* SearchStop(std::string_view name) const;

		const std::map<std::string_view, domain::Bus*>& GetBusNameToBus() const;
		const std::map<std::string_view, domain::Stop*>& GetStopNameToStop() const;

		const std::set<domain::Bus*, domain::Bus::cmp_ptr> GetStopRoutes(std::string_view name) const;
		void SetFinalStop(const domain::Bus* from, const domain::Stop* stop);
		void SetDistanceBetweenStops(const domain::Stop* from, const domain::Stop* to, int distance);
		int GetDistanceBetweenStops(const domain::Stop* from, const domain::Stop* to) const;

	private:

		domain::Stop* SearchStopFree(const std::string_view name) const;
		domain::Bus* SearchBusFree(const std::string_view name) const;

		std::map<std::string_view, domain::Bus*> BusNameToBus;
		std::map<std::string_view, domain::Stop*> StopNameToStop;

		std::deque<domain::Stop> AllStops;
		std::deque<domain::Bus> AllBuses;

	};

}