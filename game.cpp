/*
Riley Hoffman hoffm455@xserver
2016-10-24
honors1.cpp

Plays Mancala
Strategy Player vs Random PLayer
*/

#include <iostream>
using std::cout; using std::endl; using std::cin;
#include <iomanip>
using std::setfill; using std::setw;
#include <vector>
using std::vector;
#include <random>
#include <string>
using std::string;
#include <fstream>
using std::ofstream;
#include <algorithm>
using std::sort;

//checks if one of the boards is empty (aside from score pit)
bool check_zeros(const vector<long>& board1, vector<long>& board2, string& empty_board){
	vector<long> empty = {0,0,0,0,0,0};
	vector<long> board1_pits(board1.begin(),board1.end()-1);
	vector<long> board2_pits(board2.begin(),board2.end()-1);
	
	if(board1_pits == empty){
		empty_board = "b1";
		return false;
	}
	else if(board2_pits == empty){
		empty_board = "b2";
		return false;
	}
	else{
		return true;
	}
	
}

long make_move(long pit, string player, vector<long> &board1, 
	vector<long> &board2){ //pit is which pit is being moved
		
	long last_pit=0;
	
	//Strategy players move
	if(player == "SP"){
		//start of turn
		long beans = board1.at(pit);
		board1[pit] = 0;
		
		//starts at pit that is picked
		int i=1;
		for(;i<=6-pit && beans>0;i+=1){
			board1.at(pit+i)+=1;
			beans-=1;			
			last_pit=pit+i;
		}
		
		while(beans>0){//loops around the board
			for(int i=0;i<6 && beans>0;i+=1){//<6 doesn't add to 
				board2.at(i)+=1;			//opponent score pit
				beans-=1;
				last_pit=7;//don't care if last pit is on opponent side
			}
			
			int i=0;
			for(;i<=6 && beans>0;i+=1){//<=6 to adds to score pit
				board1.at(i)+=1;
				beans-=1;
				last_pit=i;
			}
		}
		//checks if player gets to grab opponents beans
		//if last pit is on players side and was previously empty(now=1)
		if(last_pit<=5 && last_pit>=0 && board1.at(last_pit) == 1){
			if(board2.at(5-last_pit)!=0){
				//add beans to score
				//cout << "StrategyPlayer captured "<< board2.at(5-last_pit)+1 << " beans." << endl;
				board1.at(6)+=board2.at(5-last_pit)+1;
				board2.at(5-last_pit)=0;
				board1.at(last_pit)=0;
			}
		}
	}
	//same process but starting on random player side
	else if(player == "RP"){
		//start of turn
		long beans = board2.at(pit);
		board2[pit] = 0;
		
		//starts at pit that is picked
		int i=1;
		for(;i<=6-pit && beans>0;i+=1){
			board2.at(pit+i)+=1;
			beans-=1;
			last_pit=pit+i;		
		}
		
		while(beans>0){//loops around the board
			for(int i=0;i<6 && beans>0;i+=1){//<6 doesn't add to 
				board1.at(i)+=1;			//opponent score pit
				beans-=1;
				last_pit=7;//don't care if last pit is on opponent side
			}
			
			int i=0;
			for(;i<=6 && beans>0;i+=1){//<=6 to adds to score pit
				board2.at(i)+=1;
				beans-=1;
				last_pit=i;
			}
		}
		//checks if player gets to grab opponents beans
		//if last pit is on players side and was previously empty(now=1)
		if(last_pit<=5 && last_pit>=0 && board2.at(last_pit) == 1){
			if(board1.at(5-last_pit)!=0){
				//add beans to score
				//cout << "RandomPlayer captured "<< board1.at(5-last_pit)+1 << " beans." << endl;
				board2.at(6)+=board1.at(5-last_pit)+1;
				board1.at(5-last_pit)=0;
				board2.at(last_pit)=0;
			}
		}
	}
	return last_pit;
}

