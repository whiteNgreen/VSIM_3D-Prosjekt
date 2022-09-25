#ifndef HOYDEKART_H
#define HOYDEKART_H

#include <visualobject.h>
#include "StandAlone/LoggerFunctions.h"

struct Rute
{
    Rute(int X, int Y)
    {
        Xindex = X;
        Yindex = Y;
    }
    int Xindex;
    int Yindex;

    /* Grensene i 2D */
    float Xmin;
    float Xmax;
    float Ymin;
    float Ymax;

    std::vector<QVector3D> mPunkter;
    float averageHeight;
};

class HoydeKart : public VisualObject
{
private:
    std::vector<QVector3D> mPunktdata;

public:
    HoydeKart(Shader* shader, float scale);

    std::vector<Rute> mRuter;
};

#endif // HOYDEKART_H
