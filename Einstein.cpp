#define _CRT_SECURE_NO_WARNINGS
#include "Einstein.h"
#include<vector>
#include<cstring>
#include<cmath>
#include<fstream>
#include<memory>

time_t now_time;
time_t begin_time;
time_t end_time;
struct tm* pt;
const double coe = 1.38;

class Board
{
public:
	std::shared_ptr<Board> parent;
	std::vector<int> validchess;			//可以走的棋子
	int chess[2] = { 0,-1 };			//导致这个棋局要走的棋子和方向
	std::vector<int> posStep[2];			//可能的走法的个数，要分种类，0水平走位、1垂直走位、2对角走位
	int color;					//红色为0，蓝色为1
	int board[5][5] = {};				//棋盘
	int visit_times = 0;
	int win_time = 0;
	double quality = 0.0;
	std::vector<std::shared_ptr<Board>> child;

	Board() {};
	Board(std::shared_ptr<Board> par, int col, int a[5][5], int pos[2]) {
		parent = par;
		color = col;
		chess[0] = pos[0];
		chess[1] = pos[1];
		if (col == 0)
		{
			for (int i = 0; i < 5; i++)
				for (int j = 0; j < 5; j++)
				{
					int temp = a[i][j];
					board[i][j] = temp;
					if (temp >= 1 && temp <= 6)
						validchess.push_back(temp);
				}
		}
		else
		{
			for (int i = 0; i < 5; i++)
				for (int j = 0; j < 5; j++)
				{
					int temp = a[i][j];
					board[i][j] = temp;
					if (temp >= 7)
						validchess.push_back(temp - 6);
				}
		}
		com_posStep();
	}
	void com_posStep() {
		if (color == 0)
		{
			for (int i = 0; i < validchess.size(); i++)
			{
				int x, y;
				for (int j = 0; j < 5; j++)
					for (int k = 0; k < 5; k++)
					{
						if (board[j][k] == validchess[i])
						{
							x = j;
							y = k;
							break;
						}
					}
				if (x == 4)
				{
					posStep[0].push_back(validchess[i]);
					posStep[1].push_back(0);
				}
				else if (y == 4)
				{
					posStep[0].push_back(validchess[i]);
					posStep[1].push_back(1);
				}
				else
				{
					posStep[0].push_back(validchess[i]);
					posStep[1].push_back(0);
					posStep[0].push_back(validchess[i]);
					posStep[1].push_back(1);
					posStep[0].push_back(validchess[i]);
					posStep[1].push_back(2);
				}
			}
		}
		else
		{
			for (int i = 0; i < validchess.size(); i++)
			{
				int x, y;
				for (int j = 0; j < 5; j++)
					for (int k = 0; k < 5; k++)
					{
						if (board[j][k] == validchess[i] + 6)
						{
							x = j;
							y = k;
							//break;
						}
					}
				if (x == 0)
				{
					posStep[0].push_back(validchess[i]);
					posStep[1].push_back(0);
				}
				else if (y == 0)
				{
					posStep[0].push_back(validchess[i]);
					posStep[1].push_back(1);
				}
				else
				{
					posStep[0].push_back(validchess[i]);
					posStep[1].push_back(0);
					posStep[0].push_back(validchess[i]);
					posStep[1].push_back(1);
					posStep[0].push_back(validchess[i]);
					posStep[1].push_back(2);
				}
			}
		}
	}
	Board(std::shared_ptr<Board> par, int col, int a[5][5], int dice) {
		parent = par;
		color = col;
		int ch[7] = { 0 };
		if (col == 0)
		{
			for (int i = 0; i < 5; i++)
				for (int j = 0; j < 5; j++)
				{
					int temp = a[i][j];
					board[i][j] = temp;
					if (temp >= 1 && temp <= 6)
						ch[temp]++;
				}
			if (ch[dice] == 1)
				validchess.push_back(dice);
			else
			{
				for (int i = dice - 1; i > 0; i--)
				{
					if (ch[i] == 1)
					{
						validchess.push_back(i);
						break;
					}
				}
				for (int i = dice + 1; i < 7; i++)
				{
					if (ch[i] == 1)
					{
						validchess.push_back(i);
						break;
					}
				}
			}
		}
		else
		{
			for (int i = 0; i < 5; i++)
				for (int j = 0; j < 5; j++)
				{
					int temp = a[i][j];
					board[i][j] = temp;
					if (temp >= 7)
						ch[temp - 6]++;
				}
			if (ch[dice - 6] == 1)
				validchess.push_back(dice - 6);
			else
			{
				for (int i = dice - 7; i > 0; i--)
				{
					if (ch[i] == 1)
					{
						validchess.push_back(i);
						break;
					}
				}
				for (int i = dice - 5; i < 7; i++)
				{
					if (ch[i] == 1)
					{
						validchess.push_back(i);
						break;
					}
				}
			}
		}
		com_posStep();
	}
	~Board() {};
};

