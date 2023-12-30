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

#include "jsonutils.h"

#include "model/editableitem.h"
#include "model/labels/labelstree.h"
#include "model/labels/label.h"
#include "model/editableitemfactory.h"

#include <QJsonArray>
#include <QColor>
#include <QPointF>
#include <QSizeF>
#include <QMetaObject>
#include <QMetaProperty>

#include "model/interfaces/jsonencodableitem.h"


const QString Aline::JsonUtils::LABEL_REF = "labels";

const QString Aline::JsonUtils::LABEL_REF_ID = "reference";
const QString Aline::JsonUtils::LABEL_NAME_ID = "name";
const QString Aline::JsonUtils::LABEL_ITEMS_REFS_ID = "marked_items";
const QString Aline::JsonUtils::LABEL_SUBLABELS_ID = "sublabels";

const QString Aline::JsonUtils::ITEM_SUBITEM_ID = "item_internalsubitems";
const QString Aline::JsonUtils::ITEM_SUBITEM_LIST = "subitemslist";

const QString Aline::JsonUtils::ITEM_REFERENT_LIST = "item_referentslist";

const QString Aline::JsonUtils::TREE_REF_ID = "reference";
const QString Aline::JsonUtils::TREE_TYPE_ID = "type";
const QString Aline::JsonUtils::TREE_NAME_ID = "name";
const QString Aline::JsonUtils::TREE_CHILDRENS_ID = "childrens";
const QString Aline::JsonUtils::TREE_ACCEPT_CHILDRENS_ID = "accept_childrens";

void Aline::JsonUtils::extractItemData(Aline::EditableItem* item,
									   QJsonObject const& obj,
									   EditableItemFactoryManager* subItemFactory,
									   QStringList const& specialSkippedProperties,
									   bool blockSignals,
									   const JsonPropExtractor* visitor) {

	if (blockSignals) {
		item->blockSignals(true);
	}

	item->blockChangeDetection(true);

	if (obj.contains(ITEM_REFERENT_LIST)) {
		QJsonValue val = obj.value(ITEM_REFERENT_LIST);
		if (val.isArray()) {
			QJsonArray referents = val.toArray();

			for (auto val : qAsConst(referents)) {
				QString referent = val.toString("");

				if (!referent.isEmpty()) {
					item->warnRefering(referent);
				}
			}
		}
	}

	JsonEncodableItem* jsonEncodable = qobject_cast<JsonEncodableItem*>(item);

	if (jsonEncodable != nullptr) {
		jsonEncodable->confgureItemFromJson(obj);

		if (blockSignals) {
			item->blockSignals(false);
		}

		item->blockChangeDetection(false);

		return;
	}

	for (QString prop : obj.keys()) {

		if (prop == Aline::EditableItem::TYPE_ID_NAME) {
			continue;
		}

		if (prop == Aline::EditableItem::CHILDREN_PROP_NAME) {
			continue;
		}

		if (prop == Aline::EditableItem::REF_PROP_NAME) {
			continue;
		}

		if (specialSkippedProperties.contains(prop)) {
			continue;
		}

		//check if the end user wanted to erase the default behavior with the visitor
		if (visitor != nullptr) {
			bool visitorDidIt = (*visitor)(obj, item, prop.toStdString().c_str());

			if (visitorDidIt) {
				continue;
			}
		}

		const QMetaObject* meta = item->metaObject();
		int prop_index = meta->indexOfProperty(prop.toStdString().c_str());

		if (obj.value(prop).isObject()) {

			QJsonObject subObj = obj.value(prop).toObject();

			if (subObj.contains(ITEM_SUBITEM_LIST)) {
				if (subObj.value(ITEM_SUBITEM_LIST).isArray()) {
					QJsonArray arr = subObj.value(ITEM_SUBITEM_LIST).toArray();
					QList<Aline::EditableItem*> itemList;

					for (QJsonValue const& val : qAsConst(arr)) {

						if (val.isObject()) {
							QJsonObject listedObj = val.toObject();

							if (listedObj.contains(EditableItem::REF_PROP_NAME) &&
									listedObj.contains(EditableItem::TYPE_ID_NAME)) {

								QString ref = listedObj.value(EditableItem::REF_PROP_NAME).toString();
								QString type = listedObj.value(EditableItem::TYPE_ID_NAME).toString();

								if (subItemFactory->hasSubItemFactoryInstalled(type)) {
									Aline::EditableItem* subItem = subItemFactory->createSubItem(type, ref, item);

									extractItemData(subItem, listedObj, subItemFactory, specialSkippedProperties, blockSignals);

									if (subItem != nullptr) {
										itemList.push_back(subItem);
									}
								}
							}
						}

					}

					item->setProperty(prop.toStdString().c_str(), QVariant::fromValue(itemList));
					continue;
				}

			} else if (subObj.contains(EditableItem::REF_PROP_NAME) &&
					   subObj.contains(EditableItem::TYPE_ID_NAME)) {

				QString ref = subObj.value(EditableItem::REF_PROP_NAME).toString();
				QString type = subObj.value(EditableItem::TYPE_ID_NAME).toString();

				if (subItemFactory->hasSubItemFactoryInstalled(type)) {
					Aline::EditableItem* subItem = subItemFactory->createSubItem(type, ref, item);

					extractItemData(subItem, subObj, subItemFactory, specialSkippedProperties, blockSignals);

					if (subItem != nullptr) {
						item->setProperty(prop.toStdString().c_str(), QVariant::fromValue(subItem));
						continue;
					}
				}
			}

		}

		QVariant var = decodeVariantFromJson(obj.value(prop), meta->property(prop_index).userType());

		if (var.type() != meta->property(prop_index).type() && meta->property(prop_index).type() != QVariant::Invalid) {
			if (meta->property(prop_index).isEnumType()) {
				bool ok;
				int val = meta->property(prop_index).enumerator().keyToValue(var.toString().toStdString().c_str(), &ok);
				if (ok) {
					var = QVariant::fromValue(val);
				}
			} else {
				var.convert(meta->property(prop_index).type());
			}
		}

		item->setProperty(prop.toStdString().c_str(), var); //set all the properties.
	}

	if (blockSignals) {
		item->blockSignals(false);
	}

	item->blockChangeDetection(false);

}

