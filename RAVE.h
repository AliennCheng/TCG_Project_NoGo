//
// Created by Alienn Cheng on 2019/1/15.
//

#ifndef RAVE_H
#define RAVE_H
#define BOARDCOL 9
#define BOARDROW 9
#define BOARDSIZE 81

class RAVE
{
private:
	vector<float> raveVisits, raveWins;
	// int iteration;

public:
	RAVE()// : iteration(0)
	{
		raveVisits.clear();
		raveWins.clear();
		for (int i = 0; i < BOARDSIZE; i++) {
			raveVisits.push_back(0);
			raveWins.push_back(0);
		}
	}

	~RAVE() {
		raveVisits.clear();
        raveVisits.shrink_to_fit();
        raveWins.clear();
        raveWins.shrink_to_fit();
	}

	// int getIteration() { return iteration; }

	// void addIteration() { ++iteration; }

	float getRaveVisits(int i) {
		if (i < 0) return raveVisits[0];
		return raveVisits[i];
	}

	float getRaveWins(int i) {
		if (i < 0) return raveWins[0];
		return raveWins[i];
	}

	float getRaveWinrate(int i) {
		if (i < 0) return raveWins[0] / raveVisits[0];
		return raveWins[i] / raveVisits[i];
	}

	void raveVisiting(int i) { 
		if (i < 0) { ++raveVisits[0]; return; }
		++raveVisits[i];
	}

	void raveWinning(int i) {
		if (i < 0) { ++raveWins[0]; return; }
		++raveWins[i];
	}
};

#endif