#pragma once

#include "opencv2/opencv.hpp"
#include <opencv2/core/core.hpp>
using namespace cv;
/**
    Class will be used to store game context
*/
enum TileState{
    TILE_STATE_EMPTY=0,
    TILE_STATE_X,
    TILE_STATE_O,
};
class GameContext{
    public:
        GameContext();
        static GameContext game;




        void BeginGame();
        void Update();
        void OutputScreen();

        void UpdateBlocks();
        bool GameEnd(){return SomeoneWon != 0;};
        Rect &GetCellBox(int i);
        TileState GetCellMark(int i);
        int IsPlaying(){return Turn;};
        float GetPercentFound(){return (FramesFound/(double)FramesCaught)*100.0; };

    private:
        float GetLeftToBegin(){return FoundCounter;};
        bool IsStarted(){return hasStarted;};

        bool CanStart();




        int CheckResult(TileState x[3][3]);


        void FindPlayerPlay();
        void UpdateContextPosition(Rect *pos);
        void ResetGamePosition(Vec4i pos,std::vector<Vec4i>);
        void NotifyFind(bool found);
        bool AdjustGamePosition(std::vector<Vec4i> &lines,Vec4i &where);

        int SomeoneWon;
        bool Turn; //Turn = true = player turn
        Vec4i startRect;
        bool hasStarted;
        bool ComputerShouldPlay;
        float FoundCounter;
        //Percent found
        int FramesCaught;
        int FramesFound;
        //

        int PlayerTreshhold;
        int ComputerTreshhold;

        Rect squarePositions[3][3];
        TileState tiles[3][3];
        int NonZerosCounter[3][3];
        float AvgValue[3][3];
};
