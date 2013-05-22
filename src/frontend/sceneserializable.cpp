//-------------------------------------------------------------------
// File: sceneserializable.cpp
// 
// Qt based xml ray tracer scene serializable implementation
//
//  
//-------------------------------------------------------------------

#define _USE_MATH_DEFINES
#include <iostream>
#include <math.h>

#include <QFile>
#include <QImage>

#include "csg/csgtree.h"
#include "csg/csgdifference.h"
#include "csg/csgintersection.h"
#include "csg/csgunion.h"
#include "csg/csgvalue.h"

#include "geometry/bbox.h"
#include "geometry/box.h"
#include "geometry/cone.h"
#include "geometry/cylinder.h"
#include "geometry/model.h"
#include "geometry/modeltriangle.h"
#include "geometry/plane.h"
#include "geometry/smoothtriangle.h"
#include "geometry/sphere.h"
#include "geometry/triangle.h"
#include "geometry/torus.h"

#include "illumination/lightsource.h"
#include "illumination/material.h"

#include "tracer/camera.h"
#include "tracer/scene.h"
#include "tracer/tracerproperties.h"

#include "sceneserializable.h"

namespace
{
	std::ostream& GDumpErrorMessage(const QDomNode& node, const QDomNode& parent, const std::string& message)
	{
		QString nodeName("undefined");
		QString parentName("undefined");
		if (node.isElement())
			nodeName = node.toElement().tagName();
		if (parent.isElement())
			parentName = node.toElement().tagName();
		std::cerr << "Error reading scene at node: <" << nodeName.toUtf8().constData() << "> with parent: <" << parentName.toUtf8().constData() << ">." << std::endl;
		std::cerr << message << std::endl;
		return std::cerr;
	}

	std::string GLightSourceTypeToString(LightSourceType type)
	{
		switch (type)
		{
		case LIGHTSOURCE_POINT:
			return std::string("Point");
		case LIGHTSOURCE_DIRECTIONAL:
			return std::string("Diral");
		case LIGHTSOURCE_SPOT:
			return std::string("Spotlight");
		}
		return std::string();
	}

	// Local readers for different scene entities
	struct VectorReader : public IXmlSerializable
	{
		Vec3D readPosition(const QDomNode *node, bool* ok)
		{
			*ok = read(node);
			if (*ok)
			{
				return Vector;
			}
			return Vec3D();
		}

		virtual bool read(const QDomNode *node)
		{
			float x, y, z;
			bool ok = true;
			ok = readAttribute(*node, "x", x) && ok;
			ok = readAttribute(*node, "y", y) && ok;
			ok = readAttribute(*node, "z", z) && ok;
			Vector.setXYZ(x, y, z);

			return ok;
		}

		Vec3D Vector;
	};

	
	struct CameraReader : IXmlSerializable
	{

		virtual bool read(const QDomNode* node)
		{
			QDomNode readNode = node->firstChild();

			CameraProperties properties;

			// Read position
			bool ok = true;
			VectorReader reader;
			// <pos>
			properties.Eye = reader.readPosition(&readNode, &ok);
			if (!ok)
			{
				GDumpErrorMessage(readNode, *node, "Failed reading camera eye position!");
				return false;
			}
			readNode = readNode.nextSibling();
			// <up>
			properties.Up	 = reader.readPosition(&readNode, &ok);
			if (!ok)
			{
				GDumpErrorMessage(readNode, *node, "Failed reading camera up vector!");
				return false;
			}
			readNode = readNode.nextSibling();
			// <look_at>
			properties.At  = reader.readPosition(&readNode, &ok);
			if (!ok)
			{
				GDumpErrorMessage(readNode, *node, "Failed reading camera look at position!");
				return false;
			}
			readNode = readNode.nextSibling();
			// <fov>
			ok = readAttribute(readNode, "angle", properties.Fov) && ok;
			if (!ok)
			{
				GDumpErrorMessage(readNode, *node, "Failed reading camera field of view attribute!");
				return false;
			}
			readNode = readNode.nextSibling();
			// <dist_to_near_plane>
			ok = readAttribute(readNode, "dist",  properties.NearPlane) && ok;;
			if (!ok)
			{
				GDumpErrorMessage(readNode, *node, "Failed reading camera near plane!");
				return false;
			}
			readNode = readNode.nextSibling();
			// <use_exposure>
			QString boolValue;
			ok = readAttribute(readNode, "state",  boolValue) && ok;;
			if (!ok)
			{
				GDumpErrorMessage(readNode, *node, "Failed reading exposure usage state!");
				return false;
			}
			properties.UseExposure = (boolValue == "true");
			readNode = readNode.nextSibling();

			// <use_gamma_correction>
			ok = readAttribute(readNode, "state",  boolValue) && ok;;
			if (!ok)
			{
				GDumpErrorMessage(readNode, *node, "Failed reading gamma correction usage state!");
				return false;
			}
			properties.UseGammaCorrection = (boolValue == "true");
			
			if (ok)
			{
				properties.Up.normalize();
				Properties = properties;
			}

			return ok;
		}

		CameraProperties Properties;
	};


	struct LightReader : public IXmlSerializable
	{
		LightReader(LightSourceType type)
			: Source(NULL),
				LightType(type)
		{
		}

