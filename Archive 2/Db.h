#include <stdio.h>
#include <sqlite3.h>
#include <stdlib.h>
#include <vector>
#include <string>

#ifndef Db_h
#define Db_h

class Db{
    //std::string user_table_name = "USERS";
    //std::string followers_table_name = "FOLLOWERS"; 

    public:
        sqlite3* DatabaseObj;
        Db(sqlite3* db);
        std::vector<std::string> userList;
        bool dataBaseCreation();
        void createUsersTable();
        void createFollowersTable();


        bool registerUser(std::string UserName);
        bool Follow(std::string user, std::string follows);
        bool unFollow(std::string user, std::string follows);

        std::vector<std::string> listAllUsers();
        std::vector<std::string> fetchAllFollowers(std::string UserName);
        std::vector<std::string> followingList(std::string UserName);
        
        bool checkUserExistence(std::string UserName);
        bool checkAlreadyFollowing(std::string user, std::string followee);
};
#endif