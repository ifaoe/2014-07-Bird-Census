/*
 * layerstack.cpp
 *
 *  Created on: Oct 6, 2014
 *      Author: axel
 */

#include <layerstack.h>

LayerStack::LayerStack(QgsMapCanvas * canvas, QgsMapLayerRegistry* lyrRegistry) : qgsLyrRegistry(lyrRegistry), mapCanvas(canvas) {
	// TODO Auto-generated constructor stub

}

LayerStack::~LayerStack() {
	// TODO Auto-generated destructor stub
}

bool LayerStack::addMapLayer(QString layerName, QgsMapLayer * mapLayer, int priority) {
	if(lyrMap.contains(priority)) {
		qWarning("Warning addMapLayer: Layer with priority %d already exists.", priority);
		return false;
	} else if (priMap.contains(layerName)){
		qWarning("Warning addMapLayer: Layer with name %s already exists.", layerName.toStdString().c_str());
		return false;
	} else {
		if (priority == -1) {
			stdPriority += 10;
			priority = stdPriority;
		}
		lyrMap[priority] = mapLayer;
		priMap[layerName]= priority;
		qgsLyrRegistry->addMapLayer(mapLayer);
		qgsLyrRegistry->reloadAllLayers();
		refreshLayerSet();
		return true;
	}
}

bool LayerStack::removeMapLayer(QString layerName) {
	if(!priMap.contains(layerName)) {
		qWarning("Warning removeMapLayer: No layer with name %s found.", layerName.toStdString().c_str());
		return false;
	} else {
		int priority = priMap[layerName];
		priMap.remove(layerName);
		lyrMap.remove(priority);
		return true;
	}
}

QgsMapLayer * LayerStack::getMapLayer(QString layerName) {
	if (!priMap.contains(layerName)) {
		return NULL;
		qWarning("Warning getMapLayer: No layer named: %s found.", layerName.toStdString().c_str());
	}
	int priority = priMap[layerName];
	return lyrMap[priority];
}

void LayerStack::registerMapLayers() {
	QMap<int,QgsMapLayer*>::iterator i;
	for(i = lyrMap.begin(); i != lyrMap.end(); ++i) {
		qgsLyrRegistry->addMapLayer(i.value());
	}
	qgsLyrRegistry->reloadAllLayers();
}

bool LayerStack::setLayerPriority(QString layerName, int priority){
	if(lyrMap.contains(priority)) {
		qWarning("Warning setLayerPriority: Layer with priority %d already exists.", priority);
		return false;
	} else if (!priMap.contains(layerName)) {
		qWarning("Warning setLayerPriority: No layer with name %s found.", layerName.toStdString().c_str());
		return false;
	} else {
		// delete old mapping and create new one
		int oldPri = priMap[layerName];
		QgsMapLayer * temp = lyrMap[oldPri];
		priMap[layerName] = priority;
		lyrMap.remove(oldPri);
		lyrMap[priority] = temp;
		return true;
	}
}

void LayerStack::refreshLayerSet() {
	QMap<int,QgsMapLayer*>::iterator i;
	QList<QgsMapCanvasLayer> layerSet;
	for(i = lyrMap.begin(); i != lyrMap.end(); ++i) {
		layerSet.append(QgsMapCanvasLayer(i.value()));
	}
	mapCanvas->setLayerSet(layerSet);
}

QList<QString> LayerStack::getLayerNames() {
	return priMap.keys();
}
