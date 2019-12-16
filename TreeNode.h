//
// Created by Alienn Cheng on 2019/12/14.
// Reference: https://github.com/memo/ofxMSAmcts/tree/master
//

#ifndef STUPIDNOGO_TREENODE_H
#define STUPIDNOGO_TREENODE_H
#define WIN 1
#define LOSE 0

#include "board.h"

class TreeNode
{
private:
    board state;			        // the state of this TreeNode
    int action;
    TreeNode* parent;		        // parent of this TreeNode
    vector<TreeNode*> children;     // all current children
    vector<TreeNode*> notVisited;   // not visited children
    vector<int> actions;            // possible actions from this state
    int visits;			            // number of times TreeNode has been visited
    int wins;
    float winrate;			        // winrate of this TreeNode
    int depth;

public:
    TreeNode(board* State, TreeNode* Parent = NULL, int Action = -1) :
        parent(Parent),
        action(Action),
        visits(0),
        wins(0),
        winrate(0),
        state(*State),
        depth(parent ? parent->depth + 1 : 0),
        actions(state.getLegalMoves())
    {
        notVisited.clear();
        children.clear();
        if (action > 1) state.add(action);
    }

    ~TreeNode() {
        notVisited.clear();
        children.clear();
        actions.clear();
    }

    // expand by adding children
    TreeNode* expand() {
        if (isFullyExpanded()) return NULL;
        if (notVisited.size() == 0) {
//cout << "building NOTVISITED" << endl;
            for (int i = 0; i < actions.size(); ++i) {
                board *b = new board(state);
                TreeNode *node = new TreeNode(b, this, actions[i]);
                notVisited.push_back(node);
            }
            default_random_engine rng = default_random_engine {};
            shuffle(begin(notVisited), end(notVisited), rng);
        }

        children.push_back(notVisited.back());
        notVisited.pop_back();
//cout << "children size: " << children.size() << "; notVisited size: " << notVisited.size() << endl;
//cout << "expanded child" << endl;
//children.back()->getState().showboard();
        return children.back();
    }

    void update(bool result) {
        ++visits;
        if (result == WIN) ++wins;
        winrate = (float)wins / (float)visits;
    }

    board& getState() { return state; }

    int getAction() { return action; }

    // check if all children have been expanded and simulated
    bool isFullyExpanded() { return children.empty() == false && children.size() == actions.size(); }

    bool isTerminal() { return actions.empty(); }

    int getNumVisits() { return visits; }

    float getWinrate() { return winrate; }

    int getDepth() { return depth; }

    int getNumChildren() { return children.size(); }

    vector<TreeNode*> getChildren() { return children; }

    TreeNode* getParent() { return parent; }
};

#endif //STUPIDNOGO_TREENODE_H
