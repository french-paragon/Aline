#ifndef ALINE_EXTERNALITEMREFERENCELISTMODEL_H
#define ALINE_EXTERNALITEMREFERENCELISTMODEL_H

#include <QAbstractListModel>
#include <QStringList>

namespace Aline {

class EditableItem;

/*!
 * \brief The ExternalItemReferenceListModel class Help to list the item refered by an editableitem
 * Listing references is a standard behaviour for editable items.
 * This model, given an editableitem with a property that represent a list of ref to other item (as a QStringList) allow to display it in widgets that uses Qt's model/view paradigm. Just provide the item and the name of the property.
 * Pay attention ! A change in the string list reset the model, so it's recommanded to use this solution only for items with small list of reference.
 */
class ExternalItemReferenceListModel : public QAbstractListModel
{
	Q_OBJECT
public:

	enum roles{
		ItemRefRole = Qt::UserRole,
		ItemNameRole = Qt::UserRole + 1,
		ItemRole = Qt::UserRole + 2
	};

	explicit ExternalItemReferenceListModel(EditableItem *parent, QString watchPropertyName);
	virtual ~ExternalItemReferenceListModel();

	virtual int rowCount(const QModelIndex &parent) const;

	virtual QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;

	virtual QHash<int, QByteArray> roleNames() const;


signals:

public slots:

	void reset();

protected:

	QStringList list() const;

	EditableItem * _parentItem;

	char* _watchedPropertyName;
	int _watchedPropertyIndex;

	QMetaObject::Connection _connection;
};

} // namespace Aline

#endif // ALINE_EXTERNALITEMREFERENCELISTMODEL_H