std::shared_ptr<Board> BestChild(std::shared_ptr<Board> v, double c);
std::shared_ptr<Board> MostWin(std::shared_ptr<Board> v);
std::shared_ptr<Board> Expand(std::shared_ptr<Board> v);
bool is_Terminal(std::shared_ptr<Board> v);
bool is_Expanded(std::shared_ptr<Board> v);
std::shared_ptr<Board> Treepolicy(std::shared_ptr<Board> v);
void oneMove(std::shared_ptr<Board> v, int newboard[5][5], int chess, int direction);
int simulate(std::shared_ptr<Board> v);
bool RandomMove(std::shared_ptr<Board> v, bool col);
void Backup(std::shared_ptr<Board> v, int result);

void to_int(std::string s, int begin, int end, int* a, int line)
{
	int i = begin;
	int j = begin + 1;
	int t = 0;
	while (j < end + 1)
	{
		if (s[j] == ',' || s[j] == ']')
		{
			std::string x = s.substr(i, j - i);
			a[line * 5 + t] = atoi(x.c_str());
			t++;
			i = j + 2;
		}
		j++;
	}
}

Einstein::Einstein()
{
	this->clientsocket.connectServer();
	clientsocket.sendMsg(ID);
}

Einstein::~Einstein()
{
	this->clientsocket.close();
}

