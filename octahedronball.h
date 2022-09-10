#ifndef OCTAHEDRONBALL_H
#define OCTAHEDRONBALL_H

#include "visualobject.h"


class OctahedronBall : public VisualObject
{
private:
    class Logger* mLogger{nullptr};

    int m_rekursjoner;
    int m_indeks;
    float size; // Radiusen til ballen
    void lagTriangel(const QVector3D& v1, const QVector3D& v2, const QVector3D& v3);
    void subDivide(const QVector3D& a, const QVector3D& b, const QVector3D&c, int n);

public:
    OctahedronBall(int recursions, float radius);
    ~OctahedronBall(){}


/* Fysikk utregning */
private:
    float GravityConstant = -9.81f;
    float Mass = 1.f;
    float Speed;
    QVector3D Velocity;

    QVector3D Position_PreviousFrame;
    QVector3D Position;
    QVector3D Estimated_Position_NextFrame;

public:
    void CalculatePhysics(float DeltaTime);


/* Rendering */
public:
    void init() override;
    void draw() override;
    void draw(QMatrix4x4& projectionMatrix, QMatrix4x4& viewMatrix) override;


};

#endif // OCTAHEDRONBALL_H
