#include <iostream>
#include <string>
#include <fstream>
#include <queue>
#include <stack>
#include <random>
#include <time.h>
using namespace std;

class Board {

public:
	int maxDistR, maxDistC;
	Board() :row(0), col(0) { }
	Board(int r = 0, int c = 0) :row(r), col(c) {
		this->state = new int* [row];
		for (int i = 0; i < row; i++) {
			state[i] = new int[col];
		}
		for (int i = 0; i < row; i++) {
			for (int j = 0; j < col; j++) {
				state[i][j] = 0;
			}
		}

		dist = new int* [r];
		for (int i = 0; i < row; i++)
			dist[i] = new int[col];

		for (int i = 0; i < row; i++) {
			for (int j = 0; j < col; j++)
				dist[i][j] = -1;
		}
	}

	int GetState(int r, int c) { return this->state[r][c]; }
	void SetAsClean(int r, int c) { this->state[r][c] = -1; }
	void SetState(int r, int c, int k) { this->state[r][c] = k; }
	void SetDist(int r, int c, int k) { this->dist[r][c] = k; }
	int GetDist(int r, int c) { return this->dist[r][c]; }
	void DistCal(int, int);
	int getR() { return row; }
	int getC() { return col; }

	~Board() {
		delete[]state;
		delete[]dist;
	}
private:
	int row, col;
	int** state;
	int** dist;
};

class Bot {
	friend Board;
public:
	void MoveU() { pos_r--; Path.push(make_pair(pos_r, pos_c)); energy--; if (energy < 0) cout << "(" << pos_r << ", " << pos_c << ")"; }
	void MoveR() { pos_c++; Path.push(make_pair(pos_r, pos_c)); energy--; if (energy < 0) cout << "(" << pos_r << ", " << pos_c << ")"; }
	void MoveD() { pos_r++; Path.push(make_pair(pos_r, pos_c)); energy--; if (energy < 0) cout << "(" << pos_r << ", " << pos_c << ")"; }
	void MoveL() { pos_c--; Path.push(make_pair(pos_r, pos_c)); energy--; if (energy < 0) cout << "(" << pos_r << ", " << pos_c << ")"; }
	queue<pair<int, int>> AnsPath() { return Path; }
	void SetPlace(int i, int j) { pos_c = i; pos_r = j; }
	void SetMaxEnergy(int e) { maxEnergy = e; }
	void SetEnergy(int e) { energy = e; }
	int CurEnergy() { return energy; }
	int MaxEnergy() { return maxEnergy; }
	int GetC() { return pos_c; }
	int GetR() { return pos_r; }
	bool FreeWalking(Board*);
	void DirectWalking(Board*);
	void WalkBack(Board*);
	void OrigPos(int pos_r, int pos_c) {
		Path.push(make_pair(pos_r, pos_c));
	}


	Bot(int x = 0, int y = 0, int e = 0) :pos_c(x), pos_r(y), energy(e), maxEnergy(e) {}
	~Bot() { }

private:
	queue<pair<int, int>> Path;
	int pos_c, pos_r;
	int energy, maxEnergy;
};