int Einstein::parse(std::string s)
{
	int k = s.find("], [", 0);
	int l = s.find("], [", k + 4);
	int m = s.find("], [", l + 4);
	int n = s.find("], [", m + 4);
	int p = s.find("]]|", n + 4);
	to_int(s, 2, k, chessboard, 0);
	to_int(s, k + 4, l, chessboard, 1);
	to_int(s, l + 4, m, chessboard, 2);
	to_int(s, m + 4, n, chessboard, 3);
	to_int(s, n + 4, p, chessboard, 4);
	std::string d = s.substr(p + 3, s.length() - p - 3);
	dice = atoi(d.c_str());
	if (logger.size() == 0)
	{
		begin_time = clock();
		return 0;
	}
	else
	{
		std::string g = logger.back();
		int last[25];
		int f = g.find("[[", 0);
		int k = g.find("], [", f + 2);
		int l = g.find("], [", k + 4);
		int m = g.find("], [", l + 4);
		int n = g.find("], [", m + 4);
		int p = g.find("]]|", n + 4);
		to_int(g, f + 2, k, last, 0);
		to_int(g, k + 4, l, last, 1);
		to_int(g, l + 4, m, last, 2);
		to_int(g, m + 4, n, last, 3);
		to_int(g, n + 4, p, last, 4);
		int diff = 0;
		for (int i = 0; i < 25; i++)
			if (chessboard[i] != last[i])
				diff++;
		if (diff <= 4)
			return 0;
		else
		{
			int ze = g.rfind(" ");
			int pr = g.rfind("|");
			int chess = std::stoi(g.substr(ze + 1, pr - ze - 1));
			std::string operation = g.substr(pr + 1, g.length() - pr - 1);
			int t = 0;
			while (last[t] != chess)
				t++;
			if (operation == "up")
			{
				last[t - 5] = chess;
				last[t] = 0;
			}
			else if (operation == "left")
			{
				last[t - 1] = chess;
				last[t] = 0;
			}
			else if (operation == "leftup")
			{
				last[t - 6] = chess;
				last[t] = 0;
			}
			else if (operation == "down")
			{
				last[t + 5] = chess;
				last[t] = 0;
			}
			else if (operation == "right")
			{
				last[t + 1] = chess;
				last[t] = 0;
			}
			else
			{
				last[t + 6] = chess;
				last[t] = 0;
			}
			end_time = clock();
			double lasting = difftime(end_time, begin_time) / (double)CLOCKS_PER_SEC;
			begin_time = clock();
			if (chess >= 7)
			{
				int red = 0;
				for (int i = 0; i < 25; i++)
					if (last[i] >= 1 && last[i] <= 6)
						red++;
				if (red == 0 || last[0] == chess)
				{
					logging("Game over! Blue wins! Time: " + std::to_string(lasting) + "s");
					return 0;
				}
				else
				{
					logging("Game over! Red wins! Time: " + std::to_string(lasting) + "s");
					return 0;
				}
			}
			else
			{
				int blue = 0;
				for (int i = 0; i < 25; i++)
					if (last[i] >= 7 && last[i] <= 12)
						blue++;
				if (blue == 0 || last[24] == chess)
				{
					logging("Game over! Red wins! Time: " + std::to_string(lasting) + "s");
					return 0;
				}
				else
				{
					logging("Game over! Blue wins! Time: " + std::to_string(lasting) + "s");
					return 0;
				}
			}
		}
	}
}

//莽夫算法
std::string fool(int dice, int board[5][5]);

//UCT

//返回最佳子节点
std::shared_ptr<Board> BestChild(std::shared_ptr<Board> v, double c)
{
	std::shared_ptr<Board> q;
	double max = -1.0;
	for (int i = 0; i < v->child.size(); i++)
	{
		double UCB = v->child[i]->quality / (double)v->child[i]->visit_times + c * sqrt(2 * log((double)v->visit_times) / (double)v->child[i]->visit_times);
		if (UCB - max > 0.0)
		{
			q = v->child[i];
			max = UCB;
		}
	}
	return q;
}

std::shared_ptr<Board> MostWin(std::shared_ptr<Board> v)
{
	std::shared_ptr<Board> p;
	double max = -1.0;
	for (int i = 0; i < v->child.size(); i++)
	{
		if (v->child[i]->quality - max > 0.0)
		{
			max = v->child[i]->quality;
			p = v->child[i];
		}
	}
	return p;
}

//拓展
std::shared_ptr<Board> Expand(std::shared_ptr<Board> v)
{
	std::vector<int> posChess[2];
	bool flag;
	for (int i = 0; i < v->posStep->size(); i++)
	{
		flag = false;
		for (int j = 0; j < v->child.size(); j++)
		{
			if (v->child[j]->chess[0] == v->posStep[0][i] && v->child[j]->chess[1] == v->posStep[1][i])
			{
				flag = true;
				break;
			}
		}
		if (!flag)
		{
			posChess[0].push_back(v->posStep[0][i]);
			posChess[1].push_back(v->posStep[1][i]);
		}
	}
	int index = rand() % posChess[0].size();
	int newBoard[5][5];
	for (int i = 0; i < 5; i++)
		for (int j = 0; j < 5; j++)
			newBoard[i][j] = v->board[i][j];
	oneMove(v, newBoard, posChess[0][index], posChess[1][index]);
	int pos[2];
	pos[0] = posChess[0][index];
	pos[1] = posChess[1][index];
	std::shared_ptr<Board> newChild(new Board(v, !(bool)v->color, newBoard, pos));
	v->child.push_back(newChild);
	return newChild;
}

