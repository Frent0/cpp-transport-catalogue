#include <fstream>

#include "stat_reader.h"
#include "input_reader.h"

int main() {

    transport::TransportCatalogue catalogue;

    FillIinTheDatabase(catalogue);

    std::vector<std::string> result1{ "0TRtmwrQQe9IyZlDhs2kKnf", "0f6xSnxAsvMBtzIVIFseg9O16" , "2" ,  "ygIMsPew6Y", "62N"  , "6Kwr",  " " ,  " " ,  " " ,  " ", "" , "" ,  " " , " "  , " ",  " " ,  " " ,  " " ,  " " , "" ,  " ", " "  , " ",  " " ,  " " ,  " " ,  " " };

    GetInformation(catalogue);


}