void addPropToObject(QJsonObject & obj, Aline::EditableItem* item, const char* prop, const Aline::JsonUtils::JsonPropEncapsulator *visitor) {

	QString sprop(prop);

	const QMetaObject* meta = item->metaObject();
	int prop_index = meta->indexOfProperty(prop);

	if (meta->property(prop_index).userType() == qMetaTypeId<Aline::EditableItem*>()) {

		Aline::EditableItem* subitem = qvariant_cast<Aline::EditableItem*>(item->property(prop));

		obj.insert(sprop, Aline::JsonUtils::encapsulateItemToJson(subitem, visitor));

	} else if (meta->property(prop_index).userType() == qMetaTypeId<QList<Aline::EditableItem*>>()) {

		QList<Aline::EditableItem*> list = qvariant_cast<QList<Aline::EditableItem*>>(item->property(prop));

		QJsonArray arr;

		for (Aline::EditableItem* subitem : list) {
			arr.push_back(Aline::JsonUtils::encapsulateItemToJson(subitem, visitor));
		}

		QJsonObject subCollection;

		subCollection.insert(Aline::JsonUtils::ITEM_SUBITEM_LIST, arr);

		obj.insert(sprop, subCollection);

	} else {

		obj.insert(sprop, Aline::JsonUtils::encodeVariantToJson(item->property(prop)) ); // insert the property.

	}

}

