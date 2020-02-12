#include "Db.h"
#include <iostream>

Db::Db(sqlite3* db){
    int rc = sqlite3_open_v2("./socialnetwork.db", &db, SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE | SQLITE_OPEN_FULLMUTEX, NULL);  
    Db::DatabaseObj = db;
    createUsersTable();
    createFollowersTable();
}

sqlite3* openDataBaseConnection(sqlite3* db){
    int rc = sqlite3_open_v2("./socialnetwork.db", &db, SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE | SQLITE_OPEN_FULLMUTEX, NULL);
    if(rc){
        return NULL;
    }
    else{
        return db;
    }
}

void closeDataBaseConnection(sqlite3* db){
    sqlite3_close(db);
}

static int fetchUsersCallback(void* userList, int colcount, char **data, char **ColName){
    std::vector<std::string>* users = static_cast<std::vector<std::string>*> (userList);
    std::cout << data[0] << "\n";
    //Db::users = static_cast<std::vector<std::string>*> (userList);
    users->push_back(data[0]);
}

std::vector<std::string> Db::listAllUsers(){
    char* zErrMsg = 0;
    //sqlite3* db = openDataBaseConnection(db);
    Db::DatabaseObj = openDataBaseConnection(Db::DatabaseObj);
    std::vector<std::string> userList;
    if(!Db::DatabaseObj){
        return userList;
    } 
    std::string sqlQuery = "SELECT * from USERS;";
    int rc = sqlite3_exec(Db::DatabaseObj, sqlQuery.c_str(), fetchUsersCallback, &userList, &zErrMsg);
    std::cout << "current users in the system" << userList.size() << "\n";
    for(int i = 0; i < userList.size(); i++){
        std::cout << userList[i] << "\n";
    }
    if (rc != SQLITE_OK){
        sqlite3_free(zErrMsg);
    }
    closeDataBaseConnection(Db::DatabaseObj);
    return userList;
}

static int checkUserExistenceCallback(void* existence, int colcount, char **data, char **ColName){
    bool* exist = (bool*) existence;
    *exist = true;
}


bool Db::checkUserExistence(std::string username){
    //sqlite3* db = openDataBaseConnection(db);
    Db::DatabaseObj = openDataBaseConnection(Db::DatabaseObj);
    char* zErrMsg = 0;
    bool exist = false;
    std::string sqlQuery = "SELECT * from USERS WHERE NAME = \"" + username + "\";";                              
    int rc = sqlite3_exec(Db::DatabaseObj, sqlQuery.c_str(), checkUserExistenceCallback, (void *)&exist, &zErrMsg);
    if (rc != SQLITE_OK){
        sqlite3_free(zErrMsg);
    }
    closeDataBaseConnection(Db::DatabaseObj);
    return exist;
}

int Db::registerUser(std::string UserName){
    char* zErrMsg = 0;
    bool alreadyExists = checkUserExistence(UserName);
    if(alreadyExists == true){
        std::cout << "User Already Exists \n";
        return 1;
    }
    //sqlite3* db = openDataBaseConnection(db);
    Db::DatabaseObj = openDataBaseConnection(Db::DatabaseObj);
    std::string sqlQuery = "INSERT INTO USERS (NAME) " \
                      "VALUES(\""+ UserName + "\");";
    int rc = sqlite3_exec(Db::DatabaseObj, sqlQuery.c_str(), NULL, 0, &zErrMsg);
    if (rc != SQLITE_OK){
        sqlite3_free(zErrMsg);
        closeDataBaseConnection(Db::DatabaseObj);
        return 5;
    }
    closeDataBaseConnection(Db::DatabaseObj);
    std::cout << "user added to table\n";
    int followRet = Follow(UserName, UserName);
    std::cout << "followRet value " << followRet << "\n";
    return followRet;
    //return 0;
}

static int fetchFollowersCallback(void* followersList, int colcount, char **data, char **ColName){
    std::vector<std::string>* followers = (std::vector<std::string>*) followersList;
    std::cout << "followers " << data[0] << "\n";
    followers->push_back(data[0]);
}

std::vector<std::string> Db::fetchAllFollowers(std::string UserName){
    
    char* zErrMsg = 0;
    //sqlite3* db = openDataBaseConnection(db);
    Db::DatabaseObj = openDataBaseConnection(Db::DatabaseObj);
    std::vector<std::string> followersList;
    if(!Db::DatabaseObj){
        return followersList;
    } 
    std::string sqlQuery ="SELECT * from FOLLOWERS WHERE FOLLOWS =\"" + UserName + "\";";
    int rc = sqlite3_exec(Db::DatabaseObj, sqlQuery.c_str(), fetchFollowersCallback, (void *)&followersList, &zErrMsg);
    if (rc != SQLITE_OK){
        sqlite3_free(zErrMsg);
    }
    std::cout << "Fetching all followers " << followersList.size() << "\n";
    for(int i = 0; i < followersList.size(); i++){
        std::cout << followersList[i] << "\n";
    }
    closeDataBaseConnection(Db::DatabaseObj);
    return followersList;
}