long random_player(std::default_random_engine &eng, 
	std::uniform_int_distribution<long> &dist ,const vector<long> &board2){
	long random_move=0,r;
	r=dist(eng);
	//makes sure the move is valid, can't choose empty pit
	while(board2.at(r)==0){
		r=dist(eng);
	}
	random_move=r;
	return random_move;
}

long strat_player(const vector<long> &board1){
	long strat_move;
	for(long l=0;l<6;++l){
		if( (board1.at(l)==(6-l))||(board1.at(l)==(19-l)) ){
			strat_move = l;
			l=6; //to stop the loop
		} 		//picks first pit that will sow into store
		else{
			if(board1.at(l)!=0){
				strat_move = l;
			}
		}
	}
	return strat_move;
}

void print_board(vector<long> board1, vector<long> board2, string player_turn, ofstream& some_file){
	/*
	------------RP's turn------------
	 RP  [04][04][04][04][04][04]  SP
	[00] [04][04][04][04][04][04] [00]
	*/
	some_file <<"------------"<<player_turn<<"'s turn------------"<<endl;
	some_file << " RP ";
	for(long l=5;l>=0;l-=1){
		some_file << "[" << setw(2) << setfill('0') << board2.at(l) << "]";
	}
	some_file << " SP " << endl;
	some_file <<"["<< setw(2) << board2.at(6) <<"]";
	for(long l=0;l<=5;l+=1){
		some_file << "[" << setw(2) << setfill('0') << board1.at(l) << "]";
	}
	some_file <<"["<< setw(2) << board1.at(6) <<"]"<< endl;
}

void single_play_game(vector<long> board1, vector<long> board2, 
	std::default_random_engine &eng, std::uniform_int_distribution<long> &dist,
	ofstream &some_file){
	//initializes game
	string player="SP";
	long sp_score=0,rp_score=0;
	bool end=true;
	while(end){
		
		print_board(board1,board2,player,some_file);
		sp_score=board1.at(6);
		rp_score=board2.at(6);
		
		if(player=="SP"){
			long strat_choice = strat_player(board1);
			//if the beans you pick up land in your store
			if( board1.at(strat_choice) == (6-strat_choice) ){
				//go again
				make_move(strat_choice,"SP",board1,board2);
				some_file << "StrategyPlayer gets to move again because" << endl;
				player="SP";
			}
			else{//otherwise
				//its the next players turn
				make_move(strat_choice,"SP",board1,board2);
				player="RP";
			}
			some_file << "Pit " << strat_choice << " was chosen by SP" << endl;
			some_file << "SP + "<< board1.at(6)-sp_score << endl;//shows change of score
			some_file << endl;
		}
		
		else if(player=="RP"){
			long random_choice = random_player(eng, dist, board2);
			//if the last bean you place is in your store
			if( (board2.at(6)) == (6-random_choice) ){
				//go again
				make_move(random_choice,"RP",board1,board2);
				some_file << "RandomPlayer gets to move again because" << endl;
				player="RP";
			}
			else{//otherwise
				//its the next players turn
				make_move(random_choice,"RP",board1,board2);
				player="SP";
			}
			some_file << "Pit " << random_choice << " was chosen by RP" << endl;
			some_file << "RP + "<< board2.at(6)-rp_score << endl;//show change of score
			some_file << endl;
		}
		
		string empty_board="";
		long leftover=0;
		end=check_zeros(board1,board2,empty_board);
		//checks if one of the boards are empty/the game is over
		//also moves the rest of the beans into other players score
		if(end==false && empty_board=="b1"){
			leftover=board2.at(0)+board2.at(1)+board2.at(2)+board2.at(3)+board2.at(4)+board2.at(5);
			board2.at(6) += leftover;
			for(int i=0;i<6;++i){
				board2.at(i)=0;
			}
			print_board(board1,board2,player,some_file);
			some_file << "RP + " << leftover << endl;//shows final change of score
			some_file << "Game End" << endl;
		}
		else if(end==false && empty_board=="b2"){
			leftover = board1.at(0)+board1.at(1)+board1.at(2)+board1.at(3)+board1.at(4)+board1.at(5);
			board1.at(6) += leftover;
			for(int i=0;i<6;++i){
				board1.at(i)=0;
			}
			print_board(board1,board2,player,some_file);
			some_file << "CP + " << leftover << endl;//shows final change of score
			some_file << "Game End" << endl;
		}
	}
}


