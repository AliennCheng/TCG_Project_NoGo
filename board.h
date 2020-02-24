//
// Created by Alienn Cheng on 2019/12/1.
//

#ifndef BOARD_H
#define BOARD_H
#include "bitboard.h"
#include <map>
#include <random>
#include <algorithm>
#define BLACK 0
#define WHITE 1
using namespace std;

class board {
public:
    bitboard bboard[2];
    bitboard ban[2];
    //bitboard liberty[BOARDSIZE];
    map<int, bitboard*> libertyMap;
    default_random_engine rng;

//    void deleteBoard();
//    void add(int where, bool who);
//    void combineLiberty (int A, int B, bool who);
//    void removeLiberty(int where, int removal, bool who);
//    void removeLiberty(int where, int removal);
//    void banSuicide(int where);
//    bool whoseTurn();
//    bool checkLegal(int where, bool who);
//    vector<int> getBlock(int value);
//    void showboard();
//    bool isEmpty();
//    int genStupidMove(int who);
//    int genRandomMove(int who);
//    void clear();

    board() {
        rng = default_random_engine {};
        for (int i = 0; i < BOARDSIZE; i++) {
            //liberty[i].initLiberty(i);
            bitboard *tmp = new bitboard(i);
            libertyMap.insert(pair<int, bitboard*>(i, tmp));
        }
    }

    board(const board& other) {
        rng = default_random_engine {};
        bboard[BLACK] = bitboard(other.bboard[BLACK]);
        bboard[WHITE] = bitboard(other.bboard[WHITE]);
        ban[BLACK] = bitboard(other.ban[BLACK]);
        ban[WHITE] = bitboard(other.ban[WHITE]);

        // deep copy libertyMap
        libertyMap.clear();
        map<int, bitboard*>::const_iterator it = other.libertyMap.begin();
        while(it != other.libertyMap.end()) {
            libertyMap[it->first] = new bitboard(*(it->second));
            ++it;
        }

    }

    void deleteBoard() {
        for (int i = 0; i < BOARDSIZE; i++) {
            delete libertyMap[i];
                // free(libertyMap[i]);
        }
    }

    void add(int where) {
        bool who = whoseTurn();
//cout << "adding " << where << " by " << who << endl;
        ban[BLACK].addB(where);
        ban[WHITE].addB(where);
        bboard[who].addB(where);
//showboard();

        // update liberty
        if (where >= BOARDROW) { // not top row
            if (bboard[who].get(where - BOARDROW))    // same color
                combineLiberty(where, where - BOARDROW, who);
            else if (bboard[!who].get(where - BOARDROW)) // opposite color
                removeLiberty(where - BOARDROW, where, who);
            else                                         // no color
                removeLiberty(where - BOARDROW, where);
        }
        if (where < BOARDSIZE - BOARDROW) { // not bottom row
            if (bboard[who].get(where + BOARDROW))
                combineLiberty(where, where + BOARDROW, who);
            else if (bboard[!who].get(where + BOARDROW))
                removeLiberty(where + BOARDROW, where, who);
            else
                removeLiberty(where + BOARDROW, where);
        }
        if (where % BOARDROW != 0) {// not left col
            if (bboard[who].get(where - 1))
                combineLiberty(where, where - 1, who);
            else if (bboard[!who].get(where - 1))
                removeLiberty(where - 1, where, who);
            else
                removeLiberty(where - 1, where);
        }
        if ((where+1) % BOARDROW != 0) {// not right col
            if (bboard[who].get(where + 1))
                combineLiberty(where, where + 1, who);
            else if (bboard[!who].get(where + 1))
                removeLiberty(where + 1, where, who);
            else
                removeLiberty(where + 1, where);
        }

        if (libertyMap[where]->count() == 1)
            checkNotSafe(where);
    }

    void combineLiberty (int A, int B, bool who) {
        if (libertyMap[A]->getAll() == libertyMap[B]->getAll()) return;
        vector<int> block = getBlock(B);
        libertyMap[A] -> minus(B);
        libertyMap[B] -> minus(A);
        (*libertyMap[A]) |= (*libertyMap[B]);
        delete libertyMap[B];
        // free(libertyMap[B]);
        for (int i = 0; i < block.size(); ++i) {
            libertyMap[block[i]] = libertyMap[A];
        }

        if (libertyMap[A] -> count() <= 1) {
            int lastLib = libertyMap[A] -> getLastLiberty();
            if (lastLib < 0) return;
            ban[!who].addB(lastLib); // ban capturing
            if (libertyMap[lastLib] -> count() == 0) { // ban suicide
                banSuicide(lastLib);
            }
        }
    }

