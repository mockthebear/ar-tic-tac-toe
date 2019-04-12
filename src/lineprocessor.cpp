#include "../include/lineprocessor.hpp"
#include <list>

#include <math.h>
Vec4i LineProcessor::AdjustLine(Vec4i l1){
    Vec4i l1c;
    if (GetDistance(0,0,l1[0],l1[1]) > GetDistance(0,0,l1[2],l1[3]))
    {
    l1c[0] = l1[2];
    l1c[1] = l1[3];
    l1c[2] = l1[0];
    l1c[3] = l1[1];
    return l1c;} else return l1;
}
double LineProcessor::GetLength(Vec4i linha){

    return sqrt((linha[2]-linha[0])*(linha[2]-linha[0]) + (linha[3]-linha[1])*(linha[3]-linha[1]));
}

float LineProcessor::GetDistance(int x,int y,int x1,int y1){
    return sqrt((x1-x)*(x1-x) + (y1-y)*(y1-y));
}


bool LineProcessor::IsOnSameSegment(Point p, Point q, Point r){
    return q.x <= std::max(p.x, r.x) && q.x >= std::min(p.x, r.x) && q.y <= std::max(p.y, r.y) && q.y >= std::min(p.y, r.y);
}

int LineProcessor::IsCollinear(Point p, Point q, Point r){
    //Found at http://www.geeksforgeeks.org/orientation-3-ordered-points/
    int orient = (q.y - p.y) * (r.x - q.x) - (q.x - p.x) * (r.y - q.y);
    return (orient == 0) ? 0 : ((orient > 0) ? 2: 1);
}

bool LineProcessor::CheckIntersec(Point p1, Point q1, Point p2, Point q2)
{
    int col1 = IsCollinear(p1, q1, p2);
    int col2 = IsCollinear(p1, q1, q2);
    int col3 = IsCollinear(p2, q2, p1);
    int col4 = IsCollinear(p2, q2, q1);
    if (col1 != col2 && col3 != col4)
        return true;
    if ( (col1 == 0 && IsOnSameSegment(p1, p2, q1)) || (col2 == 0 && IsOnSameSegment(p1, q2, q1)) || (col3 == 0 && IsOnSameSegment(p2, p1, q2)) || (col4 == 0 && IsOnSameSegment(p2, q1, q2))){
        return true;
    }else{
        return false;
    }
}



bool LineProcessor::LineIsIntersect(Vec4i l1,Vec4i l2){
    l1 = AdjustLine(l1);
    l2 = AdjustLine(l2);
    return CheckIntersec(Point(l1[0],l1[1]),Point(l1[2],l1[3]), Point(l2[0],l2[1]),Point(l2[2],l2[3]) );
}
double LineProcessor::DistancePointToLine(Vec4i line, Vec2i point)
{
    double x0,x1,x2,y0,y1,y2;
    x0 = point[0];
    y0 = point[1];
    x1 = line[0];
    x2=  line[2];
    y1 = line[1];
    y2=line[3];

    return abs((y2-y1)*x0 - (x2-x1)*y0 + (x2*y1) - (y2*x1)) /
    sqrt ( pow(y2-y1,2) + pow(x2-x1,2) ) ;
}

// calcula ponto medio do segmento de reta
Vec2i LineProcessor::MidPoint(Vec4i twopoints)
{
    Vec2i vec;
    vec[0] = (twopoints[0]+twopoints[2])/2;
    vec[1] = (twopoints[1]+twopoints[3])/2;
    return vec;
}



Vec2f findIntersection(int x1, int y1, int x2, int y2, int x3, int y3, int x4, int y4) {
float x = ( (x1*y2 - y1*x2)*(x3-x4) - (x1-x2)*(x3*y4-y3*x4) )
          / (float) ((x1-x2)*(y3-y4) - (y1-y2)*(x3-x4));

float y =  ( (x1*y2 - y1*x2)*(y3-y4) - (y1-y2)*(x3*y4-y3*x4) )
          / (float) ((x1-x2)*(y3-y4) - (y1-y2)*(x3-x4));
          Vec2f ret;
          ret[0]       = x;
          ret[1] = y;
          return ret;
}
Vec2f findIntersec(Vec4i a, Vec4i b)
{
    return findIntersection(a[0],a[1],a[2],a[3],b[0],b[1],b[2],b[3]);
}

