#include "../include/imageprocessor.hpp"
#include "../include/lineprocessor.hpp"
#include <stdio.h>

#define CAMERA_VIDEO_NAME "teste1.webm"

ImageProcessor ImageProcessor::proc;

ImageProcessor::ImageProcessor(int cameraId):camera(CAMERA_VIDEO_NAME){
    if(!camera.isOpened()){
        /*
            Error
        */
        std::cout << "Camera failed\n";
        getchar();
    }

    possuiPers = false; possuiLast = false; hasHomo = false;
    namedWindow("Camera",2);
    moveWindow("Camera",600,0);
    Mat templ_local = imread("x.png");
    Mat D;
    cvtColor( templ_local, D, COLOR_BGR2GRAY );
    mask = Mat(D.size(), D.type());
    threshold(D, mask, 100, 255, THRESH_BINARY);
}

ImageProcessor::~ImageProcessor(){

}



void ImageProcessor::ReadCamera(){

    /*
        Read the camera's frame
    */
    camera >> ProcessFrame;

    CameraMat = ProcessFrame.clone();
    OriginalCamera = ProcessFrame.clone();
    aux_m = CameraMat.clone();
    aux_m = Scalar(0,0,0);

    static bool first = true;
    if (first){
        first = false;
        std::cout << "Video size is " << CameraMat.rows << "x"<<CameraMat.cols<<"\n";
        std::cout << "Minimum line should be " << CameraMat.rows/5.0<<"\n";
    }
}

float ImageProcessor::CountNonZeros(Rect p){
    if ((p.width-p.x) < 0 || (p.height-p.y) <= 0){
        return 0;
    }
    float counter = 0;
    float maskCounter = 0;
    Mat sub(finalM, cv::Rect(p.x, p.y, p.width-p.x, p.height-p.y));
    Mat auxT,D;
    resize(mask,auxT,Size(sub.cols,sub.rows));
    sub.convertTo(D,CV_8U);
    D = D&auxT;
    for(int y0=0;y0<D.rows;y0++){
        for (int x0=0;x0<D.cols;x0++){

            int var = (int)D.at<bool>(y0,x0);
            int mvar = (int)auxT.at<bool>(y0,x0);
            if (var != 0){
                counter++;
            }
            if (mvar != 0){
                maskCounter++;
            }

        }
    }
    return (counter/(float)maskCounter);
}
Mat ImageProcessor::ProcessImage(Mat imagem_entrada, bool useSkeleton){
    int size = 2;
    Mat saida = imagem_entrada.clone();
    Mat tsrc;
    Mat element_dilate = getStructuringElement( MORPH_RECT   ,Size( 2*size + 1, 2*size+1 ),Point( size, size ) );
    /* Transforma a matriz de uma forma que possa ser processada */
    cvtColor(imagem_entrada, imagem_entrada, COLOR_BGR2GRAY);

    /* Dá um blur!! */
    GaussianBlur(imagem_entrada, saida, Size(3,3), 1.5, 1.5);
    /* Canny */
    Canny(saida, saida, 0, 70, 3);
    /* Canny */
    if (useSkeleton){
        morphologyEx(saida,saida, MORPH_CLOSE, getStructuringElement(MORPH_RECT, Size(3, 3)));
        tsrc = saida.clone();
        dilate( tsrc, saida, element_dilate );
        //dilate( finalM, finalM, element_dilate );

        tsrc = saida.clone();
        //Skeleterson
    }
    finalM = saida.clone();
    return saida;
}


