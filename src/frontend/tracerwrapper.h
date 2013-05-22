#ifndef FRONTEND_TRACERWRAPPER_H
#define FRONTEND_TRACERWRAPPER_H

#include <QImage>
#include <QSharedPointer>
	
class QPainter;
class Scene;

class TracerWrapper
{
public:
	TracerWrapper();
	~TracerWrapper();

	bool loadScene(const QString& fileName);
  void renderScene(int resolutionX, int resolutionY, int width, int height);
  void renderImage(QPainter* painter);
  void saveSceneImage(const QString& fileName);
  void setRecursionDepth(int depth);

private:
	QImage mTracerOutput,	mRenderImage;
	QSharedPointer< Scene > mScene;
	int	mTracerDepth;
};

#endif 
