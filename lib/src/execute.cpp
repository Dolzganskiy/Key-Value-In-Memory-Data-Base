#include "DataBase.h"

std::vector<std::string> Split(const std::string& line) {
    std::vector<std::string> tokens;
    std::istringstream iss(line);
    std::string token;

    while (iss >> token) {
        tokens.push_back(token);
    }

    return tokens;
}

CommandResult DataBase::Execute(const std::string& line) {
    std::vector<std::string> tokens = Split(line);

    if (tokens.empty()) {
        return "";
    }

    std::string command = tokens[0];
    std::transform(command.begin(), command.end(), command.begin(),
                [](unsigned char c) {
                    return std::toupper(c);
                });

    auto it = handlers_.find(command);
    if (it == handlers_.end()) {
        return std::unexpected("(error) unknown command");
    }

    return it->second(tokens);
}