		virtual bool read(const QDomNode* node)
		{
			QDomNode readNode = node->firstChild();

			switch (LightType)
			{
			case LIGHTSOURCE_POINT:
				Source = new PointLightSource;
				break;
			case LIGHTSOURCE_DIRECTIONAL:
				Source = new DiralLightSource;
				break;
			case LIGHTSOURCE_SPOT:
				Source = new SpotLightSource;
				break;
			}
			

			bool ok = true;
			VectorReader reader;
			// <pos>
			Source->Position = reader.readPosition(&readNode, &ok);
			if (!ok)
			{
				
				GDumpErrorMessage(readNode, 
													*node, 
													std::string("Failed reading position of light source!") + 
													std::string("While reading light source of type:") + GLightSourceTypeToString(LightType));
				
				return false;
			}
			readNode = readNode.nextSibling();
			// <dir>
			Source->Dir = reader.readPosition(&readNode, &ok); 
			if (!ok)
			{
				GDumpErrorMessage(readNode, 
													*node, 
													std::string("Failed reading direction of light source!") + 
													std::string("While reading light source of type:") + GLightSourceTypeToString(LightType));
				return false;
			}
			readNode = readNode.nextSibling();
			// <ambient_emission>
			Source->AmbIntensity = reader.readPosition(&readNode, &ok);
			if (!ok)
			{
				GDumpErrorMessage(readNode, 
													*node, 
													std::string("Failed reading ambient emission of light source!") + 
													std::string("While reading light source of type:") + GLightSourceTypeToString(LightType));
				return false;
			}
			readNode = readNode.nextSibling();
			// <diffuse_emission>
			Source->DifIntensity = reader.readPosition(&readNode, &ok);
			if (!ok)
			{
				GDumpErrorMessage(readNode, 
													*node, 
													std::string("Failed reading diffuse emission of light source!") + 
													std::string("While reading light source of type:") + GLightSourceTypeToString(LightType));
				return false;
			}
			readNode = readNode.nextSibling();
			// <specular_intensity>
			Source->SpcIntensity = reader.readPosition(&readNode, &ok);
			if (!ok)
			{
				GDumpErrorMessage(readNode, 
													*node, 
													std::string("Failed reading specular intensity of light source!") + 
													std::string("While reading light source of type:") + GLightSourceTypeToString(LightType));
				return false;
			}
			readNode = readNode.nextSibling();
			// <attenuation>
			ok = readAttribute(readNode, "const",  Source->ConstantAttenutaion)  && ok;  
			ok = readAttribute(readNode, "linear", Source->LinearAttenutaion)		 && ok;	 
			ok = readAttribute(readNode, "quad",   Source->QuadraticAttenutaion) && ok; 

			if (!ok)
			{
				GDumpErrorMessage(readNode, 
													*node, 
													std::string("Failed reading attenuation coefficients of light source!") + 
													std::string("While reading light source of type:") + GLightSourceTypeToString(LightType));
				return false;
			}

			// If light source is spotlight or directional one - read additional properties
			if (LightType == LIGHTSOURCE_DIRECTIONAL)
			{
				// <range>
				readNode = readNode.nextSibling();
				ok = readAttribute(readNode, "value",  Source->LightRange) && ok;
				if (!ok)
				{
					GDumpErrorMessage(readNode, 
														*node, 
														std::string("Failed reading directional light range!") + 
														std::string("While reading light source of type:") + GLightSourceTypeToString(LightType));
					return false;
				}
			}
			if (LightType == LIGHTSOURCE_SPOT)
			{
				// <umbra>
				readNode = readNode.nextSibling();
				ok = readAttribute(readNode, "angle",  Source->UmbraAngle) && ok;
				if (!ok)
				{
					GDumpErrorMessage(readNode, 
														*node, 
														std::string("Failed reading spot light umbra angle!") + 
														std::string("While reading light source of type:") + GLightSourceTypeToString(LightType));
					return false;
				}
				Source->UmbraAngle = Source->UmbraAngle * M_PI / 180.f;
				// <penumbra>
				readNode = readNode.nextSibling();
				ok = readAttribute(readNode, "angle",  Source->PenumbraAngle) && ok;
				if (!ok)
				{
					GDumpErrorMessage(readNode, 
														*node, 
														std::string("Failed reading spot light penumbra angle!") + 
														std::string("While reading light source of type:") + GLightSourceTypeToString(LightType));
					return false;
				}
				Source->PenumbraAngle = Source->PenumbraAngle * M_PI / 180.f;
				// <falloff>
				readNode = readNode.nextSibling();
				ok = readAttribute(readNode, "value",  Source->SpotlightFalloff) && ok;

				if (!ok)
				{
					GDumpErrorMessage(readNode, 
														*node, 
														std::string("Failed reading spot light falloff value!") + 
														std::string("While reading light source of type:") + GLightSourceTypeToString(LightType));
					return false;
				}

				Source->CosHalfUmbraAngle		 = cosf(Source->UmbraAngle / 2.f);
				Source->CosHalfPenumbraAngle = cosf(Source->PenumbraAngle / 2.f);
			}

			Source->Dir.toUnit();			

			//static const float cRadius = 0.2f;
			//Mtrl* material = new Mtrl;
			//material->AmbColor  = Source->AmbIntensity;
			//material->DifColor  = Source->DifIntensity;
			//material->SpcColor = Source->SpcIntensity;
			//material->SpcPower = 1.f;
			//// Create sphere in the position of light source
			//LightSphere = new Sphere(Source->Position, cRadius, material);
			//LightSphere->setIsLight(true);

			return ok;
		}
			
		LightSourceType LightType;
		LightSource*		Source;
		Sphere*					LightSphere;
	};

	struct TextureReader
  {
    TextureReader()
      : ObjTexture(NULL)
    {

    }

