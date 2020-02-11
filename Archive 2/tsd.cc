#include <string>
#include <grpc++/grpc++.h>
#include "socialnetwork.grpc.pb.h"
#include "Db.h"
#include "sqlite/sqlite3.h"

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
using socialnetwork::user;
using socialnetwork::listResponse;

class chatServiceImplementation final: public SocialNetwork::Service{
		private:
			sqlite3* Dbobj;
			Db database;

		public:
			chatServiceImplementation():database(Dbobj){}
			Status registerUser(
				ServerContext* context,
				const user* request,
				reqResponse* reply
			)override{
				std::string userName = request->userid();
				reply->set_success(database.registerUser(userName));
				return Status::OK;
			}
			
			Status listUser(
				ServerContext* context,
				const user* request,
				listResponse* reply
				){
					std::string userName = request->userid();
					std::vector<std::string> allUsersList = database.listAllUsers();
					reply->set_registeredusers(allUsersList);
					std::vector<std::string> followers = database.fetchAllFollowers(userName);
					reply->set_followers(followers);
					return Status::OK;
			}
			Status follow(
				ServerContext* context,
				const followRequest* request,
				reqResponse* reply
			)override{
				std::string followeeId = request->followee();
				std::string followerId = request->follower();
				
				database.Follow(followeeId, followerId);
				return Status::OK; 
			}
			Status unfollow(
				ServerContext* context,
				const followRequest* request,
				reqResponse* reply
			)override{
				std::string followeeId = request->followee();
				std::string followerId = request->follower();
				database.unFollow(followeeId, followerId);
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

