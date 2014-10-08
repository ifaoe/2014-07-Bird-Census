/*
 * layerstack.h
 *
 *  Created on: Oct 6, 2014
 *      Author: axel
 */

#ifndef LAYERSTACK_H_
#define LAYERSTACK_H_

#include <QObject>
#include <qgsmapcanvas.h>
#include <qgsmaplayer.h>
#include <qgsmaplayerregistry.h>

class LayerStack: public QObject {
public:
	LayerStack(QgsMapCanvas * mapCanvas, QgsMapLayerRegistry* qgsLyrRegistry);
	virtual ~LayerStack();

	bool addMapLayer(QString layerName, QgsMapLayer *mapLayer, int priority = -1);
	bool removeMapLayer(QString layerName);
	bool setLayerPriority(QString layerName, int priority);
	QgsMapLayer * getMapLayer(QString layerName);
	QList<QString> getLayerNames();

private:
	void registerMapLayers();
	void refreshLayerSet();
	int stdPriority = 0;
	QgsMapLayerRegistry* qgsLyrRegistry = 0;
	QgsMapCanvas *mapCanvas;
	QMap<int,QgsMapLayer*> lyrMap;	// Sorted in ascending order by key: c++ standard
	QMap<QString,int> priMap;
};

#endif /* LAYERSTACK_H_ */