    // where indicates the intersection being checked
    // removal indicates where the new piece placed
    // who indicates whom the new piece belong to (removal side)
    void removeLiberty(int where, int removal, bool who) {
        libertyMap[where] -> minus(removal);
        // ban capturing
        if (libertyMap[where]->count() == 1) {
            // ban suicide
            int eye = libertyMap[where]->getLastLiberty();
            if (libertyMap[eye]->count() == 0) {
                bool notsafe = true;
                if (eye >= BOARDROW && bboard[!who].get(eye - BOARDROW) &&
                        libertyMap[eye - BOARDROW]->count() > 1)
                    notsafe = false;
                if (eye < BOARDSIZE - BOARDROW && bboard[!who].get(eye + BOARDROW) &&
                        libertyMap[eye + BOARDROW]->count() > 1)
                    notsafe = false;
                if (eye % BOARDROW != 0 && bboard[!who].get(eye - 1) &&
                        libertyMap[eye - 1]->count() > 1)
                    notsafe = false;
                if ((eye+1) % BOARDROW != 0 && bboard[!who].get(eye + 1) &&
                        libertyMap[eye + 1]->count() > 1)
                    notsafe = false;
                if (notsafe) ban[!who].addB(eye);
            }
            // ban capturing
            int lastLib = libertyMap[where] -> getLastLiberty();
            ban[who].addB(lastLib);
        }
        // ban suicide
        if (libertyMap[removal]->count() == 1) {
            int lastLib = libertyMap[removal] -> getLastLiberty();
            ban[!who].addB(lastLib);
        }
    }

    // where indicates the intersection being checked
    // removal indicates where the new piece placed
    void removeLiberty(int where, int removal) { // not occupied; ban suicide
        libertyMap[where] -> minus(removal);
        if (libertyMap[where]->count() > 0) return;
        banSuicide(where);
    }

    void banSuicide(int where) {
        if (libertyMap[where]->count() > 0) return;

        bool safe = false;
        // for BLACK
        if (where >= BOARDROW) { // not top row
            if (bboard[BLACK].get(where - BOARDROW) && libertyMap[where - BOARDROW]->count() > 1)
                safe = true;
        }
        if (where < BOARDSIZE - BOARDROW) {
            if (bboard[BLACK].get(where + BOARDROW) && libertyMap[where + BOARDROW]->count() > 1)
                safe = true;
        }
        if (where % BOARDROW != 0) {
            if (bboard[BLACK].get(where - 1) && libertyMap[where - 1]->count() > 1)
                safe = true;
        }
        if ((where+1) % BOARDROW != 0) {
            if (bboard[BLACK].get(where + 1) && libertyMap[where + 1]->count() > 1)
                safe = true;
        }
        if (!safe) ban[BLACK].addB(where);

        safe = false;
        if (where >= BOARDROW) { // not top row
            if (bboard[WHITE].get(where - BOARDROW) && libertyMap[where - BOARDROW]->count() > 1)
                safe = true;
        }
        if (where < BOARDSIZE - BOARDROW) {
            if (bboard[WHITE].get(where + BOARDROW) && libertyMap[where + BOARDROW]->count() > 1)
                safe = true;
        }
        if (where % BOARDROW != 0) {
            if (bboard[WHITE].get(where - 1) && libertyMap[where - 1]->count() > 1)
                safe = true;
        }
        if ((where+1) % BOARDROW != 0) {
            if (bboard[WHITE].get(where + 1) && libertyMap[where + 1]->count() > 1)
                safe = true;
        }
        if (!safe) ban[WHITE].addB(where);
    }