QJsonObject Aline::JsonUtils::encapsulateItemToJson(Aline::EditableItem* item, const JsonPropEncapsulator *visitor) {

	JsonEncodableItem* jsonEncodable = qobject_cast<JsonEncodableItem*>(item);

	if (jsonEncodable != nullptr) {
		return jsonEncodable->encodeItemToJson();
	}

	//if the item is not encodable, attempt to do as best as possible based on visile properties
	QJsonObject obj;

	const QMetaObject* mobj = item->metaObject();

	for (int i = 0; i < mobj->propertyCount(); i++) {

		if (!mobj->property(i).isStored(item)) {
			continue;
		}

		const char* prop = mobj->property(i).name();

		//check if the end user wanted to erase the default behavior with the visitor
		if (visitor != nullptr) {
			bool visitorDidIt = (*visitor)(obj, item, prop);

			if (visitorDidIt) {
				continue;
			}
		}

		addPropToObject(obj, item, prop, visitor);
	}

	QList<QByteArray> dynamicProperties = item->dynamicPropertyNames();

	for (QByteArray const& cpropName : qAsConst(dynamicProperties)) {

		//check if the end user wanted to erase the default behavior with the visitor
		if (visitor != nullptr) {
			bool visitorDidIt = (*visitor)(obj, item, cpropName.toStdString().c_str());

			if (visitorDidIt) {
				continue;
			}
		}

		addPropToObject(obj, item, cpropName.toStdString().c_str(), visitor);

	}

	QJsonArray referents;
	for (QString & referent : item->listReferents()) {
		referents.push_back(referent);
	}
	if (referents.size() > 0) {
		obj.insert(ITEM_REFERENT_LIST, referents);
	}

	return obj;

}

QJsonValue Aline::JsonUtils::encodeVariantToJson(QVariant var) {

	int typeId = var.userType();
	QMetaType metaType(typeId);

	if (typeId == QVariant::PointF) {
		QPointF data = var.toPointF();

		QString rep(QString("%1,%2").arg(data.x()).arg(data.y()));

		return QJsonValue(rep);

	} else if (typeId == QVariant::SizeF) {

		QSizeF data = var.toSizeF();

		QString rep(QString("%1,%2").arg(data.width()).arg(data.height()));

		return QJsonValue(rep);

	} else if (typeId == QVariant::Color) {

		QColor col = var.value<QColor>();

		return QJsonValue(col.name(QColor::HexArgb));

	} else if (metaType.flags() & QMetaType::IsEnumeration) {

		return QJsonValue(var.toString());
	}

	return QJsonValue::fromVariant(var);

}
QVariant Aline::JsonUtils::decodeVariantFromJson(QJsonValue val, int type) {

	QMetaType metaType(type);

	if (type == QVariant::PointF) {

		QString rep = val.toString();

		QStringList reps = rep.split(",");

		if (reps.size() != 2) {
			return QVariant();
		}

		QPointF pt;

		bool ok;
		qreal x = QVariant(reps[0]).toReal(&ok);

		if (!ok) {
			return QVariant();
		}

		qreal y = QVariant(reps[1]).toReal(&ok);

		if (!ok) {
			return QVariant();
		}

		pt.setX(x);
		pt.setY(y);

		return QVariant(pt);

	} else if (type == QVariant::SizeF) {

		QString rep = val.toString();

		QStringList reps = rep.split(",");

		if (reps.size() != 2) {
			return QVariant();
		}

		QSizeF s;

		bool ok;
		qreal w = QVariant(reps[0]).toReal(&ok);

		if (!ok) {
			return QVariant();
		}

		qreal h = QVariant(reps[1]).toReal(&ok);

		if (!ok) {
			return QVariant();
		}

		s.setWidth(w);
		s.setHeight(h);

		return QVariant(s);

	} else if (type == QVariant::Color) {

		QString colorName = val.toString();

		QColor col(colorName);

		return QVariant(col);

	} else if (metaType.flags() & QMetaType::IsEnumeration) {
		return val.toVariant(); //conversion from string to enum should be managed by QVariant.
	}

	return val.toVariant();

}

