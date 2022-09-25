#include "hoydekart.h"
#include <iostream>
#include <ostream>


HoydeKart::HoydeKart(Shader* shader, float scale)
{
    setShader(shader);

    /* Max & Min verdier for hoydedataen */
    float Xmax{}, Ymax{}, Zmax{};
    float Xmin{}, Ymin{}, Zmin{};
    int count{};    // Sjekker antall linjer/punkter det er i hoydedata fila

    /* Leser hoydedata fila */
    std::string filnavn = "../VSIM_3D-Prosjekt/HoydeKart/SmallArea2.txt";
    std::ifstream inn;
    inn.open(filnavn.c_str());
    if (inn.is_open())
    {
        float X{}, Y{}, Z{};
        int c{};
        while (true)
        {
            /* Reading inn points from file */
            c++;
            if (c == 1)
            {
                inn >> X;
            }
            else if (c == 2)
            {
                inn >> Y;
            }
            else if (c == 3)
            {
                inn >> Z;

                /* Initializing the first round */
                if (count == 0)
                {
                    Xmax = X; Xmin = X;
                    Ymax = Y; Ymin = Y;
                    Zmax = Z; Zmin = Z;
                }

                /* Checking if there is a new maximum or minimum value for each coordinate */
                /* X */
                if (X > Xmax){ Xmax = X; }
                if (X < Xmin){ Xmin = X; }
                /* Y */
                if (Y > Ymax){ Ymax = Y; }
                if (Y < Ymin){ Ymin = Y; }
                /* Z */
                if (Z > Zmax){ Zmax = Z; }
                if (Z < Zmin){ Zmin = Z; }


                mPunktdata.push_back(QVector3D{X, Y, Z});
                /* Creating vertex */
//                mVertices.push_back(Vertex{ X, Y, Z });
                count++;

                c = 0;
            }

            if (inn.eof()){ break; }
        }

        inn.close();
    }
    else
    {
        LogError("Could not open file");
    }

//    for (unsigned int i{}; i < mVertices.size(); i++)
//    {
//        mVertices[i].SubtractPosition(Xmin, Ymin, Zmin);
//        mVertices[i].ScalePosition(scale);
//        mIndices.push_back(i);  // Funker ikke, vil bare kunne tegne meshen nå for å sjekke om lesing av fila funka
//    }
    /* Trekker fra minimums verdiene for xyz slik at terrenget blir tegnet i origo */
    for (unsigned int i{}; i < mPunktdata.size(); i++)
    {
        mPunktdata[i] -= QVector3D{Xmin, Ymin, Zmin};
        LogVector("Punkt: " + std::to_string(i), mPunktdata[i]);
    }

    /* Line count and Max & Min values of the area */
    Xmax -= Xmin;
    Ymax -= Ymin;
    Zmax -= Zmin;

    LogValue("Line count", count);
    LogValue("Xmax", Xmax);
//    LogValue("Xmin", Xmin);
    LogValue("Ymax", Ymax);
//    LogValue("Ymin", Ymin);
    LogValue("Zmax", Zmax);
//    LogValue("Zmin", Zmin);


    /* Regulær Triangulering */
    float resolutionX{3};
    float resolutionY{2};
//    float Resolution[2]{ resolutionX, resolutionY };
//    std::vector<std::vector<float>> rute;



    float StepLengthX = Xmax / resolutionX;
    float StepLengthY = Ymax / resolutionY;

    /* Lager alle rutene vi skal ha basert på resolution/oppløsningen som er ønskelig */
    for (int x{1}; x <= resolutionX; x++)
    {
        for (int y{1}; y <= resolutionY; y++)
        {
            Rute rute(x, y);    // Kan omtenke hvordan indekseringen gjøres

            /* Setter grensene til ruten */
            rute.Xmin = StepLengthX * (x - 1);
            rute.Xmax = StepLengthX * x;

            rute.Ymin = StepLengthY * (y - 1);
            rute.Ymax = StepLengthY * y;

            mRuter.push_back(rute);

            Log("Lagde Rute: " + std::to_string(x) + ", " + std::to_string(y));
        }
    }

    /* Går igjennom alle punktene for å finne ut av hvilke rute de tilhører */
    for (unsigned int i{}; i < mPunktdata.size(); i++)
    {
        /* Sjekker hvilken rute punktet er innenfor */
        for (unsigned int k{}; k < mRuter.size(); k++)
        {
            if (
                mPunktdata[i].x() >= mRuter[k].Xmin &&
                mPunktdata[i].x() <= mRuter[k].Xmax &&
                mPunktdata[i].y() >= mRuter[k].Ymin &&
                mPunktdata[i].y() <= mRuter[k].Ymax
                )
            {
                mRuter[k].mPunkter.push_back(mPunktdata[i]);
            }
        }
    }

    /* Regner ut gjennomsnitts høyden til hver rute */
    for (unsigned int i{}; i < mRuter.size(); i++)
    {
        float z{};
        for (unsigned int p{}; p < mRuter[i].mPunkter.size(); p++)
        {
            z += mRuter[i].mPunkter[p].z();
        }
        z /= mRuter[i].mPunkter.size();
        mRuter[i].averageHeight = z;

//        LogValue("Average height", mRuter[i].averageHeight);
//        Log("Rute Points: " + std::to_string(mRuter[i].mPunkter.size()));
    }

    /* Lag vertexer med rutene */
    // Nested For loop for(x for(y)) størrelsen basert på oppløsning
    // Må kunne finne en rute basert på denne indeksen  (hashmap?)
}
