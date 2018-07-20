#ifndef JSONUTILS_H
#define JSONUTILS_H

#include "../aline_global.h"

#include <QVariant>
#include <QJsonObject>
#include <QJsonValue>
#include <QException>

class QModelIndex;

namespace Aline {

class EditableItem;
class LabelsTree;
class Label;
class EditableItemFactoryManager;

namespace JsonUtils {

extern const QString LABEL_REF;

extern const QString LABEL_REF_ID;
extern const QString LABEL_NAME_ID;
extern const QString LABEL_ITEMS_REFS_ID;
extern const QString LABEL_SUBLABELS_ID;

extern const QString ITEM_SUBITEM_ID;
extern const QString ITEM_SUBITEM_LIST;

class ALINE_EXPORT JsonUtilsException : public QException
{
public:

	JsonUtilsException (QString ref, QString infos);
	JsonUtilsException (JsonUtilsException const& other);

	const char* what() const throw();

	void raise() const;
	JsonUtilsException *clone() const;

	QString ref() const;
	QString infos() const;

protected:

	QString _ref;
	QString _infos;
	std::string _what;
};

ALINE_EXPORT void extractItemData(Aline::EditableItem* item,
								  QJsonObject const& obj,
								  EditableItemFactoryManager* subItemFactory,
								  QStringList const& specialSkippedProperties = {},
								  bool blockSignals = true);
ALINE_EXPORT QJsonObject encapsulateItemToJson(Aline::EditableItem* item);

ALINE_EXPORT QJsonValue encodeVariantToJson(QVariant var);
ALINE_EXPORT QVariant decodeVariantFromJson(QJsonValue val, QVariant::Type type);

ALINE_EXPORT QJsonObject encodeLabelAsJson(LabelsTree const* tree, QModelIndex const& index);

ALINE_EXPORT Aline::Label* extractJsonLabel(QJsonValue const& val, Aline::LabelsTree* parent);
ALINE_EXPORT Aline::Label* extractJsonLabel(QJsonValue const& val, Aline::Label* parent);
ALINE_EXPORT void extractJsonLabelDatas(QJsonValue const& val, Aline::Label* label);

}

} // namespace Aline

#endif // JSONUTILS_H
