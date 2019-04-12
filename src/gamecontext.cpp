#include "../include/gamecontext.hpp"
#include "../include/imageprocessor.hpp"
#include "../include/lineprocessor.hpp"
#include <math.h>
#include <iostream>

#define DEFINED_LIMIAR_PERCENT 0.5f

GameContext GameContext::game;

GameContext::GameContext(){
    FoundCounter = 30;
    ComputerShouldPlay = false;
    ComputerTreshhold = 5;
    hasStarted = false;
    SomeoneWon = 0;
    Turn = true;

    FramesCaught = FramesFound = 0;
}


void GameContext::UpdateBlocks(){
    for (int y2=0;y2<3;y2++){
        for (int x2=0;x2<3;x2++){
            NonZerosCounter[y2][x2] = AvgValue[y2][x2] = ImageProcessor::proc.CountNonZeros(squarePositions[y2][x2]);
        }
    }
}

void GameContext::FindPlayerPlay(){

    //ImageProcessor::proc.CountNonZeros(squarePositions[0][0]);
    //
    //std::cout << "Read: \n";
    for (int y=0;y<3;y++){
        for (int x=0;x<3;x++){
            AvgValue[y][x] = AvgValue[y][x]*0.92 + ImageProcessor::proc.CountNonZeros(squarePositions[y][x])*0.08;
            //float changed = ImageProcessor::proc.CountNonZeros(squarePositions[y][x]);
            //std::cout << AvgValue[y][x] <<", ";
            if (AvgValue[y][x] >=DEFINED_LIMIAR_PERCENT){
                if (tiles[y][x] == TILE_STATE_EMPTY){
                    tiles[y][x] = TILE_STATE_X;
                    ComputerShouldPlay = true;
                    UpdateBlocks();
                }else if(tiles[y][x] == TILE_STATE_O){
                    ImageProcessor::proc.DisplayText("CHEATER!!!!!!!!");
                }
                //break;
            }
        }
            //std::cout << "\n";
    }

    if (!Turn){
        ComputerTreshhold--;
        if (ComputerTreshhold<=0){
            Turn = true;
            std::cout << "Checando contexto\n";
            std::cout << "The read values are: \n";
            for (int y=0;y<3;y++){
                for (int x=0;x<3;x++){
                    float changed = ImageProcessor::proc.CountNonZeros(squarePositions[y][x]);
                    std::cout << changed  << ", ";
                    if (changed >= DEFINED_LIMIAR_PERCENT){
                        if (tiles[y][x] == TILE_STATE_EMPTY){
                            tiles[y][x] = TILE_STATE_X;
                            ComputerShouldPlay = true;
                            y = 3;
                            UpdateBlocks();
                            break;
                        }else if(tiles[y][x] == TILE_STATE_O){
                            ImageProcessor::proc.DisplayText("CHEATER!!!!!!!!");
                        }
                    }
                    AvgValue[y][x] = ImageProcessor::proc.CountNonZeros(squarePositions[y][x]);
                }
                std::cout << "\n";
            }
        }
    }
}
bool GameContext::AdjustGamePosition(std::vector<Vec4i> &lines,Vec4i &where){
    //ImageProcessor::proc.DrawSquare(startRect[0],startRect[1],startRect[2],startRect[3],lines);
    PlayerTreshhold = 10;
    if (LineProcessor::GetDistance(where[0],where[1],startRect[0],startRect[1]) > 8){
        //std::cout << "Pos changed by "<<LineProcessor::GetDistance(where[0],where[1],startRect[0],startRect[1])<<"\n";
        int d1 = LineProcessor::GetDistance(where[0],where[1],where[2],where[3]);
        int d2 = LineProcessor::GetDistance(startRect[0],startRect[1],startRect[2],startRect[3]);
        //std::cout << "New dist is "<<abs(d1-d2)<<"\n";
        for (int i=0;i<4;i++)
            where[i] = where[i]*0.2 + startRect[i]*0.8;
        if (abs(d1-d2) <= 30){
            ResetGamePosition(where,lines);
        }
        return true;
    }
    return false;
}

void GameContext::Update(){
    Vec4i where;
    std::vector<Vec4i> lines;

    //getchar();
    ImageProcessor::proc.ReadCamera();
    ImageProcessor::proc.CorrectPerspective();
    ImageProcessor::proc.ProcessImage(ImageProcessor::proc.CameraMat, true);
    lines = ImageProcessor::proc.GetLines(ImageProcessor::proc.finalM);



    if (LineProcessor::hasGameOnIt(lines,where)){
        /*
            Game found
        */
        if (IsStarted()){
            if (!AdjustGamePosition(lines,where)){
                FindPlayerPlay();
            }
        }else{
            ImageProcessor::proc.DrawSquare(where[0],where[1],where[2],where[3],lines);
            NotifyFind(true);
            if (CanStart()){
                ResetGamePosition(where,lines);
                BeginGame();
            }
        }
    }else{
        /*
            Game not found
        */
        if (!IsStarted()){
            NotifyFind(false);
        }else{
            PlayerTreshhold--;
            if (PlayerTreshhold<=0){
                if (Turn){
                    std::cout << "Player Jogando\n";
                    ComputerTreshhold = 5;
                    Turn = false;
                }
            }
        }

    }

    SomeoneWon = CheckResult(tiles);
    if (SomeoneWon == 0 && ComputerShouldPlay){
        ComputerShouldPlay = false;
        int x = rand()%3;
        int y = rand()%3;
        int emptyFields = 0;
        for (int y=0;y<3;y++){
            for (int x=0;x<3;x++){
                if (tiles[y][x] == TILE_STATE_EMPTY)
                    emptyFields++;
            }
        }
        if (emptyFields >= 1){
            while (tiles[y][x] != TILE_STATE_EMPTY){
                x = rand()%3;
                y = rand()%3;
            }
            tiles[y][x] = TILE_STATE_O;
        }else{
            SomeoneWon = 3;
            std::cout << "The game has ended!!!\n";
        }
    }

    if (SomeoneWon != 0){
        if (SomeoneWon == 1){
            ImageProcessor::proc.DisplayText("Player venceu!");
        }else if(SomeoneWon == 2){
            ImageProcessor::proc.DisplayText("PC venceu!");
        }else{
            ImageProcessor::proc.DisplayText("Velha!");
        }
        return;
    }

}