void ImageProcessor::DrawSquare(int x,int y,int w,int h,std::vector<Vec4i> lines){
        rectangle(CameraMat, Point(x,y), Point(w,h), Scalar(0,255,0));
        for (unsigned int i=0;i<lines.size();i++){
            line( CameraMat, Point(lines[i][0],lines[i][1]), Point(lines[i][2],lines[i][3]), Scalar(i&1 ? 255 : 0,i&2 ? 255 : 0,i&4 ? 255 : 0), 1, 1);
        }
}
void ImageProcessor::OutputContext(GameContext &c){


    if (!c.GameEnd()){

        for( size_t i = 0; i < 9; i++ ){
            Rect r = c.GetCellBox(i);
            #ifdef DBG_MODE
            rectangle(aux_m, Point(r.x,r.y), Point(r.width,r.height), Scalar(100,0,255));
            #endif // DBG_MODE
            TileState m = c.GetCellMark(i);
            if (m == TILE_STATE_O){
                int w = (r.width-r.x);
                int h = (r.height-r.y);
                int radius = std::min(w,h)/2.0; //2.0 because this means diamater
                radius = radius*0.8; //Because we need an smallwer circle than the whole rect
                int cX = r.x + w/2.0;
                int cY = r.y + h/2.0;
                circle(aux_m, Point(cX,cY),radius, Scalar(0,255,255), 2 );
            }else if (m == TILE_STATE_X){
                Point p1(r.x,r.y);
                Point p2(r.width,r.height);
                int lenght = LineProcessor::GetDistance(p1.x,p1.y,p2.x,p2.y);
                p1.x += lenght/10.0f;
                p1.y += lenght/10.0f;
                p2.x -= lenght/10.0f;
                p2.y -= lenght/10.0f;

                line( aux_m,p1 ,p2, Scalar(255,255,0), 3, 3);
                int aux = p1.x;
                p1.x = p2.x;
                p2.x = aux;
                line( aux_m, p1, p2, Scalar(255,255,0), 3, 3);
            }
        }
    }


    if (c.IsPlaying()){
        putText(aux_m, "Your turn", Point(12,CameraMat.rows-32), FONT_HERSHEY_SIMPLEX , 1, Scalar(255,255,255), 2, 2);
    }else{
        putText(aux_m, "Waiting...", Point(12,CameraMat.rows-32), FONT_HERSHEY_SIMPLEX , 1, Scalar(255,200,255), 2, 2);
    }
    if (!c.GameEnd()){
        aux_m = PersDetransform(aux_m);
        aux_m = OriginalCamera-aux_m;
        imshow("Camera", aux_m);
    }else{
        imshow("Camera", OriginalCamera);
    }
    imshow("Ajustada",CameraMat);
    imshow("FinalM",finalM);


}

void ImageProcessor::DisplayText(const char* c){
    if (!GameContext::game.GameEnd())
        putText(aux_m, c, Point(0,CameraMat.rows/2), FONT_HERSHEY_SIMPLEX , 1, Scalar(100,23,255), 2, 2);
    else
        putText(OriginalCamera, c, Point(0,CameraMat.rows/2), FONT_HERSHEY_SIMPLEX , 1, Scalar(100,23,255), 2, 2);

}

Mat ImageProcessor::PersDetransform(Mat imagem_entrada)
{
    if (hasHomo) {
    Mat saida = imagem_entrada.clone();
    warpPerspective(imagem_entrada, saida, HomoSecond, Size(saida.cols,saida.rows), INTER_LINEAR | WARP_INVERSE_MAP, BORDER_REPLICATE);
    Mat saida2 = saida.clone();
    warpPerspective(saida, saida2, HomoFirst, Size(saida2.cols,saida2.rows), INTER_LINEAR | WARP_INVERSE_MAP, BORDER_REPLICATE);
    return saida2;
    }
    return imagem_entrada;
}

Mat ImageProcessor::PersTransform(Mat H, Mat imagem_entrada, Vec4i where)
{
    std::vector<Vec2i> from;
    from.push_back(Vec2i(where[0], where[1]));
    from.push_back(Vec2i(where[2], where[1]));
    from.push_back(Vec2i(where[0], where[3]));
    from.push_back(Vec2i(where[2], where[3]));

    int aumento = imagem_entrada.rows / 3.5;
    Vec4i quadr(0 + aumento,
                0 + aumento,
                (imagem_entrada.rows+imagem_entrada.cols)/2 - aumento,
                imagem_entrada.rows - aumento);

    Mat H2 = LineProcessor::getPersMatrix(from,quadr);

    Mat saida = imagem_entrada.clone();
    warpPerspective(imagem_entrada, saida, H, Size(saida.cols,saida.rows), INTER_LINEAR, BORDER_REPLICATE);
    Mat saida2 = saida.clone();
    warpPerspective(saida, saida2, H2, Size(saida2.cols,saida2.rows), INTER_LINEAR, BORDER_REPLICATE);
    HomoFirst = H.clone();
    HomoSecond = H2.clone();
    hasHomo = true;
    return saida2;
}