    bool read(const QString& fileName)
    {
      QImage image(fileName);

      if (image.isNull())
      {
				std::cerr << "Image " << fileName.toUtf8().constData() << " not found!" << std::endl;
				
        return false;
      }

      QImage localFormatImage = image.convertToFormat(QImage::Format_ARGB32_Premultiplied);

      const int cWidth  = localFormatImage.width();
      const int cHeight = localFormatImage.height();

      Color *textureData = new Color[cWidth * cHeight];
      for (int y = 0; y < cHeight; ++y)
      {
        for (int x = 0; x < cWidth; ++x)
        {
          QRgb pixel = localFormatImage.pixel(x, y);
          float red   = qRed(pixel) * 1.f / 255.f;
          float green = qGreen(pixel) * 1.f / 255.f;
          float blue  = qBlue(pixel) * 1.f / 255.f;

          textureData[y * cWidth + x] = Color(red, green, blue);
        }
      }

      ObjTexture = new Texture(textureData, cWidth, cHeight);
			return true;
    }

    Texture* ObjTexture;
  };

	struct MtrlReader : public IXmlSerializable
	{
		MtrlReader()
			: ObjMtrl(NULL)
		{
		}

		virtual bool read(const QDomNode* node)
		{
			QDomNode readNode = node->firstChild();

			ObjMtrl = new Mtrl;

			bool ok = true;
			VectorReader reader;

			// <ambient>
			ObjMtrl->AmbColor = reader.readPosition(&readNode, &ok);
			if (!ok)
			{
				GDumpErrorMessage(readNode, *node, "Failed reading ambient color of the material!");
				return false;
			}
			readNode = readNode.nextSibling();
			// <diffuse>
			ObjMtrl->DifColor = reader.readPosition(&readNode, &ok);
			if (!ok)
			{
				GDumpErrorMessage(readNode, *node, "Failed reading diffuse color of the material!");
				return false;
			}
			readNode = readNode.nextSibling();
			// <specular>
			ObjMtrl->SpcColor = reader.readPosition(&readNode, &ok);
			if (!ok)
			{
				GDumpErrorMessage(readNode, *node, "Failed reading specular color of the material!");
				return false;
			}
			readNode = readNode.nextSibling();

			// Read specular power
			// <specular_power>
			ok = readAttribute(readNode, "power", ObjMtrl->SpcPower) && ok;
			if (!ok)
			{
				GDumpErrorMessage(readNode, *node, "Failed reading shininess of the material!");
				return false;
			}
			
			readNode = readNode.nextSibling();
			// Read refraction coefficients
			// <refraction_coeff>
			ok = readAttribute(readNode, "theta", ObjMtrl->Density) && ok;
			if (!ok)
			{
				GDumpErrorMessage(readNode, *node, "Failed reading density of the material!");
				return false;
			}

			readNode = readNode.nextSibling();
			// Read illumination coefficients
			// <illumination_factors>
			ok = readAttribute(readNode, "illumination_factor", ObjMtrl->Illumination) && ok;
			if (!ok)
			{
				GDumpErrorMessage(readNode, *node, "Failed reading illumination factor of the material!");
				return false;
			}
			ok = readAttribute(readNode, "reflection_factor",		ObjMtrl->Reflection) && ok;
			if (!ok)
			{
				GDumpErrorMessage(readNode, *node, "Failed reading reflection factor of the material!");
				return false;
			}
			ok = readAttribute(readNode, "refraction_factor",		ObjMtrl->Refraction) && ok;
			if (!ok)
			{
				GDumpErrorMessage(readNode, *node, "Failed reading refraction factor of the material!");
				return false;
			}

			readNode = readNode.nextSibling();
			// <texture>
			// Read texture, if name if given
			if (!readNode.isNull())
			{
				QString textureName;
				ok = readAttribute(readNode, "file_name", textureName);
				if (!ok)
				{
					GDumpErrorMessage(readNode, *node, "Failed reading material texture file name!");
					return false;
				}
				if (ok && !textureName.isEmpty())
				{
					TextureReader reader;
					if (reader.read(textureName))
					{
						ObjMtrl->DifTexture = reader.ObjTexture;
					}
				}

				readNode = readNode.nextSibling();
				// Read texture scales
				// <texscaleu>
				ok = readAttribute(readNode, "scale", ObjMtrl->TexScaleU) && ok;
				if (!ok)
				{
					GDumpErrorMessage(readNode, *node, "Failed reading texture U coordinate scale!");
					return false;
				}
				readNode = readNode.nextSibling();
				// <texscalev>
				ok = readAttribute(readNode, "scale", ObjMtrl->TexScaleV) && ok;
				if (!ok)
				{
					GDumpErrorMessage(readNode, *node, "Failed reading texture V coordinate scale!");
					return false;
				}
			}

			return ok;
		}

		Mtrl *ObjMtrl; 
	};

	struct SphereReader : public IXmlSerializable
	{
		SphereReader()
			: ObjSphere(NULL)
		{
		}

		virtual bool read(const QDomNode* node)
		{
			Vec3D center;
			float    radius;

			QDomNode readNode = node->firstChild();

			bool ok = true;
			VectorReader reader;
			// Read center
			// <center>
			center = reader.readPosition(&readNode, &ok);
			if (!ok)
			{
				GDumpErrorMessage(readNode, *node, "Failed reading sphere center!");
				return false;
			}
			readNode = readNode.nextSibling();
			// Read radius
			// <radius>
			ok = readAttribute(readNode, "r", radius) && ok; 
			if (!ok)
			{
				GDumpErrorMessage(readNode, *node, "Failed reading sphere radius!");
				return false;
			}
			readNode = readNode.nextSibling();
			// Read materials
			MtrlReader matReader;
			ok = matReader.read(&readNode);

			if (ok)
			{
				ObjSphere = new Sphere(center, radius, matReader.ObjMtrl);
			}
			else
			{
				GDumpErrorMessage(readNode, *node, "Failed reading sphere material!");
			}

			return ok;
		}

