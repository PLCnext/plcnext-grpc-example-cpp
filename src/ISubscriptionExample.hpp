#include <memory>
#include "Plc/Gds/ISubscriptionService.grpc.pb.h"

namespace example
{

class ISubscriptionServiceExample
{
public:
    ISubscriptionServiceExample(std::unique_ptr<Arp::Plc::Gds::Services::Grpc::ISubscriptionService::Stub> serviceStub, std::string portname)
    {
        serviceStub_ = std::move(serviceStub);
        
        // See PLCnext Info Center for how to use the RSC Service.
        // https://www.plcnext.help/te/Communication_interfaces/Remote_Service_Calls_RSC/RSC_GDS_services.htm#ISubscriptionService
        createSubscription();
        addVariable(portname);
        subscribe();
    }

    ~ISubscriptionServiceExample()
    {
        unsubscribe();
        deleteSubscription();
    }

public:
    int readRecords()
    {
        if (!this->serviceStub_)
        {
            return EXIT_FAILURE;
        }

        grpc::ClientContext ctx = grpc::ClientContext();
        auto request = Arp::Plc::Gds::Services::Grpc::ISubscriptionServiceReadRecordsRequest();
        request.set_subscriptionid(this->subscriptionId_);
        request.set_count(0); // 0 = all records
        auto response = Arp::Plc::Gds::Services::Grpc::ISubscriptionServiceReadRecordsResponse();
        auto status = serviceStub_->ReadRecords(&ctx, request, &response);
        if (!status.ok())
        {
            std::cout << "ISubscriptionService CreateRecordingSubscription call failed: " << status.error_code() << '\n';
            return EXIT_FAILURE;
        }

        std::cout << "ISubscriptionService ReadRecords returned: " << response._returnvalue() << '\n';

        if(response._returnvalue() != Arp::Plc::Gds::Services::Grpc::DataAccessError::DAE_None)
        {
            return EXIT_FAILURE;
        }

        auto recorded = response.records();
        auto recordedSize = response.records_size();

        for (auto& task : recorded)
        {
            auto& currentTask = task.arrayvalue();
            for (auto& record : currentTask.arrayelements())
            {
                auto& currentRecord = record.arrayvalue();
                // first field timestamp unixtime in microseconds
                auto t = currentRecord.arrayelements()[0].int64value();
                // next values are the subscribed variables of this task
                uint16_t v = 0;
                // Void means no data was recorded yet
                if (currentRecord.arrayelements()[1].typecode() == Arp::Type::Grpc::CoreType::CT_Void)
                {
                    v = 0;
                }
                else
                {
                    v = currentRecord.arrayelements()[1].uint16value();
                }
                std::cout << "[" << std::quoted(this->unixTimeMicrosecondsToString(t)) << ", " << v << "]" << '\n';
            }
        }

        return EXIT_SUCCESS;
    }

private:
    std::string unixTimeMicrosecondsToString(int64_t microsecondsUnixTime)
    {
        // code from stackoverflow https://stackoverflow.com/a/74722570

        int fraction_sec = microsecondsUnixTime % 1000000u;
        time_t seconds = microsecondsUnixTime / 1000000u;

        char timestr_sec[] = "YYYY-MM-DD hh:mm:ss.ssssss";
        std::strftime(timestr_sec, sizeof(timestr_sec) - 1, "%F %T", std::gmtime(&seconds));
        std::ostringstream tout;
        tout << timestr_sec << '.' << std::setfill('0') << std::setw(6) << fraction_sec;    
        std::string timestr_micro = tout.str();

        return timestr_micro;
    }

    int deleteSubscription()
    {
        if (!this->serviceStub_)
        {
            return EXIT_FAILURE;
        }
        if (this->subscriptionId_ < 1)
        {
            return EXIT_SUCCESS;
        }

        grpc::ClientContext ctx = grpc::ClientContext();
        auto request = Arp::Plc::Gds::Services::Grpc::ISubscriptionServiceDeleteSubscriptionRequest();
        request.set_subscriptionid(this->subscriptionId_);
        auto response = Arp::Plc::Gds::Services::Grpc::ISubscriptionServiceDeleteSubscriptionResponse();
        auto status = serviceStub_->DeleteSubscription(&ctx, request, &response);
        if (!status.ok())
        {
            std::cout << "ISubscriptionService DeleteSubscription call failed: " << status.error_code() << '\n';
            return EXIT_FAILURE;
        }
        this->subscriptionId_ = 0;
        std::cout << "ISubscriptionService DeleteSubscription returned: " << response._returnvalue() << '\n';
        return EXIT_SUCCESS;
    }

