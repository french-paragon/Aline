#ifndef JSONUTILS_H
#define JSONUTILS_H

/*This file is part of the project Aline

Copyright (C) 2022 Paragon <french.paragon@gmail.com>

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU Lesser General Public
License as published by the Free Software Foundation; either
version 3 of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License
along with this program. If not, see <https://www.gnu.org/licenses/>.
*/

#include "../aline_global.h"
#include "../model/editableitemmanager.h"

#include <QVariant>
#include <QJsonObject>
#include <QJsonValue>
#include <QException>

#include <functional>

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

extern const QString ITEM_REFERENT_LIST;

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

//! \brief JsonPropEncapsulator represent an encapsulator that can be provided to the automatic json encoding function to modify its functionning.
using JsonPropEncapsulator = std::function<bool(QJsonObject &, Aline::EditableItem const *, const char*)>;
//! \brief JsonPropEncapsulator represent an encapsulator that can be provided to the automatic json extracting function to modify its functionning.
using JsonPropExtractor = std::function<bool(QJsonObject const&, Aline::EditableItem*, const char*)>;

ALINE_EXPORT void extractItemData(Aline::EditableItem* item,
								  QJsonObject const& obj,
								  EditableItemFactoryManager* subItemFactory,
								  QStringList const& specialSkippedProperties = {},
								  bool blockSignals = true,
								  const JsonPropExtractor* visitor = nullptr);
ALINE_EXPORT QJsonObject encapsulateItemToJson(Aline::EditableItem const* item, const JsonPropEncapsulator* visitor = nullptr);

ALINE_EXPORT QJsonValue encodeVariantToJson(QVariant var);
ALINE_EXPORT QVariant decodeVariantFromJson(QJsonValue val, int type);

ALINE_EXPORT QJsonObject encodeLabelAsJson(LabelsTree const* tree, QModelIndex const& index);

ALINE_EXPORT Aline::Label* extractJsonLabel(QJsonValue const& val, Aline::LabelsTree* parent);
ALINE_EXPORT Aline::Label* extractJsonLabel(QJsonValue const& val, Aline::Label* parent);
ALINE_EXPORT void extractJsonLabelDatas(QJsonValue const& val, Aline::Label* label);

ALINE_EXPORT void encapsulateTreeLeafsToJson(QJsonObject & obj, QMap<QString, QVector<EditableItemManager::treeStruct*>> const& itemsByTypes);
ALINE_EXPORT void extractTreeLeafs(QJsonObject &obj, QMap<QString, QVector<EditableItemManager::treeStruct*>> & itemsByTypes, QMap<QString, EditableItemManager::treeStruct*> & treeIndex);

}

} // namespace Aline

#endif // JSONUTILS_H
