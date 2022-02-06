#ifndef EDITABLEITEMFILTEREDCONNECTIONPROXYMODEL_H
#define EDITABLEITEMFILTEREDCONNECTIONPROXYMODEL_H

#include <QSortFilterProxyModel>

namespace Aline {

class EditableItem;

/*!
 * \brief The EditableItemFilteredConnectionProxyModel class provide a model filter capable of listing only a set of option available based on the options already set for a given editable item
 * (e.g. ask only for the mobile and home phone number if a contact already have a workplace number).
 */
class EditableItemFilteredConnectionProxyModel : public QSortFilterProxyModel
{
	Q_OBJECT
public:

	Q_PROPERTY(int refRole READ refRole CONSTANT)

	explicit EditableItemFilteredConnectionProxyModel(EditableItem* item, QString propName, int refRole, QObject* parent = nullptr);
	~EditableItemFilteredConnectionProxyModel();

	int refRole() const;

Q_SIGNALS:

public Q_SLOTS:

	void checkForbidenItemRefs();

protected:
	virtual bool filterAcceptsRow(int source_row, const QModelIndex & source_parent) const;

	void setForbidenItemRefs(const QStringList &forbidenItemRefs);

	EditableItem* _trackedItem;

	std::string _watchedPropertyName;
	int _watchedPropertyIndex;

	QStringList _forbidenItemRefs;

	QMetaObject::Connection _connection;

	int _refRole;
};

}

#endif // EDITABLEITEMFILTEREDCONNECTIONPROXYMODEL_H
