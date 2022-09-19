#ifndef OCTAHEDRONBALL_H
#define OCTAHEDRONBALL_H

#include "visualobject.h"
#include "Objects/bakke.h"


class OctahedronBall : public VisualObject
{
/* Konstruksjonen av objektet og ballen */
private:
    /*class */Logger* mLogger{nullptr};
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

    void Reset(const QVector3D& StartLocation, const QVector3D StartVelocity);
    void MoveTo(const QVector3D& Location);

    float GetRadius() const { return radius; }


/* Bakken ballen skal være på */
private:
    Bakke* mBakken{nullptr};
    unsigned int mTriangleIndex{0};

/* Fysikk utregning */
private:

    float Gravity = 9.81f;  // m/s^2
    float Mass = 1.f;       // kg
    float Elasticity = 0.8f; // How much velocity is kept when it bounces of an angle

    QVector3D Acceleration_PreviousFrame;
    QVector3D Acceleration;
    float FrictionForce;
    QVector3D FrictionAcceleration;
    QVector3D NormalForce;

    QVector3D Velocity_PreviousFrame;
    QVector3D Velocity;     // m/s
    float Speed;

    QVector3D Position_PreviousFrame;
    QVector3D Position;

public:

    /* Setter start verdier */
    void SetStartVelocity(const QVector3D StartVelocity);

    /* Kalkulerer fysikken. Inneholder all logikken som trengs til det */
    void CalculatePhysics(Bakke* bakken, float DeltaTime);
    void CalculateAcceleration(QVector3D SurfaceNormal, float FrictionForce, const int SurfaceIndex);
    void UpdateVelocity(float DeltaTime);

    void SetNormalForce(const QVector3D SurfaceNormal);

    float GetDistanceToSurface(const QVector3D Position, const QVector3D SurfacePosition, const QVector3D SurfaceNormal);

/* Rendering */
public:
    void init() override;
    void draw() override;
    void draw(QMatrix4x4& projectionMatrix, QMatrix4x4& viewMatrix) override;

/* Debug */
public:

};

#endif // OCTAHEDRONBALL_H

/* Logging functions */
inline void Log(const std::string log)
{
    Logger::getInstance()->logText(log);
}

/* Able to log different values */
inline void LogValue(const std::string Name, const float value)
{
    Logger::getInstance()->logText(Name + ": " + std::to_string(value));
}
inline void LogValue(const std::string Name, const int value)
{
    Logger::getInstance()->logText(Name + ": " + std::to_string(value));
}
inline void LogValue(const std::string Name, const unsigned int value)
{
    Logger::getInstance()->logText(Name + ": " + std::to_string(value));
}
inline void LogValue(const std::string Name, const double value)
{
    Logger::getInstance()->logText(Name + ": " + std::to_string(value));
}

/* Log a vector Logger instance */
inline void LogVector(const std::string Name, const QVector3D Vector)
{
    std::string sVector = "( " + std::to_string(Vector.x()) + ", " +std::to_string(Vector.y()) + ", " + std::to_string(Vector.z()) + " )";
    Logger::getInstance()->logText(Name + ": " + sVector);
}

inline void LogFloat(const std::string Name, const float value)
{
    Logger::getInstance()->logText(Name + ": " + std::to_string(value));
}
