#include "LedGridNewDialog.h"

LedGridNewDialog::LedGridNewDialog(QWidget *parent)
	: QDialog(parent)
{
	ui.setupUi(this);
	QMap<QString, QSize> map = {
		{"4x8", {4,8}},
		{"8x8", {8,8}},
		{"8x16", {8,16}},
		{"16x16", {16,16}},
		{"16x32", {16,32}},
		{"32x32", {32,32}}
	};

	ui.pixelSizeChoice->clear();
	for(auto&title: map.keys())
	{
		ui.pixelSizeChoice->addItem(title, map.value(title));
	}

	connect(ui.pixelHCount, static_cast<void(QSpinBox::*)(int)>(&QSpinBox::valueChanged), [=](int c)
		{
			setHorizontalPixelCount(c);
		});
	connect(ui.pixelVCount, static_cast<void(QSpinBox::*)(int)>(&QSpinBox::valueChanged), [=](int c)
		{
			setVerticalPixelCount(c);
		});
	
	connect(ui.pixelSizeChoice, &QComboBox::currentTextChanged, [=]
		{
			const auto size = ui.pixelSizeChoice->currentData().toSize();
			ui.pixelHCount->setValue(size.width());
			ui.pixelVCount->setValue(size.height());
		});

	ui.pixelSizeChoice->setCurrentText("4x8");
}

QSize LedGridNewDialog::pixelSize() const
{
	return m_pixelSize;
}

void LedGridNewDialog::setHorizontalPixelCount(int w)
{
	m_pixelSize.setWidth(w);
}

void LedGridNewDialog::setVerticalPixelCount(int h)
{
	m_pixelSize.setHeight(h);
}

void LedGridNewDialog::setPixelSize(QSize s)
{
	m_pixelSize = s;
}