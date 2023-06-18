#ifndef ALINE_JSONENCODABLEITEM_H
#define ALINE_JSONENCODABLEITEM_H

#include <QObject>
#include <QJsonObject>

namespace Aline {

/*!
 * \brief The JsonEncodableItem interface is for editable items that want to implement specific functions to be encoded to json
 */
class JsonEncodableItem
{
public:
	virtual QJsonObject encodeItemToJson() const = 0;
	virtual bool confgureItemFromJson(QJsonObject const& obj) = 0;
};

} // namespace Aline

#define JsonEncodableItemInterface_iid "org.Aline.Model.JsonEncodableItemInterface"

Q_DECLARE_INTERFACE(Aline::JsonEncodableItem, JsonEncodableItemInterface_iid)

#endif // ALINE_JSONENCODABLEITEM_H
