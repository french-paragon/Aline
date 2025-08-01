#ifndef ALINE_EDITABLEITEM_H
#define ALINE_EDITABLEITEM_H

/*This file is part of the project Aline

Copyright (C) 2022-2025 Paragon <french.paragon@gmail.com>

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

#include <QObject>
#include <QSet>
#include <QVector>

#include "../aline_global.h"

#include <array>

namespace Aline {

class EditableItemManager;

class ALINE_EXPORT EditableItem : public QObject
{
	Q_OBJECT

	friend class EditableItemManager;

public:

	/*!
	 * \brief The ManagedEditableItemReference class hold and manage a reference to another EditableItem
	 */
	class ManagedEditableItemReference {
	public:

		/*!
		 * \brief ManagedEditableItemReference default constructor for an invalid reference
		 */
		inline ManagedEditableItemReference() :
			_holder(nullptr),
			_ref("")
		{

		}

		/*!
		 * \brief ManagedEditableItemReference constructor for an empty reference
		 * \param item the item that holds the reference
		 */
		inline ManagedEditableItemReference(EditableItem* holder) :
			_holder(holder),
			_ref("")
		{
			if (holder != nullptr) {
				holder->_managedReferences.push_back(this);
			}
		}

		inline ManagedEditableItemReference(ManagedEditableItemReference const& other) :
			_holder(other._holder),
			_ref(other._ref)
		{
			if (_holder != nullptr) {
				_holder->_managedReferences.push_back(this);
			}
		}

		inline ~ManagedEditableItemReference() {
			if (_holder != nullptr) {
				_holder->_managedReferences.removeAll(this);
			}
		}

		ManagedEditableItemReference& operator=(ManagedEditableItemReference const& other);

		inline bool holdReference() const {
			return !_ref.isEmpty();
		}

		inline QString referedItem() const {
			return _ref;
		}

		bool setReference(QString url);
		void clearReference();

	protected:
		EditableItem* _holder;
		QString _ref;

		friend class EditableItem;
	};

	static QString simplifyRef(QString ref);

    static const std::array<int, 14> AllowedPropertiesTypeId;

	static const char* REF_PROP_NAME;
	static const char* TYPE_ID_NAME;

	static const char* CHILDREN_PROP_NAME;

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
	Q_PROPERTY(QString type_id READ getTypeId CONSTANT)
	Q_PROPERTY(QStringList labels READ getLabels WRITE setLabels)

	Q_PROPERTY(Aline::EditableItemManager* manager READ getManager CONSTANT STORED false)

	/*!
	 * \brief getTypeId allow to get info on the type of the editable item.
	 * \return a type id as a string.
	 */
	virtual QString getTypeId() const = 0;

	/*!
	 * \brief editAsTypeId get the type id this type should be considered as for the purpose of editing.
	 * \return the type the item should be considered as for the purpose of editing, by default getTypeId().
	 *
	 * Some classes of items use inheritance but all fit in a single editor setup for the main class,
	 * or can be edited with one special function to fit them all. This function gives a type id this class
	 * should be considered as for the purpose of editing.
	 */
	virtual QString editAsTypeId() const;

	/*!
	 * \brief getTypeName allow to get the name of the editable item type as a string.
	 * \return the name
	 */
	virtual QString getTypeName() const = 0;

	QString getRef() const;
	QString getFullRefUrl() const;

	virtual bool hasUnsavedChanges() const;
	void newUnsavedChanges();

	/*!
	 * \brief acceptChildrens indicate if the item accept childs or not.
	 * \return true if the item accept children, false otherwise.
	 */
	virtual bool acceptChildrens() const;

	virtual bool autoSave() const;

	virtual QString iconInternalUrl() const = 0;

	EditableItem *getTopLevelParentItem() const;
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
     * Default implementation ensure the item has the correct parent and register the ref after making it uniq. Call it in your overrides.
	 */
	virtual void insertSubItem(EditableItem* item);
	virtual QList<EditableItem *> getSubItems() const;

	/*!
	 * \brief getSubItemByRef return a specific subitem given its reference.
	 * \param ref the reference of the subitem.
	 * \return the item, or nullptr if no item could be found.
	 *
	 * The default implementation search all subitems extensively.
	 * This can be pretty unnefective, so the function can be overriden to be more effective is subclasses.
	 */
	virtual EditableItem* getSubItemByRef(QString const& ref) const;

	virtual QStringList getSubItemsRefsList() const;
	QString makeSubItemRefUniq(QString const& subItemRef) const;

	/*!
	 * \brief getFileReferencePropertiesName get the name of the properties which refer to a file.
	 * \return the list of names (by default nothing).
	 *
	 * Some managers might want to synchronize files over some network, this list is the list of properties which represent a reference to a file.
	 */
	virtual QStringList getFileReferencePropertiesName() const;

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

	virtual bool event(QEvent *e);

	/*!
	 * \brief blockChangeDetection block changes detection, effectivly setting the item in loading mode
	 * \param block_change_detection
	 */
	void blockChangeDetection(bool block_change_detection);

	/*!
	 * \brief managerIsConfiguring indicate if doing changes on the item will trigger an unsaved state or not.
	 * \return true if the item won't become unsaved, false otherwise.
	 *
	 * In standard operational mode, the editableitem will track changes to its properties, subitems, ...
	 * But sometimes, such behavior is imporductive. So the EditableItemManager can disable it by telling the item it's in configuration mode.
	 * Devellopers might want to check this state before changing the unsaved state.
	 */
	bool changeDetectionIsBlocked() const;

	/*!
	 * \brief getHasBeenLoadedFromDisk indicate if the item is loading from the disk or not
	 * \return true if the item is ready to use, false otherwise.
	 *
	 * By default it's false, but the editableitem manager change this behaviour after loading the item.
	 */
	bool hasBeenLoadedFromDisk() const;

	/*!
	 * \brief setupLoadingMode setup the object in loading mode
	 * \param loading if the object is in loading mode
	 *
	 * when an object is in loading mode, it will not notify other objects if it refers them.
	 * Other signals and notifications might be blocked.
	 */
	void setupLoadingMode(bool loading);

	void warnRefering(QString referentItemRef);
	void warnReferentRefChanges(QString referentItemOldRef, QString referentItemRef);
	void warnUnrefering(QString referentItemRef);

	inline QList<QString> listReferents() const {
		return _referentItems.values();
	}

	/*!
	 * \brief listRefering list the items the current item is refering
	 * \return the list of urls
	 *
	 * By default this function return nothing and needs to be reimplemented in classes which refer other classes.
	 *
	 * !If you forget to override this function while refering some items, the changeref function will fail!.
	 */
	virtual QList<QString> listRefering() const;

