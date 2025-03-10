/*

3000|------------------------------------------------------------|
    |------------------------------------------------------------|
    |------------------------------------------------------------|
    |------------------------------------------------------------|
    |------------------------------------------------------------|
2500|------------------------------------------------------------|
    |------------------------------------------------------------|
    |------------------------------------------------------------|
    |------------------------------------------------------------|
    |------------------------------------------------------------|
2000|------------------------------------------------------------|
    |------------------------------------------------------------|
    |------------------------------------------------------------|
    |------------------------------------------------------------|
    |------------------------------------------------------------|
1500|------------------------------------------------------------|
    |------------------------------------------------------------|
    |------------------------------------------------------------|
    |------------------------------------------------------------|
    |------------------------------------------------------------|
1000|X----------X-----------X-----------X-----------X-----------X|
    |           |           |           |           |           |
    0           1           2           3           4           5


(Map)[Game1:(Clicks: 2700), Game2:(Clicks: 2300), Game3:(Clicks: 2000), Game4:(Clicks: 2500), Game5:(Clicks: 1000)]

Game 1: 2700
Game 2: 2300
Game 3: 2000
Game 4: 2500
Game 5: 1000
*/

#include <vector>
#include <iostream>
#include <string>
#include <list>
#include <map>
#include <fstream>

// GLOBAL VARIABLE for counting the games until 5 before reset and outing the graph to txt
int GraphGames = 1;
int ClicksList[5] {0, 0, 0, 0, 0};

std::map<std::string, int> GraphMap;
std::pair<std::map<std::string, int>::iterator, bool> InsertToGraph;


std::string ReturnGraph(std::vector<std::vector<char>> Graph, int Line)
{
    std::string Return = "";
    for (int j = 0; j < Graph[Line].size(); j++)
    {
        //std::cout << Line << " " << j; NewLine << "\n";
        Return += Graph[Line][j];
    }
    return Return;
}

void MakeGraphToFile(std::vector<std::vector<char>> Graph)
{
    std::ofstream GraphFile("Graph.txt", std::ios::app);
    GraphFile << "3000|" << ReturnGraph(Graph, 0)  << "|\n";
    GraphFile << "    |" << ReturnGraph(Graph, 1)  << "|\n";
    GraphFile << "    |" << ReturnGraph(Graph, 2)  << "|\n";
    GraphFile << "    |" << ReturnGraph(Graph, 3)  << "|\n";
    GraphFile << "    |" << ReturnGraph(Graph, 4)  << "|\n";
    GraphFile << "2500|" << ReturnGraph(Graph, 5)  << "|\n";
    GraphFile << "    |" << ReturnGraph(Graph, 6)  << "|\n";
    GraphFile << "    |" << ReturnGraph(Graph, 7)  << "|\n";
    GraphFile << "    |" << ReturnGraph(Graph, 8)  << "|\n";
    GraphFile << "    |" << ReturnGraph(Graph, 9)  << "|\n";
    GraphFile << "2000|" << ReturnGraph(Graph, 10) << "|\n";
    GraphFile << "    |" << ReturnGraph(Graph, 11) << "|\n";
    GraphFile << "    |" << ReturnGraph(Graph, 12) << "|\n";
    GraphFile << "    |" << ReturnGraph(Graph, 13) << "|\n";
    GraphFile << "    |" << ReturnGraph(Graph, 14) << "|\n";
    GraphFile << "1500|" << ReturnGraph(Graph, 15) << "|\n";
    GraphFile << "    |" << ReturnGraph(Graph, 16) << "|\n";
    GraphFile << "    |" << ReturnGraph(Graph, 17) << "|\n";
    GraphFile << "    |" << ReturnGraph(Graph, 18) << "|\n";
    GraphFile << "    |" << ReturnGraph(Graph, 19) << "|\n";
    GraphFile << "1000|" << ReturnGraph(Graph, 20) << "|\n";
    GraphFile << "     |           |           |           |           |          |\n";
    GraphFile << "     0           1           2           3           4          5\n";
    GraphFile << "\n\n";
    GraphFile.close();
}

