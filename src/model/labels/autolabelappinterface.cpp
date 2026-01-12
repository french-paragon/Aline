#include "autolabelappinterface.h"

#include "./labelstree.h"

namespace Aline {

const char* AutoLabelAppInterface::AutoLabelAppInterfaceCode = "Aline_AppInterfaces_AutoLabels";

AutoLabelAppInterface::AutoLabelAppInterface(QObject *parent)
	: QObject{parent}
{

}

void AutoLabelAppInterface::setupAutoLabelsInTree(LabelsTree* labelTree, EditableItemManager* project) {

	if (labelTree == nullptr) {
		return;
	}

	if (labelTree->_auto_labels.size() > 0) {

		for (Label* label : qAsConst(labelTree->_auto_labels)) {
			label->deleteLater();
		}
		labelTree->_auto_labels.clear();
	}

	for (Factory const& factory : qAsConst(_factories)) {
		auto result = factory(labelTree, project);

		if (!result.functor) {
			continue;
		}

		Label* autoLabel = new Label(labelTree);
		autoLabel->setObjectName(result.name);
		autoLabel->setRef(result.ref);
		autoLabel->configureAutoFunctor(result.functor);

		connect(autoLabel, &Label::refChanged, labelTree, &LabelsTree::exchangeRef);
		connect(autoLabel, &Label::gettedRef, labelTree, &LabelsTree::registerRef);

		labelTree->_auto_labels.push_back(autoLabel);

	}

}

} // namespace Aline
