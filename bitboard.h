//
// Created by Alienn Cheng on 2019/12/1.
// A board has 81 intersections which can be represented by 2 64-bit bitboard.
//

#ifndef BITBOARD_H
#define BITBOARD_H
#define BOARDCOL 9
#define BOARDROW 9
#define BOARDSIZE 81
#include <iostream>
#include <vector>
using namespace std;

class bitboard {
public:
    unsigned long long x[(BOARDSIZE/64) + 1];

    bitboard() {
        x[0] = 0LL;
        x[1] = 0LL;
    }

    bitboard(const bitboard& other) {
        x[0] = other.x[0];
        x[1] = other.x[1];
    }

    //void initLiberty(int i) { // liberty init
    bitboard(int i) { // liberty init
        x[0] = 0LL;
        x[1] = 0LL;
        if (i >= BOARDROW) // not top row
            addB(i - BOARDROW);
        if (i < BOARDSIZE - BOARDROW) // not bottom row
            addB(i + BOARDROW);
        if (i % BOARDROW != 0) // not left col
            addB(i - 1);
        if ((i+1) % BOARDROW != 0) // not right col
            addB(i + 1);
    }

    void clear() {
        x[0] = 0; x[1] = 0;
    }

    // count the number of occupied intersections
    int count() {
        unsigned long long v,v1;
        v = (x[0] & 0x5555555555555555LL)+( (x[0]>>1) &0x5555555555555555LL);
        v1 = (x[1] & 0x5555555555555555LL)+( (x[1]>>1) &0x5555555555555555LL);
        v = (v & 0x3333333333333333LL)+( (v>>2) &0x3333333333333333LL);
        v1 = (v1 & 0x3333333333333333LL)+( (v1>>2) &0x3333333333333333LL);
        v += v1;
        v = (v & 0x0f0f0f0f0f0f0f0fLL)+( (v>>4) &0x0f0f0f0f0f0f0f0fLL);
        v = (v & 0x00ff00ff00ff00ffLL)+( (v>>8) &0x00ff00ff00ff00ffLL);
        v = (v & 0x0000ffff0000ffffLL)+( (v>>16) &0x0000ffff0000ffffLL);
        return (int)( (v & 0x00000000fffffffffLL)+( (v>>32) &0x00000000ffffffffLL) );
    }

    // check whether the specific intersection occupied
    bool get(int i) {
        return (x[i>>6] & (1LL<<(i&63))) != 0;
    }

    // remove a bit
    void minus(int i) {
        x[i>>6] &= ~ (1LL << (i &63));
    }

    // add a bit
    void addB(int i) {
        x[i>>6] |= (1LL<<(i & 63));
    }

    // return the last liberty if only 1 liberty last
    int getLastLiberty() {
        if (count() != 1) return -1;
        if (x[0] == 0LL) {
            unsigned long long tmp = x[1];
            for (int i = 0; i < 81 - 64; i++) {
                if(tmp & 1) return 64 + i;
                tmp = tmp >> 1;
            }
        } else {
            unsigned long long tmp = x[0];
            for (int i = 0; i < 64; i++) {
                if (tmp & 1) return i;
                tmp = tmp >> 1;
            }
        }
        return -1;
    }

    // OR 2 boards
    void operator|= (bitboard b) {
        x[0] |= b.x[0];
        x[1] |= b.x[1];
        return;
    }

    vector<int> getAll() {
        vector<int> tmp;
        for(int i = 0; i < BOARDSIZE; i++) {
            if (get(i))
                tmp.push_back(i);
        }
        return tmp;
    }

    void print() {
        cout << "------PRINTING-------" << endl;
        for(int i = 0; i < BOARDSIZE; i++) {
            if(get(i)) { // i occupied
                 cout << 'O';
            } else {
                cout << 'X';
            }
            if (i % BOARDCOL == BOARDCOL-1) {
                cout<<endl;
            }
        }
        cout << "----------------------" << endl;
    }

    bool isempty() {
        return (x[0]==0) && (x[1]==0);
    }
};

#endif //BITBOARD_H
