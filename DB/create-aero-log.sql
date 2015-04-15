-- ============================================================================
-- Users
-- ============================================================================

drop table users;
create table users (
   -- User ID
   uid  serial primary key,
   -- User Name das Kuezel
   uname varchar(32) unique, 
   -- E-Mailadresse
   mail varchar (128),
   -- Vorname 
   fname varchar(128),
   -- Nachname 
   sname varchar(128),
   -- Funktion
   funct varchar(128),
   -- Passwort MD5
   pass  varchar(128) 
);
comment on table users is
'Nutzertabelle des DAISI Projketes, haupstaechlich und Rollenverteilungen und zu realisieren';
comment on column users.uname is 
'Nutzername, wird eindeutig vergerben';
comment on column users.mail is 
'E-Mail des Nutzers';
comment on column users.fname is 
'Vorname';
comment on column users.sname is 
'Nachname';
comment on column users.pass is 
'Passwort';


-- ============================================================================
-- Aktuelle Nutzer DAISI
-- ============================================================================
insert into users values (0, 'root', 'weidauer@ifaoe.de','System','Administrator','Systemadministrator mit allenZugriffen', md5('18ifaoe184'));
insert into users values (1,'awe', 'weidauer@ifaoe.de', 'Alexander', 'Weidauer', 'Systementwickler und technischer Leiter', md5('18ifaoe184'));
insert into users values (2,'awg', 'axel.wegener@ifaoe.de', 'Axel', 'Wegener', 'Systementwickler, technischer Assistent, Operator', md5('18ifaoe184'));
insert into users values (3,'twig', 'twig@huckfinn.de', 'Svante', 'Peters', 'Schueler, Bildvorklassifikation', md5('18ifaoe184'));
insert into users values (4,'asu', 'schulz@ifaoe.de', 'Axel', 'Schulz', 'Bildklassifikation und QM', md5('18ifaoe184'));
insert into users values (5,'tlo', 'loeffler@ifaoe.de', 'Thomas', 'Loeffler', 
 'Flugoperator und technischer Assistent', md5('18ifaoe184'));
insert into users values (6,'kos', 'schleicher@ifaoe.de', 'Konrad', 'Schleicher', 
'Flugoperator und technischer Assistent', md5('18ifaoe184'));
insert into users values (7,'mki', 'kilian@ifaoe.de', 'Meike', 'Kilian', 
'technischer Assistent', md5('18ifaoe184'));
insert into users values (8,'rfu', 'fuerst@ifaoe.de', 'Rita', 'Fuerst', 
'Bildoperator und technischer Assistent', md5('18ifaoe184'));
insert into users values (9,'uge', 'gebhardt-jesse@ifaoe.de', 'Ulf', 'Gebhardt-Jesse', 
'Projekt- / Flugmanagement', md5('18ifaoe184'));
insert into users values (10,'mal', 'info@martin-laczny.de', 'Martin', 'Laczny', 
'Projekt- / Flugmanagement', md5('18ifaoe184'));
insert into users values (11,'kbr', 'brosda@ifaoe.de', 'Kai', 'Brosda', 
 'Flugoperator und technischer Assistent', md5('18ifaoe184'));
insert into users values (12,'pcp', 'p.cipra@krebsgruppe.de', 'Peter', 'Cipra', 
 'Pilot', md5('18ifaoe184'));
insert into users values (13,'wmk', 'w.miehlke@krebsgruppe.de', 'Willi', 'Mielke', 
 'Pilot', md5('18ifaoe184'));
insert into users values (14,'tco', 'coppack@ifaoe.de', 'Tim', 'Coppack', 
'Projekt- / Flugmanagement', md5('18ifaoe184'));

insert into users values (15,'wpi', 'piper@ifaoe.de', 'Werner', 'Piper', 
'Projekt- / Flugmanagement', md5('18ifaoe184'));

