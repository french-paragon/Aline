#ifndef ALINE_EXTERNALITEMREFERENCELISTMODEL_H
#define ALINE_EXTERNALITEMREFERENCELISTMODEL_H

#include <QSortFilterProxyModel>
#include <QStringList>

namespace Aline {

class EditableItem;

/*!
 * \brief The ExternalItemReferenceListModel class Help to list the item refered by an editableitem
 * Listing references is a standard behaviour for editable items.
 * This model, given an editableitem with a property that represent a list of ref to other item (as a QStringList) allow to display it in widgets that uses Qt's model/view paradigm. Just provide the item and the name of the property.
 * Pay attention ! A change in the string list reset the model, so it's recommanded to use this solution only for items with small list of reference.
 */
class ExternalItemReferenceListProxyModel : public QSortFilterProxyModel
{
	Q_OBJECT
public:

	explicit ExternalItemReferenceListProxyModel(EditableItem *item, QString watchPropertyName, int refRole, QObject* parent = nullptr);
	virtual ~ExternalItemReferenceListProxyModel();

	int refRole() const;


signals:

public slots:

	void reset();

protected:
	virtual bool filterAcceptsRow(int source_row, const QModelIndex & source_parent) const;

	QStringList list() const;

	EditableItem * _trackedItem;

	char* _watchedPropertyName;
	int _watchedPropertyIndex;

	QMetaObject::Connection _connection;

	int _refRole;
};

} // namespace Aline

#endif // ALINE_EXTERNALITEMREFERENCELISTMODEL_H