void oneMove(std::shared_ptr<Board> v, int newboard[5][5], int chess, int direction)
{
	int x, y;
	if (v->color == 0)
	{
		for (int i = 0; i < 5; i++)
			for (int j = 0; j < 5; j++)
				if (newboard[i][j] == chess)
				{
					x = i;
					y = j;
					//break;
				}
		if (direction == 0)
		{
			newboard[x][y + 1] = chess;
			newboard[x][y] = 0;
		}
		else if (direction == 1)
		{
			newboard[x + 1][y] = chess;
			newboard[x][y] = 0;
		}
		else
		{
			newboard[x + 1][y + 1] = chess;
			newboard[x][y] = 0;
		}
	}
	else
	{
		for (int i = 0; i < 5; i++)
			for (int j = 0; j < 5; j++)
				if (newboard[i][j] == chess + 6)
				{
					x = i;
					y = j;
					//break;
				}
		if (direction == 0)
		{
			newboard[x][y - 1] = chess + 6;
			newboard[x][y] = 0;
		}
		else if (direction == 1)
		{
			newboard[x - 1][y] = chess + 6;
			newboard[x][y] = 0;
		}
		else
		{
			newboard[x - 1][y - 1] = chess + 6;
			newboard[x][y] = 0;
		}
	}
}

//搜索策略
//判断是否为终止节点
bool is_Terminal(std::shared_ptr<Board> v)
{
	int blue, red;
	blue = red = 0;
	for (int i = 0; i < 5; i++)
		for (int j = 0; j < 5; j++)
		{
			if (v->board[i][j] >= 1 && v->board[i][j] <= 6)
				red++;
			else if (v->board[i][j] >= 7)
				blue++;
		}
	if (red == 0 || blue == 0 || v->board[0][0] >= 7 || (v->board[4][4] >= 1 && v->board[4][4] <= 6))
		return true;
	else
		return false;
}

bool is_Expanded(std::shared_ptr<Board> v)
{
	if (v->posStep[0].size() > v->child.size())
		return false;
	else
		return true;
}

std::shared_ptr<Board> Treepolicy(std::shared_ptr<Board> v)
{
	while (!is_Terminal(v))
	{
		if (!is_Expanded(v))
			return Expand(v);
		else
			v = BestChild(v, coe);
	}
	return v;
}

