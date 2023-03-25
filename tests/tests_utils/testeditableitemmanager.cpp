#include "testeditableitemmanager.h"

namespace Aline {
namespace Tests {

TestEditableItemManager::TestEditableItemManager(QObject *parent):
	EditableItemManager(parent)
{

}

bool TestEditableItemManager::hasDataSource() const {
	return true;
}

bool TestEditableItemManager::saveStruct() {
	return true;
}

bool TestEditableItemManager::saveLabels() {
	return true;
}

bool TestEditableItemManager::loadStruct() {
	return true;
}

bool TestEditableItemManager::isNetworkShared() const {
	return false;
}

void TestEditableItemManager::reset() {

	closeAll();
	cleanTreeStruct();
}

EditableItem* TestEditableItemManager::effectivelyLoadItem(QString const& ref) {

	Q_UNUSED(ref);

	return nullptr;
}

bool TestEditableItemManager::clearItemData(QString itemRef) {
	Q_UNUSED(itemRef);

	return true;
}

bool TestEditableItemManager::effectivelySaveItem(QString const& ref) {

	Q_UNUSED(ref);

	return true;

}

void TestEditableItemManager::effectivelyLoadLabels() {

	return;

}

} // namespace Tests
} // namespace Aline