		Sphere* ObjSphere;
	};

	struct PlaneReader : public IXmlSerializable
	{
		PlaneReader() :
			ObjPlane(NULL)
		{
		}

		virtual bool read(const QDomNode* node)
		{
			Vec3D normal;
			float    distance;

			QDomNode readNode = node->firstChild();

			bool ok = true;
			VectorReader reader;
			// Read center
			// <normal>
			normal = reader.readPosition(&readNode, &ok); 
			if (!ok)
			{
				GDumpErrorMessage(readNode, *node, "Failed reading plane normal!");
				return false;
			}
			readNode = readNode.nextSibling();
			// Read distance
			// <D>
			ok = readAttribute(readNode, "d", distance) && ok; 
			if (!ok)
			{
				GDumpErrorMessage(readNode, *node, "Failed reading plane distance from origin!");
				return false;
			}
			readNode = readNode.nextSibling();
			// Read materials
			MtrlReader matReader;
			ok = matReader.read(&readNode);

			if (ok)
			{
				ObjPlane = new Plane(normal.toUnit(), distance, matReader.ObjMtrl);
			}
			else
			{
				GDumpErrorMessage(readNode, *node, "Failed reading plane material!");
			}

			return true;
		}

		Plane *ObjPlane;
	};

	struct TriangleReader : public IXmlSerializable
	{
		TriangleReader() :
			ObjTriangle(NULL)
		{
		}

		virtual bool read(const QDomNode* node)
		{
			Vec3D v0;
			Vec3D v1;
			Vec3D v2;

			QDomNode readNode = node->firstChild();

			bool ok = true;
			VectorReader reader;
			// Read vertices
			// <pos>
			v0 = reader.readPosition(&readNode, &ok);
			if (!ok)
			{
				GDumpErrorMessage(readNode, *node, "Failed reading first triangle vertex!");
				return false;
			}
			readNode = readNode.nextSibling();
			// <pos>
			v1 = reader.readPosition(&readNode, &ok);
			if (!ok)
			{
				GDumpErrorMessage(readNode, *node, "Failed reading second triangle vertex!");
				return false;
			}
			readNode = readNode.nextSibling();
			// <pos>
			v2 = reader.readPosition(&readNode, &ok); 
			if (!ok)
			{
				GDumpErrorMessage(readNode, *node, "Failed reading third triangle vertex!");
				return false;
			}
			readNode = readNode.nextSibling();
			// Read materials
			MtrlReader matReader;
			ok = matReader.read(&readNode);

			if (ok)
			{
				ObjTriangle = new Triangle(v0, v1, v2, matReader.ObjMtrl);
			}
			else
			{
				GDumpErrorMessage(readNode, *node, "Failed reading triangle material!");
			}

			return true;
		}

		Triangle *ObjTriangle;
	};

	struct CylinderReader : public IXmlSerializable
	{
		CylinderReader()
			: ObjCylinder(NULL)
		{
		}

		virtual bool read(const QDomNode* node)
		{
			Vec3D top;
			Vec3D bottom;
			float    radius;

			QDomNode readNode = node->firstChild();

			bool ok = true;
			VectorReader reader;
			// Read top pnt
			// <top>
			top = reader.readPosition(&readNode, &ok); 
			if (!ok)
			{
				GDumpErrorMessage(readNode, *node, "Failed reading cylinder top pnt!");
				return false;
			}
			readNode = readNode.nextSibling();
			// Read bottom pnt
			// <bottom>
			bottom = reader.readPosition(&readNode, &ok); 
			if (!ok)
			{
				GDumpErrorMessage(readNode, *node, "Failed reading cylinder bottom pnt!");
				return false;
			}
			readNode = readNode.nextSibling();
			// Read radius
			// <radius>
			ok = readAttribute(readNode, "r", radius) && ok; 
			if (!ok)
			{
				GDumpErrorMessage(readNode, *node, "Failed reading cylinder radius!");
				return false;
			}
			readNode = readNode.nextSibling();
			// Read materials
			MtrlReader matReader;
			ok = matReader.read(&readNode);

			if (ok)
			{
				ObjCylinder = new Cylinder(top, bottom, radius, matReader.ObjMtrl);
			}
			else
			{
				GDumpErrorMessage(readNode, *node, "Failed reading cylinder material!");
			}

			return ok;
		}

		Cylinder* ObjCylinder;
	};

	struct ConeReader : public IXmlSerializable
	{
		ConeReader()
			: ObjCone(NULL)
		{
		}

		virtual bool read(const QDomNode* node)
		{
			Vec3D top;
			Vec3D bottom;
			float    radius;

			QDomNode readNode = node->firstChild();

			bool ok = true;
			VectorReader reader;
			// Read top pnt
			// <top>
			top = reader.readPosition(&readNode, &ok); 
			if (!ok)
			{
				GDumpErrorMessage(readNode, *node, "Failed reading cone top pnt!");
				return false;
			}
			readNode = readNode.nextSibling();
			// Read bottom pnt
			// <bottom>
			bottom = reader.readPosition(&readNode, &ok); 
			if (!ok)
			{
				GDumpErrorMessage(readNode, *node, "Failed reading cone bottom pnt!");
				return false;
			}
			readNode = readNode.nextSibling();
			// Read radius
			// <radius>
			ok = readAttribute(readNode, "r", radius) && ok; 
			if (!ok)
			{
				GDumpErrorMessage(readNode, *node, "Failed reading cone radius!");
				return false;
			}
			readNode = readNode.nextSibling();
			// Read materials
			MtrlReader matReader;
			ok = matReader.read(&readNode);

			if (ok)
			{
				ObjCone = new Cone(top, bottom, radius, matReader.ObjMtrl);
			}
			else
			{
				GDumpErrorMessage(readNode, *node, "Failed reading cone material!");
			}

			return ok;
		}

