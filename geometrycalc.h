#ifndef GEOMETRY_CALCULATIONS_H
#define GEOMETRY_CALCULATIONS_H

#include <qgsgeometry.h>
#include <QString>
#include <db.h>

// berechnet die entgültige Geometry des Bildes
QgsGeometry * validPolyGeometry(Db * db, QString cam, QString image);

#endif // GEOMETRY_CALCULATIONS_H
