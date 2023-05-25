#pragma once

#include "geo.h"
#include "graph.h"

#include <string>
#include <unordered_map>
#include <vector>
#include <set>
#include <string_view>


namespace domain {

	struct Stop;

	struct Bus {

		Bus(std::string_view name, const std::vector<Stop*>& stops, bool is_circle);

		std::string NameBus;
		std::vector<Stop*> Stops;
		bool CircularRoute;

		struct cmp_ptr
		{
			bool operator()(const Bus* lhs, const Bus* rhs) const
			{
				return lhs->NameBus < rhs->NameBus;
			}
		};

		Stop* FinalStop = nullptr;
	};

	struct Stop {

		Stop(std::string_view name, const geo::Coordinates& coordinates);

		std::string NameStop;
		std::set<Bus*, Bus::cmp_ptr> NameBuses;
		std::unordered_map<const Stop*, int> Distance;
		geo::Coordinates Coordinates;
	};

}