//模拟函数
bool RandomMove(std::shared_ptr<Board> v, bool col)
{
	int ch[7] = { 0 };
	int x, y;
	srand((unsigned int)clock());
	int DICE = rand() % 6 + 1;
	if (col == 0)
	{
		for (int i = 0; i < 5; i++)
			for (int j = 0; j < 5; j++)
			{
				int temp = v->board[i][j];
				if (temp >= 1 && temp <= 6)
					ch[temp]++;
			}
		if (ch[DICE] == 1)
		{
			for (int i = 0; i < 5; i++)
				for (int j = 0; j < 5; j++)
					if (v->board[i][j] == DICE)
					{
						x = i;
						y = j;
						break;
					}
			if (x == 4)
			{
				v->board[x][y + 1] = DICE;
				v->board[x][y] = 0;
			}
			else if (y == 4)
			{
				v->board[x + 1][y] = DICE;
				v->board[x][y] = 0;
			}
			else
			{
				srand((unsigned int)clock());
				int direct = rand() % 3;
				if (direct == 0)
				{
					v->board[x][y + 1] = DICE;
					v->board[x][y] = 0;
				}
				else if (direct == 1)
				{
					v->board[x + 1][y] = DICE;
					v->board[x][y] = 0;
				}
				else
				{
					v->board[x + 1][y + 1] = DICE;
					v->board[x][y] = 0;
				}
			}
		}
		else
		{
			int cand_1, cand_2;
			cand_1 = cand_2 = 9;
			for (int i = DICE - 1; i > 0; i--)
			{
				if (ch[i] == 1)
				{
					cand_1 = i;
					break;
				}
			}
			for (int i = DICE + 1; i < 7; i++)
			{
				if (ch[i] == 1)
				{
					cand_2 = i;
					break;
				}
			}
			int choice;
			if (cand_1 != 9 && cand_2 != 9)
			{
				srand((unsigned int)clock());
				int cand = rand() % 2;
				if (cand == 0)
					choice = cand_1;
				else
					choice = cand_2;
			}
			else if (cand_1 == 9)
				choice = cand_2;
			else if (cand_2 == 9)
				choice = cand_1;
			for (int i = 0; i < 5; i++)
				for (int j = 0; j < 5; j++)
					if (v->board[i][j] == choice)
					{
						x = i;
						y = j;
						break;
					}
			if (x == 4)
			{
				v->board[x][y + 1] = choice;
				v->board[x][y] = 0;
			}
			else if (y == 4)
			{
				v->board[x + 1][y] = choice;
				v->board[x][y] = 0;
			}
			else
			{
				srand((unsigned int)clock());
				int direct = rand() % 3;
				if (direct == 0)
				{
					v->board[x][y + 1] = choice;
					v->board[x][y] = 0;
				}
				else if (direct == 1)
				{
					v->board[x + 1][y] = choice;
					v->board[x][y] = 0;
				}
				else
				{
					v->board[x + 1][y + 1] = choice;
					v->board[x][y] = 0;
				}
			}
		}
	}
	else
	{
		for (int i = 0; i < 5; i++)
			for (int j = 0; j < 5; j++)
			{
				int temp = v->board[i][j];
				if (temp >= 7)
					ch[temp - 6]++;
			}
		if (ch[DICE] == 1)
		{
			for (int i = 0; i < 5; i++)
				for (int j = 0; j < 5; j++)
					if (v->board[i][j] == DICE + 6)
					{
						x = i;
								y = j;
						break;
					}
			if (x == 0)
			{
				v->board[x][y - 1] = DICE + 6;
				v->board[x][y] = 0;
			}
			else if (y == 0)
			{
				v->board[x - 1][y] = DICE + 6;
				v->board[x][y] = 0;
			}
			else
			{
				srand((unsigned int)clock());
				int direct = rand() % 3;
				if (direct == 0)
				{
					v->board[x][y - 1] = DICE + 6;
					v->board[x][y] = 0;
				}
				else if (direct == 1)
				{
					v->board[x - 1][y] = DICE + 6;
					v->board[x][y] = 0;
				}
				else
				{
					v->board[x - 1][y - 1] = DICE + 6;
					v->board[x][y] = 0;
				}
			}
		}
else
		{
			int cand_1, cand_2;
			cand_1 = cand_2 = 19;
			for (int i = DICE - 1; i > 0; i--)
			{
				if (ch[i] == 1)
				{
					cand_1 = i + 6;
					break;
				}
			}
			for (int i = DICE + 1; i < 7; i++)
			{
				if (ch[i] == 1)
				{
					cand_2 = i + 6;
					break;
				}
			}
			int choice;
			if (cand_1 != 19 && cand_2 != 19)
			{
				srand((unsigned int)clock());
				int cand = rand() % 2;
				if (cand == 0)
					choice = cand_1;
				else
					choice = cand_2;
			}
			else if (cand_1 == 19)
				choice = cand_2;
			else if (cand_2 == 19)
				choice = cand_1;
			for (int i = 0; i < 5; i++)
				for (int j = 0; j < 5; j++)
					if (v->board[i][j] == choice)
					{
						x = i;
						y = j;
						break;
					}
			if (x == 0)
			{
				v->board[x][y - 1] = choice;
				v->board[x][y] = 0;
			}
			else if (y == 0)
			{
				v->board[x - 1][y] = choice;
				v->board[x][y] = 0;
			}
			else
			{
				srand((unsigned int)clock());
				int direct = rand() % 3;
				if (direct == 0)
				{
					v->board[x][y - 1] = choice;
					v->board[x][y] = 0;
				}
				else if (direct == 1)
				{
					v->board[x - 1][y] = choice;
					v->board[x][y] = 0;
				}
				else
				{
					v->board[x - 1][y - 1] = choice;
					v->board[x][y] = 0;
				}
			}
		}
	}
	return !col;
}

