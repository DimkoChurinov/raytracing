#ifndef FRONTEND_IXMLSERIALIZABLE_H
#define FRONTEND_IXMLSERIALIZABLE_H

#include <QDomDocument>
#include <QDomElement>
#include <QDomNode>
#include <QMap>
#include <QString>
#include <QStringList>
#include <QVector>

struct IXmlSerializable
{
  static QDomNode findNode(const QDomNode& parent, const QString& name);
  template <class Subclass, class T>
  static void forEach(const Subclass& object, T& visitor)
  {
  	const Subclass::VisitEntities& entities = object.getEntities();

	  foreach (const Subclass::VisitEntity& entity, entities)
	  	visitor(entity);
  }
  template <class T>
  static bool readValue(const QDomNode& node, T& value);

  template <class T>
  static bool readAttribute(const QDomNode& node, const QString& attrName, T& value);

	
  template <class T>
  static bool readArray(const QDomNode& node, QVector< T >& value);

  template <class T>
  static bool readSiblingArray(const QDomNode& node, QVector< T >& value);

  template <class T>
  static bool convert(const QDomNode& node, T& elem);

  template <class T>
  static bool convert(const QString& string, T& elem);

 
  template <class K, class T>
  static bool readMap(const QDomNode& node, const QString& keyTag, const QString& valueTag, QMap<K, T>& map);

  static QDomNode addNode(QDomNode& parent, const QDomNode& node)
  {
  	return parent.appendChild(node);
  }

  static QDomNode addElement(QDomNode& parent, const QDomElement& element)
  {
  	return addNode(parent, element);
  }

  template <typename T>
  static bool writeNodeValue(QDomDocument& document, QDomNode& node, const T& value);

  template <typename T>
  static QDomElement addElement(QDomDocument& document, QDomNode& parent, const QString& tag, const T& value);

  static QDomElement addElement(QDomDocument& document, QDomNode& parent, const QString& tag)
  {
  	QDomElement deNewChild = document.createElement(tag);
  	return addElement(parent, deNewChild).toElement();
  }

  template <typename T>
  static bool addAttribute(QDomElement& node, const QString& name, const T& value);

  template <typename T>
  static bool convert(QDomDocument& document, QDomElement& node, const T& element);

  template <typename K, typename T>
  static bool writeMap(QDomDocument& document, QDomNode& node, const QString& mapTag, const QString& keyName, const QString& valueName, const QMap< K, T >& map);

  template <typename T>
  static bool writeContainer(QDomDocument& document, QDomNode& node, const QString& tagName, const QVector< T >& elements);

  virtual ~IXmlSerializable()
  {
  }

  virtual bool read(const QDomNode* node) = 0;
};

template <>
inline bool IXmlSerializable::readValue(const QDomNode& node, long& value)
{
	if (node.isNull()) 
    return false;
	value = node.nodeValue().toLong();
	return true;
}

template <>
inline bool IXmlSerializable::readValue(const QDomNode& node, QString& value)
{
	if (node.isNull()) 
    return false;
	value = node.nodeValue();
	return true;
}

template <>
inline bool IXmlSerializable::readAttribute(const QDomNode& node, const QString& attrName, int& value)
{
	QDomNamedNodeMap attributes = node.attributes();
	QDomNode item = attributes.namedItem(attrName);

	if (!item.isNull())
	{
		value = item.nodeValue().toInt();
		return true;
	}

	return false;
}

template <>
inline bool IXmlSerializable::readAttribute(const QDomNode& node, const QString& attrName, qint64& value)
{
	QDomNamedNodeMap attributes = node.attributes();
	QDomNode item = attributes.namedItem(attrName);
	if (!item.isNull())
	{
		value = item.nodeValue().toLongLong();
		return true;
	}

	return false;
}

template <>
inline bool IXmlSerializable::readAttribute(const QDomNode& node, const QString& attrName, uint& value)
{
	QDomNamedNodeMap attributes = node.attributes();

	QDomNode item = attributes.namedItem(attrName);

	if (!item.isNull())
	{
		value = item.nodeValue().toUInt();
		return true;
	}

	return false;
}

template <>
inline bool IXmlSerializable::readAttribute(const QDomNode& node, const QString& attrName, float& value)
{
	QDomNamedNodeMap attributes = node.attributes();

	QDomNode item = attributes.namedItem(attrName);

	if (!item.isNull())
	{
		value = item.nodeValue().toFloat();
		return true;
	}

	return false;
}

template <>
inline bool IXmlSerializable::readAttribute(const QDomNode& node, const QString& attrName, bool& value)
{
	QDomNamedNodeMap attributes = node.attributes();

	QDomNode item = attributes.namedItem(attrName);

	if (!item.isNull())
	{
		value = item.nodeValue().toInt() == 1;
		return true;
	}

	return false;
}

template <>
inline bool IXmlSerializable::readAttribute(const QDomNode& node, const QString& attrName, long& value)
{
	QDomNamedNodeMap attributes = node.attributes();

	QDomNode item = attributes.namedItem(attrName);

	if (!item.isNull())
	{
		value = item.nodeValue().toLong();
		return true;
	}

	return false;
}

