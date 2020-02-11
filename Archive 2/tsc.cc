#include <iostream>
//#include <memory>
//#include <thread>
//#include <vector>
#include <string>
#include <unistd.h>
#include <grpc++/grpc++.h>
#include "client.h"
#include "socialnetwork.grpc.pb.h"

using grpc::Channel;
using grpc::ClientContext;
using grpc::Status;
using grpc::StatusCode;

using socialnetwork::SocialNetwork;
using socialnetwork::reqResponse;
using socialnetwork::followersList;
using socialnetwork::user;
using socialnetwork::followRequest;
using socialnetwork::status;
using socialnetwork::user;
using socialnetwork::listResponse;

class Client : public IClient
{
    public:
        Client(const std::string& hname,
               const std::string& uname,
               const std::string& p)
            :hostname(hname), username(uname), port(p)
            {}

        void follow(std::string user, std::string follows, IReply& ire) {
            followRequest request;
            request.set_followee(user);
            request.set_follower(follows);
            reqResponse response;
            ClientContext context;
            Status status = stub_->follow(&context, request, &response);
            ire.grpc_status = status;
            if(!status.ok()){
                ire.comm_status = response.status();
            }
            else{
                ire.comm_status = response.status();
            }
        }
        void unfollow(std::string user, std::string follows, IReply& ire) {
            followRequest request;
            request.set_followee(user);
            request.set_follower(follows);
            reqResponse response;
            ClientContext context;
            Status status = stub_->unfollow(&context, request, &response);
            ire.grpc_status = status;
            if(status.ok()){
                ire.comm_status = response.status();
            }
            else{
                std::cout << status.error_code() << ": " << status.error_message() << std::endl;
                ire.comm_status = FAILURE_NOT_EXISTS;
            }
        }
        void listUser(std::string username, IReply& ire){
            user request;
            request.set_userid(username);
            listResponse response;
            ClientContext context;
            Status status = stub_->listUser(&context, request, &response);
            ire.grpc_status = status;
            if(status.ok()){    
                ire.comm_status = SUCCESS;
                ire.all_users = response.users();
                ire.following_users = response.following_users();
            }
            else{
                ire.comm_status = FAILURE_NOT_EXISTS;
            } 
        }
        int registerUser(std::string username){
            user request;
            request.set_userid(username);
            reqResponse response;
            ClientContext context;
            Status status = stub_->registerUser(&context, request, &response);
            if(status.ok()){
                return response.status();
            }
            else{
                std::cout << status.error_code() << ": " << status.error_message() << std::endl;
                return FAILURE_NOT_EXISTS;
            }
        }
    protected:
        virtual int connectTo();
        virtual IReply processCommand(std::string& input);
        virtual void processTimeline();
    private:
        std::string hostname;
        std::string username;
        std::string port;
        std::unique_ptr<SocialNetwork::Stub> stub_;
        std::shared_ptr<Channel> channel;
};

int main(int argc, char** argv) {

    std::string hostname = "localhost";
    std::string username = "default";
    std::string port = "5000";
    int opt = 0;
    while ((opt = getopt(argc, argv, "h:u:p:")) != -1){
        switch(opt) {
            case 'h':
                hostname = optarg;break;
            case 'u':
                username = optarg;break;
            case 'p':
                port = optarg;break;
            default:
                std::cerr << "Invalid Command Line Argument\n";
        }
    }

    Client myc(hostname, username, port);
    // You MUST invoke "run_client" function to start business logic
    myc.run_client();

    return 0;
}

int Client::connectTo()
{
	// ------------------------------------------------------------
    // In this function, you are supposed to create a stub so that
    // you call service methods in the processCommand/porcessTimeline
    // functions. That is, the stub should be accessible when you want
    // to call any service methods in those functions.
    // I recommend you to have the stub as
    // a member variable in your own Client class.
    // Please refer to gRpc tutorial how to create a stub.
	// ------------------------------------------------------------
    std::string ipaddr = hostname + ":" + port;
    std::string address(ipaddr);
    std::cout << "in connect " << address << std::endl;
    channel = grpc::CreateChannel(address, grpc::InsecureChannelCredentials());
    stub_ = socialnetwork::SocialNetwork::NewStub(channel);
    std::cout << "stub successfully created\n";
    int regVal = registerUser(username);
    if(regVal == 0){
        return 1;
    }
    return -1; // return 1 if success, otherwise return -1
}

IReply Client::processCommand(std::string& input)
{
    IReply ire;
    if (input.rfind("FOLLOW", 0) == 0) {
        follow(username, input.substr(7), ire);
    }
    else if(input.rfind("UNFOLLOW", 0) == 0){
        unfollow(username, &input[7], ire);
    }
    else if(input.rfind("LIST", 0) == 0){
        listUser(username, ire);
    }
    else if(input.rfind("TIMELINE", 0) == 0){
        ire.comm_status = SUCCESS;
        ire.grpc_status = Status::OK;
    }
    else{
        ire.comm_status = FAILURE_INVALID;
        ire.grpc_status = Status(StatusCode::INVALID_ARGUMENT, "Invalid Command");
    }



	// ------------------------------------------------------------
	// GUIDE 1:
	// In this function, you are supposed to parse the given input
    // command and create your own message so that you call an 
    // appropriate service method. The input command will be one
    // of the followings:
	//
	// FOLLOW <username>
	// UNFOLLOW <username>
	// LIST
    // TIMELINE
	//
	// - JOIN/LEAVE and "<username>" are separated by one space.
	// ------------------------------------------------------------
	
    // ------------------------------------------------------------
	// GUIDE 2:
	// Then, you should create a variable of IReply structure
	// provided by the client.h and initialize it according to
	// the result. Finally you can finish this function by returning
    // the IReply.
	// ------------------------------------------------------------
    
	// ------------------------------------------------------------
    // HINT: How to set the IReply?
    // Suppose you have "Join" service method for JOIN command,
    // IReply can be set as follow:
    // 
    //     // some codes for creating/initializing parameters for
    //     // service method
    //     IReply ire;
    //     grpc::Status status = stub_->Join(&context, /* some parameters */);
    //     ire.grpc_status = status;
    //     if (status.ok()) {
    //         ire.comm_status = SUCCESS;
    //     } else {
    //         ire.comm_status = FAILURE_NOT_EXISTS;
    //     }
    //      
    //      return ire;
    // 
    // IMPORTANT: 
    // For the command "LIST", you should set both "all_users" and 
    // "following_users" member variable of IReply.
	// ------------------------------------------------------------
    
    
    return ire;
}

void Client::processTimeline()
{
	// ------------------------------------------------------------
    // In this function, you are supposed to get into timeline mode.
    // You may need to call a service method to communicate with
    // the server. Use getPostMessage/displayPostMessage functions
    // for both getting and displaying messages in timeline mode.
    // You should use them as you did in hw1.
	// ------------------------------------------------------------

    // ------------------------------------------------------------
    // IMPORTANT NOTICE:
    //
    // Once a user enter to timeline mode , there is no way
    // to command mode. You don't have to worry about this situation,
    // and you can terminate the client program by pressing
    // CTRL-C (SIGINT)
	// ------------------------------------------------------------
}
