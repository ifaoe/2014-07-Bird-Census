// std libraries

// extra libs

//custom libs
#include <geometrycalc.h>
#include <db.h>

QgsGeometry * cutGeometry(QString cam, QString image_string) 
{
    QgsGeometry * temp_geo_1, temp_geo_2, resultgeom;
    double solar_dir = Db:getSolarAzimuth(cam_string, image_string);
    QString c1_str, c2_str;
    const QString c1 = QString::fromStdString("1");
    const QString c2 = QString::fromStdString("2");
    QString nc1_str, nc2_str, sc1_str, sc2_str;

    uint sync_id = 0;
    if (cam.compare(c1)) {
            c1_str = image_string; 
    } else {
            c2_str = image_string;
    }

    Db::readIdMapping( &sync_id, &cam1_img, &cam2_img );
    uint nsync_id = sync_id+1;
    uint ssync_id = sync_id-1;

    if (solar_dir < 90) {
        // Sonne steht im NO des Bildes
        // Schneiden Nord-Teil im Fall C1 -> SB
        // Scheiden Nord und Ost Teil im Fall C2 -> BB
        // Fall letztes Bild nicht Norden schneiden
		
        if (cam.compare(c1)) {
            /*
             * IF something goes wrong we just skip the cutting part
             */
            temp_geo_1 = Db::readImageEnvelope(c1,c1_str);
            if( Db::readIdMapping( &nsync_id, &nc1_str, &nc2_str) ) {
                QgsGeometry * nc1_img = Db::readImageEnvelope(c1,nc1_str);
                resultgeom = temp_geo_1->difference( nc1_img );
            }
        } else {
            temp_geo_1 = Db::readImageEnvelope(c2,c2_str);
            if( Db::readIdMapping( &nsync_id, &nc1_str, &nc2_str) ) {
                QgsGeometry * nc2_img = Db::readImageEnvelope(c2,nc2_str);
                QgsGeometry * nc1_img = Db::readImageEnvelope(c1,nc1_str);
                QgsGeometry * c1_img  = Db::readImageEnvelope(c1,c1_str);
                temp_geo_2 = temp_geo_1->difference( nc2_img );
                delete temp_geo_1;
                temp_geo_1 = temp_geo_2->difference( nc1_img );
                delete temp_geo_2;
                resultgeom = temp_geo_1->difference( c1_img );
                delete temp_geo_1;
            }
        } 
                
    } else if ( ( solar_dir >= 90 ) && ( solar_dir < 180 ) ) {
        // Fall SO
        // Schneiden Sueden im Fall C1 -> SB
        // Schneiden Sueden und Osten im Fall C2
        // Falls 1. Bild nicht Sueden schneidenz
        if (cam.compare(c1)) {      
            temp_geo_1 = Db::readImageEnvelope(c1,c1_str);
            if( Db::readIdMapping( &ssync_id, &sc1_str, &sc2_str) ) {
                QgsGeometry * sc1_img = Db::readImageEnvelope(c1,sc1_str);
                resultgeom = temp_geo_1->difference( sc1_img );
            }
        } else {
            temp_geo_1 = Db::readImageEnvelope(c2,c2_str);
            if( Db::readIdMapping( &ssync_id, &sc1_str, &sc2_str) ) {
                QgsGeometry * sc2_img = Db::readImageEnvelope(c2,sc2_str);
                QgsGeometry * sc1_img = Db::readImageEnvelope(c1,sc1_str);
                QgsGeometry * c1_img  = Db::readImageEnvelope(c1,c1_str);
                temp_geo_2 = temp_geo_1->difference( sc2_img );
                delete temp_geo_1;
                temp_geo_1 = temp_geo_2->difference( sc1_img );
                delete temp_geo_2;
                resultgeom = temp_geo_1->difference( c1_img );
                delete temp_geo_1;
            }
        }

    } else if ( ( solar_dir >= 180 ) && ( solar_dir < 270 ) ) {
        // Fall SW
        // Schneiden Sueden und Westen im Fall C1 -> SB
        // Schneiden Sueden im Fall C2
        // Falls 1. Bild nicht Sueden schneiden
        if (cam.compare(c1)) {
            temp_geo_1 = Db::readImageEnvelope(c1,c1_str);      
            if( Db::readIdMapping( &ssync_id, &sc1_str, &sc2_str) ) {
                QgsGeometry * sc2_img = Db::readImageEnvelope(c2,sc2_str);
                QgsGeometry * sc1_img = Db::readImageEnvelope(c1,sc1_str);
                QgsGeometry * c2_img  = Db::readImageEnvelope(c2,c2_str);
                temp_geo_2 = temp_geo_1->difference( sc2_img );
                delete temp_geo_1;
                temp_geo_1 = temp_geo_2->difference( sc1_img );
                delete temp_geo_2;
                resultgeom = temp_geo_1->difference( c2_img );
                delete temp_geo_1;
            }
        } else {
            temp_geo_1 = Db::readImageEnvelope(c2,c2_str);
            if( Db::readIdMapping( &ssync_id, &sc1_str, &sc2_str) ) {
                QgsGeometry * sc2_img = Db::readImageEnvelope(c2,sc2_str);
                resultgeom = temp_geo_1->difference( sc2_img );
            }
        }
                
    } else if (( solar_dir >= 270 ) && ( solar_dir < 360 )) {
        // Fall NW
        // Schneiden Norden und Westen im Fall C1 -> SB
        // Schneiden Norden im Fall C2
        // Falls letztes Bild nicht Norden schneiden
        if (cam.compare(c1)) {      
            temp_geo_1 = Db::readImageEnvelope(c1,c1_str);      
            if( Db::readIdMapping( &nsync_id, &nc1_str, &nc2_str) ) {
                QgsGeometry * nc2_img = Db::readImageEnvelope(c2,nc2_str);
                QgsGeometry * nc1_img = Db::readImageEnvelope(c1,nc1_str);
                QgsGeometry * c2_img  = Db::readImageEnvelope(c2,c2_str);
                temp_geo_2 = temp_geo_1->difference( nc2_img );
                delete temp_geo_1;
                temp_geo_1 = temp_geo_2->difference( nc1_img );
                delete temp_geo_2;
                resultgeom = temp_geo_1->difference( c2_img );
                delete temp_geo_1;
        } else {
            temp_geo_1 = Db::readImageEnvelope(c2,c2_str);
            if( Db::readIdMapping( &nsync_id, &nc1_str, &nc2_str) ) {
                QgsGeometry * nc2_img = Db::readImageEnvelope(c2,nc2_str);
                resultgeom = temp_geo_1->difference( nc2_img );
            }
        } 
    }
    return resultgeom;
}

