#pragma once
#include "opencv2/opencv.hpp"
#include <opencv2/core/core.hpp>


using namespace cv;

class LineProcessor{
    public:
    static bool IsOnSameSegment(Point p, Point q, Point r);
    static bool CheckIntersec(Point p1, Point q1, Point p2, Point q2);
    static int IsCollinear(Point p, Point q, Point r);
    static bool hasGameOnIt(std::vector<Vec4i> &localLines,Vec4i &where);
    static bool LineIsIntersect(Vec4i l1,Vec4i l2);
    static Vec4i AdjustLine(Vec4i l1);
    static std::vector<Vec4i> preProcessLines(std::vector<Vec4i> localLines);
    static std::vector<Vec4i> posProcessLines(std::vector<Vec4i> localLines);
    static Vec4i GetMixedLine(Vec4i l1,Vec4i l2);
    static float GetDistance(int x,int y,int x1,int y1);
    static double GetLength(Vec4i line);
    static double DistancePointToLine(Vec4i line, Vec2i point);
    static Vec2i MidPoint(Vec4i twopoints);
    static Mat getPersMatrix(std::vector<Vec2i> from, Vec4i where);
    static std::vector<Vec2i> getPersLocation(std::vector<Vec4i> lines);
};