QJsonObject Aline::JsonUtils::encodeLabelAsJson(LabelsTree const* tree, QModelIndex const& index) {

	QJsonObject obj;

	QVariant name = tree->data(index, Qt::DisplayRole);
	QVariant ref = tree->data(index, Aline::LabelsTree::LabelRefRole);

	obj.insert(LABEL_NAME_ID, QJsonValue::fromVariant(name));
	obj.insert(LABEL_REF_ID, QJsonValue::fromVariant(ref));

	QVariant items_refs = tree->data(index, Aline::LabelsTree::LabelItemsRefsRole);

	if (items_refs != QVariant()) {
		obj.insert(LABEL_ITEMS_REFS_ID, QJsonValue::fromVariant(items_refs));
	}

	if (tree->rowCount(index) > 0) {

		QJsonArray subLabels;

		for (int i = 0; i < tree->rowCount(index); i++) {
			QJsonObject subobj = encodeLabelAsJson(tree, tree->index(i, 0, index));
			subLabels.push_back(QJsonValue(subobj));
		}

		obj.insert(LABEL_SUBLABELS_ID, QJsonValue(subLabels));

	}

	return obj;
}

Aline::Label* Aline::JsonUtils::extractJsonLabel(QJsonValue const& val, Aline::LabelsTree* parent) {

	Aline::Label* l = new Aline::Label(parent);

	try {
		extractJsonLabelDatas(val, l);
	} catch (JsonUtilsException const& e) {
		Q_UNUSED(e);
		delete l;
		return nullptr;
	}

	return l;

}
Aline::Label* Aline::JsonUtils::extractJsonLabel(QJsonValue const& val, Aline::Label* parent) {

	Aline::Label* l = new Aline::Label(parent);

	try {
		extractJsonLabelDatas(val, l);
	} catch (JsonUtilsException const& e) {
		Q_UNUSED(e);
		delete l;
		return nullptr;
	}

	return l;

}
void Aline::JsonUtils::extractJsonLabelDatas(QJsonValue const& val, Aline::Label* label) {

	if (!val.isObject()) {
		throw JsonUtilsException(LABEL_REF, "Error while extracting labels");
	}

	QJsonObject obj = val.toObject();

	if (!obj.contains(LABEL_REF_ID) || !obj.contains(LABEL_NAME_ID)) {
		throw JsonUtilsException(LABEL_REF, "Missing label name or reference.");
	}

	QJsonValue ref = obj.value(LABEL_REF_ID);
	QJsonValue name = obj.value(LABEL_NAME_ID);


	if (!ref.isString() || !name.isString()) {
		throw JsonUtilsException(LABEL_REF, "Label name or reference can't be converted to string.");
	}

	label->setRef(ref.toString());
	label->setObjectName(name.toString());

	if (obj.contains(LABEL_ITEMS_REFS_ID)) {
		QJsonValue items = obj.value(LABEL_ITEMS_REFS_ID);

		if (items.isArray()) {
			for (QJsonValue value : items.toArray()) {

				if (value.isString()) {
					label->markItem(value.toString());
				}

			}
		}
	}

	if (obj.contains(LABEL_SUBLABELS_ID)) {
		QJsonValue sublabels = obj.value(LABEL_SUBLABELS_ID);

		if (sublabels.isArray()) {

			for (QJsonValue val : sublabels.toArray()) {
				Aline::Label* sublabel = extractJsonLabel(val, label);

				if (sublabel != nullptr) {
					label->addChild(sublabel);
				}
			}

		}
	}

}

void Aline::JsonUtils::encapsulateTreeLeafsToJson(QJsonObject & obj,
												  QMap<QString, QVector<EditableItemManager::treeStruct*>> const& itemsByTypes) {

	QJsonArray arrTypes;

	for (QString typeRef : itemsByTypes.keys()) {

		QJsonObject collection;
		collection.insert(TREE_TYPE_ID, typeRef);

		QJsonArray items;

		for (EditableItemManager::treeStruct* branch : itemsByTypes.value(typeRef)) {

			QJsonObject treeLeaf;

			treeLeaf.insert(TREE_REF_ID, branch->_ref);
			treeLeaf.insert(TREE_NAME_ID, branch->_name);
			treeLeaf.insert(TREE_ACCEPT_CHILDRENS_ID, branch->_acceptChildrens);

			items.push_back(treeLeaf);

		}

		collection.insert(TREE_CHILDRENS_ID, items);

		arrTypes.push_back(collection);

	}

	obj.insert(TREE_CHILDRENS_ID, arrTypes);

}