void Bot::DirectWalking(Board* board) {
	//if all cleaned, return 
	bool haveToClean;
	enum Direc {
		up, left, right, down
	};
	while (true) {
		haveToClean = false;
		//check the goal: maxDist(x,y) && state == 0
		int maxDistr = board->maxDistR, maxDistc = board->maxDistC;
		if (board->GetState(board->maxDistR, board->maxDistC) != 0) {//cleaned, find next
			int maxDist = 0;
			for (int i = 0; i < board->getR(); i++) {
				for (int j = 0; j < board->getC(); j++) {
					if (board->GetState(i, j) == 0 && board->GetDist(i, j) > maxDist) {
						maxDist = board->GetDist(i, j);
						maxDistr = i; maxDistc = j;
						haveToClean = true;
					}
				}
			}
		}
		else//goal is default to maxdist (r,c)
			haveToClean = true;
		if (!haveToClean)
			break;


		board->SetAsClean(maxDistr, maxDistc);

		//use stack find the road
		stack<int> PathStack;
		int curDist;
		while (true)
		{
			curDist = board->GetDist(maxDistr, maxDistc);
			//walk the dist-1
			if (curDist == 0) {//back to the origin
				//PathStack.pop();//start from here
				;				break;
			}
			//move left, in edge , not orig, dist-1
			while (curDist && maxDistc > 0
				&& board->GetDist(maxDistr, maxDistc - 1) + 1 == curDist) {
				PathStack.push(left);
				maxDistc--;
				board->SetAsClean(maxDistr, maxDistc);
				curDist--;
			}
			//move up
			while (curDist && maxDistr > 0
				&& board->GetDist(maxDistr - 1, maxDistc) + 1 == curDist) {
				PathStack.push(up);
				maxDistr--;
				board->SetAsClean(maxDistr, maxDistc);
				curDist--;
			}
			//move right
			while (curDist && maxDistc + 1 < board->getC()
				&& board->GetDist(maxDistr, maxDistc + 1) + 1 == curDist) {
				PathStack.push(right);
				curDist--; ++maxDistc;
				board->SetAsClean(maxDistr, maxDistc);
			}
			//move down
			while (curDist && maxDistr + 1 < board->getR()
				&& board->GetDist(maxDistr + 1, maxDistc) + 1 == curDist) {
				PathStack.push(down);
				curDist--; ++maxDistr;
				board->SetAsClean(maxDistr, maxDistc);
			}

		}
		//stack pop ansPath to bot by move as the order poped

		while (!PathStack.empty()) {
			switch (PathStack.top())
			{
			case down:
				this->MoveU();
				break;
			case up:
				this->MoveD();
				break;
			case right:
				this->MoveL();
				break;
			case left:
				this->MoveR();
				break;
			default:
				break;
			}
			PathStack.pop();
		}

		if (board->GetState(pos_r, pos_c) == -2) {
			return;
		}
		this->WalkBack(board);
		
	}
	this->WalkBack(board);
	return;
}

void Board::DistCal(int r, int c) {
	//(x,y) is the start
	//use while loop build up the distance map
	//BFS, queue
	//next = cur + 1
	for (int i = 0; i < this->getR(); i++) {
		for (int j = 0; j < this->getC(); j++)
			dist[i][j] = -100;
	}
	dist[r][c] = 0;
	//can't over size: 0 ~ r-1, 0 ~ c-1
	queue<pair<int, int>> q;
	q.push(make_pair(r, c));
	while (!q.empty()) {
		r = q.front().first;
		c = q.front().second;
		q.pop();
		if (c + 1 < col && dist[r][c + 1] == -1 && state[r][c + 1] != 1) {// (r,c+1)
			q.push(make_pair(r, c + 1));
			dist[r][c + 1] = dist[r][c] + 1;
		}
		if (r + 1 < row && dist[r + 1][c] == -1 && state[r + 1][c] != 1) {//(r+1,c)
			q.push(make_pair(r + 1, c));
			dist[r + 1][c] = dist[r][c] + 1;
		}
		if (r > 0 && dist[r - 1][c] == -1 && state[r - 1][c] != 1) {//(r-1,c)
			q.push(make_pair(r - 1, c));
			dist[r - 1][c] = dist[r][c] + 1;
		}
		if (c > 0 && dist[r][c - 1] == -1 && state[r][c - 1] != 1) {//(r,c-1)
			q.push(make_pair(r, c - 1));
			dist[r][c - 1] = dist[r][c] + 1;
		}
	}
	maxDistC = c;
	maxDistR = r;
}

