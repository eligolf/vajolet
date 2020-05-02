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
#include <cmath>

#include <cassert>

#include "parameters.h"


EvalParameters::EvalParameters()
{
	_initValues();
}

void EvalParameters::updateValues()
{
	_initValues();
}

void EvalParameters::_initValues() {
	for(auto &val: _pieceValue)
	{
		val = simdScore{0,0,0,0};
	}
	_pieceValue[whitePawns] = initialPieceValue[whitePawns];
	_pieceValue[whiteKnights] = initialPieceValue[whiteKnights];
	_pieceValue[whiteBishops] = initialPieceValue[whiteBishops];
	_pieceValue[whiteRooks] = initialPieceValue[whiteRooks];
	_pieceValue[whiteQueens] = initialPieceValue[whiteQueens];
	_pieceValue[whiteKing] = initialPieceValue[whiteKing];

	_pieceValue[blackPawns] = _pieceValue[whitePawns];
	_pieceValue[blackKnights] = _pieceValue[whiteKnights];
	_pieceValue[blackBishops] = _pieceValue[whiteBishops];
	_pieceValue[blackRooks] = _pieceValue[whiteRooks];
	_pieceValue[blackQueens] = _pieceValue[whiteQueens];
	_pieceValue[blackKing] = _pieceValue[whiteKing];
	
	_initPstValues();
}

void EvalParameters::_initPstValues()
{
	/* PST data */
	const int Center[8]	= { -3, -1, +0, +1, +1, +0, -1, -3};
	const int KFile[8]	= { +3, +4, +2, +0, +0, +2, +4, +3};
	const int KRank[8]	= { +1, +0, -2, -3, -4, -5, -6, -7};
	for(bitboardIndex piece = occupiedSquares; piece < lastBitboard; ++piece)
	{
		for(tSquare s = A1; s < squareNumber; ++s)
		{
			assert(s<squareNumber);
			_nonPawnValue[piece] = simdScore{0,0,0,0};
			_pstValue[piece][s] = simdScore{0,0,0,0};
			tRank rank = getRankOf(s);
			tFile file = getFileOf(s);

			if (isValidPiece(piece)) {
				if (!isBlackPiece(piece)) {
					if (isPawn(piece)) {
						_pstValue[piece][s] = simdScore{0,0,0,0};
						if (s == D3) {
							_pstValue[piece][s] = PawnD3;
						} else if (s == D4) {
							_pstValue[piece][s] = PawnD4;
						} else if (s == D5) {
							_pstValue[piece][s] = PawnD5;
						} else if (s == E3) {
							_pstValue[piece][s] = PawnE3;
						} else if (s == E4)	{
							_pstValue[piece][s] = PawnE4;
						} else if (s == E5)	{
							_pstValue[piece][s] = PawnE5;
						}
						_pstValue[piece][s] += PawnRankBonus * static_cast<int>(rank - 2);
						_pstValue[piece][s] += Center[file] * PawnCentering;
					} else if (isKnight(piece)) {
						_pstValue[piece][s] = KnightPST * (Center[file] + Center[rank]);
						if (rank == RANK1) {
							_pstValue[piece][s] -= KnightBackRankOpening;
						}
					} else if (isBishop(piece))	{
						_pstValue[piece][s] = BishopPST * (Center[file] + Center[rank]);
						if (rank == RANK1) {
							_pstValue[piece][s] -= BishopBackRankOpening;
						}
						if (((int)file == (int)rank) || (file + rank == 7)) {
							_pstValue[piece][s] += BishopOnBigDiagonals;
						}
					} else if (isRook(piece)) {
						_pstValue[piece][s] = RookPST * Center[file];
						if (rank == RANK1) {
							_pstValue[piece][s] -= RookBackRankOpening;
						}
					} else if (isQueen(piece)) {
						_pstValue[piece][s] = QueenPST * (Center[file] + Center[rank]);
						if (rank == RANK1) {
							_pstValue[piece][s] -= QueenBackRankOpening;
						}
					} else if (isKing(piece)) {
						_pstValue[piece][s] = simdScore{
								(KFile[file]+KRank[rank]) * KingPST[0],
								(Center[file]+Center[rank]) * KingPST[1],
								0,0};
					}
					// add piece value to pst
					if(!isKing( piece ) )
					{
						_pstValue[piece][s] += _pieceValue[piece];
					}

					if (!isPawn(piece) && !isKing(piece)) {
						_nonPawnValue[piece][0] = _pieceValue[piece][0];
						_nonPawnValue[piece][1] = _pieceValue[piece][1];
					}

				} else {
					tRank r = getRelativeRankOf(s, black);
					tFile f = file;
					_pstValue[piece][s] = -_pstValue[piece - separationBitmap][getSquare(f, r)];

					if(!isPawn(piece) && !isKing(piece)) {
						_nonPawnValue[piece][2] = _pieceValue[piece][0];
						_nonPawnValue[piece][3] = _pieceValue[piece][1];
					}
				}
			} else {
				_pstValue[piece][s] = simdScore{0,0,0,0};
			}
		}
	}

}