std::vector<Vec2i> LineProcessor::getPersLocation(std::vector<Vec4i> lines)
{

    std::vector<Vec4i> sortLines = lines;
    std::sort(sortLines.begin(), sortLines.end(), [](Vec4i a, Vec4i b) {
        float L1a = atan2(a[1] - a[3], a[0] - a[2]) * 180.0f / M_PI;
        float L1b = atan2(b[1] - b[3], b[0] - b[2]) * 180.0f / M_PI;

        if (abs(abs(L1a)-90) <= 35){
            if (abs(abs(L1b)-90) <= 35){

                return a[0] < b[0];
            }else{

                return true;
            }
        }else{
            if (abs(abs(L1b)-180) <= 35){
                return a[3] < b[3];
            }else{

                return false;
            }

        }
    });

    Vec2i canto_sd;
    Vec2f isec = findIntersec(sortLines[2],sortLines[1]);
    canto_sd[0] = sortLines[1][0] + (int) (sortLines[2][2] - isec[0]);
    canto_sd[1] = sortLines[2][3] + (int) (sortLines[1][1] - isec[1]);

    Vec2i canto_id;
    isec = findIntersec(sortLines[1],sortLines[3]);
    canto_id[0] = sortLines[1][2] + (int) (sortLines[3][2] - isec[0]);
    canto_id[1] = sortLines[3][3] + (int) (sortLines[1][3] - isec[1]);

    Vec2i canto_se;
     isec = findIntersec(sortLines[0],sortLines[2]);
    canto_se[0] = sortLines[0][0] + (int) (sortLines[2][0] - isec[0]);
    canto_se[1] = sortLines[2][1] + (int) (sortLines[0][1] - isec[1]);

    Vec2i canto_ie;
    isec = findIntersec(sortLines[0],sortLines[3]);
    canto_ie[0] = sortLines[0][2] + (int) (sortLines[3][0] - isec[0]);
    canto_ie[1] = sortLines[3][1] + (int) (sortLines[0][3] - isec[1]);

     std::vector<Vec2i> output ;
     output.push_back(canto_se);
     output.push_back(canto_sd);
     output.push_back(canto_ie);
     output.push_back(canto_id);
    return output;
}
Mat LineProcessor::getPersMatrix(std::vector<Vec2i> from, Vec4i where)
{
    Mat m ;
    //ordem dos cantos -> sup. esq, sup. dir, inf. esq., inf. dir.

    std::vector<Point2f> input, output;
    input.push_back(Point2f(from[0][0], from[0][1]));
    input.push_back(Point2f(from[1][0], from[1][1]));
    input.push_back(Point2f(from[2][0], from[2][1]));
    input.push_back(Point2f(from[3][0], from[3][1]));

    output.push_back(Point2f(where[0],where[1]));
    output.push_back(Point2f(where[2],where[1]));
    output.push_back(Point2f(where[0],where[3]));
    output.push_back(Point2f(where[2],where[3]));

    m = findHomography(input,output);
    return m;

}

