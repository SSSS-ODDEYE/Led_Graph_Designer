#include "FontLibDesigner.h"
#include <QColorDialog>
#include <QFileDialog>

FontLibDesigner::FontLibDesigner(QWidget* parent)
	: QMainWindow(parent)
{
	ui.setupUi(this);

	ui.outerColor->setStyleSheet(QString{ "background-color:%1" }.arg(ui.ledCharacterEdit->outerBorderColor().name()));
	ui.outerColor->setToolTip(ui.ledCharacterEdit->outerBorderColor().name());

	ui.innerColor->setStyleSheet(QString{ "background-color:%1" }.arg(ui.ledCharacterEdit->innerBorderColor().name()));
	ui.innerColor->setToolTip(ui.ledCharacterEdit->innerBorderColor().name());

	connect(ui.outerColor, &QPushButton::clicked, [=]
		{
			const auto color{ QColorDialog::getColor(ui.outerColor->palette().color(QPalette::Window),
															 this, "Select Outer Color") };
			ui.outerColor->setStyleSheet(QString{ "background-color:%1" }.arg(color.name()));
			ui.outerColor->setToolTip(color.name());
			ui.ledCharacterEdit->setOuterBorderColor(color);
		});
	connect(ui.innerColor, &QPushButton::clicked, [=]
		{
			const auto color{ QColorDialog::getColor(ui.innerColor->palette().color(QPalette::Window),
															 this, "Select Inner Color") };
			ui.innerColor->setStyleSheet(QString{ "background-color:%1" }.arg(color.name()));
			ui.innerColor->setToolTip(color.name());
			ui.ledCharacterEdit->setInnerBorderColor(color);
		});

	connect(ui.actionNew, &QAction::triggered, [=]
		{
			if (QDialog::Accepted == d.exec())
			{
				ui.ledCharacterEdit->setPixelSize(d.pixelSize());
			}
		});

	ui.pencilButtonGroup->setProperty("pencilIndex", ui.pencilButtonGroup->id(ui.mainColor));
	connect(ui.pencilButtonGroup, static_cast<void(QButtonGroup::*)(QAbstractButton*)>(&QButtonGroup::buttonClicked), [=](QAbstractButton* btn)
		{
			const auto id{ ui.pencilButtonGroup->id(btn) };
			if (ui.pencilButtonGroup->property("pencilIndex") == id)
			{
				const auto color{ QColorDialog::getColor(btn->palette().color(QPalette::Window),
														 this, "Select Pencil Color") };
				btn->setStyleSheet(QString{ "background-color:%1" }.arg(color.name()));
				btn->setToolTip(color.name());
				ui.ledCharacterEdit->setPencil(color);
			}
			else
			{
				ui.pencilButtonGroup->setProperty("pencilIndex", id);
				ui.ledCharacterEdit->setPencil(btn->palette().color(QPalette::Window));
			}
		});

	connect(ui.actionOpen, &QAction::triggered, [=]
		{
			const auto imageFile = QFileDialog::getOpenFileName(this, "Load Image", "",
				"Image Files(*.jpg *.png *.bmp *.ico *.pgm *.pbm);;All(*.*)");
			if (imageFile.isEmpty())
				return;
			const QPixmap image{ imageFile };
			ui.ledCharacterEdit->setColorMap(image.copy({ { 0, 0 }, ui.ledCharacterEdit->pixelSize() }));
		});

	connect(ui.loadImage, &QPushButton::clicked, ui.actionOpen, &QAction::triggered);

	codeBrowserUpdateTimer.setInterval(500);
	connect(&codeBrowserUpdateTimer, &QTimer::timeout, this, &FontLibDesigner::doUpdateCodeBrowser);

	connect(ui.ledCharacterEdit, &LedCharacterEdit::pixelDrawed, this, &FontLibDesigner::updateCodeBrowser);
	connect(ui.ledCharacterEdit, &LedCharacterEdit::colorMapChanged, this, &FontLibDesigner::updateCodeBrowser);
	connect(ui.fmtButtonGroup, &QButtonGroup::idToggled, this, &FontLibDesigner::updateCodeBrowser);
	connect(ui.colorButtonGroup, &QButtonGroup::idToggled, this, &FontLibDesigner::updateCodeBrowser);
	connect(ui.useMatrix2, &QPushButton::toggled, this, &FontLibDesigner::updateCodeBrowser);
}

void FontLibDesigner::updateCodeBrowser()
{
	codeBrowserUpdateTimer.start();
}

void FontLibDesigner::doUpdateCodeBrowser() const
{
	const auto colorVector{ ui.ledCharacterEdit->rgbEncode() };

	const bool useMatrix2 = ui.useMatrix2->isChecked();
	const bool useColor3 = ui.useColor3->isChecked();

	if (ui.useFmtHtml->isChecked())
	{
		ui.codeBrowser->setPlainText(fmtColorMap(colorVector, ui.ledCharacterEdit->pixelSize(), ", ", ", ", "", "", [=](QRgb color)
			{
				return QString::asprintf("#%02x%02x%02x", qBlue(color), qGreen(color), qRed(color));
			}));
	}
	else if (ui.useFmtCpp->isChecked())
	{
		ui.codeBrowser->setPlainText(fmtColorMap(colorVector, ui.ledCharacterEdit->pixelSize(), ", ",
			useMatrix2 ? "},\n{" : ", ", useMatrix2 ? "{\n{" : "{", useMatrix2 ? "}\n}" : "}",
			[=](QRgb color)
			{
				return useColor3
					? QString::asprintf("{0x%02x, 0x%02x, 0x%02x}", qRed(color), qGreen(color), qBlue(color))
					: QString::asprintf("0x%06x", 0xffffff & color);
			}));
	}
	else if (ui.useFmtPython->isChecked())
	{
		ui.codeBrowser->setPlainText(fmtColorMap(colorVector, ui.ledCharacterEdit->pixelSize(), ", ",
			useMatrix2 ? "],\n[" : ", ", useMatrix2 ? "[\n[" : "[", useMatrix2 ? "]\n]" : "]",
			[=](QRgb color)
			{
				return useColor3
					? QString::asprintf("[0x%02x, 0x%02x, 0x%02x]", qRed(color), qGreen(color), qBlue(color))
					: QString::asprintf("0x%06x", 0xffffff & color);
			}));
	}
	else if (ui.useFmtJson->isChecked())
	{
		ui.codeBrowser->setPlainText(fmtColorMap(colorVector, ui.ledCharacterEdit->pixelSize(), ", ",
			useMatrix2 ? "],\n[" : ", ", useMatrix2 ? "[\n[" : "[", useMatrix2 ? "]\n]" : "]",
			[=](QRgb color)
			{
				return useColor3
					? QString::asprintf("[%u, %u, %u]", qRed(color), qGreen(color), qBlue(color))
					: QString::asprintf("%u", 0xffffff & color);
			}));
	}
}

QString FontLibDesigner::fmtColorMap(const QVector<QRgb>& colors, QSize size, QString innerJoin, QString outerJoin,
	QString prefix, QString suffix, std::function<QString(QRgb)> fmtColorFunc)
{
	const auto width = size.width();
	const auto colorCount = colors.size();
	QStringList serial;

	for (int index = 0; index < colorCount; index += width)
	{
		QStringList lineSerial;
		for (int i = 0; i < width && index + i < colorCount; ++i)
			lineSerial += fmtColorFunc(colors[index + i]);
		serial += lineSerial.join(innerJoin);
	}
	return prefix + serial.join(outerJoin) + suffix;
}