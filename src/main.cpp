///////////////////////////////////////////////////////////////////////////////"
//
// Copyright PHOENIX CONTACT Deutschland GmbH
//
// SPDX-License-Identifier: MIT
//
////////////////////////////////////////////////////////////////////////////////

#include <cstdlib>
#include <iomanip>
#include <iostream>
#include <memory>
#include <chrono>
#include <thread>

#include <grpcpp/grpcpp.h>
#include "Plc/Gds/IDataAccessService.grpc.pb.h"
#include "Device/Interface/IDeviceInfoService.grpc.pb.h"
#include "ISubscriptionExample.hpp"

namespace example
{

int IDeviceInfoService_GetItems(std::unique_ptr<Arp::Device::Interface::Services::Grpc::IDeviceInfoService::Stub>& serviceStub)
{
    using namespace Arp::Device::Interface::Services;
    using namespace std::chrono;

    auto ctx = grpc::ClientContext();
    auto request = Grpc::IDeviceInfoServiceGetItemsRequest();
    // See https://www.plcnext.help/te/Service_Components/Remote_Service_Calls_RSC/RSC_device_interface_services.htm#IDeviceInfoService
    // for valid identifiers.
    request.add_identifiers("General.ArticleName");
    request.add_identifiers("General.SerialNumber");
    request.add_identifiers("General.Firmware.Version");
    auto response = Grpc::IDeviceInfoServiceGetItemsResponse();

    auto start = high_resolution_clock::now();
    auto status = serviceStub->GetItems(&ctx, request, &response);
    auto stop = high_resolution_clock::now();
    auto duration = duration_cast<milliseconds>(stop - start);

    if (!status.ok())
    {
        std::cout << "IDeviceInfoService GetItems call failed: " << status.error_code() << '\n';
        return EXIT_FAILURE;
    }
    else
    {
        std::cout << "IDeviceInfoService GetItems (" << duration.count() << "ms)" << '\n';
        std::cout << "  'General.ArticleName': " << std::quoted(response._returnvalue(0).stringvalue()) << '\n';
        std::cout << "  'General.SerialNumber': " << std::quoted(response._returnvalue(1).stringvalue()) << '\n';
        std::cout << "  'General.Firmware.Version': " << std::quoted(response._returnvalue(2).stringvalue()) << '\n';
    }
    return EXIT_SUCCESS;
}

int IDataAccessService_Read(std::unique_ptr<Arp::Plc::Gds::Services::Grpc::IDataAccessService::Stub>& serviceStub, const std::string& portname)
{
    using namespace Arp::Plc::Gds::Services;
    using namespace std::chrono;

    if (portname.empty())
    {
        std::cout << "IDataAccessService Read call failed: no port name given." << '\n';
        return EXIT_FAILURE;
    }

    auto ctx = grpc::ClientContext();
    auto request = Grpc::IDataAccessServiceReadRequest();
    // See https://www.plcnext.help/te/Service_Components/Remote_Service_Calls_RSC/RSC_GDS_services.htm#IDataAccessService
    request.add_portnames(portname);
    auto response = Grpc::IDataAccessServiceReadResponse();

    auto start = high_resolution_clock::now();
    auto status = serviceStub->Read(&ctx, request, &response);
    auto stop = high_resolution_clock::now();
    auto duration = duration_cast<milliseconds>(stop - start);

    if (!status.ok())
    {
        std::cout << "IDataAccessService Read call failed: " << status.error_code() << '\n';
        return EXIT_FAILURE;
    }
    else
    {
        std::cout << "IDataAccessService Read (" << duration.count() << "ms)" << '\n';
        std::cout << "  " << std::quoted(portname, '\'') << " value: " << response._returnvalue(0).value().uint16value() << " returned: " << response._returnvalue(0).error() << '\n';
    }
    return EXIT_SUCCESS;
}

int IDataAccessService_ReadSingle(std::unique_ptr<Arp::Plc::Gds::Services::Grpc::IDataAccessService::Stub>& serviceStub, const std::string& portname)
{
    using namespace Arp::Plc::Gds::Services;
    using namespace std::chrono;

    if (portname.empty())
    {
        std::cout << "IDataAccessService ReadSingle call failed: no port name given." << '\n';
        return EXIT_FAILURE;
    }

    auto ctx = grpc::ClientContext();
    auto request = Grpc::IDataAccessServiceReadSingleRequest();
    // See https://www.plcnext.help/te/Service_Components/Remote_Service_Calls_RSC/RSC_GDS_services.htm#IDataAccessService
    request.set_portname(portname);
    auto response = Grpc::IDataAccessServiceReadSingleResponse();

    auto start = high_resolution_clock::now();
    auto status = serviceStub->ReadSingle(&ctx, request, &response);
    auto stop = high_resolution_clock::now();
    auto duration = duration_cast<milliseconds>(stop - start);

    if (!status.ok())
    {
        std::cout << "IDataAccessService ReadSingle call failed: " << status.error_code() << '\n';
        return EXIT_FAILURE;
    }
    else
    {
        std::cout << "IDataAccessService ReadSingle (" << duration.count() << "ms)" << '\n';
        std::cout << "  " << std::quoted(portname, '\'') << " value: " << response._returnvalue().value().uint16value() << " returned: " << response._returnvalue().error() << '\n';
    }
    return EXIT_SUCCESS;
}

int IDataAccessService_Write(std::unique_ptr<Arp::Plc::Gds::Services::Grpc::IDataAccessService::Stub>& serviceStub, std::uint16_t v, const std::string& portname)
{
    using namespace Arp::Plc::Gds::Services;
    using namespace std::chrono;

    if (portname.empty())
    {
        std::cout << "IDataAccessService Write call failed: no port name given." << '\n';
        return EXIT_FAILURE;
    }

    auto ctx = grpc::ClientContext();
    auto request = Grpc::IDataAccessServiceWriteRequest();
    // See https://www.plcnext.help/te/Service_Components/Remote_Service_Calls_RSC/RSC_GDS_services.htm#IDataAccessService
    auto item = request.add_data();
    item->set_portname(portname);
    item->mutable_value()->set_uint16value(v);
    item->mutable_value()->set_typecode(Arp::Type::Grpc::CoreType::CT_Uint16);

    auto response = Grpc::IDataAccessServiceWriteResponse();

    auto start = high_resolution_clock::now();
    auto status = serviceStub->Write(&ctx, request, &response);
    auto stop = high_resolution_clock::now();
    auto duration = duration_cast<milliseconds>(stop - start);

    if (!status.ok())
    {
        std::cout << "IDataAccessService Write call failed: " << status.error_code() << '\n';
        return EXIT_FAILURE;
    }
    else
    {
        std::cout << "IDataAccessService Write (" << duration.count() << "ms)" << '\n';
        std::cout << "  " << std::quoted(portname, '\'') << " value: '" << v << "' returned: " << response._returnvalue(0) << '\n';
    }
    return EXIT_SUCCESS;
}

} // end namespace example

