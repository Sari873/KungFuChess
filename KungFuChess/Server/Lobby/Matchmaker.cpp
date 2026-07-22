#include "Matchmaker.h"
#include "../../GameConstants.h"
#include <algorithm>
#include <cmath>

namespace Server {

void Matchmaker::enqueue(PlayerConnectionPtr player) {
    if (!player) {
        return;
    }
    dequeue(player->playerId);
    queue_.push_back(std::move(player));
}

void Matchmaker::dequeue(int playerId) {
    queue_.erase(std::remove_if(queue_.begin(), queue_.end(),
                               [playerId](const PlayerConnectionPtr& p) {
                                   return p && p->playerId == playerId;
                               }),
                queue_.end());
}

int Matchmaker::positionOf(int playerId) const {
    for (std::size_t i = 0; i < queue_.size(); ++i) {
        if (queue_[i] && queue_[i]->playerId == playerId) {
            return static_cast<int>(i) + 1;
        }
    }
    return 0;
}

bool Matchmaker::tryPopPair(PlayerConnectionPtr& a, PlayerConnectionPtr& b) {
    for (std::size_t i = 0; i < queue_.size(); ++i) {
        for (std::size_t j = i + 1; j < queue_.size(); ++j) {
            if (!queue_[i] || !queue_[j]) {
                continue;
            }
            const int diff = std::abs(queue_[i]->rating - queue_[j]->rating);
            if (diff <= Kfc::Server::kRatingMatchWindow) {
                a = queue_[i];
                b = queue_[j];
                queue_.erase(queue_.begin() + static_cast<std::ptrdiff_t>(j));
                queue_.erase(queue_.begin() + static_cast<std::ptrdiff_t>(i));
                return true;
            }
        }
    }
    return false;
}

} 
