#include <iostream>
#include <cmath>
#include <string>
#include <fstream>
#include <vector>
#include <algorithm>

using namespace std;

void printpuzzle(int p[][9]);
pair<int,int> generate_box(int row, int col);
bool vertical_check(int p[][9], int n, int col);
bool box_check(int p[][9], int n,int row, int col);
void solve_puzzle(int p[][9], std::vector<pair<int,int> > &original_pos);
bool fill_cells(int p[][9], int row,int col, vector<pair<int,int> > &original_pos);
bool find_cell(int p[][9], vector<pair<int,int> > &original_pos, int &row,int &col);
bool check_empty_space(int p[][9],std::vector<pair<int,int> > & original_pos, int row, int col);

int main(int argc, char** argv) {
	//read in puzzle
	std::ifstream puzzlefile(argv[1]);
	if(!puzzlefile) {
		std::cerr << "Failed to open puzzle " << argv[1] << " for reading." << std::endl;
	}
	string numbers;
	//create board
	int puzzle[9][9];
	int i=0,j=0;
	//i is for the row and j is for the column
	//line is to move and keep track of j and i
	int line = 0;
	std::vector<pair<int,int> > original_pos;
	//insert numbers into board
	while(puzzlefile >> numbers) {
		string copy_num = numbers;
		//extract each number from the 'word'
		for (int k = 0; k < copy_num.size(); ++k) {
			string single;
			//gets a single number
			single = copy_num[k];
			puzzle[i][j] = stoi(single);
			j++;
		}
		line++;
		//reached the end of the row of the sudoku puzzle
		if (line == 3) {
			i++;
			line = 0;
			j = 0;
		}
	}
	//creates a vector of immutable positions
	for (int i = 0; i < 9; ++i)	{
		for (int j = 0; j < 9; ++j) {
			if(puzzle[i][j] != 0) {
				original_pos.push_back(make_pair(i,j));
			}
		}
	}
	printpuzzle(puzzle);
	solve_puzzle(puzzle,original_pos);
}

//print puzzle
void printpuzzle(int p[][9]) {
	for(int i=1; i<=9;i++) {
		for (int j = 1; j <= 9;j ++) {
			cout<<p[i-1][j-1];
			if (j%3 == 0) {
				cout<<" ";
			}
		}
		cout<<"\n";
		if (i%3 == 0) {
				cout<<"\n";
			}
	}
	
}

//returns false if number is found in the row 
//i.e number cannot be placed here
bool horizontal_check(int p[][9], int n, int row) {
	for (int i = 1; i <= 9; ++i) {
		if (p[row-1][i-1] == n)	{
			return false;
		}
	}
	return true;
}

//returns false if number is found in the col 
//i.e number cannot be placed here
bool vertical_check(int p[][9], int n, int col) {
	for (int i = 1; i <= 9; ++i) {
		if (p[i-1][col-1] == n)	{
			return false;
		}
	}
	return true;
}

pair<int,int> generate_box(int row, int col) {
	vector<pair<int,int> > startindex = { make_pair(0,0),
										  make_pair(0,3), 
										  make_pair(0,6),
										  make_pair(3,0),
										  make_pair(3,3),
										  make_pair(3,6), 
										  make_pair(6,0),
										  make_pair(6,3),
										  make_pair(6,6) };
	//if within bounds given then it returns that index
	for (int i = 0; i < startindex.size(); ++i)	{
		if (row-1 < (startindex[i].first+3) && col-1 < (startindex[i].second+3)) {
			return startindex[i];
		}
	}
	//if none of this applies
	return make_pair(-1,-1);
}

//returns false if number is found in the box 
//i.e number cannot be placed here
bool box_check(int p[][9], int n,int row, int col) {
	//function call to figure out which box needs to be checked
	pair<int,int> startindex = generate_box(row, col);
	//check
	for (int i = 0; i < 3; ++i)	{
		for (int j = 0; j < 3; ++j)	{
			if (p[startindex.first + i][ startindex.second + j] == n) {
				return false;
			}
		}
	}
	return true;
}

//find the next empty cell
bool find_cell(int p[][9], vector<pair<int,int> > &original_pos, int &row,int &col) {
	if ((row <= 9 && row > 0) && (col < 9 && col > 0)) {	
		//case where you just need to go to the next column
		//if its an empty space then sure change col
		if (check_empty_space(p,original_pos,row,col+1)) {
			col++;
			return true;
		}
		else { //change col try again if it wasn't an empty space
			col++;
			if(find_cell(p,original_pos,row,col))
			return true;
		}
	}
	else if( col == 9 && (row < 9 && row > 0)) {
		//case where if you've reached the end of the col and you need to go to the next row
		//found an empty cell
		col = 1;
		if (check_empty_space(p,original_pos,row+1,col)) {
			row++;
			return true;
		}
		else { //did not find an empty cell
			row++;
			find_cell(p,original_pos,row,col);
			return true;
		}
	}
	//if it reaches here then there are no more empty cells
	return false;
}

void solve_puzzle(int p[][9], std::vector<pair<int,int> > &original_pos) {
	//calls function that solves the puzzle
	if(fill_cells(p,1,1,original_pos))
	{	//print puzzle
		printpuzzle(p);
	}
	else
	{
		cout<<"No solutions";
	}
}

bool fill_cells(int p[][9], int row,int col, vector<pair<int,int> > &original_pos) {
	//searches for the next free cell
	//if none are left then sudoku puzzle is solved
	if(!find_cell(p,original_pos,row,col)) return true;
	for (int i = 1; i <=9 ; ++i) {	
		//check if number can be placed 
		//if number fails the vertical, horizontal and box check tests 
		//then next number is tried
		if (vertical_check(p,i,col) && box_check(p,i,row,col) && horizontal_check(p,i,row)) {
			p[row-1][col-1] = i;//inserts the number
			//recursive function call 
			if (fill_cells(p,row,col,original_pos))	{
				return true;
			}
			//if it reaches here then a mistake was made
			//so leaves this spot and tries another number in the previous spot
			p[row-1][col-1] = 0;
		}
	}
	return false;
}

//returns true if space is empty aka 0 and not one of the immutable positions
bool check_empty_space(int p[][9],std::vector<pair<int,int> > & original_pos, int row, int col) {
	vector<pair<int,int> >::iterator it = find(original_pos.begin(),original_pos.end(),
												make_pair(row-1,col-1));
	if (it == original_pos.end() && p[row-1][col-1] == 0)
		return true;
	return false;
}