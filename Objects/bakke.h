#ifndef BAKKE_H
#define BAKKE_H

#include <visualobject.h>


class Bakke : public VisualObject
{
private:

public:
    Bakke(Shader* shader);
    ~Bakke(){}

    void CreateTriangle(const QVector3D v1,const QVector3D v2,const QVector3D v3);

    bool BarySentricCoordinate(const QVector3D ObjectPosition, QVector3D& ObjectSurfaceHeight, QVector3D& SurfaceNormal, const int TriangleIndex);

    unsigned int mTriangleIndex{0};
};

#endif // BAKKE_H