std::vector<Vec4i> LineProcessor::posProcessLines(std::vector<Vec4i> localLines) {
        std::list<std::pair<Vec4i, double>> linhas_coef;
    // Passo 1: calcular o coeficiente angular de cada segmento de reta
    // (y2-y1)/(x2-x1)
    // e salva numa tupla juntamente com o segmento
    // salva o angulo em radianos ao inves do coeficiente puro
    for (auto & v : localLines)
    {
        std::pair<Vec4i, double> linha_coef;
        linha_coef.first = v;
        linha_coef.second = atan((v[3]-v[1])/(double)(v[2]-v[0]));
        linhas_coef.push_back(linha_coef);
    }

    // Passo 2: agrupar coeficientes similares.
    // para isso temos que obter o angulo do coeficiente com arctan,
    // e entao aplicar um algoritmo de clustering
    // algoritmo: pega o primeiro da lista. busca na lista os que distam epsilon deste 1o.
    // remove estes da lista. repete para os elementos restantes ate que nao haja mais.
    // os valores agrupados sao colocados em uma terceira lista, com um indice para
    // identificar o grupo
    // podemos tentar usar um k-means depois
    std::list<std::pair<Vec4i, double>> lista_entrada(linhas_coef.begin(), linhas_coef.end());
    std::list<std::tuple<Vec4i, double, int>> lista_saida;
    int categ_atual = 0;
    for (auto i = lista_entrada.begin(); i != lista_entrada.end(); ++i) {
        std::tuple<Vec4i, double, int> novo(AdjustLine((*i).first), (*i).second, categ_atual);
        lista_saida.push_back(novo);
        auto it2 = i;
        ++it2;
        for (; it2 != lista_entrada.end(); ) {
            if ( ( abs((*it2).second - (*i).second) < (3.14152/8)  ||
                  abs((*it2).second - ((*i).second + 3.14152) ) < (3.14152/8) ||
                  abs(((*it2).second + 3.14152) - (*i).second ) < (3.14152/8)) &&
                DistancePointToLine((*i).first, MidPoint((*it2).first)) < 32) {
                std::tuple<Vec4i, double, int> novo(AdjustLine((*it2).first), (*it2).second, categ_atual);
                lista_saida.push_back(novo);
                it2 = lista_entrada.erase(it2);
            } else {
                ++it2;
            }
        }

        categ_atual++;
    }

    std::vector<std::pair<Vec4i,int>> maiores;

    // Passo penúltimo: obtém a linha mais comprida de cada grupo
    categ_atual = 0;
    double maxdist=0; Vec4i lin_longa;
    for (auto & s : lista_saida)
    {
        if (std::get<2>(s) != categ_atual)
        {
            std::pair<Vec4i,int> par;
            par.first = lin_longa;
            par.second = categ_atual;
            maiores.push_back(par);
            categ_atual = std::get<2>(s);
            maxdist=0;
        }
        if (GetLength(std::get<0>(s)) > maxdist) {
            maxdist = GetLength(std::get<0>(s));
            lin_longa = std::get<0>(s);
        }
    }
    std::pair<Vec4i,int> par;
    par.first = lin_longa;
    par.second = categ_atual;
    maiores.push_back(par);

    // passo final: das maiores linhas, pega as linhas que o ponto inicial e final
    // distam de certa distancia
    // delas e pega a media destas.
    std::vector<Vec4i> ret;
    double x_med=0,y_med=0, x2_med=0, y2_med=0;
    int n = 0;
    auto it_r = lista_saida.begin();
    for (auto & s : maiores)
    {
        categ_atual = s.second;

        while (it_r != lista_saida.end() && std::get<2>(*it_r) == categ_atual)
        {
            if (GetDistance(s.first[0], s.first[1], std::get<0>(*it_r)[0],
                            std::get<0>(*it_r)[1]) < 50 &&
                GetDistance(s.first[2], s.first[3], std::get<0>(*it_r)[2],
                            std::get<0>(*it_r)[3]) < 50)
                            {
                                x_med = ((x_med*n) + std::get<0>(*it_r)[0]) / (n+1);
                                y_med = ((y_med*n) + std::get<0>(*it_r)[1]) / (n+1);
                                x2_med = ((x2_med*n) + std::get<0>(*it_r)[2]) / (n+1);
                                y2_med = ((y2_med*n) + std::get<0>(*it_r)[3]) / (n+1);
                                n++;
                            }
                            ++it_r;
        }
        Vec4i ret_tmp; ret_tmp[0] = x_med;
        ret_tmp[1] = y_med;
        ret_tmp[2] = x2_med;
        ret_tmp[3] = y2_med;
        ret.push_back(AdjustLine(ret_tmp));
        n=0;x_med=0;y_med=0;x2_med=0;y2_med=0;
    }

    return ret;

}

std::vector<Vec4i> LineProcessor::preProcessLines(std::vector<Vec4i> localLines){
    if (localLines.size() <= 1){
        return localLines;
    }
    std::vector<Vec4i> ret;
    //std::cout << "Started with " << localLines.size() << " line\n";
    for (uint32_t i=0;i<localLines.size();i++){
        float L1a = atan2(localLines[i][1] - localLines[i][3], localLines[i][0] - localLines[i][2]) * 180.0f / M_PI;
        //std::cout << abs(L1a) << "\n";
        if (abs(abs(L1a)-180) < 25 || abs( abs(L1a)-90) < 25)
            ret.emplace_back(localLines[i]);
        //getchar();
    }
    return ret;
}