static int fetchFollowingCallback(void* followingList, int colcount, char **data, char **ColName){
    std::vector<std::string>* followees = (std::vector<std::string>*) followingList;
    (*followees).push_back(data[1]);
}

std::vector<std::string> Db::followingList(std::string UserName){
    char* zErrMsg = 0;
    sqlite3* db = openDataBaseConnection(db);
    std::vector<std::string> followeesList;
    if(!db){
        return followeesList;
    } 
    std::cout << "Extracting follows list of " << UserName << "\n";
    std::string sqlQuery = "SELECT * from FOLLOWERS WHERE NAME = \""+UserName+ "\";";
    int rc = sqlite3_exec(db, sqlQuery.c_str(), fetchFollowingCallback, (void *)&followeesList, &zErrMsg);
    if (rc != SQLITE_OK){
        sqlite3_free(zErrMsg);
    }
    closeDataBaseConnection(db);
    return followeesList;
}

void Db::createUsersTable(){
    sqlite3* db = openDataBaseConnection(db);
    if(db){
        char* zErrMsg = 0;
        std::string sqlQuery = "CREATE TABLE IF NOT EXISTS USERS (NAME TEXT NOT NULL);";
        int rc = sqlite3_exec(db, sqlQuery.c_str(), NULL, 0, &zErrMsg);
        if (rc != SQLITE_OK){
            sqlite3_free(zErrMsg);
        }
    }
    closeDataBaseConnection(db);
}

void Db::createFollowersTable(){
    sqlite3* db = openDataBaseConnection(db);
    if(db){
        char* zErrMsg = 0;
        std::string sqlQuery = "CREATE TABLE IF NOT EXISTS FOLLOWERS (NAME TEXT NOT NULL, \
                                                          FOLLOWS TEXT NOT NULL);";
        int rc = sqlite3_exec(db, sqlQuery.c_str(), NULL, 0, &zErrMsg);
        if (rc != SQLITE_OK){
            sqlite3_free(zErrMsg);
        }
    }
    closeDataBaseConnection(db);
}

static int checkAlreadyFollowingCallback(void* follows, int colcount, char **data, char **ColName){
    bool* follow = (bool*) follows;
    *follow = true;
}

bool Db::checkAlreadyFollowing(std::string user, std::string followee){
    bool follows = false;
    char* zErrMsg = 0;
    sqlite3* db = openDataBaseConnection(db);
    std::cout << "In checking phase\n";
    std::vector<std::string> followeesList;
    if(db){
        std::cout << "DB successful open\n";
        std::string sqlQuery = "SELECT * from FOLLOWERS WHERE NAME = \""+ user + "\" \
                                            AND FOLLOWS = \"" + followee +"\";";
        int rc = sqlite3_exec(db, sqlQuery.c_str(), checkAlreadyFollowingCallback, (void *)&follows, &zErrMsg);
        if (rc != SQLITE_OK){
            sqlite3_free(zErrMsg);
        }
        closeDataBaseConnection(db);
    }
    std::cout << "DB Close\n";
    return follows;
}

int Db::Follow(std::string user, std::string follows){
    std::cout << "follow relationship invoked\n";
    bool userExists = checkUserExistence(user);
    if(userExists == false){
        return 3;
    }
    bool followsExists = checkUserExistence(follows);
    if(followsExists == false){
        return 3;
    }
    bool following = checkAlreadyFollowing(user, follows);
    //std::cout << "following " << following << "\n";
    if(following == false){
        std::cout << "creating follow relationship \n";
        char* zErrMsg = 0;
        sqlite3* db = openDataBaseConnection(db);
        std::string sqlQuery = "INSERT INTO FOLLOWERS (NAME,FOLLOWS) \
                            VALUES( \"" + user + "\",  +\"" + follows + "\");";
        int rc = sqlite3_exec(db, sqlQuery.c_str(), NULL, 0, &zErrMsg);
        if (rc != SQLITE_OK){
            sqlite3_free(zErrMsg);
        }
        closeDataBaseConnection(db);
        return 0;
    }
    else{
        return 1;
    }
}

int Db::unFollow(std::string user, std::string follows){
    bool userExists = checkUserExistence(user);
    if(userExists == false){
        return 3;
    }
    bool followsExists = checkUserExistence(follows);
    if(followsExists == false){
        return 3;
    }
    bool following = checkAlreadyFollowing(user, follows);
    if(following == true){
        char* zErrMsg = 0;
        sqlite3* db = openDataBaseConnection(db);
        std::string sqlQuery =  "DELETE FROM FOLLOWERS WHERE NAME = +\"" +user + "\" +  \
                            AND FOLLOWS = + \"" + follows + "\";";
        int rc = sqlite3_exec(db, sqlQuery.c_str(), NULL, 0, &zErrMsg);
        if (rc != SQLITE_OK){
            sqlite3_free(zErrMsg);
        }
        closeDataBaseConnection(db); 
        return 0;
    }
    else{
        return 2;
    }
}
