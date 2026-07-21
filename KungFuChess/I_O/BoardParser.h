#pragma once
#include "../Model/Board.h"
#include <string>
#include <vector>

class BoardParser {
public:
    static Board parse(const std::vector<std::string>& rows);
};
