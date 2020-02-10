#include <string>
#include <grpcpp/grpcpp.h>
#include "socialnetwork.grpc.pb.h"
#include "Db.h"
#include <sqlite3.h>

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
	private:
		Db database;

	public:
		sqlite3* Dbobj;
		chatServiceImplementation():database(Dbobj){}
		Status register(
			ServerContext* context,
			user* request,
			reqResponse* reply
		)override{
			std::string userName = request->userId;
			reply.success = database.registerUser(userName);
			return Status::OK;
		};
		
		
		Status follow(
			ServerContext* context,
			const followRequest* request,
			reqResponse* reply
			)override{
				std::string followeeId = request->followee;
				std::string followerId = request->follower;
				
				database.Follow(followeeId, followerId);
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