		Cone* ObjCone;
	};

	struct TorusReader : public IXmlSerializable
	{
		TorusReader()
			: ObjTorus(NULL)
		{
		}

		virtual bool read(const QDomNode* node)
		{
			Vec3D center;
			Vec3D axis;
			float    innerRadius;
			float    outerRadius;

			QDomNode readNode = node->firstChild();

			bool ok = true;
			VectorReader reader;
			// Read center pnt
			// <center>
			center = reader.readPosition(&readNode, &ok); 
			if (!ok)
			{
				GDumpErrorMessage(readNode, *node, "Failed reading torus center pnt!");
				return false;
			}
			readNode = readNode.nextSibling();
			// Read axis
			// <axis>
			axis = reader.readPosition(&readNode, &ok);
			if (!ok)
			{
				GDumpErrorMessage(readNode, *node, "Failed reading torus axis!");
				return false;
			}
			readNode = readNode.nextSibling();
			axis.toUnit();
			// Read inner radius
			// <inner_radius>
			ok = readAttribute(readNode, "r", innerRadius) && ok; 
			if (!ok)
			{
				GDumpErrorMessage(readNode, *node, "Failed reading torus inner radius!");
				return false;
			}
			readNode = readNode.nextSibling();
			// Read outer radius
			// <outer_radius>
			ok = readAttribute(readNode, "r", outerRadius) && ok; 
			if (!ok)
			{
				GDumpErrorMessage(readNode, *node, "Failed reading torus outer radius!");
				return false;
			}
			readNode = readNode.nextSibling();
			// Read materials
			MtrlReader matReader;
			ok = matReader.read(&readNode);

			if (ok)
			{
				ObjTorus = new Torus(center, axis, innerRadius, outerRadius, matReader.ObjMtrl);
			}
			else
			{
				GDumpErrorMessage(readNode, *node, "Failed reading torus material!");
			}

			return ok;
		}

		Torus* ObjTorus;
	};

	struct BoxReader : public IXmlSerializable
	{
		BoxReader()
			: ObjBox(NULL)
		{
		}

		virtual bool read(const QDomNode* node)
		{
			Vec3D min;
			Vec3D max;

			QDomNode readNode = node->firstChild();

			bool ok = true;
			VectorReader reader;
			// Read min pnt
			// <min>
			min = reader.readPosition(&readNode, &ok); 
			if (!ok)
			{
				GDumpErrorMessage(readNode, *node, "Failed reading box min pnt!");
				return false;
			}
			readNode = readNode.nextSibling();
			// Read max pnt
			// <max>
			max = reader.readPosition(&readNode, &ok); 
			if (!ok)
			{
				GDumpErrorMessage(readNode, *node, "Failed reading box max pnt!");
				return false;
			}
			readNode = readNode.nextSibling();
			// Read materials
			MtrlReader matReader;
			ok = matReader.read(&readNode);

			if (ok)
			{
				ObjBox = new Box(min, max, matReader.ObjMtrl);
			}
			else
			{
				GDumpErrorMessage(readNode, *node, "Failed reading box material!");
			}

			return ok;
		}

		Box* ObjBox;
	};

	class ObjLoader
	{
		struct Vertex 
		{
			Vec3D Position;
			Vec3D Normal;
			Vec3D TexCoords; // Actually 2 dimensional
		};

	public:
		ObjLoader(const Vec3D& translation, const Vec3D& scale)
			: mTranslation(translation),
				mScale(scale)
		{
		}

