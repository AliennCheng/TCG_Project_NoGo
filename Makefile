.phony: all clean

all: JustASimpleNoGo

JustASimpleNoGo: main.o
	$(CXX) -o $@ $+

main.o: main.cpp board.h bitboard.h MCTS.h TreeNode.h RAVE.h
	$(CXX) -c -o $@ $<
