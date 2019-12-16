//
// An implement playing Game NoGo
// Created by Alienn Cheng on 2019/12/1.
//
// Reference: https://github.com/lclan1024/HaHaNoGo

#include <iostream>
#include <cstdlib>
#include <ctime>
#include "board.h"
#include "MCTS.h"
using namespace std;

// transfer a GTP string to an integer indicating the board place
int GTPstringtoint(string s)
{
    if(s[0]>='a'&&s[0]<='z') {
        int x=s[0]-'a';
        if(s[0]>'i')x--;
        int y=s[1]-'1';
        return x*BOARDCOL+y;
    } else {
        int x=s[0]-'A';
        if(s[0]>'I')x--;
        int y=s[1]-'1';
        return x*BOARDCOL+y;
    }
}
// transfer an integer of a specific board place to a GTP string
string inttoGTPstring(int i)
{
    string s = "A1";
    s[0] += i / 9;
    if (s[0] >= 'I') s[0]++;
    s[1] += i % 9;
    return s;
}

int main(int argc, char** argv) {
    string s, who, where;
    double myTimer = 10000;
    bool color;
    board b;

    while(cin>>s) {
        if(s=="play"||s=="p") // play by "color" on "where"
        {
            cin >> who >> where;
            if(who[0]=='b' || who[0]=='B') color = BLACK;
            else color = WHITE;
            b.add(GTPstringtoint(where));
            cout << "=" << endl << endl;
        }
        else if(s[0]=='e') // check empty
        {
            cout << b.isEmpty() << endl << endl;
        }
        else if(s[0]=='c' || s == "clear_board") // clear
        {
            b.clear();
            cout << "=" << endl << endl;
        }
        else if(s[0]=='g' || s == "reg_genmove") // g: gen a move then play
                                                 // reg_genmove: gen a move but dont play
        {
            bool finish = true;
            string c;
            cin >> c;
            color = b.whoseTurn();
            for(int i = 0; i < BOARDSIZE; i++) {
                if(b.checkLegal(i, color))
                {
                    finish = false;
                    break;
                }
            }
            if (finish) {
                cout << "=resign" << endl << endl;
                continue;
            }

            MCTS mcts;
            int action = mcts.runMCTS(&b);
cout << endl << endl << "final choise: " << inttoGTPstring(action) << endl;
//cout << "=resign" << endl << endl;
//return 0;

            if (s != "reg_genmove") { // must play
                // int move = b.getLegalMoves().front();
                int move = action;
                if (move >= 0) {
                    b.add(move);
                    //b.showboard();
                    cout << "=" << inttoGTPstring(move) << endl << endl;
                } else {
                    cout << "=resign" << endl << endl;
                }
            }
        }
        else if (s == "protocol_version")
        {
            cout<<"= 2\n\n";
        }
        else if(s== "name")
        {
            cout << "=StupidNoGo\n\n";
        }
        else if (s == "time")
        {
            cin >> myTimer;
            myTimer *= 1000;
            myTimer++;
            cout << "=" << endl << endl;
        }
        else if(s == "boardsize")
        {
            cin >> s;
            cout<<"="<<endl<<endl;
        }
        else if(s == "komi")
        {
            cin>>s;
            cout<<"="<<endl<<endl;
        }
        else if(s == "time_left")
        {
            cin>>s>>s>>s;
            cout<<"="<<endl<<endl;
        }
        else if(s == "showboard" || s == "sb")
        {
            b.showboard();
            cout<<endl;
        }
        else if(s == "quit")
        {
            b.deleteBoard();
            cout << "=" << endl << endl;
            return 0;
        }
        else
        {
            cout<<"="<<endl<<endl;
        }
    }

    return 0;
}