		bool read(const QString& fileName)
		{
			// Read obj file and construct triangles
			QFile modelFile(fileName);

			modelFile.open(QIODevice::ReadOnly);
			if (!modelFile.isOpen())
			{
				std::cerr << "Failed loading model file: " << fileName.toUtf8().constData() << std::endl;
				return false;
			}

			std::vector<Vec3D> positions;
			std::vector<Vec3D> normals;
			std::vector<Vec3D> texCoords;

			std::vector<Vertex>	  faceVertices;
			std::vector<unsigned> faceVerticesIndices;
			std::vector<unsigned> faceIndices;

			// Result indices for triangles construction
			std::vector< unsigned > indices;
			// Result vertices for triangles construction
			std::vector< Vertex >		vertices;

			for(;;)
			{
				QString command = modelFile.readLine();
				if (modelFile.atEnd())
				{
					break;
				}

				if (command.contains("#"))
				{
					// Comment
				}
				else if (command.startsWith("v "))
				{
					// Vertex
					// Also apply translation
					Vec3D modelPosition = scale3D(fromObjLine(command, "v"), mScale) + mTranslation;
					positions.push_back(modelPosition);
				}
				else if (command.startsWith("vt "))
				{
					// Texcoords
					texCoords.push_back(fromObjLine(command, "vt"));
				}
				else if (command.startsWith("vn "))
				{
					// Normals
					normals.push_back(fromObjLine(command, "vn"));
				}
				else if (command.startsWith("f "))
				{
					faceVertices.clear();
					faceVerticesIndices.clear();

					QStringList indicesDescs = toIndicesDescriptor(command, "f");

					foreach (const QString& index, indicesDescs)
					{
						unsigned position, texcoord, normal;
						Vertex v;

						toIndices(index, &position, &normal, &texcoord);

						// OBJ uses 1-based arrays
						if (!positions.empty())
							v.Position  = positions[position - 1];
						if (!normals.empty())
							v.Normal    = normals[normal - 1];
						if (!texCoords.empty())
							v.TexCoords = texCoords[texcoord - 1];

						faceVertices.push_back(v);
						faceVerticesIndices.push_back(position);
					}
					const unsigned count = faceVertices.size();
					faceIndices.resize(count);
					for (unsigned idx = 0; idx < count; ++idx)
					{
						// Triangle strip
						if (idx > 2)
						{
							indices.push_back( faceIndices[0] );
							indices.push_back( faceIndices[idx - 1] );
						}
						faceIndices[idx] = vertices.size();
						vertices.push_back(faceVertices[idx]);
						indices.push_back(faceIndices[idx]);
					}
				}

				// Also commands can be "mtlib" and "usemtl", but we do not support them
			}

			// Now create triangles, we need only positions, maybe for now
			for (int idx = 0, count = indices.size(); idx < count; idx += 3)
			{
				Vertex a = vertices[indices[idx]];
				Vertex b = vertices[indices[idx + 1]];
				Vertex c = vertices[indices[idx + 2]];

				// Mtrl isn't assigned to triangle, it's owned by the model
				mTriangles.push_back(new ModelTriangle(a.Position,  b.Position, c.Position, 
																							 a.Normal,		b.Normal,		c.Normal,
																							 a.TexCoords,	b.TexCoords,c.TexCoords,
																							 NULL));
			}

			// Analyze positions to create bounding box
			Vec3D bboxMin;
			Vec3D bboxMax;
			for (std::vector< Vec3D >::iterator pos = positions.begin(); pos != positions.end(); ++pos)
			{
				const Vec3D& position = *pos;

				// Scan for min
				if (position.x() < bboxMin.x())
					bboxMin.setX(position.x());
				if (position.y() < bboxMin.y())
					bboxMin.setY(position.y());
				if (position.z() < bboxMin.z())
					bboxMin.setZ(position.z());

				// Scan for max
				if (position.x() > bboxMax.x())
					bboxMax.setX(position.x());
				if (position.y() > bboxMax.y())
					bboxMax.setY(position.y());
				if (position.z() > bboxMax.z())
					bboxMax.setZ(position.z());
			}

			mModelBBox.Min = bboxMin;
			mModelBBox.Max = bboxMax;
			return true;
		}

		const std::vector< ModelTriangle* > getTriangles() const
		{
			return mTriangles;
		}

		const BBox& getBoundingBox() const
		{
			return mModelBBox;
		}

	private:
		Vec3D fromObjLine(QString line /* Will be modified */, const QString& prefix)
		{
			QString			vectorString = line.remove(prefix + " ").remove("\n").remove("\r");
			QStringList coords			 = vectorString.split(" ", QString::SkipEmptyParts);
			// Remove spaces
			coords.removeAll(" ");
			coords.removeAll("");
			coords.removeAll("\n");
			if (coords.size() == 3)
				return Vec3D(coords[0].toFloat(), coords[1].toFloat(), coords[2].toFloat());

			// For texcoords
			return Vec3D(coords[0].toFloat(), coords[1].toFloat(), 0.f);
		}

		QStringList toIndicesDescriptor(QString line, const QString& prefix)
		{
			QString			 indicesString = line.remove(prefix + " ");
			QStringList	 descs = indicesString.split(" ");
			return descs;
		}

		void toIndices(const QString& line, unsigned *position, unsigned *normal, unsigned *texcoord)
		{
			QStringList indices = line.split("/");
			*position = indices[0].toUInt();
			*texcoord = indices[1].toUInt(); // Texcoord is second 
			*normal   = indices[2].toUInt();
		}

	private:
		// Translation of model vertices
		Vec3D											mTranslation;
		// Scaling of model vertices
		Vec3D											mScale;

		std::vector< ModelTriangle* > mTriangles;
		BBox													mModelBBox;
	};

	struct ModelLoader : public IXmlSerializable
	{
		ModelLoader() :
			ObjModel(NULL)
		{
		}

		virtual bool read(const QDomNode* node)
		{
			Vec3D translate;
			Vec3D scale;
			QString  fileName;
			QDomNode readNode = node->firstChild();

			
			bool ok = true;
			VectorReader reader;
			// Read translation vector
			// <translate>
			translate = reader.readPosition(&readNode, &ok);
			if (!ok)
			{
				GDumpErrorMessage(readNode, *node, "Failed reading model translation vector!");
				return false;
			}
			readNode = readNode.nextSibling();
			// Read scaling vector
			scale = reader.readPosition(&readNode, &ok); 
			if (!ok)
			{
				GDumpErrorMessage(readNode, *node, "Failed reading model scaling vector!");
				return false;
			}
			readNode = readNode.nextSibling();
			// Read model file name
			// <model>
			ok = readAttribute(readNode, "file_name", fileName);

			if (!ok)
			{
				GDumpErrorMessage(readNode, *node, "Failed reading model file name!");
				return false;
			}

			// Try to find explicitly set material
			Mtrl* modelMtrl = NULL;
			readNode = readNode.nextSibling();

			if (!readNode.isNull())
			{
				MtrlReader reader;
				if (!reader.read(&readNode))
				{
					return false;
				}
				else
				{
					GDumpErrorMessage(readNode, *node, "Failed reading model material!");
					return false;
				}
				modelMtrl = reader.ObjMtrl;
			}

			// Read model
			ObjLoader modelLoader(translate, scale);
			if (!modelLoader.read(fileName))
			{
				return false;
			}
			else
			{
				GDumpErrorMessage(readNode, *node, "Failed reading model!");
				return false;
			}

			ObjModel = new Model(modelLoader.getTriangles(), modelLoader.getBoundingBox(), modelMtrl);

			return ok;
		}

