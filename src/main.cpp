
#include <iostream>
#include <math.h>
#include <vector>
#include <map>
#include <stdio.h>
#include "../include/imageprocessor.hpp"
#include "../include/lineprocessor.hpp"
#include "../include/gamecontext.hpp"
#include <time.h>
int main(int argc, char* argv[])
{
    srand(time(NULL));
    Vec4i where;
    std::vector<Vec4i> lines;
    while (1){

        GameContext::game.Update();
        GameContext::game.OutputScreen();



        if(waitKey(30) >= 0){
            /*char buffer[50];
            sprintf(buffer,"outBin%d.png",counterSaved);
            imwrite(buffer,proc.finalM);
            sprintf(buffer,"outCol%d.png",counterSaved);
            imwrite(buffer,proc.CameraMat);
            counterSaved++;
            */
            break;
        };
        //getchar();
    }
    return 0;

}