int simulate(std::shared_ptr<Board> v)
{
	int origin[5][5];
	for (int i = 0; i < 5; i++)
		for (int j = 0; j < 5; j++)
			origin[i][j] = v->board[i][j];
	bool tempColor = v->color;
	while (!is_Terminal(v))
	{
		tempColor = RandomMove(v, tempColor);
	}
	//返回模拟结果
	int result;
	int blue, red;
	blue = red = 0;
	for (int i = 0; i < 5; i++)
		for (int j = 0; j < 5; j++)
		{
			if (v->board[i][j] >= 1 && v->board[i][j] <= 6)
				red++;
			else if (v->board[i][j] >= 7)
				blue++;
		}
	if (blue == 0 || (v->board[4][4] >= 1 && v->board[4][4] <= 6))
		result = 0;
	else
		result = 1;
	for (int i = 0; i < 5; i++)
		for (int j = 0; j < 5; j++)
			v->board[i][j] = origin[i][j];
	return result;
}

void Backup(std::shared_ptr<Board> v, int result)
{
	while (v)
	{
		v->visit_times++;							//到底是谁的visit次数？
		if ((v->color ^ result))
			v->win_time++;
		v->quality = double(v->win_time) / double(v->visit_times);		//到底是谁的visit次数？
		v = v->parent;
	}
}


void DeleteAll(std::shared_ptr<Board> v)
{
	for (int i = 0; i < v->child.size(); i++)
	{
		DeleteAll(v->child[i]);
	}
	std::vector<std::shared_ptr<Board>>().swap(v->child);
}

std::shared_ptr<Board> UCT(int dice, int board[5][5])
{
	std::shared_ptr<Board> root;
	std::shared_ptr<Board> none;
	if (dice <= 6)
		root=std::make_shared<Board>(none, 0, board, dice);
	else
		root = std::make_shared<Board>(none, 1, board, dice);
	time_t begin = clock();
	time_t end = clock();
	while (double(end - begin)/CLOCKS_PER_SEC < 5)
	{
		std::shared_ptr<Board> p;
		p = Treepolicy(root);
		int result = simulate(p);
		Backup(p, result);
		end = clock();
	}
	std::shared_ptr<Board> best;
	best = MostWin(root);
	return best;
}