-- ============================================================================
-- Groups
-- ============================================================================
drop table groups;
create table groups (
   gid serial primary key,
   gname varchar(128) unique, 
   funct varchar(128) unique 
);


insert into groups values (0,'root','Systemadministrator');
insert into groups values (1,'db-admin','Datenbankadministrator');
insert into groups values (2,'img-admin','Administrator fuer Bilddatenbanken');
insert into groups values (3,'import-images','Import von Rohdaten');
insert into groups values (4,'devel-images','Entwicklung von Rohdaten');
insert into groups values (5,'calc-images','Berechnung Referenzen/ Klassen');
insert into groups values (6,'pre-class-images','Vorklassifizieren');
insert into groups values (7,'bird-class-images','Vogelklassifikation');
insert into groups values (8,'end-class-images','QM Vogelklassifikation');
insert into groups values (9,'project-mgm','Projectmanagement');
insert into groups values (10,'flight-mgm','Flugmanagement');
insert into groups values (11,'pilot','Pilot');
insert into groups values (12,'tec-assist','Technischer Assistent');
insert into groups values (13,'tec-admin','Technischer Administrator');
insert into groups values (14,'hse-admin','Operateur Sicherheit');


-- ============================================================================
-- Tabelle der Gruppenzugehoerigkeit 
-- ============================================================================

drop table acl;
create table acl(
  aid serial primary key,
  uid int references users(uid), 
  gid int references groups(gid)
);

insert into acl values (0,0,0);
insert into acl values (1,1,0);
insert into acl values (2,1,1);
insert into acl values (3,1,2);
insert into acl values (4,1,3);
insert into acl values (5,1,4);
insert into acl values (6,1,5);
insert into acl values (7,1,6);
insert into acl values (8,2,0);
insert into acl values (9,3,6);
insert into acl values (10,4,6);
insert into acl values (11,4,7);

-- ----------------------------------------------------------------------------
-- Project
-- ----------------------------------------------------------------------------
create table projects (
   prj_id     serial primary key,
   prj_short  varchar(32),
   prj_msi    varchar(32),
   prj_name   varchar(128),
   client     varchar(128),
   leader     int references users(uid),
   vice       int references users(uid),
   rem        text
);

@TODO Korrektur und Insert
insert into projects values ('HWI3', 'P12800', '3. Testflug Wismarbucht', 'BSH/ FTZ/ Uni Aahus',
 14,  1, 'technischer Evaluierungsflug Tandemkamera, haendische Ausloesung');
insert into projects values ('GT1', 'P12800', 'GlobalTech I', 'Kavemann',
 15, 14, 'erstes Bildflugprojekt');
insert into projects values ('DTSB24','P12800','DanTysk-Sandbank-24','Eva Phillip',
 15, 14, 'zweites Bildflugprojekt');

-- ----------------------------------------------------------------------------
-- Flugzeuge
-- ----------------------------------------------------------------------------
drop table planes;
create table planes (
   pf_id serial primary key,
   call_sign varchar(16) default 'D-GKRE' , 
   pf_type   varchar(32)  default 'Partenavia P68C'
);

-- ----------------------------------------------------------------------------
-- Sensoren 
-- ----------------------------------------------------------------------------
drop table sensors;
create table sensors (
  sns_id    serial primary key, 
  sns_type  varchar(32) default 'DAISI V1.0',
  num_cam   int default 2,
  foc_len   float4 default 0.110,
  op_hgt    float4 default 423.0,
  op_vel    float4 default 180.0,
  smp_inv   float4 default 1.5,
  ovr_cam   float4 default 5.0,
  ovr_ifd   float4 default 40.0,
  ovr_xfd   float4 default  0.0,
  res_xfd   float4 default 5.2*1E-6,
  res_ifd   float4 default 5.2*1E-6,
  cells_ifd float4 default 7760.0,
  cells_xfd float4 default 10328.0,  
  tilt_xfd  float4 default 13.02,
  tilt_ifd  float4 default  0.00,
  oang_xfd  float4 default 27.42,
  oang_ifd  float4 default 20.77
  
);
insert into sensors (sns_id) values (0) ;

