#ifndef OCTAHEDRONBALL_H
#define OCTAHEDRONBALL_H

#include "visualobject.h"
#include "Objects/bakke.h"


class OctahedronBall : public VisualObject
{
/* Konstruksjonen av objektet og ballen */
private:
    class Logger* mLogger{nullptr};
    int FrameCount{};

    int m_rekursjoner;
    int m_indeks;
    float radius; // Radiusen til ballen
    void lagTriangel(const QVector3D& v1, const QVector3D& v2, const QVector3D& v3);
    void subDivide(const QVector3D& a, const QVector3D& b, const QVector3D&c, int n);
    void CalculateNormals();


public:
    OctahedronBall(int recursions, float radius);
    ~OctahedronBall(){}

    void Reset(const QVector3D& StartLocation);
    void MoveTo(const QVector3D& Location);


/* Bakken ballen skal være på */
private:
    Bakke* mBakken{nullptr};
    unsigned int mTriangleIndex{0};

/* Fysikk utregning */
private:
//    float Gravity = 9.81f;
    float Gravity = 1.f;  // Temp gravity
    float Mass = 1.f;

    QVector3D Acceleration_PreviousFrame;
    QVector3D Acceleration;

    QVector3D Velocity_PreviousFrame;
    QVector3D Velocity;
    float Speed;

    QVector3D Position_PreviousFrame;
    QVector3D Position;

public:
    void CalculatePhysics(float DeltaTime);

    /* Gets information about the surface the object is on */
    void GetSurfaceInfo(Bakke* bakken, float DeltaTime);
    void CalculateAcceleration(QVector3D SurfaceNormal, const int SurfaceIndex);
    void UpdateVelocity(float DeltaTime);
    void UpdatePosition(const QVector3D& Adjustment);


/* Rendering */
public:
    void init() override;
    void draw() override;
    void draw(QMatrix4x4& projectionMatrix, QMatrix4x4& viewMatrix) override;

/* Debug */
public:
    void LogVector(const std::string Name, const QVector3D vector);
};

#endif // OCTAHEDRONBALL_H
