#ifndef ALINE_EDITABLEITEM_H
#define ALINE_EDITABLEITEM_H

#include <QObject>

#include "../aline_global.h"

namespace Aline {

class EditableItemManager;

class ALINE_EXPORT EditableItem : public QObject
{
	Q_OBJECT

	friend class EditableItemManager;

public:

	static QString simplifyRef(QString ref);

	static const QString REF_PROP_NAME;
	static const QString TYPE_ID_NAME;

	static const QString CHILDREN_PROP_NAME;

	explicit EditableItem(QString ref, EditableItemManager *parent = nullptr);

	Q_PROPERTY(QString ref READ getRef WRITE changeRef NOTIFY refChanged)
	Q_PROPERTY(QString parentRef MEMBER _p_ref NOTIFY parentChanged)
	Q_PROPERTY(QString type_id READ getTypeId)

	/*!
	 * \brief getTypeId allow to get info on the type of the editable item.
	 * \return a type id as a string.
	 */
	virtual QString getTypeId() const = 0;

	/*!
	 * \brief getTypeName allow to get the name of the editable item type as a string.
	 * \return the name
	 */
	virtual QString getTypeName() const = 0;

	QString getRef() const;

	/*!
	 * \brief getChildrenItemsRefs is a function to fetch all children item of that item.
	 * \return a list of children items.
	 */
	QVector<QString> getChildrenItemsRefs() const;

	virtual bool getHasUnsavedChanged() const;

	/*!
	 * \brief acceptChildrens indicate if the item accept childs or not.
	 * \return true if the item accept children, false otherwise.
	 */
	virtual bool acceptChildrens() const;

	virtual bool autoSave() const;

	virtual QString iconInternalUrl() const = 0;

signals:

	void parentChanged(QString newRef);

	void visibleStateChanged(QString ref);

	void refChanged(QString newRef);
	void unsavedStateChanged(bool saveState);

public slots:

	virtual void suppress();

	virtual bool save();

	virtual void changeRef(QString const& newRef);

protected:

	void onVisibleStateChanged();

	void newUnsavedChanges();
	void clearHasUnsavedChanges();

	QString _ref;

	QString _p_ref;

	EditableItemManager* _manager;

	bool _hasUnsavedChanged;
};

} // namespace Aline

#endif // ALINE_EDITABLEITEM_H