template <>
inline bool IXmlSerializable::readAttribute(const QDomNode& node, const QString& attrName, QStringList& value)
{
	QDomNamedNodeMap attributes = node.attributes();

	QDomNode item = attributes.namedItem(attrName);

	if (!item.isNull())
	{
		QString sJoined = item.nodeValue();
		value = sJoined.split(QChar(';'), QString::SkipEmptyParts);

		return true;
	}

	return false;
}

template <>
inline bool IXmlSerializable::readAttribute(const QDomNode& node, const QString& attrName, QString& value)
{
	QDomNamedNodeMap attributes = node.attributes();

	QDomNode item = attributes.namedItem(attrName);

	if (!item.isNull())
	{
		value = item.nodeValue();
		return true;
	}

	return false;
}

template <>
inline bool IXmlSerializable::readAttribute(const QDomNode& node, const QString& attrName, QByteArray& value)
{
	QDomNamedNodeMap attributes = node.attributes();

	QDomNode item = attributes.namedItem(attrName);

	if (!item.isNull())
	{
		value = item.nodeValue().toLocal8Bit();
		return true;
	}

	return false;
}

template <class T>
inline bool IXmlSerializable::readArray(const QDomNode& node, QVector< T > &value)
{
	QDomNode child = node.firstChild();

	while (!child.isNull())
	{
		T proxy;
		convert(child, proxy);
		value.push_back(proxy);

		child = child.nextSibling();
	}

	return true;
}

template <class T>
inline bool IXmlSerializable::readSiblingArray(const QDomNode& node, QVector< T >& value)
{
	QDomNode iterable = node;

	while (!iterable.isNull())
	{
		T proxy;

		convert(iterable.firstChild(), proxy);
		value.push_back(proxy);

		iterable = iterable.nextSibling();
	}

	return true;
}

template <>
inline bool IXmlSerializable::convert(const QDomNode& node, QString& elem)
{
	elem = node.nodeValue();
	return true;
}

template <>
inline bool IXmlSerializable::convert(const QString& string, QString& elem)
{
	elem = string;
	return true;
}

template <>
inline bool IXmlSerializable::convert(const QString& string, int& elem)
{
	elem = string.toInt();
	return true;
}

template <>
inline bool IXmlSerializable::convert(const QString& string, uint& elem)
{
	elem = string.toUInt();
	return true;
}

template <>
inline bool IXmlSerializable::convert(const QString& string, float& elem)
{
	elem = string.toFloat();
	return true;
}

template <typename K, typename T>
inline bool IXmlSerializable::readMap(const QDomNode& node, const QString& keyTag, const QString& valueTag, QMap<K, T>& map)
{
	QDomNode child = node.firstChild();

	while (!child.isNull())
	{
		QString key, value;
		readAttribute(child, keyTag, key);
		readAttribute(child, valueTag, value);

		K tKey;
		T tValue;

		convert(key, tKey);
		convert(value, tValue);

		map.insert(tKey, tValue);

		child = child.nextSibling();
	}

	return true;
}

template <>
inline bool IXmlSerializable::writeNodeValue(QDomDocument& document, QDomNode& node, const int& value)
{
	QDomText valueElement = document.createTextNode(QString::number(value));
	addNode(node, valueElement);
	return true;
}

template <>
inline bool IXmlSerializable::writeNodeValue(QDomDocument& document, QDomNode& node, const float& value)
{
	QDomText valueElement = document.createTextNode(QString::number(value));
	addNode(node, valueElement);
	return true;
}

template <>
inline bool IXmlSerializable::writeNodeValue(QDomDocument& document, QDomNode& node, const QString& value)
{
	QDomText valueElement = document.createTextNode(value);
	addNode(node, valueElement);
	return true;
}


template <>
inline bool IXmlSerializable::writeNodeValue(QDomDocument& document, QDomNode& node, const bool& value)
{
	QDomText valueElement = document.createTextNode(value ? "true" : "false");
	addNode(node, valueElement);
	return true;
}

template <typename T>
inline QDomElement IXmlSerializable::addElement(QDomDocument& document, QDomNode& parent, const QString& name, const T& value)
{
	QDomElement newChild = document.createElement(name);

	writeNodeValue(document, newChild, value);

	return addElement(parent, newChild).toElement();
}

template <typename T>
inline bool IXmlSerializable::addAttribute(QDomElement& node, const QString& name, const T& value)
{
	node.setAttribute(name, value);
	return true;
}

template <typename K, typename T>
inline bool IXmlSerializable::writeMap(QDomDocument& document, QDomNode& parent, const QString& mapTag, const QString& keyName, const QString& valueName, const QMap< K, T >& map)
{
	for (QMap< K, T >::const_iterator iElem = map.begin(); iElem != map.end(); ++iElem)
	{
		QDomElement element = addElement(document, parent, mapTag);

		addAttribute(element, keyName, iElem.key());
		addAttribute(element, valueName, iElem.value());
	}

	return true;
}

template <typename T>
inline bool IXmlSerializable::writeContainer(QDomDocument& document, QDomNode& node, const QString& tagName, const QVector< T >& elements)
{
	bool r = true;
	for (int idx = 0, count = elements.size(); idx < count; ++idx)
	{
		QDomElement element = addElement(document, node, tagName);
 		r = r && convert(document, element, elements[idx]);
	}

	return r;
}
		 
#endif