#pragma once
#include "../Model/Board.h"
#include "../Input/Controller.h"
#include <iosfwd>
#include <string>
#include <vector>

class ScriptRunner {
public:
    void run(std::istream& in, std::ostream& out);

private:
    Board board_;
    Controller controller_;
    long long gameClockMs_ = 0;

    std::vector<std::vector<std::string>> grid_;
    bool boardInitialized_ = false;
    size_t rowWidth_ = 0;

    static bool isValidToken(const std::string& token);

    static Board buildBoard(const std::vector<std::vector<std::string>>& grid);

    bool parseBoardLine(const std::string& line, std::ostream& out);
    void processCommand(const std::string& line, std::ostream& out);
};
