#include "opencv2/opencv.hpp"
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/features2d/features2d.hpp>
#include <opencv2/calib3d/calib3d.hpp>
#include <algorithm>

#include <vector>
#include "gamecontext.hpp"
#pragma once
using namespace cv;
using namespace std;

class ImageProcessor{
    public:
        Mat PersTransform(Mat H, Mat imagem_entrada, Vec4i where);

        static ImageProcessor proc;
        ImageProcessor(int camera=0);
        ~ImageProcessor();
        void ReadCamera();
        void DisplayText(const char*);
        Mat ProcessImage(Mat imagem_entrada, bool useSkel = false);
        void DrawSquare (int x,int y,int w,int h,std::vector<Vec4i> l);
        static std::vector<Vec4i> GetLines(Mat img_entrada, int minimumSize=60);
        void OutputScreen(bool,bool,bool,bool,float);
        void OutputContext(GameContext &c);
        float CountNonZeros(Rect p);
        void MockLineFinder(Mat &m,std::vector<Vec4i> &lines, int minSize = 32);
        void CorrectPerspective();
        Mat PersDetransform(Mat imagem_entrada);

        Mat finalM;
        Mat CameraMat;
        Mat OriginalCamera;

    private:
        Mat aux_m;
        Mat mask;
        std::vector<Vec4i> localLines;
        VideoCapture camera;
        Mat ProcessFrame;
        std::vector<Vec2i> pontosPers;
        std::vector<Vec2i> lastWorkingPtPers;
        bool possuiLast;
        bool possuiPers;
        Vec4i lastwhere;
        Mat HomoFirst;
        Mat HomoSecond;
        bool hasHomo;
};

