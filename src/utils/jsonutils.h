#ifndef JSONUTILS_H
#define JSONUTILS_H

#include "../aline_global.h"
#include "../model/editableitemmanager.h"

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

extern const QString TREE_REF_ID;
extern const QString TREE_TYPE_ID;
extern const QString TREE_NAME_ID;
extern const QString TREE_CHILDRENS_ID;
extern const QString TREE_ACCEPT_CHILDRENS_ID;

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

ALINE_EXPORT void encapsulateTreeLeafsToJson(QJsonObject & obj, QMap<QString, QVector<EditableItemManager::treeStruct*>> const& itemsByTypes);
ALINE_EXPORT void extractTreeLeafs(QJsonObject &obj, QMap<QString, QVector<EditableItemManager::treeStruct*>> & itemsByTypes, QMap<QString, EditableItemManager::treeStruct*> & treeIndex);

}

} // namespace Aline

#endif // JSONUTILS_H