    void checkNotSafe(int where) {
        if (libertyMap[where]->count() != 1) return;

        bool color;
        if (bboard[BLACK].get(where)) color = BLACK;
        else if (bboard[WHITE].get(where)) color = WHITE;
        else return;

        int lastLibWhere = -1;
        if (where >= BOARDROW) { // not top row
            if (!bboard[color].get(where - BOARDROW) && !bboard[!color].get(where - BOARDROW))
                lastLibWhere = where - BOARDROW;
            else if (!bboard[!color].get(where - BOARDROW)) return;
        }
        if (where < BOARDSIZE - BOARDROW) {
            if (!bboard[color].get(where + BOARDROW) && !bboard[!color].get(where + BOARDROW))
                lastLibWhere = where + BOARDROW;
            else if (!bboard[!color].get(where + BOARDROW)) return;
        }
        if (where % BOARDROW != 0) {
            if (!bboard[color].get(where - 1) && !bboard[!color].get(where - 1))
                lastLibWhere = where - 1;
            else if (!bboard[!color].get(where - 1)) return;
        }
        if ((where+1) % BOARDROW != 0) {
            if (!bboard[color].get(where + 1) && !bboard[!color].get(where + 1))
                lastLibWhere = where + 1;
            else if (!bboard[!color].get(where + 1)) return;
        }
        ban[!color].addB(lastLibWhere);
    }
    // void banSuicide(int where) {
    //     int blackNeighbor = 0, whiteNeighbor = 0;
    //     bool blackSurvive = 0, whiteSurvive = 0;
    //     if (where >= BOARDROW) { // not top row
    //         if (bboard[BLACK].get(where - BOARDROW)) {
    //             blackNeighbor += 1;
    //             if (libertyMap[where - BOARDROW]->getLastLiberty() < 0)
    //                 blackSurvive = 1;
    //         } else if (bboard[WHITE].get(where - BOARDROW)) {
    //             whiteNeighbor += 1;
    //             if (libertyMap[where - BOARDROW]->getLastLiberty() < 0)
    //                 whiteSurvive = 1;
    //         } else return;
    //     }
    //     if (where < BOARDSIZE - BOARDROW) {
    //         if (bboard[BLACK].get(where + BOARDROW)) {
    //             blackNeighbor += 1;
    //             if (libertyMap[where + BOARDROW]->getLastLiberty() < 0)
    //                 blackSurvive = 1;
    //         } else if (bboard[WHITE].get(where + BOARDROW)) {
    //             whiteNeighbor += 1;
    //             if (libertyMap[where + BOARDROW]->getLastLiberty() < 0)
    //                 whiteSurvive = 1;
    //         } else return;
    //     }
    //     if (where % BOARDROW != 0) {
    //         if (bboard[BLACK].get(where - 1)) {
    //             blackNeighbor += 1;
    //             if (libertyMap[where - 1]->getLastLiberty() < 0)
    //                 blackSurvive = 1;
    //         } else if (bboard[WHITE].get(where - 1)) {
    //             whiteNeighbor += 1;
    //             if (libertyMap[where - 1]->getLastLiberty() < 0)
    //                 whiteSurvive = 1;
    //         } else return;
    //     }
    //     if ((where+1) % BOARDROW != 0) {
    //         if (bboard[BLACK].get(where + 1)) {
    //             blackNeighbor += 1;
    //             if (libertyMap[where + 1]->getLastLiberty() < 0)
    //                 blackSurvive = 1;
    //         } else if (bboard[WHITE].get(where + 1)) {
    //             whiteNeighbor += 1;
    //             if (libertyMap[where + 1]->getLastLiberty() < 0)
    //                 whiteSurvive = 1;
    //         } else return;
    //     }
    //     if (blackNeighbor == 4) { ban[WHITE].addB(where); }
    //     if (whiteNeighbor == 4) { ban[BLACK].addB(where); }
    //     if (blackSurvive == 0) { ban[BLACK].addB(where); }
    //     if (whiteSurvive == 0) { ban[WHITE].addB(where); }
    //     return;
    // }

    bool whoseTurn() {
        int b = bboard[BLACK].count();
        int w = bboard[WHITE].count();
        if (b == w) return BLACK;
        return WHITE;
    }

    bool checkLegal(int where, bool who) {
        // if (bboard[BLACK].get(where) == 1) return false;
        // if (bboard[WHITE].get(where) == 1) return false;
        if (ban[who].get(where)) return false;
        return true;
    }

    vector<int> getBlock(int value) {
        auto it = libertyMap.begin();
        vector<int> tmp;
        while(it != libertyMap.end()) {
            if(it->second == libertyMap[value]) {
                tmp.push_back(it->first);
            }
            it++;
        }
        return tmp;
    }

    void showboard() {
        cout << endl << "-----------------SHOW BOARD--------------" << endl;
        cout << "BOARD:" << endl ;
        for(int i = 0; i < BOARDSIZE; i++) {
            if(bboard[BLACK].get(i))
                cout<<'@';
            else if(bboard[WHITE].get(i))
                cout<<'O';
            else
                cout<<'.';
            if (i % BOARDROW == BOARDROW - 1)
                cout<<endl;
        }
        cout << endl << "BAN[BLACK]:" << endl ;
        for(int i = 0; i < BOARDSIZE; i++) {
            if(ban[BLACK].get(i))
                cout<<'x';
            else
                cout<<'.';
            if (i % BOARDROW == BOARDROW - 1)
                cout<<endl;
        }
        cout << endl << "BAN[WHITE]:" << endl ;
        for(int i = 0; i < BOARDSIZE; i++) {
            if(ban[WHITE].get(i))
                cout<<'x';
            else
                cout<<'.';
            if (i % BOARDROW == BOARDROW - 1)
                cout<<endl;
        }
        cout << "------------------------------------" << endl;
    }

    bool isEmpty() {
        return (bboard[BLACK].isempty()) && (bboard[WHITE].isempty());
    }