bool Bot::FreeWalking(Board* B) {//allways from origin to energy out
	bool haveMove = false;
	int direc, pos_c = this->GetC(), pos_r = this->GetR(), cur_e;
	if (this->CurEnergy() % 2 == 0)
		cur_e = this->CurEnergy() / 2;
	else
		cur_e = this->CurEnergy() / 2;

	//path = e = dist[far] + k (after reach [far] can move k steps)
	int curDist = 0;
	srand(time(NULL));
	direc = rand() % 4;

	//first walk the unpassed, set as condition for while to stop
	bool haveUnpass = true;
	switch (direc)
	{
	case 0://r u l d
		while (haveUnpass) {
			haveUnpass = false;
			if (curDist < cur_e) {//energy out or reach farest
					//walk right
					//in edge, not endpoint, unpass
				while (pos_c + 1 < (*B).getC() && (*B).GetState(pos_r, pos_c + 1) == 0) {
					//won't go dist[obj] = -1 and choose step that is +1 dist
					if ((*B).GetDist(pos_r, pos_c + 1) == curDist + 1) {
						this->MoveR();
						curDist++; pos_c++;
						(*B).SetAsClean(pos_r, pos_c);
						if (curDist >= cur_e)
							break;
					}
					else
						break;
				}

			}
			if (curDist < cur_e) {//energy out or reach farest
							//walk up
				while (pos_r > 1 && (*B).GetState(pos_r - 1, pos_c) == 0) {
					if ((*B).GetDist(pos_r - 1, pos_c) == curDist + 1) {//dist[obj] = -1
						this->MoveU();
						curDist++; pos_r--;
						(*B).SetAsClean(pos_r, pos_c);
						haveUnpass = true;
						if (curDist >= cur_e)
							break;
					}
					else
						break;
				}

			}
			if (curDist < cur_e) {//energy out or reach farest
							//walk left
				while (pos_c > 1 && (*B).GetState(pos_r, pos_c - 1) == 0) {
					if ((*B).GetDist(pos_r, pos_c - 1) == curDist + 1) {//dist[obj] = -1
						this->MoveL();
						curDist++; pos_c--;
						(*B).SetAsClean(pos_r, pos_c);
						haveUnpass = true;
						if (curDist >= cur_e)
							break;
					}
					else
						break;
				}
			}
			if (curDist < cur_e) {//energy out or reach farest
							//walk down
				while (pos_r + 1 < (*B).getR() && (*B).GetState(pos_r + 1, pos_c) == 0) {
					if ((*B).GetDist(pos_r + 1, pos_c) == curDist + 1) {//dist[obj] = -1
						this->MoveD();
						curDist++; pos_r++;
						(*B).SetAsClean(pos_r, pos_c);
						haveUnpass = true;
						if (curDist >= cur_e)
							break;
					}
					else
						break;
				}
			}
			if (haveUnpass)
				haveMove = true;
		}
		break;
	case 1:// l u r d
		while (haveUnpass) {
			haveUnpass = false;
			if (curDist < cur_e) {//energy out or reach farest
				//walk left
				while (pos_c > 1 && (*B).GetState(pos_r, pos_c - 1) == 0) {
					if ((*B).GetDist(pos_r, pos_c - 1) == curDist + 1) {//dist[obj] = -1
						this->MoveL();
						curDist++; pos_c--;
						(*B).SetAsClean(pos_r, pos_c);
						haveUnpass = true;
						if (curDist >= cur_e)
							break;
					}
					else
						break;
				}
			}
			if (curDist < cur_e) {//energy out or reach farest
				//walk up
				while (pos_r > 1 && (*B).GetState(pos_r - 1, pos_c) == 0) {
					if ((*B).GetDist(pos_r - 1, pos_c) == curDist + 1) {//dist[obj] = -1
						this->MoveU();
						curDist++; pos_r--;
						(*B).SetAsClean(pos_r, pos_c);
						haveUnpass = true;
						if (curDist >= cur_e)
							break;
					}
					else
						break;
				}

			}
			if (curDist < cur_e) {//energy out or reach farest
				//walk right
				//in edge, not endpoint, unpass
				while (pos_c + 1 < (*B).getC() && (*B).GetState(pos_r, pos_c + 1) == 0) {
					//won't go dist[obj] = -1 and choose step that is +1 dist
					if ((*B).GetDist(pos_r, pos_c + 1) == curDist + 1) {
						this->MoveR();
						curDist++; pos_c++;
						(*B).SetAsClean(pos_r, pos_c);
						if (curDist >= cur_e)
							break;
					}
					else
						break;
				}

			}
			if (curDist < cur_e) {//energy out or reach farest
				//walk down
				while (pos_r + 1 < (*B).getR() && (*B).GetState(pos_r + 1, pos_c) == 0) {
					if ((*B).GetDist(pos_r + 1, pos_c) == curDist + 1) {//dist[obj] = -1
						this->MoveD();
						curDist++; pos_r++;
						(*B).SetAsClean(pos_r, pos_c);
						haveUnpass = true;
						if (curDist >= cur_e)
							break;
					}
					else
						break;
				}
			}
			if (haveUnpass)
				haveMove = true;
		}
	case 2:// l d r u
		while (haveUnpass) {
			haveUnpass = false;

			if (curDist < cur_e) {//energy out or reach farest
							//walk left
				while (pos_c > 1 && (*B).GetState(pos_r, pos_c - 1) == 0) {
					if ((*B).GetDist(pos_r, pos_c - 1) == curDist + 1) {//dist[obj] = -1
						this->MoveL();
						curDist++; pos_c--;
						(*B).SetAsClean(pos_r, pos_c);
						haveUnpass = true;
						if (curDist >= cur_e)
							break;
					}
					else
						break;
				}
			}
			if (curDist < cur_e) {//energy out or reach farest
							//walk down
				while (pos_r + 1 < (*B).getR() && (*B).GetState(pos_r + 1, pos_c) == 0) {
					if ((*B).GetDist(pos_r + 1, pos_c) == curDist + 1) {//dist[obj] = -1
						this->MoveD();
						curDist++; pos_r++;
						(*B).SetAsClean(pos_r, pos_c);
						haveUnpass = true;
						if (curDist >= cur_e)
							break;
					}
					else
						break;
				}
			}
			if (curDist < cur_e) {//energy out or reach farest
					//walk right
					//in edge, not endpoint, unpass
				while (pos_c + 1 < (*B).getC() && (*B).GetState(pos_r, pos_c + 1) == 0) {
					//won't go dist[obj] = -1 and choose step that is +1 dist
					if ((*B).GetDist(pos_r, pos_c + 1) == curDist + 1) {
						this->MoveR();
						curDist++; pos_c++;
						(*B).SetAsClean(pos_r, pos_c);
						if (curDist >= cur_e)
							break;
					}
					else
						break;
				}

			}
			if (curDist < cur_e) {//energy out or reach farest
							//walk up
				while (pos_r > 1 && (*B).GetState(pos_r - 1, pos_c) == 0) {
					if ((*B).GetDist(pos_r - 1, pos_c) == curDist + 1) {//dist[obj] = -1
						this->MoveU();
						curDist++; pos_r--;
						(*B).SetAsClean(pos_r, pos_c);
						haveUnpass = true;
						if (curDist >= cur_e)
							break;
					}
					else
						break;
				}

			}
			if (haveUnpass)
				haveMove = true;
		}
		break;
	case 3://r d l u
		while (haveUnpass) {
			haveUnpass = false;
			if (curDist < cur_e) {//energy out or reach farest
					//walk right
					//in edge, not endpoint, unpass
				while (pos_c + 1 < (*B).getC() && (*B).GetState(pos_r, pos_c + 1) == 0) {
					//won't go dist[obj] = -1 and choose step that is +1 dist
					if ((*B).GetDist(pos_r, pos_c + 1) == curDist + 1) {
						this->MoveR();
						curDist++; pos_c++;
						(*B).SetAsClean(pos_r, pos_c);
						if (curDist >= cur_e)
							break;
					}
					else
						break;
				}

			}
			if (curDist < cur_e) {//energy out or reach farest
							//walk down
				while (pos_r + 1 < (*B).getR() && (*B).GetState(pos_r + 1, pos_c) == 0) {
					if ((*B).GetDist(pos_r + 1, pos_c) == curDist + 1) {//dist[obj] = -1
						this->MoveD();
						curDist++; pos_r++;
						(*B).SetAsClean(pos_r, pos_c);
						haveUnpass = true;
						if (curDist >= cur_e)
							break;
					}
					else
						break;
				}
			}
			if (curDist < cur_e) {//energy out or reach farest
							//walk left
				while (pos_c > 1 && (*B).GetState(pos_r, pos_c - 1) == 0) {
					if ((*B).GetDist(pos_r, pos_c - 1) == curDist + 1) {//dist[obj] = -1
						this->MoveL();
						curDist++; pos_c--;
						(*B).SetAsClean(pos_r, pos_c);
						haveUnpass = true;
						if (curDist >= cur_e)
							break;
					}
					else
						break;
				}
			}
			if (curDist < cur_e) {//energy out or reach farest
							//walk up
				while (pos_r > 1 && (*B).GetState(pos_r - 1, pos_c) == 0) {
					if ((*B).GetDist(pos_r - 1, pos_c) == curDist + 1) {//dist[obj] = -1
						this->MoveU();
						curDist++; pos_r--;
						(*B).SetAsClean(pos_r, pos_c);
						haveUnpass = true;
						if (curDist >= cur_e)
							break;
					}
					else
						break;
				}

			}
			if (haveUnpass)
				haveMove = true;
		}
		break;
	}
	//then try every pass/unpass block
	bool haveClean = false;
	haveUnpass = true;
	while (curDist < cur_e && haveUnpass) {
		haveUnpass = false;
		srand(time(NULL));
		direc = rand() % 4;
		switch (direc) {
		case 0:// r u l d
			if (curDist < cur_e) {//energy out or reach farest
				//walk right
				while (pos_c + 1 < (*B).getR()) {
					if ((*B).GetDist(pos_r, pos_c + 1) == curDist + 1) {//dist[obj] = -1
						this->MoveR();
						curDist++; pos_c++;
						if ((*B).GetState(pos_r, pos_c) == 0) {
							haveClean = true;
							(*B).SetAsClean(pos_r, pos_c);
						}
						haveUnpass = true;
						if (curDist >= cur_e)
							break;
					}
					else
						break;
				}
			}
			if (curDist < cur_e) {//energy out or reach farest
				//walk up
				while (pos_r > 1) {
					if ((*B).GetDist(pos_r - 1, pos_c) == curDist + 1) {//dist[obj] = -1
						this->MoveU();
						curDist++; pos_r--;
						if ((*B).GetState(pos_r, pos_c) == 0) {
							haveClean = true;
							(*B).SetAsClean(pos_r, pos_c);
						}
						haveUnpass = true;
						if (curDist >= cur_e)
							break;
					}
					else
						break;
				}
			}
			if (curDist < cur_e) {//energy out or reach farest
				//walk left
				while (pos_c > 1) {
					if ((*B).GetDist(pos_r, pos_c - 1) == curDist + 1) {//dist[obj] = -1
						this->MoveL();
						curDist++; pos_c--;
						if ((*B).GetState(pos_r, pos_c) == 0) {
							haveClean = true;
							(*B).SetAsClean(pos_r, pos_c);
						}
						haveUnpass = true;
						if (curDist >= cur_e)
							break;
					}
					else
						break;
				}
			}
			if (curDist < cur_e) {//energy out
				//walk down
				while (pos_r + 1 < (*B).getR()) {
					if ((*B).GetDist(pos_r + 1, pos_c) == curDist + 1) {//dist[obj] = -1
						this->MoveD();
						curDist++; pos_r++;
						if ((*B).GetState(pos_r, pos_c) == 0) {
							haveClean = true;
							(*B).SetAsClean(pos_r, pos_c);
						}
						haveUnpass = true;
						if (curDist >= cur_e)
							break;
					}
					else
						break;
				}
			}
			break;
		case 1:// r d l u 
			if (curDist < cur_e) {//energy out or reach farest
				//walk right
				while (pos_c + 1 < (*B).getR()) {
					if ((*B).GetDist(pos_r, pos_c + 1) == curDist + 1) {//dist[obj] = -1
						this->MoveR();
						curDist++; pos_c++;
						if ((*B).GetState(pos_r, pos_c) == 0) {
							haveClean = true;
							(*B).SetAsClean(pos_r, pos_c);
						}
						haveUnpass = true;
						if (curDist >= cur_e)
							break;
					}
					else
						break;
				}
			}
			if (curDist < cur_e) {//energy out
				//walk down
				while (pos_r + 1 < (*B).getR()) {
					if ((*B).GetDist(pos_r + 1, pos_c) == curDist + 1) {//dist[obj] = -1
						this->MoveD();
						curDist++; pos_r++;
						if ((*B).GetState(pos_r, pos_c) == 0) {
							haveClean = true;
							(*B).SetAsClean(pos_r, pos_c);
						}
						haveUnpass = true;
						if (curDist >= cur_e)
							break;
					}
					else
						break;
				}
			}
			if (curDist < cur_e) {//energy out or reach farest
				//walk left
				while (pos_c > 1) {
					if ((*B).GetDist(pos_r, pos_c - 1) == curDist + 1) {//dist[obj] = -1
						this->MoveL();
						curDist++; pos_c--;
						if ((*B).GetState(pos_r, pos_c) == 0) {
							haveClean = true;
							(*B).SetAsClean(pos_r, pos_c);
						}
						haveUnpass = true;
						if (curDist >= cur_e)
							break;
					}
					else
						break;
				}
			}
			if (curDist < cur_e) {//energy out or reach farest
				//walk up
				while (pos_r > 1) {
					if ((*B).GetDist(pos_r - 1, pos_c) == curDist + 1) {//dist[obj] = -1
						this->MoveU();
						curDist++; pos_r--;
						if ((*B).GetState(pos_r, pos_c) == 0) {
							haveClean = true;
							(*B).SetAsClean(pos_r, pos_c);
						}
						haveUnpass = true;
						if (curDist >= cur_e)
							break;
					}
					else
						break;
				}
			}

			break;
		case 2:// l u r d
			if (curDist < cur_e) {//energy out or reach farest
				//walk left
				while (pos_c > 1) {
					if ((*B).GetDist(pos_r, pos_c - 1) == curDist + 1) {//dist[obj] = -1
						this->MoveL();
						curDist++; pos_c--;
						if ((*B).GetState(pos_r, pos_c) == 0) {
							haveClean = true;
							(*B).SetAsClean(pos_r, pos_c);
						}
						haveUnpass = true;
						if (curDist >= cur_e)
							break;
					}
					else
						break;
				}
			}
			if (curDist < cur_e) {//energy out or reach farest
				//walk up
				while (pos_r > 1) {
					if ((*B).GetDist(pos_r - 1, pos_c) == curDist + 1) {//dist[obj] = -1
						this->MoveU();
						curDist++; pos_r--;
						if ((*B).GetState(pos_r, pos_c) == 0) {
							haveClean = true;
							(*B).SetAsClean(pos_r, pos_c);
						}
						haveUnpass = true;
						if (curDist >= cur_e)
							break;
					}
					else
						break;
				}
			}
			if (curDist < cur_e) {//energy out or reach farest
				//walk right
				while (pos_c + 1 < (*B).getR()) {
					if ((*B).GetDist(pos_r, pos_c + 1) == curDist + 1) {//dist[obj] = -1
						this->MoveR();
						curDist++; pos_c++;
						if ((*B).GetState(pos_r, pos_c) == 0) {
							haveClean = true;
							(*B).SetAsClean(pos_r, pos_c);
						}
						haveUnpass = true;
						if (curDist >= cur_e)
							break;
					}
					else
						break;
				}
			}
			if (curDist < cur_e) {//energy out
				//walk down
				while (pos_r + 1 < (*B).getR()) {
					if ((*B).GetDist(pos_r + 1, pos_c) == curDist + 1) {//dist[obj] = -1
						this->MoveD();
						curDist++; pos_r++;
						if ((*B).GetState(pos_r, pos_c) == 0) {
							haveClean = true;
							(*B).SetAsClean(pos_r, pos_c);
						}
						haveUnpass = true;
						if (curDist >= cur_e)
							break;
					}
					else
						break;
				}
			}
			break;
		case 3://l d r u
			if (curDist < cur_e) {//energy out or reach farest
				//walk left
				while (pos_c > 1) {
					if ((*B).GetDist(pos_r, pos_c - 1) == curDist + 1) {//dist[obj] = -1
						this->MoveL();
						curDist++; pos_c--;
						if ((*B).GetState(pos_r, pos_c) == 0) {
							haveClean = true;
							(*B).SetAsClean(pos_r, pos_c);
						}
						haveUnpass = true;
						if (curDist >= cur_e)
							break;
					}
					else
						break;
				}
			}
			if (curDist < cur_e) {//energy out
				//walk down
				while (pos_r + 1 < (*B).getR()) {
					if ((*B).GetDist(pos_r + 1, pos_c) == curDist + 1) {//dist[obj] = -1
						this->MoveD();
						curDist++; pos_r++;
						if ((*B).GetState(pos_r, pos_c) == 0) {
							haveClean = true;
							(*B).SetAsClean(pos_r, pos_c);
						}
						haveUnpass = true;
						if (curDist >= cur_e)
							break;
					}
					else
						break;
				}
			}
			if (curDist < cur_e) {//energy out or reach farest
				//walk right
				while (pos_c + 1 < (*B).getR()) {
					if ((*B).GetDist(pos_r, pos_c + 1) == curDist + 1) {//dist[obj] = -1
						this->MoveR();
						curDist++; pos_c++;
						if ((*B).GetState(pos_r, pos_c) == 0) {
							haveClean = true;
							(*B).SetAsClean(pos_r, pos_c);
						}
						haveUnpass = true;
						if (curDist >= cur_e)
							break;
					}
					else
						break;
				}
			}
			if (curDist < cur_e) {//energy out or reach farest
				//walk up
				while (pos_r > 1) {
					if ((*B).GetDist(pos_r - 1, pos_c) == curDist + 1) {//dist[obj] = -1
						this->MoveU();
						curDist++; pos_r--;
						if ((*B).GetState(pos_r, pos_c) == 0) {
							haveClean = true;
							(*B).SetAsClean(pos_r, pos_c);
						}
						haveUnpass = true;
						if (curDist >= cur_e)
							break;
					}
					else
						break;
				}
			}
			break;
		}

		if (haveUnpass)
			haveMove = true;
	}


	//if still have energy, beside are all smaller
	//so walk to unpassed first
	int freestep = cur_e - curDist;
	if (freestep > 0) haveMove = true;
	while (freestep > 0) {
		haveUnpass = true;
		while (haveUnpass) {
			haveUnpass = false;
			if (freestep && pos_r + 1 < (*B).getR() && (*B).GetState(pos_r + 1, pos_c) == 0) {
				//walk down
				this->MoveD();
				freestep--; pos_r++;
				if ((*B).GetState(pos_r, pos_c) == 0) {
					haveClean = true;
					(*B).SetAsClean(pos_r, pos_c);
				}
				haveUnpass = true;
			}
			if (freestep && pos_c > 1 && (*B).GetState(pos_r, pos_c - 1) == 0) {
				//walk left
				this->MoveL();
				freestep--; pos_c--;
				if ((*B).GetState(pos_r, pos_c) == 0) {
					haveClean = true;
					(*B).SetAsClean(pos_r, pos_c);
				}
				haveUnpass = true;
			}
			if (freestep && pos_r > 1 && (*B).GetState(pos_r - 1, pos_c) == 0) {
				//walk up
				this->MoveU();
				freestep--; pos_r--;
				if ((*B).GetState(pos_r, pos_c) == 0) {
					haveClean = true;
					(*B).SetAsClean(pos_r, pos_c);
				}
				haveUnpass = true;
			}
			if (freestep && pos_c + 2 < (*B).getC() && (*B).GetState(pos_r, pos_c + 1) == 0) {
				//walk right if in edge, unpass, energy enough
				this->MoveR();
				freestep--; pos_c++;
				if ((*B).GetState(pos_r, pos_c) == 0) {
					haveClean = true;
					(*B).SetAsClean(pos_r, pos_c);
				}
				haveUnpass = true;
			}

		}
		//then try every pass/unpass block
		haveUnpass = true;
		while (haveUnpass && freestep) {
			haveUnpass = false;

			if (freestep && pos_r > 1 && B->GetState(pos_r - 1, pos_c) < 1) {
				//walk up
				this->MoveU();
				freestep--; pos_r--;
				if ((*B).GetState(pos_r, pos_c) == 0) {
					haveClean = true;
					(*B).SetAsClean(pos_r, pos_c);
				}
				haveUnpass = true;
			}
			if (freestep && pos_c > 1 && B->GetState(pos_r, pos_c - 1) < 1) {
				//walk left
				this->MoveL();
				freestep--; pos_c--;
				if ((*B).GetState(pos_r, pos_c) == 0) {
					haveClean = true;
					(*B).SetAsClean(pos_r, pos_c);
				}
				haveUnpass = true;
			}
			if (freestep && pos_r + 2 < (*B).getR() && B->GetState(pos_r + 1, pos_c) < 1) {
				//walk down
				this->MoveD();
				freestep--; pos_r++;
				if ((*B).GetState(pos_r, pos_c) == 0) {
					haveClean = true;
					(*B).SetAsClean(pos_r, pos_c);
				}
				haveUnpass = true;
			}
			if (freestep && pos_c + 2 < (*B).getC() && B->GetState(pos_r, pos_c + 1) < 1) {
				//walk right if in edge, unpass, energy enough
				this->MoveR();
				freestep--; pos_c++;
				if ((*B).GetState(pos_r, pos_c) == 0) {
					haveClean = true;
					(*B).SetAsClean(pos_r, pos_c);
				}
				haveUnpass = true;
			}
		}
	}
	if (!haveClean)
		return false;
	return haveMove;
}

