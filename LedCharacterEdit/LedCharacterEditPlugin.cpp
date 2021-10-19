#include "LedCharacterEdit.h"
#include "LedCharacterEditPlugin.h"

#include <QtCore/QtPlugin>

LedCharacterEditPlugin::LedCharacterEditPlugin(QObject* parent)
	: QObject(parent)
{
	initialized = false;
}

void LedCharacterEditPlugin::initialize(QDesignerFormEditorInterface* /*core*/)
{
	if (initialized)
		return;

	initialized = true;
}

bool LedCharacterEditPlugin::isInitialized() const
{
	return initialized;
}

QWidget* LedCharacterEditPlugin::createWidget(QWidget* parent)
{
	return new LedCharacterEdit(parent);
}

QString LedCharacterEditPlugin::name() const
{
	return "LedCharacterEdit";
}

QString LedCharacterEditPlugin::group() const
{
	return "OhtoAi Plugins";
}

QIcon LedCharacterEditPlugin::icon() const
{
	return QIcon();
}

QString LedCharacterEditPlugin::toolTip() const
{
	return QString();
}

QString LedCharacterEditPlugin::whatsThis() const
{
	return QString();
}

bool LedCharacterEditPlugin::isContainer() const
{
	return false;
}

QString LedCharacterEditPlugin::domXml() const
{
	return "<widget class=\"LedCharacterEdit\" name=\"ledCharacterEdit\">\n"
		" <property name=\"geometry\">\n"
		"  <rect>\n"
		"   <x>0</x>\n"
		"   <y>0</y>\n"
		"   <width>100</width>\n"
		"   <height>100</height>\n"
		"  </rect>\n"
		" </property>\n"
		"</widget>\n";
}

QString LedCharacterEditPlugin::includeFile() const
{
	return "LedCharacterEdit.h";
}