		Model* ObjModel;
	};

	struct CSGValueReader : public IXmlSerializable
	{
		CSGValueReader()
			: Value(NULL)
		{

		}

		virtual bool read(const QDomNode* node)
		{
			// Value is one of the existing scene objects
			QDomNode readNode = node->firstChild();

			QDomElement element = readNode.toElement();

			IShape* shape = NULL;
			QString type;
			bool ok = readAttribute(element, "type", type);

			if (!ok)
			{
				GDumpErrorMessage(readNode, *node, "Failed reading CSG value type!");
				return false;
			}
			
			// Another awful branching
			if (type == "sphere")
			{
				SphereReader reader;
				if (!reader.read(&element))
				{
					GDumpErrorMessage(readNode, *node, "Failed reading CSG sphere value!");
					return false;
				}

				shape = reader.ObjSphere;
			}
			else if (type == "plane")
			{
				PlaneReader reader;
				if (!reader.read(&element))
				{
					GDumpErrorMessage(readNode, *node, "Failed reading CSG plane value!");
					return false;
				}
				shape = reader.ObjPlane;
			}
			else if (type == "triangle")
			{
				TriangleReader reader;
				if (!reader.read(&element))
				{
					GDumpErrorMessage(readNode, *node, "Failed reading CSG triangle value!");
					return false;
				}
				shape = reader.ObjTriangle;
			}
			else if (type == "cylinder")
			{
				CylinderReader reader;
				if (!reader.read(&element))
				{
					GDumpErrorMessage(readNode, *node, "Failed reading CSG cylinder value!");
					return false;
				}
				shape = reader.ObjCylinder;
			}
			else if (type == "cone")
			{
				ConeReader reader;
				if (!reader.read(&element))
				{
					GDumpErrorMessage(readNode, *node, "Failed reading CSG cone value!");
					return false;
				}
				shape = reader.ObjCone;
			}
			else if (type == "torus")
			{
				TorusReader reader;
				if (!reader.read(&element))
				{
					GDumpErrorMessage(readNode, *node, "Failed reading CSG torus value!");
					return false;
				}
				shape = reader.ObjTorus;
			}
			else if (type == "box")
			{
				BoxReader reader;
				if (!reader.read(&element))
				{
					GDumpErrorMessage(readNode, *node, "Failed reading CSG box value!");
					return false;
				}
				shape = reader.ObjBox;
			}
			else if (type == "model")
			{
				ModelLoader reader;
				if (!reader.read(&element))
				{
					GDumpErrorMessage(readNode, *node, "Failed reading CSG model value!");
					return false;
				}
				shape = reader.ObjModel;
			}

			Value = new CSGValue(shape);

			return true;
		}


		CSGNode* Value;
	};

	struct CSGOperandReader : public IXmlSerializable
	{
		CSGOperandReader()
			: Operand(NULL)
		{

		}

		virtual bool read(const QDomNode* node);

		CSGNode* Operand;
	};

	struct CSGOperationReader : public IXmlSerializable
	{
		CSGOperationReader()
			: Operation(NULL)
		{
		}

		virtual bool read(const QDomNode* node);

		CSGOperation* Operation;
	};

	// Operation and operand readers are dependent
	bool CSGOperandReader::read(const QDomNode* node)
	{
		QDomNode readNode = node->firstChild();

		QDomElement element = readNode.toElement();

		// Read operation or value
		if (element.tagName() == "value")
		{
			CSGValueReader reader;
			if (!reader.read(&readNode))
				return false;
			Operand = reader.Value;
		}
		else if (element.tagName() == "operation")
		{
			CSGOperationReader reader;
			if (!reader.read(&readNode))
				return false;
			Operand = reader.Operation;
		}
		else
		{
			return false;
		}

		return true;
	}

	bool CSGOperationReader::read(const QDomNode* node)
	{
		bool ok = true;

		QString operationType;
		// Read type of the operation
		ok = readAttribute(*node, "type", operationType);

		// Read operands
		QDomNode readNode = node->firstChild();

		CSGNode *lHand  = NULL;
		CSGNode *rHand = NULL;

		CSGOperandReader reader;

		if (!reader.read(&readNode))
		{
			GDumpErrorMessage(readNode, *node, std::string("Failed reading CSG operation l operand! ") + std::string("Operation type") + operationType.toUtf8().constData());
			return false;
		}

		lHand = reader.Operand;

		readNode = readNode.nextSibling();

		if (!reader.read(&readNode))
		{
			GDumpErrorMessage(readNode, *node, std::string("Failed reading CSG operation l operand! ") + std::string("Operation type") + operationType.toUtf8().constData());
			return false;
		}

		rHand = reader.Operand;

		if (!lHand || !rHand)
		{
			GDumpErrorMessage(readNode, *node, std::string("Failed reading CSG operation l operand! ") + std::string("Operation type") + operationType.toUtf8().constData());
			return false;
		}


		if (operationType == "union")
		{
			Operation = new CSGUnion(lHand, rHand);
		}
		else if (operationType == "intersection")
		{
			Operation = new CSGCIsect(lHand, rHand);
		}
		else if (operationType == "difference")
		{
			Operation = new CSGDifference(lHand, rHand);
		}
		else
		{
			GDumpErrorMessage(readNode, *node, std::string("Failed reading CSG operation l operand! ") + 
																				 std::string("Operation type  ") + operationType.toUtf8().constData() + std::string(" isn't supported!"));
			return false;
		}

		return true;
	}

