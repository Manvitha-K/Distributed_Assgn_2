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
using socialnetwork::timelineResponse;

class chatServiceImplementation final: public SocialNetwork::Service{
		private:
			sqlite3* Dbobj;
			Db database;
			std::vector<std::string> activeUsers;

		public:
			chatServiceImplementation():database(Dbobj){}
			Status registerUser(
				ServerContext* context,
				const user* request,
				reqResponse* reply
			)override{
				std::string userName = request->userid();
				std::cout << "userName " << userName << "\n";
				for(int i = 0; i < activeUsers.size(); i++){
					if(activeUsers[i] == userName){
						std::cout << "ALREADY EXISTS\n";
						reply->set_status(1);
						return Status::OK;
					}
				}
				int dataBaseStatus = database.registerUser(userName);
				std::cout << "ADDED TO DATABASE\n";
				if(dataBaseStatus == 0){
					activeUsers.push_back(userName);
				}
				reply->set_status(dataBaseStatus);
				return Status::OK;
			}
			
			Status listUser(
				ServerContext* context,
				const user* request,
				listResponse* reply
				)override{
					std::string userName = request->userid();
					std::vector<std::string> allUsersList = database.listAllUsers();
					reply->set_status(0);
					for(int i=0; i < allUsersList.size(); i++){
   						reply->add_users(allUsersList[i]);
					}
					std::vector<std::string> followers = database.fetchAllFollowers(userName);
					for(int i=0; i < followers.size(); i++){
   						reply->add_following_users(followers[i]);
					}
					return Status::OK;
			}
			Status follow(
				ServerContext* context,
				const followRequest* request,
				reqResponse* reply
			)override{
				std::string followeeId = request->followee();
				std::string followerId = request->follower();
				std::cout << "followeeId " << followeeId << "\n";
				std::cout << "followerId " << followerId << "\n";
				
				int retVal = database.Follow(followeeId, followerId);
				reply->set_status(retVal);
				return Status::OK; 
			}
			Status unfollow(
				ServerContext* context,
				const followRequest* request,
				reqResponse* reply
			)override{
				std::string followeeId = request->followee();
				std::string followerId = request->follower();
				int retVal = database.unFollow(followeeId, followerId);
				reply->set_status(retVal);
				return Status::OK;
			}
			Status timeline(ServerContext* context,
							const user* request,
							timelineResponse* reply
			)override{
				std::string userId = request->userid;
				database.fetchTimeLine(userId);
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

