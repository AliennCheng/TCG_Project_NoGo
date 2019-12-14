//
// Created by Alienn Cheng on 2019/12/1.
//

#ifndef NOGO_BOARD_H
#define NOGO_BOARD_H
#include "bitboard.h"
#include <map>
#define BLACK 0
#define WHITE 1
using namespace std;

class board {
public:
    bitboard bboard[2];
    bitboard ban[2];
    //bitboard liberty[BOARDSIZE];
    map<int, bitboard*> libertyMap;

    void deleteBoard();
    void add(int where, bool who);
    void combineLiberty (int A, int B, bool who);
    void removeLiberty(int where, int removal, bool who);
    void removeLiberty(int where, int removal);
    void banSuicide(int where);
    bool whoseTurn();
    bool checkLegal(int where, bool who);
    vector<int> getBlock(int value);
    void showboard();
    bool isEmpty();
    int genStupidMove(int who);
    void clear();

    board() {
        for (int i = 0; i < BOARDSIZE; i++) {
            //liberty[i].initLiberty(i);
            bitboard *tmp = new bitboard(i);
            libertyMap.insert(pair<int, bitboard*>(i, tmp));
        }
    }

    void deleteBoard() {
        for (int i = 0; i < BOARDSIZE; i++) {
            if (libertyMap[i])
                free(libertyMap[i]);
        }
    }

    void add(int where, bool who) {
        ban[0].addB(where);
        ban[1].addB(where);
        bboard[who].addB(where);

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
    }

    void combineLiberty (int A, int B, bool who) {
        if (libertyMap[A]->getAll() == libertyMap[B]->getAll()) return;
        vector<int> block = getBlock(B);
        libertyMap[A] -> minus(B);
        libertyMap[B] -> minus(A);
        (*libertyMap[A]) |= (*libertyMap[B]);
        free(libertyMap[B]);
        for (int i = 0; i < block.size(); ++i) {
            libertyMap[block[i]] = libertyMap[A];
        }

        if (libertyMap[A] -> count() == 1) {
            int lastLib = libertyMap[A] -> getLastLiberty();
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
            int lastLib = libertyMap[where] -> getLastLiberty();
            ban[who].addB(lastLib);
        }
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
        int blackNeighbor = 0, whiteNeighbor = 0;
        bool blackSurvive = 0, whiteSurvive = 0;
        if (where >= BOARDROW) { // not top row
            if (bboard[BLACK].get(where - BOARDROW)) {
                blackNeighbor += 1;
                if (libertyMap[where - BOARDROW]->getLastLiberty() < 0)
                    blackSurvive = 1;
            } else if (bboard[WHITE].get(where - BOARDROW)) {
                whiteNeighbor += 1;
                if (libertyMap[where - BOARDROW]->getLastLiberty() < 0)
                    whiteSurvive = 1;
            }
        }
        if (where < BOARDSIZE - BOARDROW) {
            if (bboard[BLACK].get(where + BOARDROW)) {
                blackNeighbor += 1;
                if (libertyMap[where + BOARDROW]->getLastLiberty() < 0)
                    blackSurvive = 1;
            } else if (bboard[WHITE].get(where + BOARDROW)) {
                whiteNeighbor += 1;
                if (libertyMap[where + BOARDROW]->getLastLiberty() < 0)
                    whiteSurvive = 1;
            }
        }
        if (where % BOARDROW != 0) {
            if (bboard[BLACK].get(where - 1)) {
                blackNeighbor += 1;
                if (libertyMap[where - 1]->getLastLiberty() < 0)
                    blackSurvive = 1;
            } else if (bboard[WHITE].get(where - 1)) {
                whiteNeighbor += 1;
                if (libertyMap[where - 1]->getLastLiberty() < 0)
                    whiteSurvive = 1;
            }
        }
        if ((where+1) % BOARDROW != 0) {
            if (bboard[BLACK].get(where + 1)) {
                blackNeighbor += 1;
                if (libertyMap[where + 1]->getLastLiberty() < 0)
                    blackSurvive = 1;
            } else if (bboard[WHITE].get(where + 1)) {
                whiteNeighbor += 1;
                if (libertyMap[where + 1]->getLastLiberty() < 0)
                    whiteSurvive = 1;
            }
        }
        if (blackNeighbor == 4) { ban[WHITE].addB(where); }
        if (whiteNeighbor == 4) { ban[BLACK].addB(where); }
        if (blackSurvive == 0) { ban[BLACK].addB(where); }
        if (whiteSurvive == 0) { ban[WHITE].addB(where); }
        return;
    }

    bool whoseTurn() {
        int b = bboard[BLACK].count();
        int w = bboard[WHITE].count();
        if (b == w) return BLACK;
        return WHITE;
    }

    bool checkLegal(int where, bool who) {
        if (bboard[BLACK].get(where) == 1) return false;
        if (bboard[WHITE].get(where) == 1) return false;
        if (ban[who].get(where) == 1) return false;
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
        cout << endl << "BOARD:" << endl ;
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

    int genStupidMove(int who) {
        for (int i = 0; i < BOARDSIZE; ++i)
            if (checkLegal(i, who))
                return i;
        return -1;
    }

    void clear() {
        bboard[0].clear();
        bboard[1].clear();
        ban[0].clear();
        ban[1].clear();
        libertyMap.clear();
        for (int i = 0; i < BOARDSIZE; i++) {
            bitboard *tmp = new bitboard(i);
            libertyMap.insert(pair<int, bitboard*>(i, tmp));
        }
    }
};

#endif //NOGO_BOARD_H
