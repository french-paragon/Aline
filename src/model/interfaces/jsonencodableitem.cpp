#include "jsonencodableitem.h"

#include "../editableitem.h"

#include <QJsonArray>

namespace Aline {

QJsonObject JsonEncodableItem::basicObjectPropsToJson(EditableItem const* item) {

	QJsonObject obj;

	obj.insert(EditableItem::REF_PROP_NAME, item->getRef());
	obj.insert(EditableItem::TYPE_ID_NAME, item->getTypeId());
	obj.insert("objectName", item->objectName());

	QJsonArray labels;
	for (QString & str: item->getLabels()) {
		labels.push_back(str);
	}
	obj.insert("labels", labels);

	return obj;

}

void JsonEncodableItem::basicObjectPropsFromJson(EditableItem* item, QJsonObject const& obj) {

	if (obj.contains("objectName")) {
		item->setObjectName(obj.value("objectName").toString(""));
	}

	if (obj.contains("labels")) {
		QJsonArray labels = obj.value("labels").toArray(QJsonArray());

		QStringList lst;
		for (QJsonValue const& val : qAsConst(labels)) {
			QString str = val.toString("");

			if (!str.isEmpty()) {
				lst.push_back(str);
			}
		}

		item->setLabels(lst);
	}

}

} // namespace Aline
