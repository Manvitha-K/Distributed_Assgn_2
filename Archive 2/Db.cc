#include "Db.h"
#include <iostream>

Db::Db(sqlite3* db){
    int rc = sqlite3_open_v2("./socialnetwork.db", &db, SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE | SQLITE_OPEN_FULLMUTEX, NULL);  
    Db::DatabaseObj = db;
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
    users->push_back(data[0]);
}

std::vector<std::string> Db::listAllUsers(){
    char* zErrMsg = 0;
    sqlite3* db = openDataBaseConnection(db);
    //Db::DatabaseObj = openDataBaseConnection(Db::DatabaseObj);
    //Db::userList.clear();
    if(!db){
        return userList;
    } 
    std::string sqlQuery = "SELECT * from USERS;";
    std::vector<std::string> userList;
    int rc = sqlite3_exec(db, sqlQuery.c_str(), fetchUsersCallback, &userList, &zErrMsg);
    if (rc != SQLITE_OK){
        sqlite3_free(zErrMsg);
    }
    closeDataBaseConnection(db);
    return Db::userList;
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

bool Db::registerUser(std::string UserName){
    char* zErrMsg = 0;
    createUsersTable();
    bool alreadyExists = checkUserExistence(UserName);
    if(alreadyExists == true){
        return false;
    }
    //sqlite3* db = openDataBaseConnection(db);
    Db::DatabaseObj = openDataBaseConnection(Db::DatabaseObj);
    std::string sqlQuery = "INSERT INTO USERS (NAME) " \
                      "VALUES(\""+ UserName + "\");";
    int rc = sqlite3_exec(Db::DatabaseObj, sqlQuery.c_str(), NULL, 0, &zErrMsg);
    if (rc != SQLITE_OK){
        sqlite3_free(zErrMsg);
    }
    closeDataBaseConnection(Db::DatabaseObj);
    createFollowersTable();
    Follow(UserName, UserName);
    return true;
}

static int fetchFollowersCallback(void* followersList, int colcount, char **data, char **ColName){
    std::vector<std::string>* followers = (std::vector<std::string>*) followersList;
    (*followers).push_back(data[0]);
}

std::vector<std::string> Db::fetchAllFollowers(std::string UserName){
    char* zErrMsg = 0;
    sqlite3* db = openDataBaseConnection(db);
    std::vector<std::string> followersList;
    if(!db){
        return followersList;
    } 
    std::string sqlQuery ="SELECT * from FOLLOWERS WHERE FOLLOWS =\"" + UserName + "\";";
    int rc = sqlite3_exec(db, sqlQuery.c_str(), fetchUsersCallback, (void *)&followersList, &zErrMsg);
    if (rc != SQLITE_OK){
        sqlite3_free(zErrMsg);
    }
    closeDataBaseConnection(db);
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
    std::string sqlQuery = "SELECT * from FOLLOWERS WHERE NAME = \""+UserName+ "\";";
    int rc = sqlite3_exec(db, sqlQuery.c_str(), fetchUsersCallback, (void *)&followeesList, &zErrMsg);
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

bool Db::Follow(std::string user, std::string follows){
    bool following = checkAlreadyFollowing(user, follows);
    std::cout << "following " << following << "\n";
    if(following == false){
        char* zErrMsg = 0;
        sqlite3* db = openDataBaseConnection(db);
        std::string sqlQuery = "INSERT INTO FOLLOWERS (NAME,FOLLOWS) \
                            VALUES( \"" + user + "\",  +\"" + follows + "\");";
        int rc = sqlite3_exec(db, sqlQuery.c_str(), NULL, 0, &zErrMsg);
        if (rc != SQLITE_OK){
            sqlite3_free(zErrMsg);
        }
        closeDataBaseConnection(db);
    }
    return true;
}

bool Db::unFollow(std::string user, std::string follows){
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
        return true;
    }
    return false;
}
