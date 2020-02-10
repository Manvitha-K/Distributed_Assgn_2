#include <string>
#include <grpcpp/grpcpp.h>
#include "socialnetwork.grpc.pb.h"

using grpc::Server;
using grpc::ServerBuilder;
using grpc::ServerContext;
using grpc::Status;

using socialnetwork::SocialNetwork;
using socialnetwork::reqResponse;
using socialnetwork::followersList;
using socialnetwork::user;
using socialnetwork::followRequest;
using socialnetwork::status;

class chatServiceImplementation final: public SocialNetwork::Service{
	Status follow(
		ServerContext* context,
		const followRequest* request,
		reqResponse* reply
		)override{
			int followeeId = request->followee;
			int followerId = request->follower;

			return Status::OK; 
		}
};



void Run() {
	std::string address("0.0.0.0:5000");
	chatServiceImplementation service;
	ServerBuilder builder;
	builder.AddListeningPort(address, grpc::InsecureServerCredentials());
	builder.RegisterService(&service);

	std::unique_ptr<Server> server(builder.BuildAndStart());
	std::cout << "Server listening on port: " << address << std::endl;
	server->Wait();
}

int  main()
{
	Run();
	return 0;
}

