/*
	This file is part of Vajolet.

    Vajolet is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    Vajolet is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Vajolet.  If not, see <http://www.gnu.org/licenses/>
*/

#ifndef MOVEPICK_H_
#define MOVEPICK_H_

#include "bitBoardIndex.h"
#include "MoveList.h"
#include "move.h"
#include "score.h"


class Position;
class SearchData;

class MovePicker
{
public:
	//--------------------------------------------------------
	// constructor
	//--------------------------------------------------------
	MovePicker(const Position & p, const SearchData& sd = _defaultSearchData, unsigned int ply = 0, const Move & ttm = Move::NOMOVE);
	// todo transform them into constructor? create base class and derived?
	int setupQuiescentSearch(const bool inCheck,const int depth);
	void setupProbCutSearch(bitboardIndex capturePiece);
	
	//--------------------------------------------------------
	// public methods
	//--------------------------------------------------------
	// todo ha senso ?
	bool isKillerMove( Move &m ) const;
	// todo return type const move&
	Move getNextMove(void);
private:

	//--------------------------------------------------------
	// static const
	//--------------------------------------------------------
	static const int MAX_MOVE_PER_POSITION = 250;
	static const int MAX_BAD_MOVE_PER_POSITION = 32;
	
	static const SearchData _defaultSearchData;
	
	//--------------------------------------------------------
	// enum
	//--------------------------------------------------------
	enum eStagedGeneratorState
	{
		getTT,
		generateCaptureMoves,
		iterateGoodCaptureMoves,
		getKillers,
		getCounters,
		generateQuietMoves,
		iterateQuietMoves,
		iterateBadCaptureMoves,
		finishedNormalStage,

		getTTevasion,
		generateCaptureEvasionMoves,
		iterateCaptureEvasionMoves,
		generateQuietEvasionMoves,
		iterateQuietEvasionMoves,
		finishedEvasionStage,

		getQsearchTT,
		generateQuiescentMoves,
		iterateQuiescentMoves,
		finishedQuiescentStage,

		getProbCutTT,
		generateProbCutCaptures,
		iterateProbCutCaptures,
		finishedProbCutStage,

		getQsearchTTquiet,
		generateQuiescentCaptures,
		iterateQuiescentCaptures,
		generateQuietCheks,
		iterateQuietChecks,
		finishedQuiescentQuietStage,

	};

	//--------------------------------------------------------
	// private members
	//--------------------------------------------------------
	eStagedGeneratorState stagedGeneratorState;
	
	MoveList<MAX_MOVE_PER_POSITION> _moveList;
	MoveList<MAX_BAD_MOVE_PER_POSITION> _badCaptureList;
	
	unsigned int _killerPos;
	Score _captureThreshold;
	
	const Position &_pos;
	const SearchData &_sd;
	
	unsigned int _ply;
	Move _ttMove;
	
	Move _killerMoves[2];
	Move _counterMoves[2];
	
	//--------------------------------------------------------
	// private methods
	//--------------------------------------------------------
	void _scoreCaptureMoves();
	void _scoreQuietMoves();
	void _scoreQuietEvasion();

};

#endif