void GameContext::OutputScreen(){
    if (IsStarted()){
        ImageProcessor::proc.OutputContext(GameContext::game);
    }else{
        ImageProcessor::proc.OutputScreen(true,true,false,true,GameContext::game.GetLeftToBegin());
    }
}
TileState GameContext::GetCellMark(int i){
    int y = floor(i/3);
    return tiles[y][i%3];
}

Rect &GameContext::GetCellBox(int i){
    int y = floor(i/3);
    return squarePositions[y][i%3];
}
void GameContext::NotifyFind(bool found){
    if (found){
        FramesFound++;
        FoundCounter--;
    }else{
        FoundCounter+=0.5;
        FoundCounter = std::min(30.0f,FoundCounter);
    }
    if (FramesFound != 0)
        FramesCaught++;
};
bool GameContext::CanStart(){
    return FoundCounter <= 0;
};
void GameContext::ResetGamePosition(Vec4i where,std::vector<Vec4i> line){
    if (line.size() != 4){
        return;
    }
    Rect r[9];
    startRect = where;
    where[2] = where[2]-where[0];
    where[3] = where[3]-where[1];
    Rect mainRect(where[0],where[1],where[2],where[3]);
    //std::cout << "The game total area is in this rect:" << where[0] << ","<<where[1] << " with size: " << where[2]<<":"<<where[3]<<"\n";
    int counter=0;
    /*r[0] = Rect(mainRect.x,mainRect.y,line[0][0],line[2][1]);
    r[1] = Rect(line[0][0],mainRect.y,line[1][0],line[2][1]);
    r[2] = Rect(line[1][0],mainRect.y,mainRect.x+mainRect.width,line[2][3]);
    r[3] = Rect(mainRect.x,line[2][1],line[0][0],line[3][1]);
    r[4] = Rect(line[0][0],line[2][1],line[1][0],line[3][1]);
    r[5] = Rect(line[1][0],line[2][3],mainRect.x+mainRect.width,line[3][3]);
    r[6] = Rect(mainRect.x,line[3][1],line[0][2],mainRect.y+mainRect.height);
    r[7] = Rect(line[0][2],line[3][1],line[1][2],mainRect.y+mainRect.height);
    r[8] = Rect(line[1][2],line[3][3],line[3][2],mainRect.y+mainRect.height);
    */
    for (float stepY=0;stepY<3;stepY++){
      for (float stepX=0;stepX<3;stepX++){
            r[counter] = Rect(  mainRect.x+mainRect.width*(stepX/3.0f ) ,
                                mainRect.y+mainRect.height*(stepY/3.0f ),
                                mainRect.x+(mainRect.width)*( (stepX+1)/3.0f),
                                mainRect.y+(mainRect.height)*( (stepY+1)/3.0f)
                            );
            //std::cout << "Cell ["<<counter<<" in pos:" << (r[counter].x) << ","<< (r[counter].y) << " with size: " <<(r[counter].width)<<":"<<(r[counter].height)<<"\n";
            counter++;
        }
    }
    UpdateContextPosition(r);
}
void GameContext::UpdateContextPosition(Rect *pos){
    int counter=0;
    for (int y=0;y<3;y++){
        for (int x=0;x<3;x++){
            squarePositions[y][x] = pos[counter];

            //std::cout << NonZerosCounter[y][x] << ", ";
            counter++;
        }
        //std::cout << "\n";
    }
};


void GameContext::BeginGame(){
    hasStarted = true;
    for (int y=0;y<3;y++){
        for (int x=0;x<3;x++){
            tiles[y][x] = TILE_STATE_EMPTY;
            AvgValue[y][x] = NonZerosCounter[y][x] = ImageProcessor::proc.CountNonZeros(squarePositions[y][x]);
        }
    }
    std::cout << "Acurracy was " << GetPercentFound() << " ["<<FramesFound<<" / "<<FramesCaught<<"]\n";
    std::cout << "Game Begin\n";
};

int GameContext::CheckResult(TileState x[3][3]) {
	TileState result;
	int MCheck[8][6] = {{0, 0, 0, 1, 0, 2},{1, 0, 1, 1, 1, 2},{2, 0, 2, 1, 2, 2},
                        {0, 0, 1, 0, 2, 0}, {0, 1, 1, 1, 2, 1}, {0, 2, 1, 2, 2, 2},
                        {0, 0, 1, 1, 2, 2}, {2, 0, 1, 1, 0, 2},
                        };
	for (int i=0; i<8; i++) {
		if ((x[MCheck[i][0]][MCheck[i][1]] == x[MCheck[i][2]][MCheck[i][3]])
            && (x[MCheck[i][2]][MCheck[i][3]] == x[MCheck[i][4]][MCheck[i][5]]))
			result = x[MCheck[i][0]][MCheck[i][1]];
	}
	if (result == TILE_STATE_X){
		return 1;
	} else {
		if (result == TILE_STATE_O) {
			return 2;
		}
		else {
			return 0;
		}
	}
	return 0;
}
