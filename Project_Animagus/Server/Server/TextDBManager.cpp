#include "TextDBManager.h"
#include <fstream>

bool TextDBManager::DBConnect()
{
    std::lock_guard<std::mutex> lock(m_mutex);

    std::ifstream file("user_db.txt");
    if (!file.is_open()) {
        std::cerr << "Failed to open user_db.txt" << std::endl;
        return false;
    }

    m_userList.clear();

    std::string line;
    while (std::getline(file, line)) 
    {
        std::istringstream iss(line);
        std::string id, password, name;
        int is_login;
        if (!(iss >> id >> password >> name >> is_login))
            continue;
        
        UserInfo user{ id, password, name, (is_login != 0) };
        m_userList[id] = user;
    }

    file.close();
    return true;
}

bool TextDBManager::DBDisconnect()
{
    std::lock_guard<std::mutex> lock(m_mutex);

    std::ofstream file("user_db.txt", std::ios::trunc);
    if (!file.is_open()) {
        std::cerr << "Failed to open user_db.txt for writing" << std::endl;
        return false;
    }

    for (const auto& [id, user] : m_userList)
    {
        file << user.id << " "
            << user.password << " "
            << user.name << " "
            << /*(user.is_login ? 1 : 0)*/ 0 << std::endl;
    }

    file.close();
    return true;
}

bool TextDBManager::DBFindById(const char* id, const char* passwd, char* name, char* flag)
{
    std::lock_guard<std::mutex> lock(m_mutex);

    std::string userId(id), userPassword(passwd);
    if (false == m_userList.contains(userId)) {
        *flag = Protocol::LOGIN_NOEX; // No such user
        return false;
    }

    UserInfo& user = m_userList[userId];
    if (true == user.is_login) {
        *flag = Protocol::LOGIN_USING; // User is already logged in
        return false;
    }

    if (user.password != userPassword) {
        *flag = Protocol::LOGIN_ERR; // Incorrect password
        return false;
    }

    strcpy_s(name, Protocol::MAX_NAME_LEN, user.name.c_str());
    user.is_login = true;

    return true;
}

bool TextDBManager::DBLogOutById(const char* id)
{
    std::lock_guard<std::mutex> lock(m_mutex);

    std::string userId(id);
    if (false == m_userList.contains(userId)) return false;

    UserInfo& user = m_userList[userId];
    if (false == user.is_login) return false;

    user.is_login = false;

    return true;
}

bool TextDBManager::DBSignUp(const char* id, const char* passwd, char* name)
{
    std::lock_guard<std::mutex> lock(m_mutex);

    std::string userId(id), userPassword(passwd), userName(name);
    if (true == m_userList.contains(userId)) return false;

    UserInfo user{ userId, userPassword, userName, false };
    m_userList[userId] = user;

    return true;
}

bool TextDBManager::DBDeleteUserById(const char* id)
{
    std::lock_guard<std::mutex> lock(m_mutex);

    std::string userId(id);
    if (false == m_userList.contains(userId)) return false;
    m_userList.erase(userId);
    
    return true;
}