int Einstein::handle()
{
	int t = clientsocket.recvMsg();
	if (t == 0)
	{
		char* a = clientsocket.getRecvMsg();
		std::string rec = a;
		parse(rec);
		int board[5][5];
		for (int i = 0; i < 5; i++)
			for (int j = 0; j < 5; j++)
				board[i][j] = chessboard[i * 5 + j];
		//std::string str = fool(dice, board);
		std::shared_ptr<Board> best;
		best = UCT(dice, board);
		std::string str;
		if (dice <= 6)
		{
			if (best->chess[1] == 0)
				str = std::to_string(best->chess[0]) + "|right";
			else if (best->chess[1] == 1)
				str = std::to_string(best->chess[0]) + "|down";
			else if (best->chess[1] == 2)
				str = std::to_string(best->chess[0]) + "|rightdown";
		}
		else
		{
			if (best->chess[1] == 0)
				str = std::to_string(best->chess[0] + 6) + "|left";
			else if (best->chess[1] == 1)
				str = std::to_string(best->chess[0] + 6) + "|up";
			else if (best->chess[1] == 2)
				str = std::to_string(best->chess[0] + 6) + "|leftup";
		}
		now_time = time(NULL);
		clientsocket.sendMsg(str.c_str());
		std::string m = rec + " operation " + str;
		pt = localtime(&now_time);
		std::string log = std::to_string(pt->tm_year + 1900) + "-" + std::to_string(pt->tm_mon + 1) + "-" + std::to_string(pt->tm_mday) + " " + std::to_string(pt->tm_hour) + "-" + std::to_string(pt->tm_min) + "-" + std::to_string(pt->tm_sec) + " : " + m;
		logging(log);
		std::cout << best->quality * 100.0 << "%" << std::endl << std::endl;
		DeleteAll(best->parent);
		return 1;
	}
	else
	{
		std::string g = logger.back();
		int last[25];
		int f = g.find("[[", 0);
		int k = g.find("], [", f + 2);
		int l = g.find("], [", k + 4);
		int m = g.find("], [", l + 4);
		int n = g.find("], [", m + 4);
		int p = g.find("]]|", n + 4);
		to_int(g, f + 2, k, last, 0);
		to_int(g, k + 4, l, last, 1);
		to_int(g, l + 4, m, last, 2);
		to_int(g, m + 4, n, last, 3);
		to_int(g, n + 4, p, last, 4);
		int diff = 0;
		for (int i = 0; i < 25; i++)
			if (chessboard[i] != last[i])
				diff++;
		if (diff <= 4)
			return 0;
		else
		{
			int ze = g.rfind(" ");
			int pr = g.rfind("|");
			int chess = std::stoi(g.substr(ze + 1, pr - ze - 1));
			std::string operation = g.substr(pr + 1, g.length() - pr - 1);
			int t = 0;
			while (last[t] != chess)
				t++;
			if (operation == "up")
			{
				last[t - 5] = chess;
				last[t] = 0;
			}
			else if (operation == "left")
			{
				last[t - 1] = chess;
				last[t] = 0;
			}
			else if (operation == "leftup")
			{
				last[t - 6] = chess;
				last[t] = 0;
			}
			else if (operation == "down")
			{
				last[t + 5] = chess;
				last[t] = 0;
			}
			else if (operation == "right")
			{
				last[t + 1] = chess;
				last[t] = 0;
			}
			else
			{
				last[t + 6] = chess;
				last[t] = 0;
			}
			end_time = clock();
			double lasting = difftime(end_time, begin_time) / (double)CLOCKS_PER_SEC;
			if (chess >= 7)
			{
				int red = 0;
				for (int i = 0; i < 25; i++)
					if (last[i] >= 1 && last[i] <= 6)
						red++;
				if (red == 0 || last[0] == chess)
				{
					logging("Game over! Blue wins! Time: " + std::to_string(lasting) + "s");
					return 0;
				}
				else
				{
					logging("Game over! Red wins! Time: " + std::to_string(lasting) + "s");
					return 0;
				}
			}
			else
			{
				int blue = 0;
				for (int i = 0; i < 25; i++)
					if (last[i] >= 7 && last[i] <= 12)
						blue++;
				if (blue == 0 || last[24] == chess)
				{
					logging("Game over! Red wins! Time: " + std::to_string(lasting) + "s");
					return 0;
				}
				else
				{
					logging("Game over! Blue wins! Time: " + std::to_string(lasting) + "s");
					return 0;
				}
			}
		}
		return 0;
	}
}

int Einstein::logging(std::string s)
{
	logger.push_back(s);
	std::cout << s << std::endl;
	return 0;
}

