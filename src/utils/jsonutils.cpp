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


const QString Aline::JsonUtils::LABEL_REF = "labels";

const QString Aline::JsonUtils::LABEL_REF_ID = "reference";
const QString Aline::JsonUtils::LABEL_NAME_ID = "name";
const QString Aline::JsonUtils::LABEL_ITEMS_REFS_ID = "marked_items";
const QString Aline::JsonUtils::LABEL_SUBLABELS_ID = "sublabels";

const QString Aline::JsonUtils::ITEM_SUBITEM_ID = "item_internalsubitems";
const QString Aline::JsonUtils::ITEM_SUBITEM_LIST = "subitemslist";

void Aline::JsonUtils::extractItemData(Aline::EditableItem* item, QJsonObject const& obj, EditableItemFactoryManager* subItemFactory, QStringList const& specialSkippedProperties, bool blockSignals) {

	if (blockSignals) {
		item->blockSignals(true);
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

		if (prop == ITEM_SUBITEM_ID) {
			continue;
		}

		if (specialSkippedProperties.contains(prop)) {
			continue;
		}

		const QMetaObject* meta = item->metaObject();
		int prop_index = meta->indexOfProperty(prop.toStdString().c_str());

		if (obj.value(prop).isObject()) {

			QJsonObject subObj = obj.value(prop).toObject();

			if (subObj.contains(ITEM_SUBITEM_LIST)) {
				if (subObj.value(ITEM_SUBITEM_LIST).isArray()) {
					QJsonArray arr = subObj.value(ITEM_SUBITEM_LIST).toArray();
					QList<Aline::EditableItem*> itemList;

					for (QJsonValue val : arr) {

						if (val.isObject()) {
							QJsonObject listedObj = val.toObject();

							if (listedObj.contains(EditableItem::REF_PROP_NAME) &&
									listedObj.contains(EditableItem::TYPE_ID_NAME)) {

								QString ref = listedObj.value(EditableItem::REF_PROP_NAME).toString();
								QString type = listedObj.value(EditableItem::TYPE_ID_NAME).toString();

								if (subItemFactory->hasSubItemFactoryInstalled(type)) {
									Aline::EditableItem* subItem = subItemFactory->createSubItem(type, ref, item);

									extractItemData(subItem, subObj, subItemFactory, specialSkippedProperties, blockSignals);

									if (subItem != nullptr) {
										itemList.push_back(subItem);
									}
								}
							}
						}

					}

					item->setProperty(prop.toStdString().c_str(), QVariant::fromValue(itemList), true);
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
						item->setProperty(prop.toStdString().c_str(), QVariant::fromValue(subItem), true);
						continue;
					}
				}
			}

		}

		QVariant var = decodeVariantFromJson(obj.value(prop), meta->property(prop_index).type());

		if (var.type() != meta->property(prop_index).type() && meta->property(prop_index).type() != QVariant::Invalid) {
			var.convert(meta->property(prop_index).type());
		}

		item->setProperty(prop.toStdString().c_str(), var, true); //set all the properties.
	}

	if (obj.contains(ITEM_SUBITEM_ID)) {

		QJsonValue subItems_val = obj.value(ITEM_SUBITEM_ID);

		if (subItems_val.isArray()) {
			QJsonArray arr = subItems_val.toArray();

			for (QJsonValue v : arr) {

				if (v.isObject()) {
					QJsonObject subObj = v.toObject();

					QString ref;
					QString type;

					if (subObj.contains(EditableItem::REF_PROP_NAME) &&
							subObj.contains(EditableItem::TYPE_ID_NAME)) {

						ref = subObj.value(EditableItem::REF_PROP_NAME).toString();
						type = subObj.value(EditableItem::TYPE_ID_NAME).toString();

						if (subItemFactory->hasSubItemFactoryInstalled(type)) {
							Aline::EditableItem* subItem = subItemFactory->createSubItem(type, ref, item);

							extractItemData(subItem, subObj, subItemFactory, specialSkippedProperties, blockSignals);

							if (subItem != nullptr) {
								item->insertSubItem(subItem);
							}
						}

					}
				}

			}
		}

	}

	if (blockSignals) {
		item->blockSignals(false);
	}

}

void addPropToObject(QJsonObject & obj, Aline::EditableItem* item, const char* prop) {

	QString sprop(prop);

	const QMetaObject* meta = item->metaObject();
	int prop_index = meta->indexOfProperty(prop);

	if (meta->property(prop_index).type() == qMetaTypeId<Aline::EditableItem*>()) {

		Aline::EditableItem* subitem = qvariant_cast<Aline::EditableItem*>(item->property(prop));

		obj.insert(sprop, Aline::JsonUtils::encapsulateItemToJson(subitem));

	} else if (meta->property(prop_index).type() == qMetaTypeId<QList<Aline::EditableItem*>>()) {

		QList<Aline::EditableItem*> list = qvariant_cast<QList<Aline::EditableItem*>>(item->property(prop));

		QJsonArray arr;

		for (Aline::EditableItem* subitem : list) {
			arr.push_back(Aline::JsonUtils::encapsulateItemToJson(subitem));
		}

		QJsonObject subCollection;

		subCollection.insert(Aline::JsonUtils::ITEM_SUBITEM_LIST, arr);

		obj.insert(sprop, subCollection);

	} else {

		obj.insert(sprop, Aline::JsonUtils::encodeVariantToJson(item->property(prop)) ); // insert the property.

	}

}

QJsonObject Aline::JsonUtils::encapsulateItemToJson(Aline::EditableItem* item) {

	QJsonObject obj;

	const QMetaObject* mobj = item->metaObject();

	for (int i = 0; i < mobj->propertyCount(); i++) {

		if (!mobj->property(i).isStored(item)) {
			continue;
		}

		const char* prop = mobj->property(i).name();

		addPropToObject(obj, item, prop);
	}

	QList<QByteArray> dynamicProperties = item->dynamicPropertyNames();

	for (QByteArray cpropName : dynamicProperties) {

		addPropToObject(obj, item, cpropName.toStdString().c_str());

	}

	QJsonArray internal_items_array;

	for (Aline::EditableItem* internal_item : item->getSubItems()) {
		QJsonObject sub_obj = encapsulateItemToJson(internal_item);

		internal_items_array.append(sub_obj);

	}

	if (internal_items_array.size() > 0) {
		obj.insert(ITEM_SUBITEM_ID, internal_items_array);
	}

	return obj;

}

QJsonValue Aline::JsonUtils::encodeVariantToJson(QVariant var) {

	if (var.type() == QVariant::PointF) {
		QPointF data = var.toPointF();

		QString rep(QString("%1,%2").arg(data.x()).arg(data.y()));

		return QJsonValue(rep);

	} else if (var.type() == QVariant::SizeF) {

		QSizeF data = var.toSizeF();

		QString rep(QString("%1,%2").arg(data.width()).arg(data.height()));

		return QJsonValue(rep);

	} else if (var.type() == QVariant::Color) {

		QColor col = var.value<QColor>();

		return QJsonValue(col.name(QColor::HexArgb));
	}

	return QJsonValue::fromVariant(var);

}
QVariant Aline::JsonUtils::decodeVariantFromJson(QJsonValue val, QVariant::Type type) {

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