int main(int argc, char *argv[])
{
    std::string portname = "Arp.Plc.Eclr/wDO16";

    if (argc > 1)
    {
        std::string pn(argv[1]);
        if (pn.find("/") != std::string::npos)
        {
            portname = pn;
        }
    }

    std::cout << "Running the plcnext-grpc-example..." << '\n';
    std::cout << "Using the uint16 port: " << std::quoted(portname, '\'') << "." << '\n';

    std::shared_ptr<grpc::Channel> channel = grpc::CreateChannel(
        "unix:/run/plcnext/grpc.sock",
        grpc::InsecureChannelCredentials());

    auto dataAccessService = Arp::Plc::Gds::Services::Grpc::IDataAccessService();
    auto deviceInfoService = Arp::Device::Interface::Services::Grpc::IDeviceInfoService();
    auto subscriptionService = Arp::Plc::Gds::Services::Grpc::ISubscriptionService();

    auto dataAccessStub = dataAccessService.NewStub(channel);
    auto deviceInfoStub = deviceInfoService.NewStub(channel);
    auto subscriptionStub = subscriptionService.NewStub(channel);

    example::IDeviceInfoService_GetItems(deviceInfoStub);
    example::IDataAccessService_Read(dataAccessStub, portname);
    example::IDataAccessService_Write(dataAccessStub, 1, portname);
    example::IDataAccessService_ReadSingle(dataAccessStub, portname);
    example::IDataAccessService_Write(dataAccessStub, 0, portname);
    example::IDataAccessService_ReadSingle(dataAccessStub, portname);

    auto subscription = example::ISubscriptionServiceExample(std::move(subscriptionStub), portname);
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    subscription.readRecords();

    return EXIT_SUCCESS;
}