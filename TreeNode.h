//
// Created by Alienn Cheng on 2019/12/14.
// Reference: https://github.com/memo/ofxMSAmcts/tree/master
//

#ifndef TREENODE_H
#define TREENODE_H
#define WIN 1
#define LOSE 0

#include <cmath>
#include "board.h"
#include "RAVE.h"

class TreeNode
{
private:
    board state;			        // the state of this TreeNode
    int action;
    TreeNode* parent;		        // parent of this TreeNode
    vector<TreeNode*> children;     // all current children
    vector<TreeNode*> notVisited;   // not visited children
    vector<int> actions;            // possible actions from this state
    float visits;			        // N; number of times TreeNode has been visited
    int wins;                       // Q
    float winrate;			        // Q/N; winrate of this TreeNode
    int depth;
    float UCT, visitsP, tmp;
    bool stupid;

public:
    TreeNode(board State, TreeNode* Parent = nullptr, int Action = -1) :
        parent(Parent),
        action(Action),
        visits(1.0),
        visitsP(1.0),
        tmp(0),
        wins(0),
        winrate(0),
        state(State),
        depth(parent == nullptr ? 0 : parent->depth + 1),
        UCT(0),
        actions(state.getLegalMoves()),
        stupid(false)
    {
        notVisited.clear();
        children.clear();
        if (action > -1) {
            state.add(action);
            if (state.libertyMap[action]->count() == 0) {
                bool color = state.whoseTurn(), occupied = true;
                if (action >= BOARDROW) { // not top
                    if (state.bboard[!color].get(action - BOARDROW)) { occupied = false; }
                }
                if (action < BOARDSIZE - BOARDROW) { // not bottom
                    if (state.bboard[!color].get(action + BOARDROW)) { occupied = false; }
                }
                if (action % BOARDROW != 0) { // not left
                    if (state.bboard[!color].get(action - 1)) { occupied = false; }
                }
                if ((action+1) % BOARDROW != 0) { // not right
                    if (state.bboard[!color].get(action + 1)) { occupied = false; }
                }
                if (occupied) stupid = true;
            }
        }
        // if (action == 0  || action == 1  || action == 2  || action == 9  || action == 10 || action == 18 ||
        //     action == 6  || action == 7  || action == 8  || action == 16 || action == 17 || action == 26 ||
        //     action == 54 || action == 63 || action == 64 || action == 72 || action == 73 || action == 74 ||
        //     action == 62 || action == 70 || action == 71 || action == 78 || action == 79 || action == 80)
        //     { visits = 2; wins = 2; }
        // else if (action == 3  || action == 4  || action == 5  || action == 11 || action == 12 || action == 14 ||
        //          action == 15 || action == 19 || action == 20 || action == 24 || action == 25 || action == 27 ||
        //          action == 28 || action == 34 || action == 35 || action == 36 || action == 44 || action == 45 ||
        //          action == 46 || action == 52 || action == 53 || action == 55 || action == 56 || action == 60 ||
        //          action == 61 || action == 65 || action == 66 || action == 68 || action == 69 || action == 75 ||
        //          action == 76 || action == 77)
        //     { visits = 1; wins = 1; }
    }

    ~TreeNode() {
        if (!notVisited.empty())
            for (TreeNode* ptr : notVisited) delete ptr;
        notVisited.clear();
        notVisited.shrink_to_fit();
        if (!children.empty())
            for (TreeNode* ptr : children) delete ptr;
        children.clear();
        children.shrink_to_fit();
        actions.clear();
        actions.shrink_to_fit();
    }

    // expand by adding children
    TreeNode* expand() {
        if (isFullyExpanded()) return nullptr;
        if (notVisited.size() == 0) {
// cout << "building NOTVISITED" << endl;
            vector<TreeNode*> stupidPlay;
            // stupidPlay.clear();
            for (int i = 0; i < actions.size(); ++i) {
                board b = state;
                TreeNode *node = new TreeNode(b, this, actions[i]);
// cout << "---~~~~" << endl;
                if (node->getStupid()) stupidPlay.push_back(node);
                else notVisited.push_back(node);
            }
// cout << "---" << endl; 
            if (notVisited.empty())
                if (!stupidPlay.empty())
                    notVisited.assign(stupidPlay.begin(), stupidPlay.end());
                else return nullptr;
            else {
                if (!stupidPlay.empty())
                    for (TreeNode* ptr : stupidPlay) delete ptr;
                stupidPlay.clear();
                stupidPlay.shrink_to_fit();
            }
// cout << "-------" << endl;
            if (notVisited.size() > 1) {
                default_random_engine rng = default_random_engine {};
                shuffle(begin(notVisited), end(notVisited), rng);
            }
        }

        children.push_back(notVisited.back());
        notVisited.pop_back();
// cout << "children size: " << children.size() << "; notVisited size: " << notVisited.size() << endl;
//cout << "expanded child" << endl;
//children.back()->getState().showboard();
        return children.back();
    }

    void update(bool result, RAVE *rave) {
// cout << "UPDATINGGGGG" << endl;
        if (result == WIN) {
            if (!stupid) {
                ++wins;
// cout << "1" << endl;
                rave->raveWinning(action);
// cout << "2" << endl;
            }
        }
        if (visits < 1) visits = 1; 
        if (wins <= 0) wins = 0;
// cout << "3" << endl;
        winrate = (float)wins / (float)visits;
// cout << "4" << endl;
        if (parent != nullptr) visitsP = parent->getNumVisits();
        if (visitsP < 1) visitsP = 1.0;
        tmp = visitsP;
// cout << "vP" << visitsP << endl;
        if (visitsP <= 1) {
// cout << "winrate" << winrate << endl;
            if (winrate > 0) UCT = winrate / 2;
            else UCT = 0;
// cout << "vP <= 1, UCT=" << UCT << endl;
        }
        else {
// cout << "winrate" << winrate << endl;
// cout << "vP" << visitsP << endl;
// cout << "visits" << visits << endl;
            if (winrate <= 0) winrate = 0;
// cout << "vP" << tmp << endl;
            if (tmp < 1) tmp = 1;
// cout << "vPnew" << tmp << endl;
            tmp = log(tmp);
// cout << "logvP" << tmp << endl;
            tmp = tmp / visits;
// cout << "logvP / v" << tmp << endl;
            tmp = sqrt(tmp);
// cout << "sqrt" << tmp << endl;
            tmp = tmp * 0.25;
// cout << "W" << tmp << endl;
            //UCT = (winrate/2) + sqrt(log(visitsP) / (float)visits) * WEIGHT;
            UCT = winrate/2 + tmp;
// cout << "vP=" << visitsP << ",logvP/v=" << log(visitsP)/visits << ",UCT=" << UCT << endl;
        }
// cout << "end of update" << endl;
    }

    board getState() { return state; }

    int getAction() { return action; }

    // check if all children have been expanded and simulated
    bool isFullyExpanded() {
// cout << "FFFchildren size:" << children.size() << " actions size:" << actions.size() << endl;
        return children.empty() == false && children.size() == actions.size(); }

    bool isTerminal() { 
// cout << "TTT" << actions.empty() << endl;
        return actions.empty(); }

    float getNumVisits() { return visits; }

    void visiting() { ++visits; }

    float getWinrate() { return winrate; }

    int getDepth() { return depth; }

    int getNumChildren() { return children.size(); }

    vector<TreeNode*> getChildren() { return children; }

    TreeNode* getParent() { return parent; }

    float getUCT() { return UCT; }

    bool getStupid() { return stupid; }
};

#endif //TREENODE_H
