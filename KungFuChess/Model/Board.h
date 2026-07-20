#pragma once
#include "Piece.h"
#include "Position.h"
#include <memory>
#include <vector>

class Board {
public:
    Board();
    Board(int rows, int cols);

    int getRows() const;
    int getCols() const;

    bool inBounds(const Position& pos) const;
    bool isEmpty(const Position& pos) const;
    bool addPiece(std::unique_ptr<Piece> piece);

    bool removePiece(const Position& pos);
    bool removePieceById(int pieceId);

    /** Clear a piece from the grid without capturing it (e.g. while Moving). */
    void detachPiece(Piece* piece);

    /** Place a detached (or already owned) piece onto an empty cell. */
    bool placePiece(Piece* piece, const Position& dest);

    const Piece* getPieceAt(const Position& pos) const;
    Piece* getPieceAt(const Position& pos);

    bool isFriendly(const Position& a, const Position& b) const;
    void movePiece(const Position& src, const Position& dst);

    Piece* findPieceById(int pieceId);
    const Piece* findPieceById(int pieceId) const;

private:
    int rows_;
    int cols_;
    std::vector<std::vector<Piece*>> grid_;      
    std::vector<std::unique_ptr<Piece>> pieces_; 
};
