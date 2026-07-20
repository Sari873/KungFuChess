#pragma once
#include "../Engine/GameEngine.h"
#include "../Model/Position.h"
#include <memory>


class Controller {
public:
    explicit Controller(GameEngine& engine);

    void handleClick(int x, int y);
    void resetSelection();

    bool hasSelection() const;
    Position getSelected() const;

    GameEngine& engine();
    const GameEngine& engine() const;

    void clearSelection();
    void select(const Position& cell);

private:
    class ISelectionState {
    public:
        virtual ~ISelectionState() = default;
        virtual void onClick(Controller& controller, bool inBoard, const Position& cell) = 0;
    };

    class IdleState final : public ISelectionState {
    public:
        void onClick(Controller& controller, bool inBoard, const Position& cell) override;
    };

    class SelectedState final : public ISelectionState {
    public:
        void onClick(Controller& controller, bool inBoard, const Position& cell) override;
    };

    void setState(std::unique_ptr<ISelectionState> state);

    GameEngine& engine_;
    std::unique_ptr<ISelectionState> state_;
    bool hasSelection_ = false;
    Position selected_;
};
