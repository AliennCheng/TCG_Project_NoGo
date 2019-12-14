//
// An implement playing Game NoGo
// Created by Alienn Cheng on 2019/12/1.
//
// Reference: https://github.com/lclan1024/HaHaNoGo

#include <iostream>
#include <cstdlib>
#include <ctime>
#include "board.h"
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
            b.add(GTPstringtoint(where), color);
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
            int start, elapsed;
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
//            tree.reset(b);
//            elapsed = clock();
//            start = clock();
//            int simulationFinishedCnt = 0;
//            while(simulationFinishedCnt < simulationCnt)
//            {
//                tree.run_a_cycle();
//                tree.run_a_cycle();
//                tree.run_a_cycle();
//                tree.run_a_cycle();
//                tree.run_a_cycle();
//                tree.run_a_cycle();
//                tree.run_a_cycle();
//                tree.run_a_cycle();
//                tree.run_a_cycle();
//                tree.run_a_cycle();
//                simulationFinishedCnt +=10;
//                e=clock();
//                if(simulationFinishedCnt %10000==0)
//                {
//                    tree.show_path();
//                }
//            }
//            k = tree.root -> getbestmove();
//            ucbnode* tmp = tree.root -> childptr;
//            int best_move = (tmp+k)->place;
//            policy = tree.root->getPolicy();
//            tree.root ->show_child();
//            value = tree.root ->show_inf(k);
//            cerr<<"simulation time : "<< (double)(e-st) / 1000.0<<endl;
//            cerr<<"average deep : "<<(double)tree.total / (double)i<<endl;
//            cerr<<"total node : "<< tree.totalnode<<endl;
//            cerr<<"average speed : "<< (simulationFinishedCnt*1000) / (e-st) <<endl;
//            tree.show_path();
            if (s != "reg_genmove") { // must play
//              b.add(best_move, !b.just_play_color());
                int move = b.genRandomMove(color);
                if (move >= 0) {
                    b.add(move, color);
                    b.showboard();
                    cout << "=" << inttoGTPstring(move) << endl << endl;
                } else {
                    cout << "=resign" << endl << endl;
                }
            }
//            if(value > 0.2)
//            {
//                cout<<"="<<inttoGTPstring(best_move)<<endl<<endl;
//            }else
//            {
//                cout<<"=resign"<<endl<<endl;
//            }

//            tree.clear();

        }
//        else if (s == "policy")
//        {
//            for (int i = 0; i < 9; i++)
//            {
//                for (int j = 0; j < 9; j++)
//                {
//                    cout << policy[i * 9 + j] << ' ';
//                }
//                cout << endl;
//            }
//        }
//        else if (s == "value")
//        {
//            cout << value << endl;
//        }
        else if (s == "protocol_version")
        {
            cout<<"= 2\n\n";
        }
//        else if (s == "rev")
//        {
//            int bsize, wsize, tsize;
//            int bone[BOARDSSIZE], wone[BOARDSSIZE], two[BOARDSSIZE];
//            float x;
//            cin >> x;
//            float sum = 0;
//            for (int i = 0; i < x; i++)
//            {
//                board tmpb = b;
//                tmpb.getv(bone, wone, two, bsize, wsize, tsize);
//                sum += tmpb.simulate(!tmpb.just_play_color(), bone, wone, two, bsize, wsize, tsize);
//            }
//            cout << sum / x << endl;
//        }
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