void ImageProcessor::OutputScreen(bool lines, bool processed,bool dilated,bool showLines,float left){
    Mat linesq = CameraMat.clone();
    //#ifdef DBG_MODE
        for( size_t i = 0; i < localLines.size(); i++ ){
            Vec4i l = localLines[i];
            line( linesq, Point(l[0], l[1]), Point(l[2], l[3]), Scalar(i&1 ? 0 : 255,i&1 ? 255 : 0,i&2 ? 255 : 0), 1, 1);
        }
    //#endif // DBG_MODE
    char buffer[50];
    sprintf(buffer,"%d left",(int)left);
    putText(linesq, buffer, Point(0,linesq.rows-32), FONT_HERSHEY_SCRIPT_SIMPLEX , 1, Scalar::all(255), 2, 2);
    /*
        Porcentagem de quadros achados
    */
    sprintf(buffer,"%.2f%%",GameContext::game.GetPercentFound());
    putText(linesq, buffer, Point(linesq.cols/2,linesq.rows-32), FONT_HERSHEY_SCRIPT_SIMPLEX , 1, Scalar(255,100,255), 2, 2);


    if (lines)
        imshow("Camera",linesq);

//    if (dilated)
//        imshow("Big", tsrc);
}

std::vector<Vec4i> ImageProcessor::GetLines(Mat imagem_entrada, int minimumSize){
    std::vector<Vec4i> linhasDetectadas;
    if (imagem_entrada.rows <= 0){
        return linhasDetectadas;
    }

    HoughLinesP(imagem_entrada, linhasDetectadas, 1, CV_PI/180, 60, imagem_entrada.rows/6.0, 8 );

    linhasDetectadas = LineProcessor::preProcessLines(linhasDetectadas);
    linhasDetectadas = LineProcessor::posProcessLines(linhasDetectadas);
    ImageProcessor::proc.localLines = linhasDetectadas;
    return linhasDetectadas;
}

Vec4i GetWhere(std::vector<Vec2i> &from)
{
    Vec4i where;
    where[0] = std::min(from[0][0],std::min(from[1][0],std::min(from[2][0],from[3][0])));
    where[1] = std::min(from[0][1],std::min(from[1][1],std::min(from[2][1],from[3][1])));
        where[2] = std::max(from[0][0],std::max(from[1][0],std::max(from[2][0],from[3][0])));
        where[3] = std::max(from[0][1],std::max(from[1][1],std::max(from[2][1],from[3][1])));
        return where;
}