Q_SIGNALS:

	void parentChanged(QString newRef);

	void visibleStateChanged(QString ref);

	void refSwap(QString oldRef, QString newRef);
	void refChanged(QString newRef);
	void unsavedStateChanged(bool saveState);

	void labelAdded(QString const& label);
	void labelRemoved(QString const& label);

public Q_SLOTS:

	virtual void suppress();

	virtual bool save();

	virtual void changeRef(QString const& newRef);

protected:

	/*!
	 * \brief treatParentRefChange is called by a parent editable item to let subitem warn referents about their changes in url.
	 * \param oldParentUrl the old url of the parent
	 * \param newParentUrl the new url of the parent
	 */
	void treatParentRefChange(QString oldParentUrl, QString newParentUrl);

	/*!
	 * \brief warnReferedRefChanges warn an item refering another item that this other item is about to change ref.
	 * \param oldRef The old url ref of the refered item.
	 * \param newRef The new url ref of the refered item.
	 * By default, editableItem don't refer to other items, so this function do nothing.
	 * A subclass of editable item which refer to other items needs to reimplement this function to be warned about references changes.
	 */
	virtual void warnReferedRefChanges(QString oldRef, QString newRef);
	void updateDynamicReferences(QString oldRef, QString newRef);

	/*!
	 * \brief warnReferedRemoved warn an item refering another item that this other item is about to be supressed from the project.
	 * \param ref The reference of the item.
	 *
	 * By default, editableItem don't refer to other items, so this function do nothing.
	 * A subclass of editable item which refer to other items needs to reimplement this function to be warned about its refered items being deleted^.
	 */
	virtual void warnReferedRemoved(QString ref);

	void onVisibleStateChanged();

	void clearHasUnsavedChanges();

	/*!
	 * \brief onLoadingDone is an event handler called when the loading of an object is done.
	 *
	 * Default implementation does nothing.
	 *
	 * It is recommended to not try to load other items in this handler, to avoid complicated inter-dependancies in the loading chain.
	 */
	virtual void onLoadingDone();

	QString _ref;

	QString _p_ref;

	bool _hasUnsavedChanged;

	QSet<QString> _usedRef;

	/*!
	 * \brief removeSubItemRef clear a subitem ref from the list of used ref
	 * \param item the item which ref should be removed
	 *
	 * call this function when you remove a subitem from the item.
	 */
	void removeSubItemRef(EditableItem* item);

private:

	void setParentItem(EditableItem* parent);

	EditableItemManager* _manager;
	EditableItem* _parentItem;

	QStringList _labels;

	bool _changeDetectionIsBlocked;
	bool _hasBeenLoadedFromDisk;

	/*!
	 * \brief _referentItems Items may refer to other item. They have to warn them that they are refered, so that refered items can forward ref changes to referent items.
	 */
	QSet<QString> _referentItems;
	/*!
	 * \brief _managedReferences are automatically registered to be updated in case the corresponding item change.
	 */
	QVector<ManagedEditableItemReference*> _managedReferences;

};

} // namespace Aline

Q_DECLARE_METATYPE(Aline::EditableItem*)
Q_DECLARE_METATYPE(QList<Aline::EditableItem*>)

#endif // ALINE_EDITABLEITEM_H
