#include <iostream>
#include <QCoreApplication>
#include <QFileInfo>
#include <QUrl>
#include "Frontend/tracerwrapper.h"

int raytracing(QString sceneFile, QString outputFile, int resX, int resY, int traceDepth);
int cmdRead(int argc, char *argv[], QString *sceneFile, QString *outputFile, int *resX, int *resY, int *traceDepth);

int main(int argc, char *argv[])
{
	QCoreApplication app(argc, argv);
	
  QString sceneFile, outputFile;
	int	resX = 0, resY = 0, traceDepth = -1;

	if (cmdRead(argc, argv, &sceneFile, &outputFile, &resX, &resY, &traceDepth))
    if (!raytracing(sceneFile, outputFile, resX, resY, traceDepth))
      return -1;

	return 0;
}

int cmdRead(int argc, char *argv[], QString *sceneFile, QString *outputFile, int *resX, int *resY, int *traceDepth)
{
  if (argc < 5)
	{
		std::cout << "example: rt.exe --scene=myScene.xml --resolution_x=1024 --resolution_y=768 --output=myImage.png"  << std::endl;
    return 0;  
  }
  else
  {
    for (int idx = 0; idx < argc; ++idx)
	  {
		  QString arg(argv[idx]);

		  if (arg.contains("--scene"))
		  {
			  *sceneFile = arg.remove("--scene=");
			  sceneFile->remove("\"");
		  }
		  else if (arg.contains("--resolution_y"))
		  {
			  *resY = arg.remove("--resolution_y=").toInt();
		  }
		  else if (arg.contains("--resolution_x"))
		  {
			  *resX = arg.remove("--resolution_x=").toInt();
		  }
		  else if (arg.contains("--output"))
		  {
			  *outputFile = arg.remove("--output=");
			  outputFile->remove("\"");
		  }
		  else if (arg.contains("--trace_depth"))
		  {
			  *traceDepth = arg.remove("--trace_depth=").toInt();
		  }
	  }
    return 1;
  }
  return 1;
}

int raytracing(QString sceneFile, QString outputFile, int resX, int resY, int traceDepth)
{
  TracerWrapper wrapper;
	wrapper.setRecursionDepth(traceDepth);

  // loading scene fron xml
	std::cout << "Scene loading..." << std::endl;
	if (!wrapper.loadScene(sceneFile)) 
		return 0;
	std::cout << "Scene loaded" << std::endl;

  // rendering scene
  std::cout << "Ray tracing start..." << std::endl;
	wrapper.renderScene(resX, resY, resX, resY);

  // saving render result into image file
	std::cout << "Saving result..." << std::endl;
	wrapper.saveSceneImage(outputFile);
	std::cout << "Image file get." << std::endl;
  
  std::cout << "Ray tracing complite=)" << std::endl;
  return 1;
}