	//--------------------------------------------

	struct CSGTreeReader : public IXmlSerializable
	{
		CSGTreeReader()
			: Tree(NULL)
		{

		}

		virtual bool read(const QDomNode* node)
		{
			// Start reading from root
			QDomNode readNode = node->firstChild();

			QDomElement element = readNode.toElement();

			if (element.isNull())
			{
				return false;
			}

			if (element.tagName() == "operation")
			{
				CSGOperationReader reader;

				if (!reader.read(&readNode))
				{
					GDumpErrorMessage(readNode, *node, "Failed reading CSG tree!");
					return false;
				}
				Tree = new CSGTree(reader.Operation);
			}
			else if (element.tagName() == "value") // Tree contains only one object
			{
				CSGValueReader reader;
				if (!reader.read(&readNode))
				{
					GDumpErrorMessage(readNode, *node, "Failed reading CSG tree!");
					return false;
				}
				Tree = new CSGTree(reader.Value);
			}
			else
			{
				GDumpErrorMessage(readNode, *node, "CSG tree must have operation or value as root node!");
				return false;
			}


			return true;
		}

		CSGTree* Tree;
	};
}

SceneSerializable::SceneSerializable()
{
}

SceneSerializable::~SceneSerializable()
{
}

QSharedPointer< Scene > SceneSerializable::readScene(const QString& fileName)
{
	mScene = QSharedPointer< Scene >();

	QFile sceneFile(fileName);

	sceneFile.open(QIODevice::ReadOnly);
	if (!sceneFile.isOpen())
	{
		std::cerr << "Can't open scene file " << fileName.toUtf8().constData() << std::endl;
		return mScene;
	}

	QDomDocument document;

	if (!document.setContent(&sceneFile))
	{
		std::cerr << "Malformed scene file! Please, check XML syntax!" << std::endl;
		return mScene;
	}

	// Read xml document
	QDomElement docElem = document.documentElement();

	if (!read(&docElem))
	{
		std::cerr << "Scene reading failed!" << std::endl;
		mScene = QSharedPointer< Scene >();
		return mScene;
	}

	return mScene;
}

bool SceneSerializable::read(const QDomNode* node)
{
	mScene = QSharedPointer< Scene >(new Scene);
	// We must be in <scene> node, so start reading from first child
	QDomNode currentNode = node->firstChild();

	while (!currentNode.isNull())
	{
		QDomElement element = currentNode.toElement();
		const QString& tag  = element.tagName();
		// Awful if branching
		if (tag == "camera")
		{
			CameraReader reader;
			if (!reader.read(&element))
				return false;
			mScene->setupCamera(reader.Properties);
		}
		else if (tag == "light")
		{
			QString type;
			bool ok = readAttribute(element, "type", type);
			LightSourceType lightType;
			if (type == "pnt")
			{
				lightType = LIGHTSOURCE_POINT;
			}
			else if (type == "directional")
			{
				lightType = LIGHTSOURCE_DIRECTIONAL;
			}
			else if (type == "spotlight")
			{
				lightType = LIGHTSOURCE_SPOT;
			}
			LightReader reader(lightType);
			if (!reader.read(&element))
				return false;
			mScene->addLightSource(reader.Source);
			//mScene->addObject(reader.LightSphere);
		}
		else if (tag == "object")
		{
			QString type;
			bool ok = readAttribute(element, "type", type);
			// Another awful branching
			if (type == "sphere")
			{
				SphereReader reader;
				if (!reader.read(&element))
					return false;
				mScene->addObject(reader.ObjSphere);
			}
			else if (type == "plane")
			{
				PlaneReader reader;
				if (!reader.read(&element))
					return false;
				mScene->addObject(reader.ObjPlane);
			}
			else if (type == "triangle")
			{
				TriangleReader reader;
				if (!reader.read(&element))
					return false;
				mScene->addObject(reader.ObjTriangle);
			}
			else if (type == "cylinder")
			{
				CylinderReader reader;
				if (!reader.read(&element))
					return false;
				mScene->addObject(reader.ObjCylinder);
			}
			else if (type == "cone")
			{
				ConeReader reader;
				if (!reader.read(&element))
					return false;
				mScene->addObject(reader.ObjCone);
			}
			else if (type == "torus")
			{
				TorusReader reader;
				if (!reader.read(&element))
					return false;
				mScene->addObject(reader.ObjTorus);
			}
			else if (type == "box")
			{
				BoxReader reader;
				if (!reader.read(&element))
					return false;
				mScene->addObject(reader.ObjBox);
			}
			else if (type == "model")
			{
				ModelLoader reader;
				if (!reader.read(&element))
					return false;
				mScene->addObject(reader.ObjModel);
			}
		}
		else if (tag == "csg")
		{
			CSGTreeReader reader;
			if (!reader.read(&element))
				return false;
			mScene->addObject(reader.Tree);
		}
		else if (tag == "background")
		{
			MtrlReader reader;
			if (!reader.read(&(element.firstChild())))
					return false;
			mScene->setBackground(reader.ObjMtrl);
		}

		currentNode = currentNode.nextSibling();
	}

	// TODO:
	// Read them from somewhere
	TracerProperties* props = new TracerProperties;
	props->MaxRayRecursionDepth  = 10;
	props->MaxRayReflectionDepth = 10;
	mScene->setTracerProperties(props);
	return true;
}
