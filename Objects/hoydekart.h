#ifndef HOYDEKART_H
#define HOYDEKART_H

#include <visualobject.h>
#include <unordered_map>
#include "StandAlone/LoggerFunctions.h"

//std::pair<int, int> indeksering;



class Rute : VisualObject
{
public:
    Rute(Shader* shader, int index, int X, int Y);
    ~Rute(){}

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
    float averageHeight{0.1f};
    QVector3D Center;

    void MakeCenter()
    {
        Center.setX((Xmin + Xmax)/2);
        Center.setY((Ymin + Ymax)/2);
        if (mPunkter.size() != 0)
        {
            Center.setZ(averageHeight);
        }
        else
        {
            averageHeight = 0.1f;
            Center.setZ(averageHeight);
        }

//        Log("Point size of: " + std::to_string(TriangleIndex) + " : " + std::to_string(mPunkter.size()));
    }
    void MakeRute()
    {

        mVertices.push_back(Vertex{Xmin, Ymin, 0.1}); // a
        mVertices.push_back(Vertex{Xmax, Ymin, 0.1}); // b
        mVertices.push_back(Vertex{Xmax, Ymax, 0.1}); // c
        mVertices.push_back(Vertex{Xmin, Ymax, 0.1}); // d

        mIndices = { 0, 1, 2, 3 };

        for (auto& it : mVertices)
        {
            it.Vertex::setNormal(1, 1, 1);
        }

        VisualObject::init();
    }

    /* Rendering av Rute */
    void draw(QMatrix4x4 &projectionMatrix, QMatrix4x4 &viewMatrix) override;
};

class HoydeKart : public VisualObject
{
private:
    std::vector<QVector3D> mPunktdata;

public:
    HoydeKart(Shader* shader, float scale);

    /* Max & Min verdier for hoydedataen */
    float Xmax{}, Ymax{}, Zmax{};
    float Xmin{}, Ymin{}, Zmin{};

    void ReadComplexPointCloud(std::string file, int PointRes);
    void WriteSimplePointCloud();
    void ReadSimplePointCloud();

    void WriteMeshToFile();
    void GetMeshFromFile();

    std::vector<Rute*> mRuter;

//    void CalculateNormal()

//    std::unordered_map<std::pair<int, int>, Rute> mRuter2;

public: /* Rendering */
    virtual void draw(QMatrix4x4& projectionMatrix, QMatrix4x4& viewMatrix) override;
};

#endif // HOYDEKART_H