int CreateGraph(std::map<std::string, int> GameAndClicks, bool Force) // GameAndClicks = GameNUM: value int
{
    std::ostream& NewLine = std::cout;
    /* 
    Vector whole lengt = 60
    1 : slot 12, 2 : 12, 3 : 12, 4 : 12, 5 : 12
    1 : slot 12, 2 : 24, 3 : 36, 4 : 48, 5 : 60 

    ** EVERY SLOT HAS TO -1 ONCE ** 
    
    3000 = : 0
    2900 = : 1, 2800 = : 2, 2700 = : 3, 2600 = : 4, 2500 = : 5, 2400 = : 6, 2300 = : 7, 2200 = : 8, 2100 = : 9, 2000 = : 10
    1900 = : 11, 1800 = : 12, 1700 = : 13, 1600 = : 14, 1500 = : 15, 1400 = : 16, 1300 = : 17, 1200 = : 18, 1100 = : 19
    1000 >= : 20
    */
    std::vector<std::vector<char>> Graph = 
    {
        {'a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a'},
        {'a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a'},
        {'a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a'},
        {'a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a'},
        {'a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a'},
        {'a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a'},
        {'a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a'},
        {'a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a'},
        {'a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a'},
        {'a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a'},
        {'a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a'},
        {'a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a'},
        {'a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a'},
        {'a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a'},
        {'a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a'},
        {'a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a'},
        {'a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a'},
        {'a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a'},
        {'a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a'},
        {'a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a'},
        {'a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a'}
    };

    std::cout << "Add the clicks to the list: ClicksList[i] = GameClicks['Game' + game number]"; NewLine << "\n";
    std::cout << "Before: "; for (int i = 0; i < 5; i++) {std::cout << ClicksList[i] << ":" << i << " ";} NewLine << "\n"; // Outs ClicksList before adding

    // Adds clicks to list and if not enough games played sets rest to 0 : This means the action is forced
    if (Force == true) // Forced means it will do the action no matter if the games has been played or not 
    {
        while(true) {for (int i = 5; i > GraphGames; i--) {ClicksList[i] = 0;} break;} /*Works: Loops while doing for loop until i > GraphGames : loop will set the not played games to 0*/
        for (int i = 0; i != GraphGames; i++) {ClicksList[i] = GameAndClicks["Game" + std::to_string(i + 1)];} /*Works: Adds the remaning clicks (known clicks) to the list i != GraphGames*/
    }
    else {for (int i = 0; i < 5; i++) {ClicksList[i] = GameAndClicks["Game" + std::to_string(i + 1)];}} // The normal loop for the upper one but what should be run always when GraphGames == 5

    std::cout << "After: "; for (int i = 0; i < 5; i++) {std::cout << ClicksList[i] << ":" << i << " ";} NewLine << "\n";

    std::map<int, int> ClicksToRow; // Contains conversion from clicks to row
    ClicksToRow[3000] = 0; ClicksToRow[2300] = 7;  ClicksToRow[1600] = 14;
    ClicksToRow[2900] = 1; ClicksToRow[2200] = 8;  ClicksToRow[1500] = 15;
    ClicksToRow[2800] = 2; ClicksToRow[2100] = 9;  ClicksToRow[1400] = 16;
    ClicksToRow[2700] = 3; ClicksToRow[2000] = 10; ClicksToRow[1300] = 17;
    ClicksToRow[2600] = 4; ClicksToRow[1900] = 11; ClicksToRow[1200] = 18;
    ClicksToRow[2500] = 5; ClicksToRow[1800] = 12; ClicksToRow[1100] = 19;
    ClicksToRow[2400] = 6; ClicksToRow[1700] = 13; ClicksToRow[1000] = 20;
    int RowList[6] = {20, 0, 0, 0, 0, 0}; // Stores the rows : clicks 2700 to 2700 = 3

    //std::cout << "Before : Rowlist: "; for (int i = 0; i < 6; i++) {std::cout << RowList[i] << ":" << i << " * ";}
    NewLine << "\n";
    std::cout << "Total of 5 outs should be done"; NewLine << "\n";

    // make for loop to go through all the clicks in ClicksList add them to RowList (Continues)
    int j = 1; // WORKS AS I BUT BIGGER VALUE
    for (int i = 0; i < 5; i++)
    {
        int FinalValue;
        int ClicksMap = ClicksList[i]; // List[5] = {}
        int Row = 0; // Dont know how this code works :)
        // If ClicksMap and ClicksList doesnt't match something is wrong
        if (ClicksMap > 3000) {Row = ClicksToRow[3000]; RowList[j] = Row; std::cout << "Over 3000 : " << "Row: " << Row << " ClicksMap: " << ClicksMap << " Final value: 3000"; NewLine << "\n";}
        else if (ClicksMap < 1000) {Row = ClicksToRow[1000]; RowList[j] = Row; std::cout << "Less 1000 : " << "Row: " << Row << " ClicksMap: " << ClicksMap << " Final value: 1000"; NewLine << "\n";}
        else
        {
            int div;
            for (div = 1; div <= ClicksMap; div *= 10) {}
            while(div > 100) {div /= 10;}
            std::string str = std::to_string(ClicksMap / div);

            if (ClicksMap >= stoi(str + "50")) {Row = ClicksToRow[stoi(str + "00") + 100]; RowList[j] = Row; std::cout << "Clicks = >= xx50 : str xx50 : " << stoi(str + "50") << " : "; FinalValue = stoi(str + "00") + 100;}
            else {Row = ClicksToRow[stoi(str + "00")]; RowList[j] = Row; std::cout << "Clicks ! >= xx50 : str xx00 : " << stoi(str + "00") << " : "; FinalValue = stoi(str + "00");}
            std::cout << "Row: " << Row << " ClicksMap: " << ClicksMap << " Final value: " << FinalValue; NewLine << "\n";        
        }
        j++;
    }
    NewLine << "\n"; std::cout << "Rowlist: "; for (int i = 0; i < 6; i++) {std::cout << RowList[i] << ":" << i << " * ";}; NewLine << "\n"; NewLine << "\n";
    // if clicks != || !> xx50 : clicks == xx00 :: else clicks == xx00 + 100
    

    std::cout << "Insert char to vector"; NewLine << "\n";
    std::cout << "RowList includes of 6 values {20, game1, game2, game3, game4, game5} ** Just a char out **"; NewLine << "\n";
    int a = 11; // Column slots for exact location of the marked row / |1   |2
    int times = 0; // 
    // Inserting the peaks as 'b' to the graph
    for (int i = 0; i < 6;)
    {
        if (times == 0)
        {
            Graph[RowList[i]][0] = 'b';
            std::cout << "Row: " << i << " Times repeated: " << times << " Graph make as 'b' if == 0" <<std::endl;
            i++;
            times++;
        }
        else
        {
            Graph[RowList[i]][a] = 'b';
            std::cout << "Row: " << i << " Times repeated: " << times << " Column: " << a << " Basic output to graph: Rowlist[i][a] i=(For i < 6),a=ColumnSlot "<<std::endl;
            a += 12;
            i++;
            times++;
        }
    }
    NewLine << "\n";

    std::cout << "Change letters to symbol fr 'a' / 'b' to '-' / 'X', **Peak** means X (Peak) in the output"; NewLine << "\n";
    // Changes letters to symbols
    for (int i = 0; i < Graph.size(); i++)
    {
        for (int j = 0; j < Graph[i].size(); j++)
        {
            if (Graph[i][j] == 'b') 
            {
                std::cout << i << " " << j << " **PEAK**" << " || ";
                Graph[i][j] = 'X';
            }
            /*
            else if (Graph[i][j] == 'o')
            {
                std::cout << i << " " << j << " || ";
                Graph[i][j] = ''; // Printed
            }
            */
            else
            {
                std::cout << i << " " << j << " || ";
                Graph[i][j] = '-';
            }
        }
    }

    NewLine << "\n"; NewLine << "\n";
    /* 
    As for now it is guaranteed to be 11 slots between every points 

    Make as o // The lines between "Clicked peaks" Marked as "o" printed as ???
    */

    std::cout << "Out the graph: std::string ReturnGraph([Vector],[Row]) : Outs diagram per row"; NewLine << "\n";

    MakeGraphToFile(Graph);

    //std::cout; NewLine << "\n";
    //std::cout << "3000|" << ReturnGraph(Graph, 0)  << "|"; NewLine << "\n";
    //std::cout << "    |" << ReturnGraph(Graph, 1)  << "|"; NewLine << "\n";
    //std::cout << "    |" << ReturnGraph(Graph, 2)  << "|"; NewLine << "\n";
    //std::cout << "    |" << ReturnGraph(Graph, 3)  << "|"; NewLine << "\n";
    //std::cout << "    |" << ReturnGraph(Graph, 4)  << "|"; NewLine << "\n";
    //std::cout << "2500|" << ReturnGraph(Graph, 5)  << "|"; NewLine << "\n";
    //std::cout << "    |" << ReturnGraph(Graph, 6)  << "|"; NewLine << "\n";
    //std::cout << "    |" << ReturnGraph(Graph, 7)  << "|"; NewLine << "\n";
    //std::cout << "    |" << ReturnGraph(Graph, 8)  << "|"; NewLine << "\n";
    //std::cout << "    |" << ReturnGraph(Graph, 9)  << "|"; NewLine << "\n";
    //std::cout << "2000|" << ReturnGraph(Graph, 10) << "|"; NewLine << "\n";
    //std::cout << "    |" << ReturnGraph(Graph, 11) << "|"; NewLine << "\n";
    //std::cout << "    |" << ReturnGraph(Graph, 12) << "|"; NewLine << "\n";
    //std::cout << "    |" << ReturnGraph(Graph, 13) << "|"; NewLine << "\n";
    //std::cout << "    |" << ReturnGraph(Graph, 14) << "|"; NewLine << "\n";
    //std::cout << "1500|" << ReturnGraph(Graph, 15) << "|"; NewLine << "\n";
    //std::cout << "    |" << ReturnGraph(Graph, 16) << "|"; NewLine << "\n";
    //std::cout << "    |" << ReturnGraph(Graph, 17) << "|"; NewLine << "\n";
    //std::cout << "    |" << ReturnGraph(Graph, 18) << "|"; NewLine << "\n";
    //std::cout << "    |" << ReturnGraph(Graph, 19) << "|"; NewLine << "\n";
    //std::cout << "1000|" << ReturnGraph(Graph, 20) << "|"; NewLine << "\n";
    //std::cout << "     |           |           |           |           |          |"; NewLine << "\n";
    //std::cout << "     0           1           2           3           4          5"; NewLine << "\n";
    //std::cout << "Game 1: " << ClicksList[1] << " Game 2: " << ClicksList[2] << " Game 3: " << ClicksList[3] << " Game 4: " << ClicksList[4] << " Game 5: " << ClicksList[5]; NewLine << "\n";
    
    return 0;
}

int CallGraph(bool Force)
{   
    CreateGraph(GraphMap, Force);
    return 0;
}

//int main()
//{
//    for (int i = 0; i < 5; i++) {std::cout << i;}
//}