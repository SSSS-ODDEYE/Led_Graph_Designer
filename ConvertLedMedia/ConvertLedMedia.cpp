#include "ConvertLedMedia.h"

#include <QDropEvent>
#include <QMimeData>
#include <QFileDialog>
#include <QMovie>
#include <QTimer>
#include <json.hpp>
#include <QApplication>

ConvertLedMedia::ConvertLedMedia(QWidget* parent)
	: QMainWindow(parent)
{
	ui.setupUi(this);

	if (QFile configFile{ "config.json" }; configFile.exists())
	{
		configFile.open(QFile::ReadOnly | QFile::Text);
		
		auto j{ nlohmann::json::parse(configFile.readAll().toStdString()) };
		configFile.close();

		if (j.contains("use_color3"))
			m_fmtColor3 = j["use_color3"];
		if(j.contains("save_origin_path"))
			m_saveOriginPath = j["save_origin_path"];
		if(j.contains("file_suffix"))
			m_fileSuffix = QString::fromStdString(j["file_suffix"]);
	}

}

void ConvertLedMedia::dropEvent(QDropEvent* event)
{
	ui.titleLabel->setStyleSheet("border-width:0px;");
	ui.titleLabel->setText("Processing...");
	setAcceptDrops(false);
	QApplication::processEvents();

	const QMimeData* mimeData = event->mimeData();

	if (!mimeData->hasUrls())
	{
		return;
	}
	QStringList s;
	const auto urlList = mimeData->urls();

	QMovie movie(this);
	for (auto url : urlList)
	{
		movie.setFileName(url.toLocalFile());
		if (!movie.isValid())
			continue;

		QFileInfo inf(url.toLocalFile());
		auto fileName = inf.fileName();
		auto path = m_saveOriginPath ? inf.canonicalPath() + "/" : ".";

		QFile file(QString{ "%1/%2.%3" }.arg(path, inf.completeBaseName(), m_fileSuffix));
		for (int index = 0; file.exists(); ++index)
			file.setFileName(QString{ "%1/%2_%4.%3" }.arg(path, inf.completeBaseName(), m_fileSuffix).arg(index));
		file.open(QFile::WriteOnly | QFile::Text);
		QTextStream stream(&file);
		auto frameCount{ movie.frameCount() };
		if (frameCount == 0)
			frameCount = 1;

		movie.jumpToFrame(0);
		stream << "{";
		stream << "\n  \"version\":20211015";
		stream << ",\n  \"logic_width\":" << movie.currentImage().width();
		stream << ",\n  \"logic_height\":" << movie.currentImage().height();
		stream << ",\n  \"frame_count\":" << frameCount;
		stream << ",\n  \"frames\":[";
		
		for (int frameIndex = 0; frameIndex < frameCount; ++frameIndex)
		{
			if (frameIndex == 0)
				stream << "\n    {";
			else
				stream << ",\n    {";
			movie.jumpToFrame(frameIndex);
			auto image = movie.currentImage();
			const auto colCount = image.width();
			const auto rowCount = image.height();

			const bool isBigFrame = colCount > 800 || rowCount > 800;

			ui.titleLabel->setText(QString{ "Process %1 (%2/%3)" }.arg(fileName).arg(frameIndex).arg(frameCount));
			QApplication::processEvents();

			stream << "\n      \"duration\":" << movie.nextFrameDelay();
			stream << ",\n      \"width\":" << colCount;
			stream << ",\n      \"height\":" << rowCount;

			const auto bytesPerPixel{ image.depth() / 8 };

			stream << ",\n      \"image\":[";
			for (int rowIndex = 0; rowIndex < rowCount; ++rowIndex)
			{
				QStringList lineString;
				auto lineDataPtr{ image.constScanLine(rowIndex) };
				if (isBigFrame)
				{
					ui.titleLabel->setText(QString{ "Process %1 (%2/%3) %4%" }.arg(fileName).arg(frameIndex)
						.arg(frameCount).arg(QString::number(rowIndex * 100 / (rowCount - 1.0), 'f', 2)));
					QApplication::processEvents();
				}

				if (rowIndex != 0)
					stream << ",";
				stream << "\n        [";
				for (int colIndex = 0; colIndex < colCount; ++colIndex)
				{
					if (colIndex != 0)
						stream << ", ";
					if (!m_fmtColor3)
						stream << QString::asprintf("%u", 0xffffff & qRgb(lineDataPtr[2], lineDataPtr[1], lineDataPtr[0]));
					else
						stream << QString::asprintf("[%d, %d, %d]", lineDataPtr[2], lineDataPtr[1], lineDataPtr[0]);
					lineDataPtr += bytesPerPixel;
				}
				stream << "]";
			}
			stream << "\n      ]";
			stream << "\n    }";
		}

		stream << "\n  ]";
		stream << "\n}";

		file.close();
	}

	setAcceptDrops(true);
	QTimer::singleShot(500, [=] {ui.titleLabel->setText("Drop Image Here"); });
}

void ConvertLedMedia::dragLeaveEvent(QDragLeaveEvent* event)
{
	ui.titleLabel->setStyleSheet("border-width:0px;");
	QMainWindow::dragLeaveEvent(event);
}

void ConvertLedMedia::dragEnterEvent(QDragEnterEvent* event)
{
	bool ignoreDrop{ true };
	auto urlList = event->mimeData()->urls();
	for (const auto& url : urlList)
	{
		QMovie movie(url.toLocalFile());
		if (movie.isValid())
		{
			ignoreDrop = false;
			break;
		}
	}

	if (!ignoreDrop)
	{
		ui.titleLabel->setStyleSheet("border-width:2px;");
		event->acceptProposedAction();
	}
	else
		event->ignore();
}