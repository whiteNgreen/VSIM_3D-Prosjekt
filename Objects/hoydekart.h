#ifndef HOYDEKART_H
#define HOYDEKART_H

#include <visualobject.h>
#include <unordered_map>
#include "StandAlone/LoggerFunctions.h"

//std::pair<int, int> indeksering;

struct Rute
{
    Rute(int index, int X, int Y)
    {
        Xindex = X;
        Yindex = Y;
        TriangleIndex = index;
    }
//    std::pair<int, int> Index;
    int Xindex;
    int Yindex;
    int TriangleIndex;

    /* Grensene i 2D */
    float Xmin;
    float Xmax;
    float Ymin;
    float Ymax;

    std::vector<QVector3D> mPunkter;
    float averageHeight;
    QVector3D Center;

    void MakeCenter()
    {
        Center.setX((Xmin + Xmax)/2);
        Center.setY((Ymin + Ymax)/2);
        Center.setZ(averageHeight);
    }
};

class HoydeKart : public VisualObject
{
private:
    std::vector<QVector3D> mPunktdata;

public:
    HoydeKart(Shader* shader, float scale);

    std::vector<Rute> mRuter;

//    std::unordered_map<std::pair<int, int>, Rute> mRuter2;
};

#endif // HOYDEKART_H