-- ----------------------------------------------------------------------------
-- Flight sessions 
-- ----------------------------------------------------------------------------
drop table sessions cascade;
create table sessions (
   ses_id     serial primary key,
   ses_key    varchar(64) unique,
   prj_ref    integer references projects(prj_id),
   sns_ref    integer references sensors(sns_id),
   pln_ref    integer references planes(pf_id),
   sector     int not null,
   date_start timestamp,
   date_end   timestamp,
   cam_op     int references users(uid),
   tec_op     int references users(uid),
   prj_op     int references users(uid),
   flg_op     int references users(uid),
   pilot1     int references users(uid),
   pilot2     int references users(uid),
   rem        text
);

TODO Einpflegen 
insert into sessions (ses_key, prj_ref, sns_ref, pln_ref, sector, date_start, date_end,
 cam_op, tec_op, prj_op, flg_op, pilot1, pilot2) values
('2014-05-16-GT1',....)


-- ----------------------------------------------------------------------------
-- Wetterbedingungen 
-- ----------------------------------------------------------------------------
create table weather (
   wth_id serial primary key,
   ses_ref   integer references sessions(ses_id),
   time_from timestamp,
   time_to   timestamp,
   rem text 
);

-- ----------------------------------------------------------------------------
-- See Bedingungen 
-- ----------------------------------------------------------------------------
create table sea (
   see_id serial primary key,
   ses_ref   integer references sessions(ses_id),
   time_from timestamp,
   time_to   timestamp,
   rem text 
);

-- ----------------------------------------------------------------------------
-- Artentabelle
-- ----------------------------------------------------------------------------
\i taxa.pg.sql

-- ----------------------------------------------------------------------------
-- Verhalten
-- ----------------------------------------------------------------------------
drop table behavior;
create table behavior (
  bhv_id      serial primary key,
  bhv_method  varchar(8) not null,
  bhv_key_de  varchar(8) unique not null,
  bhv_desc_de varchar(64) unique not null
);

--  DBF bedeutet DigitalerBildFlug
insert into behavior (bhv_method, bhv_key_de, bhv_desc_de) values
('DBF', 's','schwimmend'),
('DBF', 'ss','schwimmend mit Schiff assoziiert'),
('DBF', 'f','fliegend'),
('DBF', 'fa','auffliegend'),
('DBF', 'fl','landend'),
('DBF', 'fs','fliegend mit Schiff assoziiert'),
('DBF', 'r','rastend'),
('DBF', 'rs','rastend mit Schiff assoziiert'),
('DBF', 'rf','rastend fressend'),
('DBF', 't','tauchend'),
('DBF', 'ts','tauchend mit Schiff assoziiert'),
('DBF', 'tt','fluchttauchend'),
('DBF', 'ka','keine Angabe');


-- ----------------------------------------------------------------------------
-- Alter
-- ----------------------------------------------------------------------------
drop table gender;
create table gender (
  gnd_id      serial primary key,
  gnd_key_de  varchar(8) unique not null,
  gnd_key_en  varchar(8) unique not null,
  gnd_desc_en varchar(64) unique not null,
  gnd_desc_de varchar(64) unique not null,
  gnd_imp     varchar(64) unique not null
);

insert into gender (gnd_key_de, gnd_key_en, gnd_desc_en, gnd_desc_de, gnd_imp) values
 ('U','U', 'undefined', 'unbestimmt', 'unklar'),
 ('M','M', 'male', 'männlich', 'Männchen'),
 ('W','F', 'female','weiblich', 'Weibchen'),
 ('J','I', 'immature','juvenil','immat'),
 ('A', 'A', 'adult','adult','adult');

