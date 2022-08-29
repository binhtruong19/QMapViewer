#include "layer.h"
#include "QJsonObject"
#include "QJsonArray"

Layer::Layer()
{

}

bool Layer::SetFeatureType(const string &FT)
{
    FeatureType = Feature::Feature_Type(FT);
    if (FeatureType==_FeatureType::Unknown)
        return false;
    else
        return true;
}
void Layer::SetFeatureType(_FeatureType FT)
{
    FeatureType = FT;
}
bool Layer::SetFeatureType(const QString &FT)
{
    FeatureType = Feature::Feature_Type(FT.toStdString());
    if (FeatureType==_FeatureType::Unknown)
        return false;
    else
        return true;
}
_FeatureType Layer::GetFeatureType()
{
    return FeatureType;
}
Layer::Layer(const Layer& L)
{
    features = L.features;
    FeatureType = L.FeatureType;
}
Layer& Layer::operator = (const Layer &L)
{
    features = L.features;
    FeatureType = L.FeatureType;
    return *this;
}
void Layer::AppendToFeatures(const Feature &feature)
{
    features.push_back(feature);
}

bool Layer::GetFromJsonDocument(const QJsonDocument &JsonDoc)
{
    QJsonObject JsonObject = JsonDoc.object();
    foreach(const QString& key, JsonObject.keys()) {
        QJsonValue value = JsonObject.value(key);
        qDebug() << "Key = " << key << ", Value = " << value.toString();
    }
    QJsonArray Jfeatures = JsonObject.value("features").toArray();
    for (int i=0; i<Jfeatures.count(); i++) {
        QJsonObject value = Jfeatures[i].toObject();
        QJsonObject geometry = value["geometry"].toObject();
        qDebug()<<"Geometry: " <<geometry;
        QJsonArray coordinates = geometry["coordinates"].toArray();
        QString type = geometry["type"].toString();
        qDebug() << "Key = " << i << ", Coordinates = " << coordinates << ", Type = " << type;
        Feature feature;
        feature.GetGeometryFromJsonArray(coordinates);
        features.push_back(feature);
    }

    return true;
}

QVector<QVector<shared_ptr<QGraphicsItem>>> Layer::toGraphicItems()
{
    QVector<QVector<shared_ptr<QGraphicsItem>>> out;
    for (int i=0; i<features.size(); i++)
    {
        out.push_back(features[i].toGraphicItems());
    }
    return out;
}

double Layer::GetRange(range rng, dir dr)
{
    if (features.size()==0)
        return 0;
    double extreme = features[0].GetRange(rng,dr);
    for (int i=1; i<features.size(); i++)
    {
        if (rng==range::max)
        {
            extreme = max(extreme,features[i].GetRange(rng,dr));
        }
        if (rng==range::min)
        {
            extreme = min(extreme,features[i].GetRange(rng,dr));
        }

    }
    return extreme;
}

QRectF Layer::GetBoundingRect()
{
    return QRectF(GetRange(range::min,dir::x),GetRange(range::min,dir::y),GetRange(range::max,dir::x)-GetRange(range::min,dir::x),GetRange(range::max,dir::y)-GetRange(range::min,dir::y));
}