SearchParameters::SearchParameters():
razorMargin(19469),
razorMarginDepth(0),
razorMarginCut(0),
razorDepth(67),
razorReturn(false),

staticNullMovePruningDepth(129),
staticNullMovePruningValue(365),
staticNullMovePruningImprovingBonus(2142),

nullMovePruningDepth(17),
nullMovePruningReduction(53),
nullMovePruningBonusDepth(0.25),
nullMovePruningBonusThreshold(9393),
nullMovePruningBonusAdditionalRed(16),
nullMovePruningVerificationDepth(196),

probCutDepth(85),
probCutDelta(8164),
probCutDepthRed(47),

iidDepthPv(79),
iidDepthNonPv(132),
iidStaticEvalBonus(9576),
iidDepthRed(32),
iidDepthRedFactor(4),

singularExtensionPVDepth(96),
singularExtensionNonPVDepth(125),
singularExtensionTtDepth(48),
singularExtensionScoreDepthBonus(20),
singularExtensionExt(16),

dangerousMoveExtension(16),
checkMoveExtension(8),

FutilityMoveCountsDepth(16),

futilityDepth(112),

negativeSeeSkipDepth(64),

lmrDepthLimitInf(48)
{
	_updatePars();
}

void SearchParameters::_updatePars() 
{
	/***************************************************
	 * FUTILITY MOVE COUNT
	 ***************************************************/
	for (unsigned int d = 0; d < 32; ++d)
	{
		FutilityMoveCounts[0][d] = int(2.52 + 0.704 * std::pow( d, 1.8));
		FutilityMoveCounts[1][d] = int(4.5 + 0.704 * std::pow( d, 2.0));
	}

	/***************************************************
	 * FUTILITY
	 ***************************************************/
	for (unsigned int d = 0; d < 12; ++d)
	{
		futilityMargin[d] = 10000 + d * 10000;
	}
	/***************************************************
	 * LRM
	 ***************************************************/

	for (unsigned int mc = 1; mc < LmrLimitMove; ++mc)
	{
		PVreduction[0][0][mc] = 0;
		nonPVreduction[1][0][mc] = 0;
	}
	for (unsigned int d = 1; d < LmrLimit * 16; ++d)
	{
		for (unsigned int mc = 1; mc < LmrLimitMove; ++mc)
		{
			double dd = (16.0 * d) / 16;
			
			double    PVRed = -1.5 + 0.33 * log(dd) * log(double(mc));
			double nonPVRed = -1.2 + 0.37 * log(dd) * log(double(mc));

			PVreduction[1][d][mc] = (Score)(PVRed >= 1.0 ? floor(PVRed * int(16)) : 0);
			nonPVreduction[1][d][mc] = (Score)(nonPVRed >= 1.0 ? floor(nonPVRed * int(16)) : 0);

			PVreduction[0][d][mc] = PVreduction[1][d][mc];
			nonPVreduction[0][d][mc] = nonPVreduction[1][d][mc];

			if(    PVreduction[0][d][mc] > int(16) ){    PVreduction[0][d][mc] += int(16); }
			if( nonPVreduction[0][d][mc] > int(16) ){ nonPVreduction[0][d][mc] += int(16); }
		}
	}
}