void ImageProcessor::CorrectPerspective()
{
    //std::cout << "Iniciando correcao" << std::endl;
    std::vector<Vec4i> lines;
    Mat H;
    ImageProcessor::ProcessImage(CameraMat,true);
    lines = ImageProcessor::GetLines(finalM);
    Vec4i where;
    bool hasgame = LineProcessor::hasGameOnIt(lines,where);

    if (hasgame) {
        std::vector<Vec2i> from = LineProcessor::getPersLocation(lines);

        if (!possuiPers)
        {
            pontosPers = from;
            possuiPers = true;
        }
        else {
            double alfa = 0.1;
            for (int i = 0; i < 4; i++)
            {
                for (int j = 0; j < 2; j++)
                {
                    pontosPers[i][j] = (int) ( alfa * from[i][j] + (1-alfa) * pontosPers[i][j]);
                }
            }
        }
        possuiPers = true;

        H = LineProcessor::getPersMatrix(pontosPers,GetWhere(pontosPers));
        if ((H.rows != 3 || H.cols != 3) && possuiLast) {
            H = LineProcessor::getPersMatrix(lastWorkingPtPers,lastwhere);
//            line( ProcessFrame, Point(GetWhere(lastWorkingPtPers)[0],GetWhere(lastWorkingPtPers)[1]), Point(GetWhere(lastWorkingPtPers)[2],GetWhere(lastWorkingPtPers)[3]), Scalar(255,255,255), 1, 1);
//            line( ProcessFrame, Point(lastWorkingPtPers[0][0],lastWorkingPtPers[0][1]), Point(lastWorkingPtPers[1][0],lastWorkingPtPers[1][1]), Scalar(255,255,255), 1, 1);
//            line( ProcessFrame, Point(lastWorkingPtPers[0][0],lastWorkingPtPers[0][1]), Point(lastWorkingPtPers[2][0],lastWorkingPtPers[2][1]), Scalar(255,255,255), 1, 1);
//            line( ProcessFrame, Point(lastWorkingPtPers[1][0],lastWorkingPtPers[1][1]), Point(lastWorkingPtPers[3][0],lastWorkingPtPers[3][1]), Scalar(255,255,255), 1, 1);
//            line( ProcessFrame, Point(lastWorkingPtPers[2][0],lastWorkingPtPers[2][1]), Point(lastWorkingPtPers[3][0],lastWorkingPtPers[3][1]), Scalar(255,255,255), 1, 1);

            CameraMat = ImageProcessor::PersTransform(H, ProcessFrame, lastwhere);

            ProcessFrame = CameraMat.clone();
        } else if (H.rows == 3 && H.cols == 3) {
           lastWorkingPtPers = pontosPers;
           lastwhere = GetWhere(pontosPers);
           possuiLast = true;
//            line( ProcessFrame, Point(GetWhere(lastWorkingPtPers)[0],GetWhere(lastWorkingPtPers)[1]), Point(GetWhere(lastWorkingPtPers)[2],GetWhere(lastWorkingPtPers)[3]), Scalar(255,255,255), 1, 1);
//            line( ProcessFrame, Point(lastWorkingPtPers[0][0],lastWorkingPtPers[0][1]), Point(lastWorkingPtPers[1][0],lastWorkingPtPers[1][1]), Scalar(255,255,255), 1, 1);
//            line( ProcessFrame, Point(lastWorkingPtPers[0][0],lastWorkingPtPers[0][1]), Point(lastWorkingPtPers[2][0],lastWorkingPtPers[2][1]), Scalar(255,255,255), 1, 1);
//            line( ProcessFrame, Point(lastWorkingPtPers[1][0],lastWorkingPtPers[1][1]), Point(lastWorkingPtPers[3][0],lastWorkingPtPers[3][1]), Scalar(255,255,255), 1, 1);
//            line( ProcessFrame, Point(lastWorkingPtPers[2][0],lastWorkingPtPers[2][1]), Point(lastWorkingPtPers[3][0],lastWorkingPtPers[3][1]), Scalar(255,255,255), 1, 1);

           CameraMat = ImageProcessor::PersTransform(H, ProcessFrame, lastwhere);
           ProcessFrame = CameraMat.clone();
        }
    }
    else if (possuiLast)
    {
        H = LineProcessor::getPersMatrix(lastWorkingPtPers,lastwhere);
//        line( ProcessFrame, Point(GetWhere(lastWorkingPtPers)[0],GetWhere(lastWorkingPtPers)[1]), Point(GetWhere(lastWorkingPtPers)[2],GetWhere(lastWorkingPtPers)[3]), Scalar(255,255,255), 1, 1);
//        line( ProcessFrame, Point(lastWorkingPtPers[0][0],lastWorkingPtPers[0][1]), Point(lastWorkingPtPers[1][0],lastWorkingPtPers[1][1]), Scalar(255,255,255), 1, 1);
//        line( ProcessFrame, Point(lastWorkingPtPers[0][0],lastWorkingPtPers[0][1]), Point(lastWorkingPtPers[2][0],lastWorkingPtPers[2][1]), Scalar(255,255,255), 1, 1);
//        line( ProcessFrame, Point(lastWorkingPtPers[1][0],lastWorkingPtPers[1][1]), Point(lastWorkingPtPers[3][0],lastWorkingPtPers[3][1]), Scalar(255,255,255), 1, 1);
//        line( ProcessFrame, Point(lastWorkingPtPers[2][0],lastWorkingPtPers[2][1]), Point(lastWorkingPtPers[3][0],lastWorkingPtPers[3][1]), Scalar(255,255,255), 1, 1);

        CameraMat = ImageProcessor::PersTransform(H, ProcessFrame, lastwhere);

        ProcessFrame = CameraMat.clone();
    }
    //std::cout << "Finalizando correcao" << std::endl;
}