void Bot::WalkBack(Board* board) {
	
	//every step walk to dist-1 , and try state==0 first
	int pos_r = this->pos_r, pos_c = this->pos_c;
	if (board->GetState(pos_r, pos_c) == -2) {
		this->SetEnergy(this->MaxEnergy());
		return;
	}
	
	int curDist = board->GetDist(pos_r, pos_c);

	//walk only state == 0
	bool haveUnpass = true;
	while (haveUnpass) {
		haveUnpass = false;
		//move left
		if (pos_c > 0 && board->GetDist(pos_r, pos_c - 1) + 1 == curDist
			&& board->GetState(pos_r, pos_c - 1) == 0 && curDist > 1) {
			this->MoveL();
			curDist--; pos_c--;
			haveUnpass = true;
		}
		//move down
		if (pos_r + 1 < board->getR() && board->GetDist(pos_r + 1, pos_c) + 1 == curDist
			&& board->GetState(pos_r + 1, pos_c) == 0 && curDist > 1) {
			this->MoveD();
			curDist--; pos_r++;
			haveUnpass = true;
		}
		//move right
		if (pos_c + 1 < board->getC() && board->GetDist(pos_r, pos_c + 1) + 1 == curDist
			&& board->GetState(pos_r, pos_c + 1) == 0 && curDist > 1) {
			this->MoveR();
			curDist--; pos_c++;
			haveUnpass = true;
		}
		//move up
		if (pos_r > 0 && board->GetDist(pos_r - 1, pos_c) + 1 == curDist
			&& board->GetState(pos_r - 1, pos_c) == 0 && curDist > 1) {
			this->MoveU();
			curDist--; pos_r--;
			haveUnpass = true;
		}
	}

	haveUnpass = true;
	while (haveUnpass) {
		haveUnpass = false;
		//move left
		if (pos_c > 0 && board->GetDist(pos_r, pos_c - 1) + 1 == curDist
			&& board->GetState(pos_r, pos_c - 1) != 1 && curDist > 1) {
			this->MoveL(); 
			curDist--; pos_c--;
			haveUnpass = true;
		}
		//move down
		if (pos_r + 1 < board->getR() && board->GetDist(pos_r + 1, pos_c) + 1 == curDist
			&& board->GetState(pos_r + 1, pos_c) != 1 && curDist > 1) {
			this->MoveD();
			curDist--; pos_r++;
			haveUnpass = true;
		}
		//move right
		if (pos_c + 1 < board->getC() && board->GetDist(pos_r, pos_c + 1) + 1 == curDist
			&& board->GetState(pos_r, pos_c + 1) != 1 && curDist > 1) {
			this->MoveR();
			curDist--; pos_c++;
			haveUnpass = true;
		}
		//move up
		if (pos_r > 0 && board->GetDist(pos_r - 1, pos_c) + 1 == curDist
			&& board->GetState(pos_r - 1, pos_c) != 1 && curDist > 1) {
			this->MoveU();
			curDist--; pos_r--;
			haveUnpass = true;
		}
	}
	if (pos_r > 0 && board->GetState(pos_r - 1, pos_c) == -2) {
		this->MoveU();
		this->SetEnergy(this->MaxEnergy());
		board->SetState(pos_r - 1, pos_c, -2);
	}
	else if (pos_c > 0 && board->GetState(pos_r, pos_c - 1) == -2) {
		this->MoveL();
		this->SetEnergy(this->MaxEnergy());
		board->SetState(pos_r, pos_c - 1, -2);
	}
	else if (pos_c + 1 < board->getC() && board->GetState(pos_r, pos_c + 1) == -2) {
		this->MoveR();
		this->SetEnergy(this->MaxEnergy());
		board->SetState(pos_r, pos_c + 1, -2);
	}
	else{
		this->MoveD();
		this->SetEnergy(this->MaxEnergy());
		board->SetState(pos_r + 1, pos_c, -2);
	}
	
	return;
}