-- ----------------------------------------------------------------------------
-- Quality
-- ----------------------------------------------------------------------------
drop table quality;
create table quality (
  qlb_id  serial primary key,
  qlb_spec     varchar(16),
  qlb_type     varchar(16),
  qlb_ord      integer,
  qlb_key_de   varchar(32),
  qlb_desc_de  varchar(64), 
  qlb_val_from    float,
  qlb_val_to      float
);
comment on table quality is
  'Qualitaetsmerkmale im Bildsignal';

insert into quality (qlb_id,qlb_spec, qlb_type, qlb_ord, qlb_key_de, qlb_desc_de, qlb_val_from, qlb_val_to) values 
(0,'DEF', 'DBF',-1,'nicht definiert','nicht definiert', 'NAN', 'NAN');

insert into quality (qlb_spec, qlb_type, qlb_ord, qlb_key_de, qlb_desc_de, qlb_val_from, qlb_val_to) values 
('SIG', 'DBF',6,'nicht mehr erkennbar','Flächendeckendes Glare und Rauschen.', '-INFINITY', 0.0),
('SIG', 'DBF',5,'stark verrauscht','Bildstörungen inmehr als der Hälfte der Fläche.', 0.0, 0.25),
('SIG', 'DBF',4,'schlechte Sicht','Bildstörungen in mehr als ein Drittel der Fläche.'  , 0.25, 0.5),
('SIG', 'DBF',3,'mäßige Sicht','Es liegen Bildstörungen bis zu 30% vor.' , 0.50, 0.75),
('SIG', 'DBF',2,'gute Sicht', 'Es liegen Bildstöruneg vor aber weniger als 10%.', 0.75, 1.0),
('SIG', 'DBF',1,'klare Sicht','Es liegen keine Bildstörungen vor.', 1.0, 'INFINITY'),

('ART', 'DBF',6,'nicht bestimmbar','Objekt ist nicht erkennbar', '-INFINITY', 0.0),
('ART', 'DBF',5,'Vogel ohne Kennung','Objekt stellt einen Vogel oder einen Säuger ohne Artenkennung dar.', 0.0, 0.25),
('ART', 'DBF',4,'mehrdeutig bestimmbar','Keine Bestimmung auf Artenniveau.'  , 0.25, 0.5),
('ART', 'DBF',3,'bestimmbar','Art ist bestimmbar es liegen aber Unsicherheiten vor.' , 0.50, 0.75),
('ART', 'DBF',2,'gut bestimmbar','Art ist bestimmbar, eine Abstimmung ist erwünscht.', 0.75, 1.0),
('ART', 'DBF',1,'eindeutig bestimmbar','Art ist eindeutig bestimmbar.', 1.0, 'INFINITY'),

('BHV', 'DBF',6,'nicht bestimmbar','Das Verhalten kann nicht zugeordnet werden.', '-INFINITY', 0.0),
('BHV', 'DBF',5,'ohne eindeutiges Verhalten','Das Verhalten ist mehrdeutig. ', 0.0, 0.25),
('BHV', 'DBF',4,'mehrdeutig bestimmbar','Verhalten ist bestimmbar aber mehrdeutig und Unsicherheiten hinsichtlich der Einordnug liegen vor.'  , 0.25, 0.5),
('BHV', 'DBF',3,'bestimmbar','Das Verhalten ist eindeutig bestimmbar, es liegen Unsicherheiten vor.' , 0.50, 0.75),
('BHV', 'DBF',2,'gut bestimmbar','Das Verhalten ist bestimmbar, eine Abstimmung durch Kollegen ist erwünscht.', 0.75, 1.0),
('BHV', 'DBF',1,'eindeutig bestimmbar','Eindeutig bestimmbares Verhalten.', 1.0, 'INFINITY');