bool LineProcessor::hasGameOnIt(std::vector<Vec4i> &localLines,Vec4i &where){
    /*
        Check if has context
    */

    if (localLines.size() < 2){
        return false;
    }
    std::map<int,std::vector<int>> Collisions;

    for (uint32_t i=0;i<localLines.size();i++){
        #ifdef DBG_MODE
        std::cout << "The line " << i << " is crossed by ";
        #endif // DBG_MODE
        for (uint32_t b=0;b<localLines.size();b++){
            if (i != b){
                if (LineIsIntersect(localLines[i],localLines[b])){
                    #ifdef DBG_MODE
                    std::cout << b << ", ";
                    #endif
                    Collisions[i].emplace_back(b);
                }
            }
        }
        #ifdef DBG_MODE
        std::cout << "\n";
        #endif // DBG_MODE
    }

    //The game has only 4 lines, but each line HAS to be touched only 2 times
    uint32_t equalLines = 2;
    std::vector<Vec4i> validLines;
    std::vector<int> ids;
    for (auto &it : Collisions){
        //Means only touched twice
        if (it.second.size() == equalLines){ //Count as itself
            #ifdef DBG_MODE
            std::cout << "The line" << it.first << "Belongs to context with "<<it.second.size() <<":";
            #endif
            bool found = true;
            for (uint32_t b=0;b<it.second.size();b++){
                #ifdef DBG_MODE
                std::cout << it.second[b] << ",";
                #endif
                //The line I contains a and c; So lines A and lines C must contains I
                int toLook = it.second[b];


                if ( Collisions[toLook].size() == equalLines){

                    for (unsigned int i=0;i<equalLines;i++){
                        int thisEl = Collisions[toLook][i];
                        if (thisEl == it.first){
                            found = true;
                            break;
                        }else{
                            found = false;
                        }
                    }
                }else{
                    found = false;
                }
            }
            if (found){

                ids.emplace_back(it.first);
                validLines.emplace_back(localLines[it.first]);
            }
            #ifdef DBG_MODE
            std::cout << "\n";
            #endif
        }

    }
    #ifdef DBG_MODE
    std::cout << "Valid lines:" << validLines.size() << "\n";
    #endif // DBG_MODE
    if (validLines.size() != 4){
        #ifdef DBG_MODE
        std::cout << "Failed\n";
        #endif // DBG_MODE
        return false;
    }

    #ifdef DBG_MODE
    std::cout << "Pass\n";
    #endif
    where[0] = where[1] = 99999;
    where[2] = where[3] = -9999;
    /*std::vector<Vec4i> sortLines = validLines;
    int n1=0;
    int n2=2;
    for (uint32_t i=0;i<validLines.size();i++){
        float L1a = atan2(validLines[i][1] - validLines[i][3], validLines[i][0] - validLines[i][2]) * 180.0f / M_PI;
        if (abs(abs(L1a)-90) <= 35){
            //Verticais
            sortLines[n1] = validLines[i];
            n1++;
        }else{
            sortLines[n2] = validLines[i];
            n2++;
        }
    }
    if (sortLines[0][0] > sortLines[1][0]){
        //Linha 0 vem depois da linha 1
        Vec4i aux = sortLines[0];
        sortLines[0] = sortLines[1];
        sortLines[1] = aux;
    }

    if (sortLines[2][3] > sortLines[3][3]){
        Vec4i aux = sortLines[3];
        sortLines[3] = sortLines[2];
        sortLines[2] = aux;
    }

    localLines = sortLines;*/
    localLines = validLines;
    for (uint32_t i=0;i<validLines.size();i++){
        #ifdef DBG_MODE
        std::cout << "The line valid "<<i<<" is: "<<validLines[i][0]<<","<<validLines[i][1]<<","<<validLines[i][2]<<","<<validLines[i][3]<<","<<validLines[i][4]<<"\n";
        #endif // DBG_MODE
        validLines[i] = AdjustLine(validLines[i]);
        where[0] = std::min(where[0],validLines[i][0]);
        where[1] = std::min(where[1],validLines[i][1]);
        where[2] = std::max(where[2],validLines[i][2]);
        where[3] = std::max(where[3],validLines[i][3]);
    }
    #ifdef DBG_MODE
    std::cout << "The avg square means: "<<where[0]<<","<<where[1]<<","<<where[2]<<","<<where[3]<<","<<where[4]<<"\n";
    #endif // DBG_MODE

    return true;
}
