#include "stat_reader.h"
#include "input_reader.h"

int main() {
    transport::TransportCatalogue catalogue;
    FillIinTheDatabase(catalogue);
    GetInformation(catalogue);
}