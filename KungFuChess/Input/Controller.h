#pragma once
#include "../Presentation/DisplayTypes.h"
#include "../Presentation/IGameSession.h"
#include <memory>


class Controller {
public:
    explicit Controller(IGameSession& session);

    void handleClick(int x, int y);
    void resetSelection();

    bool hasSelection() const;
    CellCoord getSelected() const;

    IGameSession& session();
    const IGameSession& session() const;

    void clearSelection();
    void select(const CellCoord& cell);

private:
    class ISelectionState {
    public:
        virtual ~ISelectionState() = default;
        virtual void onClick(Controller& controller, bool inBoard, const CellCoord& cell) = 0;
    };

    class IdleState final : public ISelectionState {
    public:
        void onClick(Controller& controller, bool inBoard, const CellCoord& cell) override;
    };

    class SelectedState final : public ISelectionState {
    public:
        void onClick(Controller& controller, bool inBoard, const CellCoord& cell) override;
    };

    void setState(std::unique_ptr<ISelectionState> state);

    IGameSession& session_;
    std::unique_ptr<ISelectionState> state_;
    bool hasSelection_ = false;
    CellCoord selected_;
};
