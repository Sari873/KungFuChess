#pragma once
#include "../Engine/GameEngine.h"
#include "../Input/Controller.h"
#include <iosfwd>
#include <string>
#include <vector>


class ScriptRunner {
public:
    void run(std::istream& in, std::ostream& out);

private:
    static bool isValidToken(const std::string& token);
    static Board buildBoard(const std::vector<std::vector<std::string>>& grid);

    std::vector<std::vector<std::string>> grid_;
    size_t rowWidth_ = 0;

    bool parseBoardLine(const std::string& line, std::ostream& out);
    void processCommand(const std::string& line, GameEngine& engine,
        Controller& controller, std::ostream& out);
};
