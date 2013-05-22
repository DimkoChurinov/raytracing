#ifndef FRONTEND_SCENESERIALIZABLE_H
#define FRONTEND_SCENESERIALIZABLE_H

#include <QSharedPointer>

#include "ixmlserializable.h"

class Scene;

class SceneSerializable : private IXmlSerializable
{
public:
	explicit SceneSerializable();
	virtual ~SceneSerializable();
	QSharedPointer< Scene > readScene(const QString& fileName);
protected:
	virtual bool read(const QDomNode* node);
private:
	QSharedPointer< Scene > mScene;
};

#endif