-- ----------------------------------------------------------------------------
-- Bildteile
-- ----------------------------------------------------------------------------
drop table image_tiles cascade;
create table image_tiles (
  itl_id    serial primary key,
  prj_ref   integer references projects(prj_id),
  ses_ref   integer references sessions(ses_id),
  sync_ref  integer,
  trc_ref   integer,
  cam       integer,
  cam_ref   varchar(64),
  sector    integer,
  qlb_ref   integer references quality(qlb_id),
  img_w     integer not null,
  img_h     integer not null,
  img_x     integer not null,
  img_y     integer not null,
  seen      integer default 0,
  uid       integer references users(uid),
  gid       integer references groups(gid)
);


-- ----------------------------------------------------------------------------
-- Zensus
-- ----------------------------------------------------------------------------
drop table census cascade;
create table census (
   cns_id     serial primary key,
   prj_ref    integer references projects(prj_id),
   ses_ref    integer references sessions(ses_id),
   itl_ref    integer references image_tiles(itl_id),
   sector     integer,
   sync_ref   integer,
   trc_ref    integer,
   cam        integer,
   cam_ref    varchar(64),
   tx_ref     integer references taxa(tx_id),
   bhv_ref    integer references behavior(bhv_id),
   qlb_ref    integer references quality(qlb_id),
   gnd_ref    integer references gender(gnd_id),
   img_x      integer not null,
   img_y      integer not null,
   wld_x      integer,
   wld_y      integer, 
   uid        integer references users(uid)
);


-- Abschliessen der Installation von Zwischenstaeden
\q
-- ============================================================================
-- SYNC der 32er Sector 
-- ============================================================================
DELETE FROM geometry_columns WHERE f_table_schema='public' AND f_table_name='sync_utm32';
DROP TABLE sync_utm32;

-- Tageshundertstel sind Tageszeitwerte innerhalb eine Monats mit
-- dhs: $dy*24*360000+$hr*360000+$mi*6000+$sc*100;

CREATE TABLE sync_utm32 (

  -- Primaerschluessel
  sync_id serial not null primary key,

  -- Status des Datensatzes
  status  varchar(33),

  -- Session ID z.B. 2014-04-19-GlobaltecI
  session varchar(64),

  -- Projketnummer 
  prj_id  int,

  -- Sequenzzaehler GPS
  gps_id  int,

  -- Transektnummer
  gps_trc int,

  -- Bildnummer des GPS
  gps_img int,

  -- GPS Datum YYYY-mm-dd
  gps_dt  date,

  -- GPS Zeit HH:MM:SS.DDD
  gps_tm  time,

  -- GPS Timestamp [dhs]
  gps_ts  int,

  -- Bildname 
  cam1_id varchar(64),

  -- Aufnahmedatum
  cam1_dt date,

  -- Aufnahmezeit fuer Bild (z.Z. Dateizeit)
  cam1_tm time,

  -- Zeitmarke [dhs]
  cam1_ts  int,

  -- Zeitmarke korrigiert Offset Rechneruhr [dhs]
  cam1_cts int,

  -- Differenz der korrigierten Zeitmarke zum GPS [dhs]
  cam1_dts int,

  -- Offset der Rechneruhr (AVG) [dhs]
  cam1_oclk int,

  -- Trennschaerfe im Intervall (STD) [dhs]
  cam1_tclk int,

  -- Konfiguration
  cam1_cfg varchar(4) default 'BB',

  -- Bildname
  cam2_id varchar(64),

  -- Aufnahmedatum
  cam2_dt date,

  -- Aufnahmezeit fuer Bild (z.Z. Dateizeit)
  cam2_tm time,

  -- Zeitmarke [dhs]
  cam2_ts  int,

  -- Zeitmarke korrigiert Offset Rechneruhr [dhs]
  cam2_cts int,

  -- Differenz der korrigierten Zeitmarke zum GPS [dhs]
  cam2_dts int,

  -- Offset der Rechneruhr (AVG) [dhs]
  cam2_oclk int,

  -- Trennschaerfe im Intervall (STD) [dhs]
  cam2_tclk int,

  -- Konfiguration
  cam2_cfg varchar(4) default 'SB',

  -- geographische Laenge 
  lon float8,

  -- geographische Breite
  lat float8,

  -- Position in UTM33
  x float8,  y float8,  z float8,
  
  -- Geschindigkeit und Kurs ueber Grund [km/h,Grad]
  sog float8,  cog float8,

  -- Rollwinkel Flugzeug und Stabi [Grad]
  roll_plane  float8,  roll_stabi  float8,

  -- Neigewinkel Flugzeug und Stabi [Grad]
  pitch_plane float8,  pitch_stabi float8,

  -- Heading Flugzeug und Stabi [Grad]
  head_plane  float8,  head_stabi  float8,
  
  -- Passpunkte Steuerboard Welkoordinaten
  pp_wx_sb text, pp_wy_sb text, 

  -- Passpunkte Backboard Weltkoordinaten
  pp_wx_bb text, pp_wy_bb text, 

  -- Passpunkte Steuerboard Bildkoordinaten
  pp_ix_sb text, pp_iy_sb text, 

  -- Passpunkte Backboard Bildkoordinaten
  pp_ix_bb text, pp_iy_bb text 
);

