#include <remotedict.h>

#include <iostream>

#include <proto/api.pb.h>
#include <proto/api.grpc.pb.h>

#include <grpc/grpc.h>
#include <grpcpp/create_channel.h>



struct RemoteDictionaryImpl
{
    std::shared_ptr<grpc::Channel> channel;
    std::unique_ptr<grpcdict::RemoteDictionary::Stub> stub;

    RemoteDictionaryImpl(std::shared_ptr<grpc::Channel>&& _channel, 
        std::unique_ptr<grpcdict::RemoteDictionary::Stub>&& _stub):
        channel(std::move(_channel)), stub(std::move(_stub))
    {}
};

RemoteDictionary::RemoteDictionary(const std::string& remoteAddress)
{
    auto channel = grpc::CreateChannel(remoteAddress.c_str(), grpc::InsecureChannelCredentials());
    auto stub = grpcdict::RemoteDictionary::NewStub(channel);
    pImpl = std::make_unique<RemoteDictionaryImpl>(std::move(channel), std::move(stub));
}

RemoteDictionary::~RemoteDictionary()
{}

std::chrono::system_clock::time_point getDeadline(int seconds)
{
    return std::chrono::system_clock::now() + std::chrono::seconds(seconds);
}



std::optional<std::string> RemoteDictionary::remoteGet(const std::string& key)
{
    grpcdict::GetParams params;
    grpcdict::GetResponse result;
    params.set_key(key);

    grpc::ClientContext context;
    context.set_deadline(getDeadline(1));

    const grpc::Status status = pImpl->stub->Get(&context, params, &result);

    if (status.ok())
    {
        if (result.found())
        {
            return result.value();
        }
        else
        {
            // sadly cannot use std::expected here
            std::cout << "Get failed: " << result.value() << std::endl;
        }
    }
    return {};
}



bool RemoteDictionary::remoteSet(const std::string& key, const std::string& value)
{
    grpcdict::SetParams params;
    grpcdict::SetResponse result;
    params.set_key(key);
    params.set_value(value);

    grpc::ClientContext context;
    context.set_deadline(getDeadline(1));

    const grpc::Status status = pImpl->stub->Set(&context, params, &result);

    if (status.ok())
    {
        if (result.error().empty())
        {
            return true;
        }
        else
        {
            // sadly cannot use std::expected here
            std::cout << "Set failed: " << result.error() << std::endl;
        }
    }
    return false;
}



RemoteDictionary::Stats RemoteDictionary::remoteStats()
{
    grpcdict::StatsResponse result;

    grpc::ClientContext context;
    context.set_deadline(getDeadline(1));

    const grpc::Status status = pImpl->stub->Stats(&context, {}, &result);

    if (status.ok())
    {
        return Stats{
            result.ntotalget(),
            result.ntotalset(),
            result.nsuccessfulget(),
            result.nfailedget()
        };
    }
    return {};
}



