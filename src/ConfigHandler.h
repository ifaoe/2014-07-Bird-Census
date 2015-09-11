//    This program is free software: you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation, either version 3 of the License, or
//    (at your option) any later version.
//
//    This program is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//    GNU General Public License for more details.
//
//    You should have received a copy of the GNU General Public License
//    along with this program.  If not, see <http://www.gnu.org/licenses/>.

#ifndef CONFIGHANDLER_H_
#define CONFIGHANDLER_H_

#include <QSettings>
#include <QDebug>

struct DatabaseInfo {
	QString id = "local";
	QString host = "localhost";
	int port = 5432;
	QString name = "daisi";
	QString password = "18ifaoe184";
	QString user = "daisi";
};

class ConfigHandler : public QSettings{
public:
	/*
	 * TODO: Make private!
	 */
	QString current_cam;
	QString current_image;
	void InitSettings();
	void AddDatabase(const QString & id, const QString & host, int port, const QString & name,const QString & user,
			const QString & password);
	bool removeDatabase(const QString & id);
	void setAppPosition(QPoint pos);
	QPoint getAppPosition();
	void setAppSize(QSize size);
	QSize getAppSize();
	void setAppMaximized(bool max);
	bool getAppMaximized();
	void setPreferredDatabase(const QString & database);
	QString getPreferredDatabase();
	void setPreferredSession(const QString & session);
	QString getPreferredSession();
	QStringList getDatabaseList();
	DatabaseInfo getDatabaseInfo(const QString & id);
	QString getUser() { return user; }
	QString replaceProjectSettings(const QString & query);
	void setAdmin(bool admin);
	bool getAdmin();
	void setProjectId(const QString & id) { project_id = id; }
	QString getProjectId() { return project_id; }
	void setFlightId(const QString & id) {flight_id = id; }
	QString getFlightId() { return flight_id; }
	void setUtmSector(int sector) { utm_sector = sector; }
	int getUtmSector() { return utm_sector; }
	void setProjectPath(const QString & path) { project_path = path; }
	QString getProjectPath() { return project_path; }
	void setCurrentCam(const QString & cam) { current_cam = cam; }
	QString getCurrentCam() {return current_cam; }
	void setCurrentImage(const QString & image) { current_image = image; }
	QString getCurrentImage() { return current_image; }
	void setQGisPrefixPath(const QString & path);
	QString getQGisPrefixPath();
	int getRedChannel();
	double getRedMinValue();
	double getRedMaxValue();
	int getBlueChannel();
	double getBlueMinValue();
	double getBlueMaxValue();
	int getGreenChannel();
	double getGreenMinValue();
	double getGreenMaxValue();
	int getTileWidth();
	int getTileHeight();
	double getSelectDistance();


	private:
	QString user;
	QString project_id;
	QString flight_id;
	int utm_sector;
	QString project_path;
};

//const QString ACFG_SQL_QRY_READ_IMAGES =
//		"SELECT tp, px, py, ux, uy, lx, ly, rcns_id FROM raw_census WHERE session ='$(session)' "
//		"AND cam  = '%1' AND img = '%2' AND usr = '%3' AND epsg = $(utmSector) order by rcns_id";
const QString ACFG_SQL_QRY_READ_RIMAGE =
		"SELECT rimg_id, tm_when, tm_seen FROM raw_images WHERE session ='$(session)'"
		" AND cam  = '%1'  AND img = '%2' AND usr = '%3' AND epsg  = $(utmSector)";
const QString ACFG_SQL_QRY_READ_RIMAGE_TILE =
		"SELECT rtls_id, ux, uy, w, h, tm_when, tm_seen FROM raw_tiles WHERE session ='$(session)'"
		" AND cam  = '%1' AND img = '%2' AND usr = '%3' AND epsg  = $(utmSector)";
const QString ACFG_SQL_QRY_READ_RCENSUS = "SELECT rcns_id, usr, tp, ux, uy, lx, ly  FROM raw_census WHERE session ='$(session)' "
		"AND cam  = '%1' AND img = '%2' AND usr='%3' AND epsg = $(utmSector) order by rcns_id";
const QString ACFG_SQL_QRY_READ_RCENSUS_ADMIN =
		"SELECT rcns_id, usr, tp, ux, uy, lx, ly FROM raw_census WHERE session ='$(session)' "
		"AND cam  = '%1' AND img = '%2' AND epsg = $(utmSector) order by rcns_id";
const QString ACFG_SQL_QRY_DEL_RCENSUS      =
		"DELETE FROM raw_census WHERE rcns_id =%1 AND session='$(session)' AND cam='%2' AND img='%3'";
const QString ACFG_SQL_QRY_READ_IMGENV      =
		"SELECT st_astext(geo_rc_bb) as bb, st_astext(geo_rc_sb) as sb, cam1_cfg as cfg1, "
		"cam2_cfg as cfg2 FROM sync_utm$(utmSector) WHERE cam%1_id = '%2' AND session = '$(flight)'";
const QString ACFG_SQL_QRY_READ_FDATA       =
		"SELECT lon, lat, cog, gps_dt, gps_tm FROM sync_utm$(utmSector) WHERE cam%1_id = '%2' "
		"AND session = '$(flight)'";
const QString ACFG_SQL_QRY_READ_SDATA       =
		"SELECT lon, lat, cog, gps_dt, gps_tm FROM sync_utm$(utmSector) WHERE session = '$(flight)'"
		" AND gps_trc = %1 AND $(filter)";
const QString ACFG_SQL_QRY_READ_ID_MAP      =
		"SELECT sync_id, cam1_id, cam2_id, gps_trc FROM sync_utm$(utmSector) WHERE %1 = '%2' "
		"AND session = '$(flight)'";
const QString ACFG_SQL_QRY_READ_DONE        =
		"SELECT distinct(img) FROM raw_images WHERE cam = '%1' AND session = '$(session)' AND rdy=1";
const QString ACFG_SQL_QRY_READ_IMAGES =
		"SELECT trc, cam, img FROM image_properties WHERE %1 AND project_list @> ARRAY['$(session)'] "
		"ORDER BY trc, cam, img";
const QString ACFG_SQL_QRY_READ_IMAGES_NOT_READY =
		"SELECT i.trc, i.cam, i.img, r.rdy FROM "
		"(SELECT trc, cam, img FROM image_properties WHERE project_list @> ARRAY['$(session)'] )as i "
		"LEFT JOIN (SELECT cam, img, rdy FROM raw_images WHERE %1 AND session='$(session)' AND rdy=1) as r "
		"ON i.cam=r.cam AND i.img=r.img "
		"WHERE r.rdy IS NULL ORDER BY i.trc, i.cam, i.img";
const QString ACFG_SQL_QRY_READ_VALIDPOLY =
		"SELECT ST_AsText(cut_env) FROM image_properties WHERE cam='%1' AND img='%2' AND session = '$(flight)'";

#endif /* CONFIGHANDLER_H_ */