    int unsubscribe()
    {
        if (!this->serviceStub_)
        {
            return EXIT_FAILURE;
        }
        if (this->subscriptionId_ < 1)
        {
            return EXIT_SUCCESS;
        }

        grpc::ClientContext ctx = grpc::ClientContext();
        auto request = Arp::Plc::Gds::Services::Grpc::ISubscriptionServiceUnsubscribeRequest();
        request.set_subscriptionid(this->subscriptionId_);
        auto response = Arp::Plc::Gds::Services::Grpc::ISubscriptionServiceUnsubscribeResponse();
        auto status = serviceStub_->Unsubscribe(&ctx, request, &response);
        if (!status.ok())
        {
            std::cout << "ISubscriptionService Unsubscribe call failed: " << status.error_code() << '\n';
            return EXIT_FAILURE;
        }
        std::cout << "ISubscriptionService Unsubscribe returned: " << response._returnvalue() << '\n';
        return EXIT_SUCCESS;
    }

    int subscribe()
    {
        if (!this->serviceStub_)
        {
            return EXIT_FAILURE;
        }
        // call Subscribe to start the data sampling
        grpc::ClientContext ctx = grpc::ClientContext();
        auto request = Arp::Plc::Gds::Services::Grpc::ISubscriptionServiceSubscribeRequest();
        request.set_subscriptionid(this->subscriptionId_);
        request.set_samplerate(0);
        auto response = Arp::Plc::Gds::Services::Grpc::ISubscriptionServiceSubscribeResponse();
        auto status = serviceStub_->Subscribe(&ctx, request, &response);
        if (!status.ok())
        {
            std::cout << "ISubscriptionService Subscribe call failed: " << status.error_code() << '\n';
            return EXIT_FAILURE;
        }
        std::cout << "ISubscriptionService Subscribe returned: " << response._returnvalue() << '\n';
        if(response._returnvalue() != Arp::Plc::Gds::Services::Grpc::DataAccessError::DAE_None)
        {
            return EXIT_FAILURE;
        }
        return EXIT_SUCCESS;
    }

    int createSubscription()
    {
        if (!this->serviceStub_)
        {
            return EXIT_FAILURE;
        }
        // create the recording subscription with a capacity of 30 records
        grpc::ClientContext ctx = grpc::ClientContext();
        auto request = Arp::Plc::Gds::Services::Grpc::ISubscriptionServiceCreateRecordingSubscriptionRequest();
        request.set_recordcount(30);
        auto response = Arp::Plc::Gds::Services::Grpc::ISubscriptionServiceCreateRecordingSubscriptionResponse();
        auto status = serviceStub_->CreateRecordingSubscription(&ctx, request, &response);
        if (!status.ok())
        {
            std::cout << "ISubscriptionService CreateRecordingSubscription call failed: " << status.error_code() << '\n';
            return EXIT_FAILURE;
        }
        
        this->subscriptionId_ = response._returnvalue();
        std::cout << "ISubscriptionService CreateRecordingSubscription 'SubscriptionId' = ' " << this->subscriptionId_ << '\n';

        return EXIT_SUCCESS;
    }

    int addVariable(std::string variableName)
    {
        if (!this->serviceStub_)
        {
            return EXIT_FAILURE;
        }
        auto ctx = grpc::ClientContext();
        auto request = Arp::Plc::Gds::Services::Grpc::ISubscriptionServiceAddVariableRequest();
        request.set_subscriptionid(subscriptionId_);
        request.set_variablename(variableName);
        auto response = Arp::Plc::Gds::Services::Grpc::ISubscriptionServiceAddVariableResponse();
        auto status = serviceStub_->AddVariable(&ctx, request, &response);
        if (!status.ok())
        {
            std::cout << "ISubscriptionService AddVariable call failed: " << status.error_code() << '\n';
            return EXIT_FAILURE;
        }
        std::cout << "ISubscriptionService AddVariable(" << std::quoted(variableName) << ") returned: " << response._returnvalue() << '\n';
        if(response._returnvalue() != Arp::Plc::Gds::Services::Grpc::DataAccessError::DAE_None)
        {
            return EXIT_FAILURE;
        }

        return EXIT_SUCCESS;
    }
private:
    uint32_t subscriptionId_ = 0;
    std::unique_ptr<Arp::Plc::Gds::Services::Grpc::ISubscriptionService::Stub> serviceStub_;
};

} // end namespace example