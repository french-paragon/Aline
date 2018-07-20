#ifndef ALINE_EDITABLEITEM_H
#define ALINE_EDITABLEITEM_H

#include <QObject>
#include <QSet>

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

	/*!
	 * \brief EditableItem constructor for item that belongs to a project.
	 * \param ref The ref of the item.
	 * \param parent The parent manager of the item.
	 *
	 * EditableItems can be root items in a project or subitems for another item. This constructor is for the root items.
	 *
	 * When you subclass EditableItem you can enforce that the subclass has to be used as root item or subitem
	 * by providing only constructors that call the associated constructor of the parent class.
	 */
	explicit EditableItem(QString ref, EditableItemManager *parent = nullptr);

	/*!
	 * \brief EditableItem constructor for item that is a subitem for another item.
	 * \param ref The ref of the item.
	 * \param parent The parent item of the item.
	 *
	 * EditableItems can be root items in a project or subitems for another item. This constructor is for the subitems.
	 *
	 * When you subclass EditableItem you can enforce that the subclass has to be used as root item or subitem
	 * by providing only constructors that call the associated constructor of the parent class.
	 */
	explicit EditableItem(QString ref, EditableItem *parent = nullptr);

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

	virtual bool getHasUnsavedChanged() const;

	/*!
	 * \brief acceptChildrens indicate if the item accept childs or not.
	 * \return true if the item accept children, false otherwise.
	 */
	virtual bool acceptChildrens() const;

	virtual bool autoSave() const;

	virtual QString iconInternalUrl() const = 0;

	EditableItem *getParentItem() const;
	EditableItemManager *getManager() const;

	/*!
	 * \brief insertSubItem tell the item that a subitem has been created.
	 * \param item the item that have been inserted
	 *
	 * The subitems are managed using Qt parent/children system. Most of the time externally instanced subitems will just be used as a storage,
	 * so there's nothing to worry about. But when instanciating some objects one might need to reset some internal subitems.
	 * For those kinds of usage it might be usefull to do a bit more of processing.
	 * Child classes can do so by subclassing this method.
	 *
	 * Default implementation ensure the item has the correct parent and register the ref after making it uniq. Call it in your overhides.
	 */
	virtual void insertSubItem(EditableItem* item);
	virtual QList<EditableItem *> getSubItems() const;

	/*!
	 * \brief getFileReferencePropertiesName get the name of the properties which refer to a file.
	 * \return the list of names (by default nothing).
	 *
	 * Some managers might want to synchronize files over some network, this list is the list of properties which represent a reference to a file.
	 */
	virtual QStringList getFileReferencePropertiesName() const;

	bool setProperty(const char *name, const QVariant &value, bool noStatesChanges = false);

	QStringList getLabels() const;
	/*!
	 * \brief setLabels change all the labels at once
	 * \param labels the labels to set
	 *
	 * this function will first remove all the labels one by one using removeLabel then add the new ones.
	 * This is meant to ensure signals are triggered properly and the item stay in a valid state.
	 * There is an exception, when signals are blocked, it will just replace the list.
	 * Usually this function is meant to be called at initialization, when signals are blocked.
	 */
	void setLabels(const QStringList &labels);

	bool hasLabel(QString const& labelRef) const;
	bool addLabel(QString const& labelRef);
	bool removeLabel(QString const& labelRef);

signals:

	void parentChanged(QString newRef);

	void visibleStateChanged(QString ref);

	void refSwap(QString oldRef, QString newRef);
	void refChanged(QString newRef);
	void unsavedStateChanged(bool saveState);

	void labelAdded(QString const& label);
	void labelRemoved(QString const& label);

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

	bool _hasUnsavedChanged;

	QString makeSubItemRefUniq(QString const& subItemRef) const;
	QSet<QString> _usedRef;

	void removeSubItemRef(EditableItem* item);

private:

	void setParentItem(EditableItem* parent);

	EditableItemManager* _manager;
	EditableItem* _parentItem;

	QStringList _labels;

};

} // namespace Aline

Q_DECLARE_METATYPE(Aline::EditableItem*)
Q_DECLARE_METATYPE(QList<Aline::EditableItem*>)

#endif // ALINE_EDITABLEITEM_H