int Einstein::writelog()
{
	std::ofstream fout;
	filename = std::to_string(pt->tm_year + 1900) + "-" + std::to_string(pt->tm_mon + 1) + "-" + std::to_string(pt->tm_mday) + "-181860013.log";	
	fout.open(filename);
	std::list<std::string>::iterator itor;
	itor = logger.begin();
	while (itor != logger.end())
		fout << *itor++ << std::endl;
	return 0;
}

std::string fool(int dice, int board[5][5])
{
	int chess[7] = { 0 };
	if (dice <= 6)
	{
		for (int i = 0; i < 5; i++)
			for (int j = 0; j < 5; j++)
				if (board[i][j] >= 1 && board[i][j] <= 6)
					chess[board[i][j]]++;
		int x, y;
		if (chess[dice] == 1)
		{
			for (int i = 0; i < 5; i++)
				for (int j = 0; j < 5; j++)
					if (board[i][j] == dice)
					{
						x = i;
						y = j;
					}
		}
		else
		{
			//从中间向两边找
			int x1, x2, y1, y2;
			x1 = x2 = y1 = y2 = 9;
			for (int k = dice - 1; k > 0; k--)
				if (chess[k] == 1)
				{
					for (int i = 0; i < 5; i++)
						for (int j = 0; j < 5; j++)
							if (board[i][j] == k)
							{
								x1 = i;
								y1 = j;
							}
					break;
				}
			for (int k = dice + 1; k < 7; k++)
				if (chess[k] == 1)
				{
					for (int i = 0; i < 5; i++)
						for (int j = 0; j < 5; j++)
							if (board[i][j] == k)
							{
								x2 = i;
								y2 = j;
							}
					break;
				}
			if ((4 - x1) * (4 - x1) + (4 - y1) * (4 - y1) > (4 - x2) * (4 - x2) + (4 - y2) * (4 - y2))
			{
				x = x2;
				y = y2;
			}
			else
			{
				x = x1;
				y = y1;
			}
		}
		if (x == 4)
		{
			std::string a = std::to_string(board[x][y]) + "|right";
			return a;
		}
		else if (y == 4)
		{
			std::string a = std::to_string(board[x][y]) + "|down";
			return a;
		}
		else
		{
			std::string a = std::to_string(board[x][y]) + "|rightdown";
			return a;
		}
	}
	else
	{
		for (int i = 0; i < 5; i++)
			for (int j = 0; j < 5; j++)
				if (board[i][j] >= 7 && board[i][j] <= 12)
					chess[board[i][j] - 6]++;
		int x, y;
		if (chess[dice - 6] == 1)
		{
			for (int i = 0; i < 5; i++)
				for (int j = 0; j < 5; j++)
					if (board[i][j] == dice)
					{
						x = i;
						y = j;
					}
		}
		else
		{
			//从中间向两边找
			int x1, x2, y1, y2;
			x1 = x2 = y1 = y2 = 9;
			for (int k = dice - 6 - 1; k > 0; k--)
				if (chess[k] == 1)
				{
					for (int i = 0; i < 5; i++)
						for (int j = 0; j < 5; j++)
							if (board[i][j] == k + 6)
							{
								x1 = i;
								y1 = j;
							}
					break;
				}
			for (int k = dice - 6 + 1; k < 7; k++)
				if (chess[k] == 1)
				{
					for (int i = 0; i < 5; i++)
						for (int j = 0; j < 5; j++)
							if (board[i][j] == k + 6)
							{
								x2 = i;
								y2 = j;
							}
					break;
				}
			if (x1 * x1 + y1 * y1 > x2 * x2 + y2 * y2)
			{
				x = x2;
				y = y2;
			}
			else
			{
				x = x1;
				y = y1;
			}
		}
		if (x == 0)
		{
			std::string a = std::to_string(board[x][y]) + "|left";
			return a;
		}
		else if (y == 0)
		{
			std::string a = std::to_string(board[x][y]) + "|up";
			return a;
		}
		else
		{
			std::string a = std::to_string(board[x][y]) + "|leftup";
			return a;
		}
	}
}