-- Eckpunkte Backboard NW, NO, SW, SO
select AddGeometryColumn('public','sync_utm32','geo_nw_bb',32632,'POINT',2);
select AddGeometryColumn('public','sync_utm32','geo_no_bb',32632,'POINT',2);
select AddGeometryColumn('public','sync_utm32','geo_sw_bb',32632,'POINT',2);
select AddGeometryColumn('public','sync_utm32','geo_so_bb',32632,'POINT',2);

-- Envelope Backboard 
select AddGeometryColumn('public','sync_utm32','geo_rc_bb',32632,'POLYGON',2);

-- Eckpunkte Steuerboard NW, NO, SW, SO
select AddGeometryColumn('public','sync_utm32','geo_nw_sb',32632,'POINT',2);
select AddGeometryColumn('public','sync_utm32','geo_no_sb',32632,'POINT',2);
select AddGeometryColumn('public','sync_utm32','geo_sw_sb',32632,'POINT',2);
select AddGeometryColumn('public','sync_utm32','geo_so_sb',32632,'POINT',2);

-- Envelope Steuerboard
select AddGeometryColumn('public','sync_utm32','geo_rc_sb',32632,'POLYGON',2);

-- Nadir fuer beide Kameras 
select AddGeometryColumn('public','sync_utm32','geo_ct',32632,'POINT',2);

-- Flugrichtung 
select AddGeometryColumn('public','sync_utm32','geo_dr',32632,'LINESTRING',2);

-- ----------------------------------------------------------------------------
-- Sync der 33er Sector 
-- ----------------------------------------------------------------------------
DELETE FROM geometry_columns WHERE f_table_schema='public' AND f_table_name='sync_utm33';
DROP TABLE sync_utm33;