void Aline::JsonUtils::extractTreeLeafs(QJsonObject &obj,
										QMap<QString, QVector<EditableItemManager::treeStruct*>> & itemsByTypes,
										QMap<QString, EditableItemManager::treeStruct*> & treeIndex) {

	if (!obj.contains(TREE_CHILDRENS_ID)) {

		throw JsonUtilsException("", QString("Error while parsing JSON data, missing reference for childrens."));

	}

	QJsonValue val = obj.value(TREE_CHILDRENS_ID);

	if (!val.isArray()) {
		throw JsonUtilsException("", QString("Error while parsing JSON data, childrens reference point to a non array."));
	}

	QJsonArray arr = val.toArray();

	for (QJsonValue v : arr) {

		if (v.isObject()) {

			QJsonObject o = v.toObject();

			if (o.contains(TREE_TYPE_ID) && o.contains(TREE_CHILDRENS_ID)) {

				QJsonValue typeV = o.value(TREE_TYPE_ID);

				if (!typeV.isString()) {
					continue;
				}

				QString typeId = typeV.toString();

				if (!itemsByTypes.contains(typeId)) {
					itemsByTypes.insert(typeId, QVector<EditableItemManager::treeStruct*>());
				}

				QJsonValue childrensV = o.value(TREE_CHILDRENS_ID);

				if (!childrensV.isArray()) {
					continue;
				}

				for (QJsonValue c : childrensV.toArray()) {

					if (c.isObject()) {
						QJsonObject leafO = c.toObject();

						if (leafO.contains(TREE_REF_ID) && leafO.contains(TREE_NAME_ID)) {

							QJsonValue refV = leafO.value(TREE_REF_ID);
							QJsonValue nameV = leafO.value(TREE_NAME_ID);

							if (refV.isString() && nameV.isString()) {

								EditableItemManager::treeStruct* leaf = new EditableItemManager::treeStruct();

								leaf->_ref = refV.toString();
								leaf->_name = nameV.toString();

								if (leafO.contains(TREE_ACCEPT_CHILDRENS_ID)) {
									QJsonValue ac = leafO.value(TREE_ACCEPT_CHILDRENS_ID);

									if (ac.isBool()) {
										leaf->_acceptChildrens = ac.toBool();
									} else {
										leaf->_acceptChildrens = false;
									}
								} else {
									leaf->_acceptChildrens = false;
								}

								leaf->_type_ref = typeId;

								treeIndex.insert(leaf->_ref, leaf);
								itemsByTypes[typeId].push_back(leaf);

							}
						}
					}

				}

			}

		}

	}

}


Aline::JsonUtils::JsonUtilsException::JsonUtilsException (QString ref, QString infos) :
	_ref(ref),
	_infos(infos)
{
	QString what = "[Error while parsing \"" + _ref + "\"]: " + _infos;
	_what = what.toStdString();
}
Aline::JsonUtils::JsonUtilsException::JsonUtilsException (JsonUtilsException const& other) :
	JsonUtilsException(other.ref(), other.infos())
{

}

const char* Aline::JsonUtils::JsonUtilsException::what() const throw() {

	return _what.c_str();

}


QString Aline::JsonUtils::JsonUtilsException::ref() const
{
	return _ref;
}

QString Aline::JsonUtils::JsonUtilsException::infos() const
{
	return _infos;
}

void Aline::JsonUtils::JsonUtilsException::raise() const {
	throw *this;
}

Aline::JsonUtils::JsonUtilsException * Aline::JsonUtils::JsonUtilsException::clone() const {
	return new JsonUtilsException(*this);
}