    int genStupidMove(bool who) {
        for (int i = 0; i < BOARDSIZE; ++i)
            if (checkLegal(i, who))
                return i;
        return -1;
    }

    vector<int> getLegalMoves() {
        bool who = whoseTurn();
        vector<int> legalMoves;
        for (int i = 0; i < BOARDSIZE; ++i) {
            if (checkLegal(i, who))
                legalMoves.push_back(i);
        }
        if (legalMoves.size() > 1) shuffle(begin(legalMoves), end(legalMoves), rng);
        return legalMoves;
    }

    bool isTerminal() {
        vector<int> legal = getLegalMoves();
        if (legal.size() == 0) {
            legal.clear();
            legal.shrink_to_fit();
            return true;
        } else {
            legal.clear();
            legal.shrink_to_fit();
            return false;
        }
    }

    void clear() {
        bboard[0].clear();
        bboard[1].clear();
        ban[0].clear();
        ban[1].clear();
// cout << "clearing" << endl;
        for (int i = 0; i < libertyMap.size(); ++i) {
            if (libertyMap[i] != nullptr) {
                vector<int> vec = getBlock(i);
                delete libertyMap[i];
                for (int j : vec) libertyMap[j] = nullptr;
                vec.clear();
                vec.shrink_to_fit();
            }
        }
// cout << "end of clear" << endl;
        libertyMap.clear();
        for (int i = 0; i < BOARDSIZE; i++) {
            bitboard *tmp = new bitboard(i);
            libertyMap.insert(pair<int, bitboard*>(i, tmp));
        }
    }

    int heuristicPlay(bool color) {
        for (int i = 0; i < BOARDSIZE; ++i) {
            // if there's an intersection with only 1 liberty is surrounded by the opponent,
            // occupy it to take advantage of "no taking"
            // if there's an intersection with only 1 liberty is surrounded by myself,
            // occupy its last liberty to take advantage of "no suicide"
            if (libertyMap[i]->count() == 1) {
                if (bboard[BLACK].get(i) || bboard[WHITE].get(i)) continue;

                int opp = 0, me = 0, action = -1;
                if (i >= BOARDROW) { // not top
                    if (bboard[color].get(i - BOARDROW)) { ++me; }
                    else if (bboard[!color].get(i - BOARDROW)) { ++opp; }
                    else { action = i - BOARDROW; }
                }
                if (i < BOARDSIZE - BOARDROW) { // not bottom
                    if (bboard[color].get(i + BOARDROW)) { ++me; }
                    else if (bboard[!color].get(i + BOARDROW)) { ++opp; }
                    else { action = i + BOARDROW; }
                }
                if (i % BOARDROW != 0) { // not left
                    if (bboard[color].get(i - 1)) { ++me; }
                    else if (bboard[!color].get(i - 1)) { ++opp; }
                    else { action = i - 1; }
                }
                if ((i+1) % BOARDROW != 0) { // not right
                    if (bboard[color].get(i + 1)) { ++me; }
                    else if (bboard[!color].get(i + 1)) { ++opp; }
                    else { action = i + 1; }
                }

                if (opp > 0 && me == 0) {
                    if (checkLegal(i, color)) return i;
                    else continue;
                } else if (opp == 0 && me > 0) {
                    if (checkLegal(action, color)) return action;
                    else continue;
                }
                continue;
            }
        }

        for (int i = 0; i < BOARDSIZE; ++i) {
            if (libertyMap[i]->count() == 2) {
                int counter = 0, action = -1;
                if (bboard[BLACK].get(i) || bboard[WHITE].get(i)) continue;

                if (i >= BOARDROW) { // not top
                    if (bboard[color].get(i - BOARDROW)) ++counter;
                    else if (bboard[!color].get(i - BOARDROW)) continue;
                    else action = i - BOARDROW;
                } else { ++counter; }

                if (i < BOARDSIZE - BOARDROW) { // not bottom
                    if (bboard[color].get(i + BOARDROW)) ++counter;
                    else if (bboard[!color].get(i + BOARDROW)) continue;
                    else action = i + BOARDROW;
                } else { ++counter; }

                if (i % BOARDROW != 0) { // not left
                    if (bboard[color].get(i - 1)) ++counter;
                    else if (bboard[!color].get(i - 1)) continue;
                    else action = i - 1;
                } else { ++counter; }

                if ((i+1) % BOARDROW != 0) { // not right
                    if (bboard[color].get(i + 1)) ++counter;
                    else if (bboard[!color].get(i + 1)) continue;
                    else action = i + 1;
                } else { ++counter; }

                if (counter == 2) {
                    if (checkLegal(action, color)) return action;
                    else continue;
                }
            }
        }
        return -1;
    }
};

#endif //BOARD_H