CREATE TABLE sync_utm33 (

  -- Primaerschluessel
  sync_id serial not null primary key,

  -- Status des Datensatzes
  status  varchar(33),

  -- Session ID z.B. 2014-04-19-GT1
  session varchar(64),

  -- Projketnummer 
  prj_id  int,

  -- Sequenzzaehler GPS
  gps_id  int,

  -- Transektnummer
  gps_trc int,

  -- Bildnummer des GPS
  gps_img int,

  -- GPS Datum YYYY-mm-dd
  gps_dt  date,

  -- GPS Zeit HH:MM:SS.DDD
  gps_tm  time,

  -- GPS Timestamp [dhs]
  gps_ts  int,

  -- Bildname 
  cam1_id varchar(64),

  -- Aufnahmedatum
  cam1_dt date,

  -- Aufnahmezeit fuer Bild (z.Z. Dateizeit)
  cam1_tm time,

  -- Zeitmarke [dhs]
  cam1_ts  int,

  -- Zeitmarke korrigiert Offset Rechneruhr [dhs]
  cam1_cts int,

  -- Differenz der korrigierten Zeitmarke zum GPS [dhs]
  cam1_dts int,

  -- Offset der Rechneruhr (AVG) [dhs]
  cam1_oclk int,

  -- Trennschaerfe im Intervall (STD) [dhs]
  cam1_tclk int,

  -- Konfiguration
  cam1_cfg varchar(4) default 'BB',

  -- Bildname
  cam2_id varchar(64),

  -- Aufnahmedatum
  cam2_dt date,

  -- Aufnahmezeit fuer Bild (z.Z. Dateizeit)
  cam2_tm time,

  -- Zeitmarke [dhs]
  cam2_ts  int,

  -- Zeitmarke korrigiert Offset Rechneruhr [dhs]
  cam2_cts int,

  -- Differenz der korrigierten Zeitmarke zum GPS [dhs]
  cam2_dts int,

  -- Offset der Rechneruhr (AVG) [dhs]
  cam2_oclk int,

  -- Trennschaerfe im Intervall (STD) [dhs]
  cam2_tclk int,

  -- Konfiguration
  cam2_cfg varchar(4) default 'SB',

  -- geographische Laenge 
  lon float8,

  -- geographische Breite
  lat float8,

  -- Position in UTM33
  x float8,  y float8,  z float8,
  
  -- Geschindigkeit und Kurs ueber Grund [km/h,Grad]
  sog float8,  cog float8,

  -- Rollwinkel Flugzeug und Stabi [Grad]
  roll_plane  float8,  roll_stabi  float8,

  -- Neigewinkel Flugzeug und Stabi [Grad]
  pitch_plane float8,  pitch_stabi float8,

  -- Heading Flugzeug und Stabi [Grad]
  head_plane  float8,  head_stabi  float8,
  
  -- Passpunkte Steuerboard Welkoordinaten
  pp_wx_sb text, pp_wy_sb text, 

  -- Passpunkte Backboard Weltkoordinaten
  pp_wx_bb text, pp_wy_bb text, 

  -- Passpunkte Steuerboard Bildkoordinaten
  pp_ix_sb text, pp_iy_sb text, 

  -- Passpunkte Backboard Bildkoordinaten
  pp_ix_bb text, pp_iy_bb text 
);


-- Eckpunkte Backboard NW, NO, SW, SO
select AddGeometryColumn('public','sync_utm33','geo_nw_bb',32633,'POINT',2);
select AddGeometryColumn('public','sync_utm33','geo_no_bb',32633,'POINT',2);
select AddGeometryColumn('public','sync_utm33','geo_sw_bb',32633,'POINT',2);
select AddGeometryColumn('public','sync_utm33','geo_so_bb',32633,'POINT',2);

-- Envelope Backboard 
select AddGeometryColumn('public','sync_utm33','geo_rc_bb',32633,'POLYGON',2);

-- Eckpunkte Steuerboard NW, NO, SW, SO
select AddGeometryColumn('public','sync_utm33','geo_nw_sb',32633,'POINT',2);
select AddGeometryColumn('public','sync_utm33','geo_no_sb',32633,'POINT',2);
select AddGeometryColumn('public','sync_utm33','geo_sw_sb',32633,'POINT',2);
select AddGeometryColumn('public','sync_utm33','geo_so_sb',32633,'POINT',2);

-- Envelope Steuerboard
select AddGeometryColumn('public','sync_utm33','geo_rc_sb',32633,'POLYGON',2);

-- Nadir fuer beide Kameras 
select AddGeometryColumn('public','sync_utm33','geo_ct',32633,'POINT',2);

-- Flugrichtung 
select AddGeometryColumn('public','sync_utm33','geo_dr',32633,'LINESTRING',2);


-- -----------------------------------------------------------------------
-- Rechtevergabe der Tabellen 
-- -----------------------------------------------------------------------
alter table sync_utm32 owner to awe;
alter table sync_utm33 owner to awe;