int main(int argc, char* argv[])
{
	int row, col, maxEnergy;

	ifstream f_in;
	f_in.open(argv[1], std::ifstream::in);//as input, and only read
	if (!f_in.is_open())
	{
		cout << "Open file fails!\n";
		return 0;
	}
	else cout << "-in_file opened\n";
	f_in >> row >> col >> maxEnergy;
	Board board(row, col);//build the ground
	Bot bot;
	bot.SetMaxEnergy(maxEnergy);
	bot.SetEnergy(maxEnergy);
	for (int i = 0; i < row; i++) {
		for (int j = 0; j < col; j++) {
			char a;
			f_in >> a;
			if (a == 'R') {
				board.SetState(i, j, -2);
				bot.SetPlace(j, i);
				bot.OrigPos(i, j);
			}
			else if (a == '0')
				board.SetState(i, j, 0);
			else if (a == '1')
				board.SetState(i, j, 1);
			else if (a == ' ')
				cout << "p";
			else
				cout << "Wrong input on the map!";
		}
	}
	
	//set distance map
	board.DistCal(bot.GetR(), bot.GetC());
	//random walking
	bool move = true;
	while (move) {
		move = bot.FreeWalking(&board);
		bot.WalkBack(&board);
	}
	//if still are uncleaned places, go there by direct walk

	bot.DirectWalking(&board);
	cout << "done";

	f_in.close();
	if (f_in.is_open())
	{
		cout << "Close file fails!\n";
		return 2;
	}


	std::ofstream f_out_path;
	f_out_path.open("final.path", std::ofstream::out | std::ofstream::trunc);
	if (!f_out_path.is_open())
		cout << "close file fails! f_out_path\n";
	else
		cout << "f_out_path opened\n";

	queue<pair<int, int>> ansQue = bot.AnsPath();
	f_out_path << ansQue.size() << endl;
	while (!ansQue.empty())
	{
		f_out_path << ansQue.front().first << " " << ansQue.front().second << endl;
		ansQue.pop();
	}

	f_out_path.close();
	if (f_out_path.is_open())
		std::cout << "close file fails! f_out\n";

	return 0;
}

//g++ -O2 -std=c++11 108062224_proj2.cpp
//./a.exe floor.data