void multi_play_game(long rounds,vector<long> board1, 
	vector<long> board2, ofstream &some_file, vector<long>& scores1,
	vector<long>& scores2,long& win1,long& win2,long& tie,long dice_roll,std::default_random_engine &eng, 
	std::uniform_int_distribution<long> &dist ){
	//exactly the same as single play, however it collects data on scores
	//after the game is done, and does not print out visual board & such
	string player="";
	//seed is count of games for random player
		if(dice_roll<=2){
			player="RP";
		}
		else if(dice_roll>=3){
			player="SP";
		}
		//initializes game
		bool end=true;
		while(end){
			
			//sp turn
			if(player=="SP"){
				long strat_choice = strat_player(board1);
				//if the beans you pick up land in your store
				if( board1.at(strat_choice) == (6-strat_choice) ){
					//make move and go again
					make_move(strat_choice,"SP",board1,board2);
					player="SP";
				}
				else{//otherwise
					//make move but its the next players turn
					make_move(strat_choice,"SP",board1,board2);
					player="RP";
				}
			}
			//rp turn
			else if(player=="RP"){
				long random_choice = random_player(eng, dist, board2);
				//if the last bean you place is in your store
				if( (board2.at(6)) == (6-random_choice) ){
					//make move and go again
					make_move(random_choice,"RP",board1,board2);
					player="RP";
				}
				else{//otherwise
					//make move but its the next players turn
					make_move(random_choice,"RP",board1,board2);
					player="SP";
				}
			}
			
			//checks if game is over by checking if a board is empty
			//if so moves the rest of the beans into other players score
			//also records final scores in score vectors
			string empty_board="";
			long leftover=0;
			end=check_zeros(board1,board2,empty_board);
			//if board1 is empty
			if(end==false && empty_board=="b1"){
				leftover=board2.at(0)+board2.at(1)+board2.at(2)+board2.at(3)+board2.at(4)+board2.at(5);
				board2.at(6) += leftover;
				for(int i=0;i<6;++i){
					board2.at(i)=0;
				}
				//since game has ended, record final scores
				scores1.push_back(board1.at(6));
				scores2.push_back(board2.at(6));
				
				//record number of times player won
				if(board1.at(6)>board2.at(6)){
					win1+=1;
				}
				else if(board2.at(6)>board1.at(6)){
					win2+=1;
				}
				else if(board2.at(6)==board1.at(6)){
					tie+=1;
				}
			}
			//if board2 is empty
			else if(end==false && empty_board=="b2"){
				leftover = board1.at(0)+board1.at(1)+board1.at(2)+board1.at(3)+board1.at(4)+board1.at(5);
				board1.at(6) += leftover;
				for(int i=0;i<6;++i){
					board1.at(i)=0;
				}				
				//since game has ended, record final scores
				scores1.push_back(board1.at(6));
				scores2.push_back(board2.at(6));
				//record number of times player won
				if(board1.at(6)>board2.at(6)){
					win1+=1;
				}
				else if(board2.at(6)>board1.at(6)){
					win2+=1;
				}
				else if(board2.at(6)==board1.at(6)){
					tie+=1;
				}
			}
		}
	
}
void loop_multi_play_game(long rounds,vector<long> board1, 
	vector<long> board2, ofstream &some_file){
		
	vector<long> scores1,scores2;
	long win1=0,win2=0,tie=0;
	std::default_random_engine eng(rounds);
	std::uniform_int_distribution<long> dist(0,5);
	
	for(long count=1;count<=rounds;count+=1){
		long dice_roll = dist(eng);
		multi_play_game(rounds,board1,board2,some_file,scores1,scores2,win1,win2,tie,dice_roll,eng,dist);
	}
	double average1=0,average2=0;
	double median1=0,median2=0;
	long highest1=0,highest2=0;
	long lowest1=50,lowest2=50;
	
	//to get averages & highest & lowest
	for(long l:scores1){
		average1+=l;
		if(l>highest1){
			highest1=l;
		}
		if(l<lowest1){
			lowest1=l;
		}
	}
	average1/=scores1.size();

	for(long l:scores2){
		average2+=l;
		if(l>highest2){
			highest2=l;
		}
		if(l<lowest2){
			lowest2=l;
		}
	}
	average2/=scores2.size();

	/* To see unsorted scores:	
		for(long l:scores1){
			some_file<<l<<",";
		}some_file<<endl;
		for(long l:scores2){
			some_file<<l<<",";
		}some_file<<endl;
	*/
	
	//sort the scores vectors
	sort(scores1.begin(),scores1.end());
	sort(scores2.begin(),scores2.end());
	
	//to get medians
	if(scores1.size()%2==1){//is odd
		median1=scores1.at(scores1.size()/2);
	}
	else if(scores1.size()%2==0){//is even
		median1=((scores1.at(scores1.size()/2.0))+(scores1.at((scores1.size()/2.0)-1)))/2.0;
	}
	
	if(scores2.size()%2==1){//is odd
		median2=scores2.at(scores2.size()/2);
	}
	else if(scores2.size()%2==0){//is even
		median2=((scores2.at(scores2.size()/2.0))+(scores2.at((scores2.size()/2.0)-1)))/2.0;
	}
	
	some_file << "Rounds played: "<< rounds << endl;
	some_file << "Games Tied:" << tie << endl;
	some_file << endl;
	
	some_file << "RandomPlayer" << endl;
	some_file << "Games Won:" << win2 << endl;
	some_file << "Average: " << average2 << endl;
	some_file << "Median: " << median2 << endl;
	some_file << "Lowest Score: " << lowest2 << endl;
	some_file << "Highest Score: " << highest2 << endl;
	some_file << endl;
	
	some_file << "StrategyPlayer" << endl;
	some_file << "Games Won:" << win1 << endl;
	some_file << "Average: " << average1 << endl;
	some_file << "Median: " << median1 << endl;
	some_file << "Lowest Score: " << lowest1 << endl;
	some_file << "Highest Score: " << highest1 << endl;
	some_file << endl;
	
	some_file << "Win Percentage" << endl;
	float rounds_float=rounds;
	some_file << "RP:" << 100.0*(win2/rounds_float) << "%" << endl;
	some_file << "SP:" << 100.0*(win1/rounds_float) << "%" << endl;
	some_file << "Tie:" << 100.0*(tie/rounds_float) << "%" << endl;
	
}

/*
Board representation w/ indicies of vectors:
6 5 4 3 2 1 0

0 4 4 4 4 4 4    b2 rp:random player
  4 4 4 4 4 4 0  b1 sp:strategy player

  0 1 2 3 4 5 6
*/
int main(){
						// 0 1 2 3 4 5 6
	vector<long> board1 = {4,4,4,4,4,4,0}; //score at ends
						// 0 1 2 3 4 5 6
	vector<long> board2 = {4,4,4,4,4,4,0};//b2 reverse layout
	
	//input random seed and number of rounds
	long seed,rounds;
	cin >> seed;
	cin >> rounds;
	
	std::default_random_engine eng(seed);
	std::uniform_int_distribution<long> dist(0,5);
	
	//start single play
	ofstream single_play;
	single_play.open ("single_play.txt");
	
	single_play_game(board1,board2,eng,dist,single_play);
	
	single_play.close();
	
	//start multi play
	ofstream multi_play;
	multi_play.open ("multiple_play.txt");
	
	loop_multi_play_game(rounds,board1,board2,multi_play);
	
	multi_play.close();
}

