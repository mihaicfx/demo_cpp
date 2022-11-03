#include <iostream>
#include <memory>
#include <getopt.h>

#include <proto/api.pb.h>
#include <proto/api.grpc.pb.h>

#include <grpc/grpc.h>
#include <grpcpp/server_builder.h>

#include "dictionary.h"

class RemoteDictionaryService final : public grpcdict::RemoteDictionary::Service 
{
public:
    RemoteDictionaryService(std::shared_ptr<Dictionary> dict) : mDict(dict) {}

    virtual grpc::Status Get([[maybe_unused]] grpc::ServerContext* context, const grpcdict::GetParams* request, grpcdict::GetResponse* response)
    {
        auto result = mDict->get(request->key());

        std::cout << "Get(" << request->key() << ") = " << (result? *result: "-") << std::endl;

        response->set_found(result.has_value());
        response->set_value(result? *result: "Not found");

        return grpc::Status::OK;
    }

    virtual grpc::Status Set([[maybe_unused]] grpc::ServerContext* context, const grpcdict::SetParams* request, grpcdict::SetResponse* response)
    {
        std::cout << "Set(" << request->key() << ", " << request->value() << ")" << std::endl;
    
        mDict->set(request->key(), request->value());

        response->set_error(""); // TODO: is there reason for failure?

        return grpc::Status::OK;
    }

    virtual grpc::Status Stats([[maybe_unused]] grpc::ServerContext* context, [[maybe_unused]] const grpcdict::NoParams* request, grpcdict::StatsResponse* response)
    {
        auto stats = mDict->stats();
        std::cout << "Stats: get = " << stats.nTotalGet << " / set = " << stats.nTotalSet << std::endl;

        response->set_ntotalget(stats.nTotalGet);
        response->set_ntotalset(stats.nTotalSet);
        response->set_nsuccessfulget(stats.nSuccessfulGet); 
        response->set_nfailedget(stats.nFailedGet);

        return grpc::Status::OK;
    }

private:
    std::shared_ptr<Dictionary> mDict;
};

int main(int argc, char* argv[])
{
    static struct option long_options[] =
    {
        {"bloom_filter", no_argument, NULL, 'b'},
        {NULL, 0, NULL, 0}
    };

    bool useBloomFilter = false;
    char ch;
    while ((ch = getopt_long(argc, argv, "b", long_options, NULL)) != -1)
    {
        switch (ch)
        {
            case 'b':
                useBloomFilter = true;
                break;
            default:
                std::cout << "Invalid option: " << optarg;
        }
    }


    grpc::ServerBuilder builder;
    builder.AddListeningPort("0.0.0.0:50051", grpc::InsecureServerCredentials());

    auto dict = std::make_shared<Dictionary>(useBloomFilter);
    RemoteDictionaryService service{dict};
    builder.RegisterService(&service);

    std::unique_ptr<grpc::Server> server(builder.BuildAndStart());
    server->Wait();
    
    return 0;
}
