#ifndef OCTAHEDRONBALL_H
#define OCTAHEDRONBALL_H

#include "visualobject.h"
#include "Objects/bakke.h"


class OctahedronBall : public VisualObject
{
/* Konstruksjonen av objektet og ballen */
private:
    class Logger* mLogger{nullptr};

    int m_rekursjoner;
    int m_indeks;
    float size; // Radiusen til ballen
    void lagTriangel(const QVector3D& v1, const QVector3D& v2, const QVector3D& v3);
    void subDivide(const QVector3D& a, const QVector3D& b, const QVector3D&c, int n);
    void CalculateNormals();

public:
    OctahedronBall(int recursions, float radius);
    ~OctahedronBall(){}


/* Bakken ballen skal være på */
private:
    Bakke* mBakken{nullptr};
    unsigned int mTriangleIndex{0};

/* Fysikk utregning */
private:
    float Gravity = 9.81f;
    float Mass = 1.f;

    QVector3D Acceleration;

    QVector3D Velocity;
    float Speed;

    QVector3D Position_PreviousFrame;
    QVector3D Position;
    QVector3D Estimated_Position_NextFrame;

public:
    void CalculatePhysics(float DeltaTime);

    /* Gets information about the surface the object is on */
    void GetSurfaceInfo(Bakke* bakken, float DeltaTime);
    void CalculateAcceleration(QVector3D SurfaceNormal);
    void UpdateVelocity(float DeltaTime);
    void UpdatePosition();


/* Rendering */
public:
    void init() override;
    void draw() override;
    void draw(QMatrix4x4& projectionMatrix, QMatrix4x4& viewMatrix) override;


};

#endif // OCTAHEDRONBALL_H
